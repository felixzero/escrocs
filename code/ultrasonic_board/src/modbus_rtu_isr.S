#include <avr/io.h>

#define PORTB_IO 0x18

.global USART_TXC_vect, USART_RXC_vect, TIMER2_COMP_vect
.global rx_buffer, rx_buffer_index, rx_complete

.data

.align 8
rx_buffer: .space 128, 0
rx_buffer_index: .byte 0
rx_complete: .byte 0 

.text

USART_TXC_vect:
    cbi PORTB_IO, 2
    reti

USART_RXC_vect:
    push r0
    lds r0, 0x5F
    push r0
    push XL
    push XH

    ; X = rx_buffer + rx_buffer_index
    lds XL, rx_buffer_index
    ldi XH, hi8(rx_buffer)

    ; *X++ = UDR
    lds r0, UDR
    st X+, r0
    andi XL, 0x7F

    ; rx_buffer_index++
    sts rx_buffer_index, XL

    ; Reset end of transmission timer
    ldi XL, 0
    sts TCNT2, XL
    ldi XL, _BV(WGM21) | _BV(CS22)
    sts TCCR2, XL

    pop XH
    pop XL
    pop r0
    sts 0x5F, r0
    pop r0
    reti

TIMER2_COMP_vect:
    push r0
    lds r0, 0x5F
    push r0
    push r17

    ; Disable timer
    ldi r17, _BV(WGM21)
    sts TCCR2, r17

    ; rx_complete = true
    ldi r17, 1
    sts rx_complete, r17

    pop r17
    pop r0
    sts 0x5F, r0
    pop r0
    reti
