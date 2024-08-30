#include <avr/interrupt.h>
#include "millis.h"

// Millisecond timer, inspired by the Arduino library
// Uses Timer0 interrupts

volatile uint32_t g_Millis=0;		//Global var. for counting milliseconds. 
									//Don't touch without disabling interrupts.

void initMillis() {
	//--------------------------------------------------
	// Timer0 for for the `millis()` function
	//--------------------------------------------------
	TCCR0A = (1<<WGM01);			//Clear TImer on Compar Match Mode (2), no pin output
	TCCR0B = (1<<CS02) | (0<<CS01) | (1<<CS00);//TOP=OCR0A, 1024 prescaler
	OCR0A = T0_RELOAD;				//244: Overflow every 15.68 ms
	TIMSK0 = (1<<OCIE0A);
}

// Return ellapsed time since startup in [ms]
uint32_t millis(){
    uint32_t m;
    uint8_t oldSREG = SREG;
     // disable interrupts while we read timer0_millis or we might get an
    // inconsistent value (e.g. in the middle of a write to timer0_millis)
    cli();
    m = g_Millis;
    SREG = oldSREG;
    return m;
}

ISR( TIMER0_COMPA_vect ){			//Called every 15.68 ms
	static uint8_t usFract=0;
	g_Millis += MILLIS_INC;
	usFract  += MILLIS_INC_FRACT>>3;// 680 / 8 =  85.0
	if( usFract >= 1000>>3 ){		//1000 / 8 = 125.0
		usFract -= 1000>>3;			//Fractional part added up to 1 ms
		g_Millis++;
	}
}