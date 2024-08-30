
#include "soft_serial.h"
#include <avr/io.h>
#include <util/delay.h>

//set PD5 as output and low
void init_soft_serial(void) {
    DDRD |= _BV(5);
	PORTD &= ~_BV(5);
}

//Help from https://www.avrfreaks.net/s/topic/a5C3l000000UJx3EAG/t090487
void send_serial(const char data[]) {
	uint16_t cur_index = 0;
    while(*data != '\0') {
		PORTD &= ~_BV(5);
		_delay_us(UART_SOFT_DELAY_US);
		char cur_data = data[cur_index++];
		for (char i=0; i< 8; i++)
		{
			if( cur_data & 1 )
				PORTD |= (1 << PD5);
			else
				PORTD &= ~(1 << PD5);
			cur_data >>= 1;
			_delay_us(UART_SOFT_DELAY_US);
		}
		PORTD |= (1 << PD5);
		_delay_us(UART_SOFT_DELAY_US);
    }
}
