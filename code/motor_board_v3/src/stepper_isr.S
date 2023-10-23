#include <avr/io.h>

#define STATUS_REG 0x5F
#define SQRT_DIFF_MAX_PRECALCULATED_VALUE 4096

#define STRUCT_ACCELERATION_STEP_NEXT_STEP 0
#define STRUCT_ACCELERATION_STEP_PERIOD_PREFACTOR 2
#define STRUCT_ACCELERATION_STEP_FINAL_INDICE 4
#define STRUCT_ACCELERATION_STEP_CURRENT_INDICE 6
#define STRUCT_ACCELERATION_STEP_INDICE_INCREMENT 8
#define STRUCT_ACCELERATION_STEP_POLARITY 10

.global TIMER1_OVF_vect, TIMER3_OVF_vect, TIMER4_OVF_vect
.global stepper0_step, stepper_1_step, stepper_2_step

.text

.macro define_ovf_isr ISR, stepper_step, ICRL, ICRH, polarity_bit
; Total: 141 cycles
\ISR:
    ; Prologue: save context [25 cycles]
    push r0
    push r1
    clr r1
    lds r0, STATUS_REG
    push r0
    push r26 ; X
    push r27
    push r28 ; Y
    push r29
    push r30 ; Z
    push r31
    push r16
    push r17

    ; Y = \stepper_step [4 cycles]
    lds YL, \stepper_step + (STRUCT_ACCELERATION_STEP_NEXT_STEP)
    lds YH, \stepper_step + (STRUCT_ACCELERATION_STEP_NEXT_STEP + 1)

    ; Z = Y->current_indice [4 cycles]
    ldd ZL, Y + (STRUCT_ACCELERATION_STEP_CURRENT_INDICE)
    ldd ZH, Y + (STRUCT_ACCELERATION_STEP_CURRENT_INDICE + 1)

    ; If (current_indice != final_indice) [8 cycles]
    ; Skip step increment
    ldd r0, Y + (STRUCT_ACCELERATION_STEP_FINAL_INDICE)
    cp r0, ZL
    brne 1f
    ldd r0, Y + (STRUCT_ACCELERATION_STEP_FINAL_INDICE + 1)
    cp r0, ZH
    brne 1f

    ; \stepper_step = \stepper_step->next_step [13 cycles]
    ldd r0, Y + (STRUCT_ACCELERATION_STEP_NEXT_STEP)
    ldd YH, Y + (STRUCT_ACCELERATION_STEP_NEXT_STEP + 1)
    mov YL, r0
    sts \stepper_step, YL
    sts \stepper_step+1, YH
    ldd ZL, Y + (STRUCT_ACCELERATION_STEP_CURRENT_INDICE)
    ldd ZH, Y + (STRUCT_ACCELERATION_STEP_CURRENT_INDICE + 1)

1:
    ; PORTE[\polarity_bit] = \stepper_step->polarity [8 cycles]
    cbi 0x0E, \polarity_bit
    ldd r16, Y + (STRUCT_ACCELERATION_STEP_POLARITY)
    sbrc r16, 0
    sbi 0x0E, \polarity_bit

    ; X = \stepper_step->period_prefactor [4 cycles]
    ldd XL, Y + (STRUCT_ACCELERATION_STEP_PERIOD_PREFACTOR)
    ldd XH, Y + (STRUCT_ACCELERATION_STEP_PERIOD_PREFACTOR + 1)

    ; if current_indice >= SQRT_DIFF_MAX_PRECALCULATED_VALUE, skip [2 cycles]
    cpi ZH, hi8(SQRT_DIFF_MAX_PRECALCULATED_VALUE)
    brsh 2f

    ; r17:r16 = sqrt_diff_precalc[current_indice << 1] [11 cycles]
    lsl ZL
    rol ZH
    ldi r16, hi8(sqrt_diff_precalc)
    add ZH, r16
    lpm r16, Z+
    lpm r17, Z

    ; Z = X = \stepper_step->period_prefactor
    movw Z, X

    ; X = (r17:r16 * Z) / 256 [16 cycles]
    clr XH
    mul r16, ZL
    mov XL, r1
    mul r17, ZL
    add XL, r0
    adc XH, r1
    mul r16, ZH
    add XL, r0
    adc XH, r1
    mul r17, ZH
    add XH, r0
    clr r1

2:
    ; ICR1 = X [4 cycles]
    sts \ICRL, XL
    sts \ICRH, XH

    ; Z = Y->current_indice [4 cycles]
    ldd ZL, Y + (STRUCT_ACCELERATION_STEP_CURRENT_INDICE)
    ldd ZH, Y + (STRUCT_ACCELERATION_STEP_CURRENT_INDICE + 1)

    ; Y->current_indice += Y->indice_increment [10 cycles]
    ldd r16, Y + (STRUCT_ACCELERATION_STEP_INDICE_INCREMENT)
    ldd r17, Y + (STRUCT_ACCELERATION_STEP_INDICE_INCREMENT + 1)
    add ZL, r16
    adc ZH, r17
    std Y + (STRUCT_ACCELERATION_STEP_CURRENT_INDICE), ZL
    std Y + (STRUCT_ACCELERATION_STEP_CURRENT_INDICE + 1), ZH

    ; Epilogue: restore context [28 cycles]
    pop r17
    pop r16
    pop r31
    pop r30
    pop r29
    pop r28
    pop r27
    pop r26
    pop r0
    sts STATUS_REG, r0
    pop r1
    pop r0
    reti
.endm

define_ovf_isr TIMER1_OVF_vect, stepper0_step, ICR1L, ICR1H, 0
define_ovf_isr TIMER3_OVF_vect, stepper1_step, ICR3L, ICR3H, 1
define_ovf_isr TIMER4_OVF_vect, stepper2_step, ICR4L, ICR4H, 2
