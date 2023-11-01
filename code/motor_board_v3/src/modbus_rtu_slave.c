#include "modbus_rtu_slave.h"
#include "crc_computation.h"
#include "com_registers.h"

#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define MODBUS_EXCEPTION_ILLEGAL_FUNCTION 0x01
#define MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS 0x02
#define MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE 0x03

#define UBRR_VALUE ((F_CPU / 16 / RS485_BAUD_RATE) - 1)
#define TIMEOUT_RATE (2 * RS485_BAUD_RATE / 11 / 7)
#define TIMEOUT_VALUE (F_CPU / 64 / TIMEOUT_RATE + 2)

extern volatile uint8_t rx_buffer[];
extern volatile uint8_t rx_buffer_index;
extern volatile bool rx_complete;
static uint8_t tx_buffer[256];
#define TX_BUFFER_DATA (&tx_buffer[2])

static void transmit_raw_rs485(const uint8_t *data, uint8_t len);
static uint8_t handle_read_coil_or_input_status(uint16_t starting_address, uint16_t number_to_read, uint8_t function_code);
static uint8_t handle_read_register(uint16_t starting_address, uint16_t number_to_read, uint8_t function_code);
static uint8_t handle_write_single_coil(uint16_t address, bool value, uint8_t rx_size);
static uint8_t handle_preset_single_register(uint16_t address, uint16_t value, uint8_t rx_size);
static uint8_t handle_force_multiple_coils(uint16_t starting_address, uint16_t number_to_write, uint8_t byte_count, uint8_t *coil_values);
static uint8_t handle_preset_multiple_registers(uint16_t starting_address, uint16_t number_to_write, uint8_t byte_count, uint8_t *values);
static uint8_t handle_read_device_identification(uint8_t object_id);
static uint8_t handle_exception_response(uint8_t exception_code);

void init_modbus_rtu_slave(void)
{
    cli();

    // Set TX enable pin as output
    DDRB |= _BV(2);

    // Initiate TIMER0 as end of transmission timer
    // CTC to OCRA, interrupt on OCRB
    // fosc = f / 64 = 250kHz
    TCCR0A = _BV(WGM01);
    TCCR0B = _BV(CS01) | _BV(CS00);
    TIMSK0 = _BV(OCIE0B);
    TCNT0 = 0;
    OCR0A = 1;
    OCR0B = TIMEOUT_VALUE;

    // Init UART
    UBRR1H = UBRR_VALUE >> 8;
    UBRR1L = UBRR_VALUE & 0xFF;
    // Enable RX, TX and their interrupts
    UCSR1B = _BV(RXEN1) | _BV(TXEN1) | _BV(RXCIE1) | _BV(TXCIE1);
    // Set frame format: 8 data + 2 stop bits
    UCSR1C = _BV(USBS1) | _BV(UCSZ10) | _BV(UCSZ11);

    sei();
}

void poll_modbus_rtu_operations(void)
{
    // No new UART frame
    if (!rx_complete) {
        return;
    }
    uint8_t rx_size = rx_buffer_index;
    rx_complete = false;
    rx_buffer_index = 0;

    // Ignore messages to other slaves or with bad CRC
    if (rx_buffer[0] != MODBUS_SLAVE_ADDR) {
        return;
    }
    if (compute_crc((uint8_t*)rx_buffer, rx_size) != 0) {
        return;
    }

    // Header, common to all function codes
    tx_buffer[0] = rx_buffer[0];
    tx_buffer[1] = rx_buffer[1];
    uint8_t tx_size = 2;

    // Handle according to the correct function code
    uint16_t address_in_message = rx_buffer[2] << 8 | rx_buffer[3];
    uint16_t size_or_value_in_message = rx_buffer[4] << 8 | rx_buffer[5];
    switch (rx_buffer[1]) {
    case 0x01:
        tx_size += handle_read_coil_or_input_status(address_in_message, size_or_value_in_message, 0x01);
        break;
    case 0x02:
        tx_size += handle_read_coil_or_input_status(address_in_message, size_or_value_in_message, 0x02);
        break;
    case 0x03:
        tx_size += handle_read_register(address_in_message, size_or_value_in_message, 0x03);
        break;
    case 0x04:
        tx_size += handle_read_register(address_in_message, size_or_value_in_message, 0x04);
        break;
    case 0x05:
        tx_size += handle_write_single_coil(address_in_message, size_or_value_in_message, rx_size);
        break;
    case 0x06:
        tx_size += handle_preset_single_register(address_in_message, size_or_value_in_message, rx_size);
        break;
    case 0x0F:
        tx_size += handle_force_multiple_coils(address_in_message, size_or_value_in_message, rx_buffer[6], (uint8_t*)rx_buffer + 7);
        break;
    case 0x10:
        tx_size += handle_preset_multiple_registers(address_in_message, size_or_value_in_message, rx_buffer[6], (uint8_t*)rx_buffer + 7);
        break;
    case 0x2B:
        if (rx_buffer[2] == 0x0E) {
            tx_size += handle_read_device_identification(rx_buffer[4]);
            break;
        }
    default:
        tx_size += handle_exception_response(MODBUS_EXCEPTION_ILLEGAL_FUNCTION);
    }

    // Footer: CRC value
    uint16_t crc = compute_crc(tx_buffer, tx_size);
    tx_buffer[tx_size] = crc >> 8;
    tx_buffer[tx_size + 1] = crc & 0xFF;
    tx_size += 2;

    transmit_raw_rs485(tx_buffer, tx_size);
}

static void transmit_raw_rs485(const uint8_t *data, uint8_t len)
{
    PORTB |= _BV(2);
    for (uint8_t i = 0; i < len; ++i) {
        // Wait until the TX buffer is empty
        while (!(UCSR1A & _BV(UDRE1)));
        UDR1 = data[i];
    }
}

static uint8_t handle_read_coil_or_input_status(uint16_t starting_address, uint16_t number_to_read, uint8_t function_code)
{
    if (number_to_read / 8 + 5 > 255) {
        return handle_exception_response(MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE);
    }
    uint8_t number_of_coil_bytes = number_to_read / 8 + (number_to_read % 8 != 0);
    TX_BUFFER_DATA[0] = number_of_coil_bytes;

    for (uint16_t i = 0; i < number_of_coil_bytes; ++i) {
        TX_BUFFER_DATA[1 + i] = 0;
    }
    for (uint16_t i = 0; i < number_to_read; ++i) {
        bool value;
        if (!read_com_coil(starting_address + i, &value)) {
            return handle_exception_response(MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS);
        }
        if (value) {
            TX_BUFFER_DATA[i / 8] |= _BV(i % 8);
        }
    }

    return 1 + number_of_coil_bytes;
}

static uint8_t handle_read_register(uint16_t starting_address, uint16_t number_to_read, uint8_t function_code)
{
    if (number_to_read * 2 + 5 > 255) {
        return handle_exception_response(MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE);
    }
   
    TX_BUFFER_DATA[0] = 2 * number_to_read;
    for (uint16_t i = 0; i < number_to_read; ++i) {
        uint16_t value;
        if (!read_com_register(starting_address + i, &value)) {
            return handle_exception_response(MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS);
        }
        TX_BUFFER_DATA[1 + 2 * i] = value >> 8;
        TX_BUFFER_DATA[2 + 2 * i] = value & 0xFF;
    }
    return 1 + 2 * number_to_read;
}

static uint8_t handle_write_single_coil(uint16_t address, bool value, uint8_t rx_size)
{
    if (!write_com_coil(address, value)) {
        return handle_exception_response(MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE);
    }
    memcpy(tx_buffer, (uint8_t*)rx_buffer, rx_size);
    return rx_size - 2;
}

static uint8_t handle_preset_single_register(uint16_t address, uint16_t value, uint8_t rx_size)
{
    if (!write_com_register(address, value)) {
        return handle_exception_response(MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS);
    }
    memcpy(tx_buffer, (uint8_t*)rx_buffer, rx_size);
    return rx_size - 2;
}

static uint8_t handle_force_multiple_coils(uint16_t starting_address, uint16_t number_to_write, uint8_t byte_count, uint8_t *coil_values)
{
    for (uint16_t i = 0; i < number_to_write; ++i) {
        if (!write_com_coil(starting_address + i, coil_values[i])) {
            return handle_exception_response(MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE);
        }
    }
    TX_BUFFER_DATA[0] = starting_address << 8;
    TX_BUFFER_DATA[1] = starting_address & 0xFF;
    TX_BUFFER_DATA[2] = number_to_write << 8;
    TX_BUFFER_DATA[3] = number_to_write & 0xFF;
    return 4;
}

static uint8_t handle_preset_multiple_registers(uint16_t starting_address, uint16_t number_to_write, uint8_t byte_count, uint8_t *values)
{
    for (uint16_t i = 0; i < number_to_write; ++i) {
        uint16_t value = values[2 * i] << 8 | values[2 * i + 1];
        if (!write_com_register(starting_address + i, value)) {
            return handle_exception_response(MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS);
        }
    }

    TX_BUFFER_DATA[0] = starting_address << 8;
    TX_BUFFER_DATA[1] = starting_address & 0xFF;
    TX_BUFFER_DATA[2] = number_to_write << 8;
    TX_BUFFER_DATA[3] = number_to_write & 0xFF;
    return 4;
}

static uint8_t handle_read_device_identification(uint8_t object_id)
{
    TX_BUFFER_DATA[0] = 0x0E;
    TX_BUFFER_DATA[1] = 0x04;
    TX_BUFFER_DATA[2] = 0x81;
    TX_BUFFER_DATA[3] = 0x00;
    TX_BUFFER_DATA[4] = 0x00;
    TX_BUFFER_DATA[5] = 1;
    TX_BUFFER_DATA[6] = object_id;

    const char *identification_to_send = NULL;
    switch (object_id) {
    case 0x00:
        identification_to_send = vendor_name;
        break;
    case 0x01:
        identification_to_send = product_code;
        break;
    case 0x02:
        identification_to_send = revision;
        break;
    default:
        identification_to_send = vendor_name;
    }

    TX_BUFFER_DATA[7] = strlen(identification_to_send);
    strcpy((char*)&TX_BUFFER_DATA[8], identification_to_send);

    return 8 + TX_BUFFER_DATA[7];
}

static uint8_t handle_exception_response(uint8_t exception_code)
{
    tx_buffer[1] |= 0x80; // Set high bit of function code to signal exception
    TX_BUFFER_DATA[0] = exception_code;
    return 1;
}
