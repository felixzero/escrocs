#include <avr/io.h>

#define PORTB_IO 0x05
#define PORTD_IO 0x0B
#define STATUS_REG 0x5F

.global motor_pwm_buffer_1, motor_pwm_buffer_2, current_buffer_hi, pwm_buffer_swap, TIMER1_COMPA_vect, TIMER1_COMPB_vect
; -------------------------------------------------------------------------------------
.section .data

.align 8                                            ; Align required to guarantee all variables have the same high byte address
motor_pwm_buffer_1:
portb_values:       .byte 0x00, 0x00, 0x00, 0x00
portd_values:       .byte 0x00, 0x00, 0x00, 0x00
channel_values:     .byte 0x00, 0x00, 0x00, 0x00

current_index:      .byte 0x00
current_buffer_hi:  .byte hi8(motor_pwm_buffer_1)
pwm_buffer_swap:    .byte 0x00

.align 8                                            ; Align required to guarantee all variables have the same high byte address
motor_pwm_buffer_2:
                    .byte 0x00, 0x00, 0x00, 0x00
                    .byte 0x00, 0x00, 0x00, 0x00
                    .byte 0x00, 0x00, 0x00, 0x00

; -------------------------------------------------------------------------------------
.section .text

; ISR triggered when timer 1 == OCR1A
; It updates the value of PORTB and PORTD with precomputed values in portb_values and portd_values
; It must run in less than 64 cycles
TIMER1_COMPA_vect:
    ; Prologue
    push r18
    lds r18, STATUS_REG
    push r18                                       ; Save status register (0x5F)
    push r26
    push r27                                       ; Save X register
    ; All variables are on the same page
    lds r27, current_buffer_hi
    ; PORTB = portb_values[current_index];
    lds r26, (current_index)                       ; X (r26:r27) = current_index
    subi r26, lo8(-(portb_values))                 ; X (r26:r27) = portb_values + current_index
    ld r18, X                                      ; r18 = *(portb_values + current_index)
    out PORTB_IO, r18                              ; *PORTB = r18
    ; PORTD = portd_values[current_index];
    subi r26, (portb_values-portd_values)          ; X (r26:r27) += portd_values - portb_values
    ld r18, X                                      ; r18 = *(portb_values + current_index)
    out PORTD_IO, r18                              ; PORTD = r18
    ; current_index = (current_index + 1) % 4;
    lds r26, (current_index)
    inc r26
    andi r26, 0x03                                 ; r26 = (current_index + 1) & 0x03
    sts (current_index), r26                       ; current_index = r26
    ; OCR1A = channel_values[current_index];
    subi r26, lo8(-(channel_values))               ; X (r26:r27) = channel_values + current_index
    ld r18, X                                      ; r18 = channel_values[current_index]
    ldi r26, OCR1AL
    ldi r27, 0x00                                  ; X (r26:r27) = OCR1AL
    st X, r18                                      ; OCR1AL = channel_values[current_index]
    ; Epilogue
    pop r27
    pop r26
    pop r18
    sts STATUS_REG, r18                            ; Restore status register
    pop r18
    reti                                           ; Return from interrupt


; ISR triggered when timer 1 == OCR1B
; At the beginning of each PWM loop, checks if a buffer swap is requested and perform it
; It must run in less than 64 cycles
TIMER1_COMPB_vect:
    ; Prologue
    push r18
    lds r18, STATUS_REG
    push r18                                       ; Save status register (0x5F)
    ; if pwm_buffer_swap == 0 return
    lds r18, pwm_buffer_swap
    cpi r18, 0x00
    breq 3f
    ; current_buffer_hi = motor_pwm_buffer_1 if current_buffer_hi == motor_pwm_buffer_2 else motor_pwm_buffer_2
    lds r18, current_buffer_hi
    cpi r18, hi8(motor_pwm_buffer_1)               ; if current_buffer_hi == motor_pwm_buffer_1
    breq 1f
    ldi r18, hi8(motor_pwm_buffer_1)
    sts current_buffer_hi, r18                     ; current_buffer_hi = motor_pwm_buffer_1
    rjmp 2f
1:
    ldi r18, hi8(motor_pwm_buffer_2)
    sts current_buffer_hi, r18                     ; current_buffer_hi = motor_pwm_buffer_2
2:
    ; Reset pwm_buffer_swap flag
    ldi r18, 0x00
    sts pwm_buffer_swap, r18                       ; pwm_buffer_swap = 0
3:
    ; Epilogue
    pop r18
    sts STATUS_REG, r18                            ; Restore status register
    pop r18
    reti                                           ; Return from interrupt