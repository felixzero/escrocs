#include "modbus_rtu_master.h"
#include "modbus_rtu_crc_table.h"
#include "../system/task_priority.h"

#include <freertos/FreeRTOS.h>
#include <string.h>
#include <driver/uart.h>
#include <esp_err.h>
#include <esp_log.h>

#define TAG "Modbus RTU"

#define UART_NUM UART_NUM_1
#define UART_BAUD_RATE 1000000
#define BUFFER_SIZE 1024
#define RECEIVE_TIMEOUT_TICKS 1
#define ECHO_READ_TOUT 3
#define MAX_QUERY_SIZE 64
#define MAX_RESPONSE_SIZE 64

#define TX_PIN 22
#define RX_PIN 21
#define TXE_PIN 4

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
static esp_err_t query_modbus_rtu(size_t message_len, uint8_t slave_addr, uint8_t function_code);
static void send_raw_rs485(const void *buffer, size_t len);
static size_t receive_raw_rs485(void *buffer, size_t expected_len);

static uint8_t query_buffer[MAX_QUERY_SIZE], response_buffer[MAX_RESPONSE_SIZE];
#define QUERY_BUFFER_DATA (&query_buffer[2])
#define RESPONSE_BUFFER_DATA (&response_buffer[2])

static SemaphoreHandle_t modbus_mutex;

static void install_uart_task(void *parameters)
{
    // Configure UART to RS485 transceiver
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_2,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT
    };
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM, 2 * BUFFER_SIZE, 0, 0, NULL, ESP_INTR_FLAG_LEVEL3 | ESP_INTR_FLAG_IRAM));
    ESP_ERROR_CHECK(uart_set_mode(UART_NUM, UART_MODE_RS485_HALF_DUPLEX));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM, TX_PIN, RX_PIN, TXE_PIN, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_set_rx_timeout(UART_NUM, ECHO_READ_TOUT));
    uart_set_always_rx_timeout(UART_NUM, true);

    modbus_mutex = xSemaphoreCreateMutex();
    if (!modbus_mutex) {
        ESP_LOGE(TAG, "Could not create mutex");
    }

    TaskHandle_t parent_task = (TaskHandle_t)parameters;
    xTaskNotifyGive(parent_task);
    vTaskDelete(NULL);
}

void init_modbus_rtu_master(void)
{
    // Install UART on the time critical core
    TaskHandle_t task;
    TaskHandle_t current_task = xTaskGetCurrentTaskHandle();
    xTaskCreatePinnedToCore(install_uart_task, "install_uart", TASK_STACK_SIZE, current_task, 1, &task, TIME_CRITICAL_CORE);
    ulTaskNotifyTake(true, portMAX_DELAY);
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
    QUERY_BUFFER_DATA[0] = address >> 8;
    QUERY_BUFFER_DATA[1] = address & 0xFF;
    QUERY_BUFFER_DATA[2] = value ? 0xFF : 0x00;
    QUERY_BUFFER_DATA[3] = 0;

    return query_modbus_rtu(4, slave_addr, 0x05);
}

esp_err_t modbus_preset_single_register(uint8_t slave_addr, uint16_t address, uint16_t value)
{
    QUERY_BUFFER_DATA[0] = address >> 8;
    QUERY_BUFFER_DATA[1] = address & 0xFF;
    QUERY_BUFFER_DATA[2] = value >> 8;
    QUERY_BUFFER_DATA[3] = value & 0xFF;

    return query_modbus_rtu(4, slave_addr, 0x06);
}

esp_err_t modbus_force_multiple_coils(uint8_t slave_addr, uint16_t starting_coil_address, uint16_t number_of_coils_to_write, bool *input)
{
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

    return query_modbus_rtu(5 + byte_count, slave_addr, 0x0F);
}

esp_err_t modbus_preset_multiple_registers(uint8_t slave_addr, uint16_t starting_address, uint16_t number_of_registers_to_write, uint16_t *input)
{
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

    return query_modbus_rtu(5 + byte_count, slave_addr, 0x10);
}

esp_err_t modbus_read_device_identification(uint8_t slave_addr, uint8_t object_id, char *identification, size_t max_length)
{
    QUERY_BUFFER_DATA[0] = 0x0E; // MEI type for device identification, read Modbus application protocol 1.1b3, p43
    QUERY_BUFFER_DATA[1] = 0x04; // One specific object
    QUERY_BUFFER_DATA[2] = object_id;

    esp_err_t err = query_modbus_rtu(3, slave_addr, 0x2B);
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
    QUERY_BUFFER_DATA[0] = starting_address >> 8;
    QUERY_BUFFER_DATA[1] = starting_address & 0xFF;
    QUERY_BUFFER_DATA[2] = number_to_read >> 8;
    QUERY_BUFFER_DATA[3] = number_to_read & 0xFF;

    esp_err_t err = query_modbus_rtu(4, slave_addr, function_code);
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
    QUERY_BUFFER_DATA[0] = starting_address >> 8;
    QUERY_BUFFER_DATA[1] = starting_address & 0xFF;
    QUERY_BUFFER_DATA[2] = number_of_registers_to_read >> 8;
    QUERY_BUFFER_DATA[3] = number_of_registers_to_read & 0xFF;

    esp_err_t err = query_modbus_rtu(4, slave_addr, function_code);
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

static esp_err_t query_modbus_rtu(size_t message_len, uint8_t slave_addr, uint8_t function_code)
{
    query_buffer[0] = slave_addr;
    query_buffer[1] = function_code;
    uint16_t crc = compute_crc(query_buffer, message_len + 2);
    query_buffer[message_len + 2] = crc >> 8;
    query_buffer[message_len + 3] = crc & 0xFF;

    xSemaphoreTake(modbus_mutex, portMAX_DELAY);
    send_raw_rs485(query_buffer, message_len + 4);
    size_t received_len = receive_raw_rs485(response_buffer, MAX_RESPONSE_SIZE);
    xSemaphoreGive(modbus_mutex);

    if (received_len <= 0) {
        return ESP_ERR_TIMEOUT;
    }

    uint16_t checked_crc = compute_crc(response_buffer, received_len);
    if (checked_crc != 0) {
        return ESP_ERR_INVALID_CRC;
    }
    if (response_buffer[1] == (function_code | 0x80)) {
        return ESP_ERR_INVALID_ARG;
    }
    if ((response_buffer[0] != slave_addr) || (response_buffer[1] != function_code)) {
        return ESP_ERR_INVALID_RESPONSE;
    }

    return ESP_OK;
}

static void send_raw_rs485(const void *buffer, size_t len)
{
    ESP_ERROR_CHECK_WITHOUT_ABORT(uart_flush(UART_NUM));
    uart_write_bytes(UART_NUM, buffer, len);
    ESP_ERROR_CHECK_WITHOUT_ABORT(uart_wait_tx_done(UART_NUM, RECEIVE_TIMEOUT_TICKS));
}

static size_t receive_raw_rs485(void *buffer, size_t expected_len)
{
    return uart_read_bytes(UART_NUM, buffer, expected_len, RECEIVE_TIMEOUT_TICKS);
}
