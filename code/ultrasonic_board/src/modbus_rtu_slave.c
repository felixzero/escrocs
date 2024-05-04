#include "modbus_rtu_slave.h"
#include "modbus_slave.h"
#include "com_registers.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#define UBRR_VALUE ((F_CPU / 16 / RS485_BAUD_RATE) - 1)
#define TIMEOUT_RATE (2 * RS485_BAUD_RATE / 11 / 7)
#define TIMEOUT_VALUE (F_CPU / 64 / TIMEOUT_RATE)

extern volatile uint8_t rx_buffer[];
extern volatile uint8_t rx_buffer_index;
extern volatile bool rx_complete;
static uint8_t tx_buffer[8];

static void transmit_raw_rs485(const uint8_t *data, uint8_t len);

void init_modbus_rtu_slave(void)
{
    cli();

    // Set TX enable pin as output
    DDRB |= _BV(2);
    PORTB &= ~_BV(2);

    // Initiate TIMER2 as end of transmission timer
    // CTC to OCRA, interrupt on overflow
    // fosc = f / 64
    // Timer is disabled at start
    TCCR2 = _BV(WGM21);
    TIMSK |= _BV(OCIE2);
    TCNT2 = 0;
    OCR2 = TIMEOUT_VALUE;

    // Init UART
    UBRRH = UBRR_VALUE >> 8;
    UBRRL = UBRR_VALUE & 0xFF;
    // Enable RX, TX and their interrupts
    UCSRB = _BV(RXEN) | _BV(TXEN) | _BV(RXCIE) | _BV(TXCIE);
    // Set frame format: 8 data + 2 stop bits
    UCSRC = _BV(URSEL) | _BV(USBS) | _BV(UCSZ0) | _BV(UCSZ1);

    sei();
}

#include <util/delay.h>

void poll_modbus_rtu_operations(void)
{
    // No new UART frame
    if (!rx_complete) {
        return;
    }
    uint8_t rx_size = rx_buffer_index;
    rx_complete = false;
    rx_buffer_index = 0;

    uint8_t tx_size = handle_modbus_request(MODBUS_SLAVE_ADDR, (uint8_t*)rx_buffer, tx_buffer, rx_size);
    if (tx_size > 0) {
        transmit_raw_rs485(tx_buffer, tx_size);
    }
}

static void transmit_raw_rs485(const uint8_t *data, uint8_t len)
{
    PORTB |= _BV(2);
    for (uint8_t i = 0; i < len; ++i) {
        // Wait until the TX buffer is empty
        while (!(UCSRA & _BV(UDRE)));
        UDR = data[i];
    }
}
