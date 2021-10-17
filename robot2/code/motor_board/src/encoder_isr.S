#include <avr/io.h>

#define PINB_IO 0x03
#define PIND_IO 0x09
#define STATUS_REG 0x5F

.global hall_count_1, hall_count_2, hall_count_3, PCINT0_vect, PCINT2_vect
; -------------------------------------------------------------------------------------
.section .data

previous_channel_a: .byte 0x00
previous_channel_b: .byte 0x00
hall_count_1:       .byte 0x00, 0x00
hall_count_2:       .byte 0x00, 0x00
hall_count_3:       .byte 0x00, 0x00
; -------------------------------------------------------------------------------------
.section .text

; Read bit PIN_A of port PORT_A and put it in the last bit of REG_A
; Do the same with B
.macro load_channels REG_A, PORT_A, PIN_A, REG_B, PORT_B, PIN_B
    sbic \PORT_A, \PIN_A
    ori \REG_A, 0x01                                ; Last bit of CHANNELA = PORTB[5]
    sbic \PORT_B, \PIN_B
    ori \REG_B, 0x01                                ; Last bit of CHANNELB = PORTB[7]
.endm


.macro increment_canal CANAL
    ror r28                                         ; is_changed >> 1
    brcc 1f                                         ; Skip increment if is_changed(r28)[0] == 0

    lds r26, \CANAL
    lds r27, \CANAL+1                               ; r26:r27 = CANAL

    sbrs r29, 0
    rjmp 2f
    adiw r26, 1
    rjmp 3f
2:
    sbiw r26, 1                                     ; CANAL++ if direction(r29)[0] == 1 else CANAL--
3:
    sts \CANAL, r26
    sts \CANAL+1, r27                               ; Save CANAL
1:
    ror r29                                         ; direction >> 1
.endm

PCINT0_vect:
PCINT2_vect:
    ; Prologue
    sei                                             ; Allow nested interrupts
    push r18
    lds r18, STATUS_REG
    push r18                                        ; Save status register (0x5F)
    push r26
    push r27
    push r28
    push r29

    ; Load the encoder values of channel A in r26 and channel B in r27
    ldi r26, 0x00                                   ; CHANNELA = 0
    ldi r27, 0x00                                   ; CHANNELB = 0
    load_channels r26, PIND_IO, 4, r27, PIND_IO, 7
    lsl r26
    lsl r27
    load_channels r26, PINB_IO, 3, r27, PINB_IO, 4
    lsl r26
    lsl r27
    load_channels r26, PIND_IO, 2, r27, PINB_IO, 5

    ; Calculate the is_changed flagbyte in r28
    lds r28, previous_channel_a
    eor r28, r26
    lds r29, previous_channel_b
    eor r29, r27
    or r28, r29                                     ; r28 (is_changed) = (C_A ^ prev_a) | (C_B ^ prev_b)

    ; Calculate the direction flagbyte in r29
    lds r29, previous_channel_b
    eor r29, r26
    com r29                                         ; r29 (direction) = ~(C_A ^ prev_b)

    ; Save channels A and B into previous_channel
    sts previous_channel_a, r26
    sts previous_channel_b, r27

    ; Iterate over channels
    increment_canal hall_count_3
    increment_canal hall_count_2
    increment_canal hall_count_1

    ; Epilogue
    pop r29
    pop r28
    pop r27
    pop r26
    pop r18 
    sts STATUS_REG, r18                             ; Restore status register
    pop r18
    reti
