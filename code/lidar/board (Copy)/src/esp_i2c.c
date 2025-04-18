#include "esp_i2c.h"
#include "lidar.h"
#include "collision_handler.h"

#include "driver/i2c_slave.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "string.h"
#include <inttypes.h>

#define QUEUE_STRING(queue, fmt, ...) do {                            \
    char *stringToQueue = (char *)pvPortMalloc(32 * sizeof(char));    \
    snprintf(stringToQueue, 32, fmt, __VA_ARGS__);                    \
    xQueueSend((queue), &stringToQueue, portMAX_DELAY);               \
} while(0)

// Define the I2C port number
#define I2C_PORT_NUM I2C_NUM_0

// Define the I2C pins
#define I2C_SLAVE_SDA_IO 3
#define I2C_SLAVE_SCL_IO 4

static i2c_slave_dev_handle_t slave_handle;
static QueueHandle_t i2c_receive_queue, log_queue;
i2c_slave_rx_done_event_data_t rx_data;

typedef struct {
    uint8_t data[I2C_BUFFER_SIZE];
    size_t length;
    bool is_requested; //if false, it is a received
} i2c_context_t;

static i2c_context_t context = {
    .length = 0,
};

static IRAM_ATTR bool i2c_slave_rx_done_callback(i2c_slave_dev_handle_t channel, const i2c_slave_rx_done_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_wakeup = pdFALSE;
    QueueHandle_t receive_queue = (QueueHandle_t)user_data;
    xQueueSendFromISR(receive_queue, edata, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

// Initialize the I2C slave
void i2c_slave_task(void *pvParameters) {
    uint8_t *send_data_buffer = (uint8_t *) malloc(I2C_BUFFER_SIZE);
    uint8_t *receive_data_buffer = (uint8_t *) malloc(I2C_BUFFER_SIZE);
    uint8_t send_len = 0;
    i2c_slave_config_t i2c_slv_config = {
        .addr_bit_len = I2C_ADDR_BIT_LEN_7,
        .i2c_port = I2C_PORT_NUM,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .slave_addr = I2C_SLAVE_ADDRESS,
        .send_buf_depth = I2C_BUFFER_SIZE,
        .receive_buf_depth = I2C_BUFFER_SIZE,
        .flags.enable_internal_pullup = true,
    };

    QueueHandle_t i2c_receive_queue = xQueueCreate(1, sizeof(i2c_slave_rx_done_event_data_t));
    i2c_slave_dev_handle_t slave_handle;
    ESP_ERROR_CHECK(i2c_new_slave_device(&i2c_slv_config, &slave_handle));
    i2c_slave_event_callbacks_t cbs = {
        .on_recv_done = i2c_slave_rx_done_callback,
    };
    ESP_ERROR_CHECK(i2c_slave_register_event_callbacks(slave_handle, &cbs, i2c_receive_queue));
    
    i2c_slave_rx_done_event_data_t rx_data;
    ESP_ERROR_CHECK(i2c_slave_receive(slave_handle, receive_data_buffer, I2C_BUFFER_SIZE));
    ESP_LOGI("I2C", "init done");

    while (true) {
        if (xQueueReceive(i2c_receive_queue, &rx_data, pdMS_TO_TICKS(10)) == pdTRUE) {
            if(rx_data.length > 0) {
                switch (rx_data.buffer[0]) {
                    case I2C_REG_IS_OK:
                        send_data_buffer[0] = is_lidar_running() ? 5 : 4;
                        send_len = 1;
                        break;
                    case I2C_REG_CONE:
                        float unmap_center_angle = (rx_data.buffer[1] / 256.0 * 2 * M_PI) - M_PI;
                        float unmap_half_cone_width = rx_data.buffer[2] / 256.0 * M_PI;
        
                        update_cone(unmap_center_angle, unmap_half_cone_width);
                        QUEUE_STRING(log_queue, "new cone : %.2e %.2e", unmap_center_angle, unmap_half_cone_width);
                        break;
                    case I2C_REG_BOOL_OBSTACLE:
                        send_data_buffer[0] = (uint8_t) has_obstacle() + 4;
                        send_len = 1;
                        QUEUE_STRING(log_queue, "BOOL_OBS : %i", data_buffer[0]);
                        break;
                    case I2C_REG_MM_OBSTACLE:
                        uint16_t dist = closest_obstacle_dist();
                        send_data_buffer[0] = (uint8_t) dist;
                        send_data_buffer[1] = (uint8_t) (dist >> 8);
                        send_len = 2;
                        QUEUE_STRING(log_queue, "MM_OBS : %i, %i", data_buffer[0], data_buffer[1]);
                        break;
                    case I2C_REG_BUTTON5:
                        send_data_buffer[0] = (uint8_t) gpio_get_level(GPIO_NUM_5) + 4;
                        send_len = 1;
                        break;
                    case I2C_REG_BUTTON7:
                        send_data_buffer[0] = (uint8_t) gpio_get_level(GPIO_NUM_7) + 4;
                        send_len = 1;
                        break;
                    default:
                        ESP_LOGE("I2C", "Invalid register %"PRIu8, cur_ctxt.data[0]);
                        break;
                }
            }
        }
    }
    vTaskDelete(NULL);
}

void printer_log_task(void *pvParameters) {
    char *log;
    for(;;) {
        while (uxQueueMessagesWaiting(log_queue) > 0) {
            if (xQueueReceive(log_queue, &log, portMAX_DELAY)) {
                //ESP_LOGI("PRINTER", "%s", log);
                vPortFree(log);
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}