#include "modbus.h"

#include <avr/io.h>

#define USART_BAUDRATE 19200
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

void init_modbus(void)
{
    // Set PB as half duplex flow control port
    DDRB |= _BV(2);
    DDRD |= _BV(1);

    // Enable UART RX and TX
    UCSRB = _BV(RXEN) | _BV(TXEN);
    // Even parity bit, 1 stop bit, 8 bits words
    UCSRC = _BV(URSEL) | _BV(UPM1) | _BV(UCSZ1) | _BV(UCSZ2);
    // Set baud rate
    UBRRH = (BAUD_PRESCALE >> 8);
    UBRRL = BAUD_PRESCALE;
}

void send_character(char c)
{
    while (!(UCSRA & _BV(UDRE)));
    PORTB |= _BV(2);
    UDR = c;
}

char poll_character(void)
{
    if (!(UCSRA & _BV(RXC))) {
        return 0;
    }

    return UDR;
}
