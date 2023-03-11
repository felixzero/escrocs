#include "global_definitions.h"
#include <avr/io.h>

.section .bss

.global led_strip_payload

.comm led_strip_payload, (NUMBER_OF_LED * 3) 

.text

.global led_strip_bit_banging

#define port_high r18
#define port_low r19
#define current_byte r20
#define bit_value_1 r21
#define bit_value_2 r22
#define loop_counter r24

; LED protocol follows this specification:
; Bit period should be 1.25 us +/- 0.60 (i.e. between 6 and 14 CPU cycles at 8 MHz)
; Period is composed of high level then low level
; For bit 0, high level time is 0.3 us +/- 0.15 (i.e. between 2 and 3 CPU cycles at 8 MHz)
; For bit 1, high level time is 0.6 us +/- 0.15 (i.e. between 4 and 6 CPU cycles at 8 MHz)

.macro double_nop
    rjmp .+0
.endm

.macro send_high_time current_bit_value, next_bit_value
    out _SFR_IO_ADDR(LED_PORT), port_high
    mov \next_bit_value, port_low
    ; End of high level time for bit 0
    out _SFR_IO_ADDR(LED_PORT), \current_bit_value
.endm

.macro send_low_time next_bit, next_bit_value
    sbrc current_byte, \next_bit
    mov \next_bit_value, port_high
    ; End of high level time for bit 1
    out _SFR_IO_ADDR(LED_PORT), port_low
.endm

.macro send_bit next_bit, current_bit_value, next_bit_value
    ; Start of high level time (cycles = 0)
    out _SFR_IO_ADDR(LED_PORT), port_high
    mov \next_bit_value, port_low
    ; End of high level time for bit 0
    out _SFR_IO_ADDR(LED_PORT), \current_bit_value
    double_nop
    sbrc current_byte, \next_bit
    mov \next_bit_value, port_high
    ; End of high level time for bit 1
    out _SFR_IO_ADDR(LED_PORT), port_low
.endm

led_strip_bit_banging:
    ; port_high = LED_PORT value when pin is high
    in port_high, _SFR_IO_ADDR(LED_PORT)
    sbr port_high, LED_PIN
    ; port_low = LED_PORT value when pin is low
    in port_low, _SFR_IO_ADDR(LED_PORT)
    cbr port_low, LED_PIN

    ; Initial setup: first bit is initialized outside the loop
    ; X (r26:r27) contains the pointer to the payload
    ldi r26, lo8(led_strip_payload)
    ldi r27, hi8(led_strip_payload)
    ld current_byte, X+

    mov bit_value_1, port_low
    sbrc current_byte, 7
    mov bit_value_1, port_high
    ldi loop_counter, (NUMBER_OF_LED * 3)

loop_over_bytes:
    ; Send out current byte
    ; Each bit is send in two times:
    ; the "high time", that sets the output to high level, then to correct level
    ; a 2 cycles delay
    ; the "low time", that sets the output to the low level, and low next bit
    ; another 2 cycles delay
    ; loop control instructions are fit in place of delays

    send_high_time bit_value_1, bit_value_2
    double_nop
    send_low_time 6, bit_value_2
    double_nop

    send_high_time bit_value_2, bit_value_1
    double_nop
    send_low_time 5, bit_value_1
    double_nop

    send_high_time bit_value_1, bit_value_2
    double_nop
    send_low_time 4, bit_value_2
    double_nop

    send_high_time bit_value_2, bit_value_1
    double_nop
    send_low_time 3, bit_value_1
    double_nop

    send_high_time bit_value_1, bit_value_2
    double_nop
    send_low_time 2, bit_value_2
    double_nop

    send_high_time bit_value_2, bit_value_1
    double_nop
    send_low_time 1, bit_value_1
    double_nop

    send_high_time bit_value_1, bit_value_2
    double_nop
    send_low_time 0, bit_value_2
    sbiw loop_counter, 1 ; loop_counter--

    send_high_time bit_value_2, bit_value_1
    ld current_byte, X+ ; current_byte = *(++X)
    send_low_time 7, bit_value_1
    brne loop_over_bytes ; if loop_counter != 0 goto loop_over_bytes

finish:
    ; Put reset condition
    out _SFR_IO_ADDR(LED_PORT), port_low
    ; Return to C
    ret
