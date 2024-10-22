#include "modbus_rtu_master.h"
#include "modbus_rtu_crc_table.h"
#include "task_priority.h"

#include <freertos/FreeRTOS.h>
#include <string.h>
#include <driver/uart.h>
#include <hal/uart_hal.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_timer.h>

#define TAG "Modbus RTU"

#define UART_NUM                UART_NUM_1
#define UART_BAUD_RATE          500000
#define BUFFER_SIZE             1024
#define RECEIVE_TIMEOUT_TICKS   1
#define RECEIVE_TIMEOUT_US      1000
#define RECEIVE_POLL_US         500
#define ECHO_READ_TOUT          3
#define MAX_QUERY_SIZE          64
#define MAX_RESPONSE_SIZE       64

#define TX_PIN                  22
#define RX_PIN                  21
#define TXE_PIN                 4

static esp_err_t read_coil_or_input_status(
    uint8_t slave_addr,
    uint16_t starting_address,
    uint16_t number_to_read,
    bool *output,
    uint8_t function_code
);
static esp_err_t read_registers(
    uint8_t slave_addr,
    uint16_t starting_address,
    uint16_t number_of_registers_to_read,
    uint16_t *output,
    uint8_t function_code
);
static esp_err_t query_modbus_rtu(size_t message_len, size_t expected_len, uint8_t slave_addr, uint8_t function_code, uint8_t *query_buffer, uint8_t *response_buffer);
static void send_raw_i2c(const void *buffer, size_t len);
static size_t receive_raw_i2c(void *buffer, size_t expected_len);
static void send_raw_rs485(const void *buffer, size_t len);
static size_t receive_raw_rs485(void *buffer, size_t expected_len);

#define QUERY_BUFFER_DATA (&query_buffer[2])
#define RESPONSE_BUFFER_DATA (&response_buffer[2])

static SemaphoreHandle_t modbus_mutex;
static uint8_t current_i2c_address_for_modbus = 0;
static uart_hal_context_t hal_context;
static spinlock_t spinlock = portMUX_INITIALIZER_UNLOCKED;

static void IRAM_ATTR uart_intr_handle(void *arg);

esp_err_t init_modbus_rtu_master(void)
{
    esp_err_t err = ESP_OK;
    ESP_LOGI(TAG, "Initializing Modbus subsystem");

    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_2,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    err = uart_param_config(UART_NUM, &uart_config);
    if (err) {
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        return err;
    }

    err = uart_set_pin(UART_NUM, TX_PIN, RX_PIN, TXE_PIN, UART_PIN_NO_CHANGE);
    if (err) {
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        return err;
    }

    // Unregister software UART ISR and register ours
    hal_context.dev = UART_LL_GET_HW(UART_NUM);

    uart_isr_handle_t isr_handle;
    //err = esp_intr_alloc(ETS_UART1_INTR_SOURCE, (void*)NULL, uart_intr_handle, NULL, &isr_handle);
    if (err) {
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        return err;
    }

    uart_intr_config_t intr_config = {
        .intr_enable_mask = UART_TX_DONE_INT_ENA_M
    };
    err = uart_intr_config(UART_NUM, &intr_config);
    if (err) {
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        return err;
    }

    modbus_mutex = xSemaphoreCreateMutex();
    if (!modbus_mutex) {
        ESP_LOGE(TAG, "Could not create mutex");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t modbus_read_coil_status(uint8_t slave_addr, uint16_t starting_coil_address, uint16_t number_of_coils_to_read, bool *output)
{
    return read_coil_or_input_status(slave_addr, starting_coil_address, number_of_coils_to_read, output, 0x01);
}

esp_err_t modbus_read_input_status(uint8_t slave_addr, uint16_t starting_input_address, uint16_t number_of_inputs_to_read, bool *output)
{
    return read_coil_or_input_status(slave_addr, starting_input_address, number_of_inputs_to_read, output, 0x02);
}

esp_err_t modbus_read_holding_registers(uint8_t slave_addr, uint16_t starting_address, uint16_t number_of_registers_to_read, uint16_t *output)
{
    return read_registers(slave_addr, starting_address, number_of_registers_to_read, output, 0x03);
}

esp_err_t modbus_read_input_registers(uint8_t slave_addr, uint16_t starting_address, uint16_t number_of_registers_to_read, uint16_t *output)
{
    return read_registers(slave_addr, starting_address, number_of_registers_to_read, output, 0x04);
}

esp_err_t modbus_force_single_coil(uint8_t slave_addr, uint16_t address, bool value)
{
    uint8_t query_buffer[MAX_QUERY_SIZE], response_buffer[MAX_RESPONSE_SIZE];

    QUERY_BUFFER_DATA[0] = address >> 8;
    QUERY_BUFFER_DATA[1] = address & 0xFF;
    QUERY_BUFFER_DATA[2] = value ? 0xFF : 0x00;
    QUERY_BUFFER_DATA[3] = 0;

    return query_modbus_rtu(4, 8, slave_addr, 0x05, query_buffer, response_buffer);
}

esp_err_t modbus_preset_single_register(uint8_t slave_addr, uint16_t address, uint16_t value)
{
    uint8_t query_buffer[MAX_QUERY_SIZE], response_buffer[MAX_RESPONSE_SIZE];

    QUERY_BUFFER_DATA[0] = address >> 8;
    QUERY_BUFFER_DATA[1] = address & 0xFF;
    QUERY_BUFFER_DATA[2] = value >> 8;
    QUERY_BUFFER_DATA[3] = value & 0xFF;

    return query_modbus_rtu(4, 6, slave_addr, 0x06, query_buffer, response_buffer);
}

esp_err_t modbus_force_multiple_coils(uint8_t slave_addr, uint16_t starting_coil_address, uint16_t number_of_coils_to_write, bool *input)
{
    uint8_t query_buffer[MAX_QUERY_SIZE], response_buffer[MAX_RESPONSE_SIZE];

    QUERY_BUFFER_DATA[0] = starting_coil_address >> 8;
    QUERY_BUFFER_DATA[1] = starting_coil_address & 0xFF;
    QUERY_BUFFER_DATA[2] = number_of_coils_to_write >> 8;
    QUERY_BUFFER_DATA[3] = number_of_coils_to_write & 0xFF;
    uint8_t byte_count = number_of_coils_to_write / 8 + (number_of_coils_to_write % 8 != 0);
    QUERY_BUFFER_DATA[4] = byte_count;
    for (int i = 0; i < byte_count; ++i) {
        QUERY_BUFFER_DATA[5 + i] = 0;
    }
    for (int i = 0; i < number_of_coils_to_write; ++i) {
        if (input[i]) {
            QUERY_BUFFER_DATA[5 + i / 8] |= (1 << (i % 8));
        }
    }

    return query_modbus_rtu(5 + byte_count, 8, slave_addr, 0x0F, query_buffer, response_buffer);
}

esp_err_t modbus_preset_multiple_registers(uint8_t slave_addr, uint16_t starting_address, uint16_t number_of_registers_to_write, uint16_t *input)
{
    uint8_t query_buffer[MAX_QUERY_SIZE], response_buffer[MAX_RESPONSE_SIZE];

    QUERY_BUFFER_DATA[0] = starting_address >> 8;
    QUERY_BUFFER_DATA[1] = starting_address & 0xFF;
    QUERY_BUFFER_DATA[2] = number_of_registers_to_write >> 8;
    QUERY_BUFFER_DATA[3] = number_of_registers_to_write & 0xFF;
    uint8_t byte_count = 2 * number_of_registers_to_write;
    QUERY_BUFFER_DATA[4] = byte_count;
    for (int i = 0; i < number_of_registers_to_write; ++i) {
        QUERY_BUFFER_DATA[5 + 2 * i] = input[i] >> 8;
        QUERY_BUFFER_DATA[6 + 2 * i] = input[i] & 0xFF;
    }

    return query_modbus_rtu(5 + byte_count, 8, slave_addr, 0x10, query_buffer, response_buffer);
}

esp_err_t modbus_read_device_identification(uint8_t slave_addr, uint8_t object_id, char *identification, size_t max_length)
{
    uint8_t query_buffer[MAX_QUERY_SIZE], response_buffer[MAX_RESPONSE_SIZE];

    QUERY_BUFFER_DATA[0] = 0x0E; // MEI type for device identification, read Modbus application protocol 1.1b3, p43
    QUERY_BUFFER_DATA[1] = 0x04; // One specific object
    QUERY_BUFFER_DATA[2] = object_id;

    esp_err_t err = query_modbus_rtu(3, 16, slave_addr, 0x2B, query_buffer, response_buffer);
    if (err != ESP_OK) {
        return err;
    }

    if (RESPONSE_BUFFER_DATA[6] != object_id) {
        return ESP_ERR_INVALID_RESPONSE;
    }

    size_t string_length = RESPONSE_BUFFER_DATA[7];
    char *message_string = (char*)&RESPONSE_BUFFER_DATA[8];
    if (string_length > max_length - 1) {
        string_length = max_length - 1;
    }
    strncpy(identification, message_string, string_length);
    identification[string_length] = 0;

    return ESP_OK;
}

static esp_err_t read_coil_or_input_status(
    uint8_t slave_addr,
    uint16_t starting_address,
    uint16_t number_to_read,
    bool *output,
    uint8_t function_code
)
{
    uint8_t query_buffer[MAX_QUERY_SIZE], response_buffer[MAX_RESPONSE_SIZE];

    QUERY_BUFFER_DATA[0] = starting_address >> 8;
    QUERY_BUFFER_DATA[1] = starting_address & 0xFF;
    QUERY_BUFFER_DATA[2] = number_to_read >> 8;
    QUERY_BUFFER_DATA[3] = number_to_read & 0xFF;

    uint8_t number_of_coil_bytes = number_to_read / 8 + (number_to_read % 8 != 0);
    esp_err_t err = query_modbus_rtu(4, 5 + number_of_coil_bytes, slave_addr, function_code, query_buffer, response_buffer);
    if (err != ESP_OK) {
        return err;
    }

    uint8_t byte_count = RESPONSE_BUFFER_DATA[0];
    if (8 * byte_count < number_to_read) {
        return ESP_ERR_INVALID_SIZE;
    }

    for (size_t i = 0; i < number_to_read; ++i) {
        output[i] = RESPONSE_BUFFER_DATA[1 + i / 8] & (1 << (i % 8));
    }
    return ESP_OK;
}

static esp_err_t read_registers(
    uint8_t slave_addr,
    uint16_t starting_address,
    uint16_t number_of_registers_to_read,
    uint16_t *output,
    uint8_t function_code
)
{
    uint8_t query_buffer[MAX_QUERY_SIZE], response_buffer[MAX_RESPONSE_SIZE];

    QUERY_BUFFER_DATA[0] = starting_address >> 8;
    QUERY_BUFFER_DATA[1] = starting_address & 0xFF;
    QUERY_BUFFER_DATA[2] = number_of_registers_to_read >> 8;
    QUERY_BUFFER_DATA[3] = number_of_registers_to_read & 0xFF;

    esp_err_t err = query_modbus_rtu(4, 5 + 2 * number_of_registers_to_read, slave_addr, function_code, query_buffer, response_buffer);
    if (err != ESP_OK) {
        return err;
    }

    uint8_t byte_count = RESPONSE_BUFFER_DATA[0];
    if (byte_count < 2 * number_of_registers_to_read) {
        return ESP_ERR_INVALID_SIZE;
    }

    for (size_t i = 0; i < number_of_registers_to_read; ++i) {
        output[i] = (RESPONSE_BUFFER_DATA[1 + 2 * i] << 8) | (RESPONSE_BUFFER_DATA[2 + 2 * i] & 0xFF);
    }
    return ESP_OK;
}

static esp_err_t query_modbus_rtu(size_t message_len, size_t expected_len, uint8_t slave_addr, uint8_t function_code, uint8_t *query_buffer, uint8_t *response_buffer)
{
    query_buffer[0] = slave_addr;
    query_buffer[1] = function_code;
    uint16_t crc = compute_crc(query_buffer, message_len + 2);
    query_buffer[message_len + 2] = crc >> 8;
    query_buffer[message_len + 3] = crc & 0xFF;

    esp_err_t err = ESP_OK;
    for (int i = 0; i < 5; ++i) {
        // FIXME: Ugly hack
        size_t received_len;
        if (slave_addr == 0x44) { // if motor
            send_raw_rs485(query_buffer, message_len + 4);
            received_len = receive_raw_rs485(response_buffer, expected_len);
        } else { // us
            current_i2c_address_for_modbus = 0x12;
            send_raw_i2c(query_buffer, message_len + 4);
            received_len = receive_raw_i2c(response_buffer, expected_len);
        }

        if (received_len <= 0) {
            err = ESP_ERR_TIMEOUT;
            continue;
        }

        uint16_t checked_crc = compute_crc(response_buffer, received_len);
        if (checked_crc != 0) {
            err = ESP_ERR_INVALID_CRC;
            continue;
        }
        if (response_buffer[1] == (function_code | 0x80)) {
            err = ESP_ERR_INVALID_ARG;
            continue;
        }
        if ((response_buffer[0] != slave_addr) || (response_buffer[1] != function_code)) {
            err = ESP_ERR_INVALID_RESPONSE;
            continue;
        }

        return ESP_OK;
    }

    return err;
}

static void send_raw_rs485(const void *buffer, size_t len)
{
    uint32_t write_size = 0;

    uart_hal_rxfifo_rst(&hal_context);
    uart_hal_set_rts(&hal_context, 0);
    uart_hal_write_txfifo(&hal_context, buffer, len, &write_size);
}

static size_t receive_raw_rs485(void *buffer, size_t expected_len)
{
    int rx_buffer_length = 0, length = 0;
    int64_t start_listening_time = esp_timer_get_time();

    while ((rx_buffer_length == 0) || (length != 0)) {
        length = 0;
        uart_hal_read_rxfifo(&hal_context, buffer + rx_buffer_length, &length);
        rx_buffer_length += length;
        int64_t initial_wait_time = esp_timer_get_time();

        if (esp_timer_get_time() > start_listening_time + RECEIVE_TIMEOUT_US) {
            break;
        }
        while (esp_timer_get_time() <= initial_wait_time + ECHO_READ_TOUT * 11);
    }

    uart_hal_rxfifo_rst(&hal_context);
    return rx_buffer_length;
}

static void IRAM_ATTR uart_intr_handle(void *arg)
{
    uint32_t uart_intr_status = uart_hal_get_intsts_mask(&hal_context);

    if (uart_intr_status & UART_INTR_TX_DONE) {
        portENTER_CRITICAL_ISR(&spinlock);
        uart_hal_set_rts(&hal_context, 1);
        uart_hal_clr_intsts_mask(&hal_context, UART_INTR_TX_DONE);
        portEXIT_CRITICAL_ISR(&spinlock);
        return;
    }

    uart_hal_clr_intsts_mask(&hal_context, UART_LL_INTR_MASK);
}
