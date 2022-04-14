#include "lidar/uart_receiver.h"
#include "system/task_priority.h"
#include "wireless/httpd.h"
#include "localization/pose_refinement.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/uart.h>
#include <esp_log.h>
#include <esp_err.h>
#include <string.h>
#include <hal/uart_hal.h>

#define TAG "UART"
#define UART_RX_PIN 16
#define UART_PORT UART_NUM_2
#define UART_DEV UART2
#define UART_BUFFER_SIZE 512
#define PACKET_QUEUE_LENGTH 64

#define PACKET_SYNC_BYTE 0xFA
#define DISTANCE_DATA_INVALID_FLAG 0x8000
#define DISTANCE_STRENGTH_WARNING_FLAG 0x4000
#define DISTANCE_VALUE_MASK (uint16_t)~(DISTANCE_DATA_INVALID_FLAG | DISTANCE_STRENGTH_WARNING_FLAG)

struct sample_data {
    uint16_t distance;
    uint16_t strength;
} __attribute__((packed));

struct lidar_data_packet {
    uint8_t sync_char;
    uint8_t index;
    uint16_t motor_speed;
    struct sample_data samples[4];
    uint16_t checksum;
} __attribute__((packed));

#define PACKET_SIZE (sizeof(struct lidar_data_packet))

struct lidar_data_buffer_t {
    uint16_t distances[NUMBER_OF_ANGLES];
    uint16_t intensities[NUMBER_OF_ANGLES];
    pose_t calculated_pose;
} __attribute__((packed));

static QueueHandle_t uart_packet_queue;
static uart_hal_context_t hal_context;
static spinlock_t spinlock = portMUX_INITIALIZER_UNLOCKED;
static uint8_t packet_buffer[PACKET_SIZE * PACKET_QUEUE_LENGTH];
static int current_packet_index_in_isr = 0;

static void IRAM_ATTR uart_intr_handle(void *arg);
static void uart_receiver_task(void *parameters);
static bool is_packet_valid(struct lidar_data_packet *packet);

void init_uart_receiver(void)
{
	uart_config_t uart_config = {
		.baud_rate = 115200,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE
	};

    // Configure hardware UART
    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, UART_PIN_NO_CHANGE, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    // Install UART driver
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, UART_BUFFER_SIZE, 0, 0, NULL, 0));

    // Create packet queue
    uart_packet_queue = xQueueCreate(PACKET_QUEUE_LENGTH, sizeof(void*));

    // Unregister software UART ISR and register ours
    uart_intr_config_t intr_config = {
        .intr_enable_mask = UART_RXFIFO_FULL_INT_ENA_M,
        .rxfifo_full_thresh = 8 * PACKET_SIZE
    };

    hal_context.dev = UART_LL_GET_HW(UART_NUM_2);
    ESP_ERROR_CHECK(uart_isr_free(UART_PORT));
    ESP_ERROR_CHECK(uart_isr_register(UART_PORT, uart_intr_handle, NULL, ESP_INTR_FLAG_IRAM, NULL));
    ESP_ERROR_CHECK(uart_enable_rx_intr(UART_PORT));
    ESP_ERROR_CHECK(uart_intr_config(UART_PORT, &intr_config));

    // Start packet handling task
    TaskHandle_t task;
    xTaskCreate(uart_receiver_task, "uart_receiver", TASK_STACK_SIZE, NULL, UART_RECEIVER_PRIORITY, &task);
}

void switch_off_uart_receiver(void)
{
    ESP_ERROR_CHECK_WITHOUT_ABORT(uart_disable_rx_intr(UART_PORT));
}


static void IRAM_ATTR uart_intr_handle(void *arg)
{
    uint32_t uart_intr_status = uart_hal_get_intsts_mask(&hal_context);
    BaseType_t higher_priority_task_awaken = false;

    // Unrecognized interrupt, ignored
    if (!(uart_intr_status & UART_RXFIFO_FULL_INT_CLR)) {
        uart_hal_clr_intsts_mask(&hal_context, uart_intr_status);
        return;
    }

    portENTER_CRITICAL_ISR(&spinlock);
    uint32_t fifo_length = uart_hal_get_rxfifo_len(&hal_context);

    while (fifo_length > 2 * PACKET_SIZE) {
        uint8_t *current_buffer = packet_buffer + current_packet_index_in_isr * PACKET_SIZE;
        int length = 1;

        // Synchronize on alignment character
        do {
            uart_hal_read_rxfifo(&hal_context, current_buffer, &length);
            fifo_length--;
        } while (current_buffer[0] != PACKET_SYNC_BYTE && fifo_length >= PACKET_SIZE);

        // Retrieve the rest of the packet
        if (current_buffer[0] == PACKET_SYNC_BYTE) {
            length = PACKET_SIZE - 1;
            uart_hal_read_rxfifo(&hal_context, current_buffer + 1, &length);
            fifo_length -= length;
        }

        // If we reconstructed a full packet, advertise it
        if (length == PACKET_SIZE - 1) {
            xQueueSendFromISR(uart_packet_queue, &current_buffer, &higher_priority_task_awaken);
            current_packet_index_in_isr = (current_packet_index_in_isr + 1) % PACKET_QUEUE_LENGTH;
        }
    }

    uart_hal_clr_intsts_mask(&hal_context, UART_INTR_RXFIFO_FULL);

    portEXIT_CRITICAL_ISR(&spinlock);

    if (higher_priority_task_awaken) {
        portYIELD_FROM_ISR();
    }
}

static void uart_receiver_task(void *parameters)
{
    static struct lidar_data_buffer_t lidar_data_buffer;

    while (true) {
        struct lidar_data_packet *packet;
        
        if (!xQueueReceive(uart_packet_queue, &packet, 100)) {
            ESP_LOGI(TAG, "Timeout while fetching lidar UART packet");
            continue;
        }

        // Call motor feedback control regulation
        float motor_speed = (float)(packet->motor_speed) / 64.0;
        xTaskNotify(motor_control_task, *(uint32_t*)&motor_speed, eSetValueWithOverwrite);

        if (!is_packet_valid(packet)) {
            continue;
        }

        // Fill distance and strength buffers
        uint8_t offset_index = packet->index - 0xA0;
        for (int i = 0; i < 3; i++) {
            if (packet->samples[i].distance & DISTANCE_DATA_INVALID_FLAG) {
                continue;
            }
            lidar_data_buffer.distances[4 * offset_index + i] = packet->samples[i].distance & DISTANCE_VALUE_MASK;
            lidar_data_buffer.intensities[4 * offset_index + i] = packet->samples[i].strength;
        }

        // Process point cloud upon RX completion
        if (offset_index == NUMBER_OF_ANGLES / 4 - 1) {
            refine_pose_from_point_cloud(lidar_data_buffer.distances, lidar_data_buffer.intensities);
            compute_obstacle_clusters(lidar_data_buffer.distances);
            send_to_clients((uint8_t*)&lidar_data_buffer, sizeof(lidar_data_buffer));
        }
    }
}

static bool is_packet_valid(struct lidar_data_packet *packet)
{
    uint8_t *raw_packet = (uint8_t*)packet;

    uint32_t checksum = 0;
    for (int i = 0; i < 20; i += 2) {
        checksum = (checksum << 1) + *(uint16_t *)&raw_packet[i];
    }
    checksum = (checksum + (checksum >> 15)) & 0x7FFF;
    return (checksum == packet->checksum) && (packet->index >= 0xA0) && (packet->index < 0xFA);
}