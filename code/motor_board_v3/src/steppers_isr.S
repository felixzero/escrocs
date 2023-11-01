#include <avr/io.h>

.global TIMER1_OVF_vect, TIMER3_OVF_vect, TIMER4_OVF_vect
.global encoder_values

.text

.macro define_ovf_isr ISR, channel
\ISR:
    ; Prologue, saving context
    push r0
    push r1
    lds r0, 0x5F
    push r0
    push r26
    push r27
    
    ; X = encoder_values[channel]
    lds r26, encoder_values + 0 + \channel * 2
    lds r27, encoder_values + 1 + \channel * 2

    ; X += 1 if PORTE[channel] else -1
    adiw r26, 1
    in r0, 0x0E
    sbrs r0, \channel
    sbiw r26, 2

    ; encoder_values[channel] = X
    sts encoder_values + 0 + \channel * 2, r26
    sts encoder_values + 1 + \channel * 2, r27

    ; Epilogue, restoring context
    pop r27
    pop r26
    pop r0
    sts 0x5F, r0
    pop r1
    pop r0
    reti
.endm

define_ovf_isr TIMER1_OVF_vect, 0
define_ovf_isr TIMER3_OVF_vect, 1
define_ovf_isr TIMER4_OVF_vect, 2
