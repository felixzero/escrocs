#include <avr/io.h>

#define PORTB_IO 0x05
#define TCNT0_IO 0x26

.global USART1_TX_vect, USART1_RX_vect, TIMER0_COMPB_vect
.global rx_buffer, rx_buffer_index, rx_complete

.data

.align 8
rx_buffer: .space 256, 0
rx_buffer_index: .byte 0
rx_complete: .byte 0 

.text

USART1_TX_vect:
    cbi PORTB_IO, 2
    reti

USART1_RX_vect:
    push r0
    lds r0, 0x5F
    push r0
    push XL
    push XH

    ; X = rx_buffer + rx_buffer_index
    lds XL, rx_buffer_index
    ldi XH, hi8(rx_buffer)

    ; *X++ = UDR1
    lds r0, UDR1
    st X+, r0

    ; rx_buffer_index++
    sts rx_buffer_index, XL

    ; Reset end of transmission timer
    ldi XL, 2
    out TCNT0_IO, XL

    pop XH
    pop XL
    pop r0
    sts 0x5F, r0
    pop r0
    reti

TIMER0_COMPB_vect:
    push r0
    lds r0, 0x5F
    push r0
    push r17

    ; rx_complete = true
    ldi r17, 1
    sts rx_complete, r17

    pop r17
    pop r0
    sts 0x5F, r0
    pop r0
    reti
