/* 
#ifndef DEBUGPRINT_H
#define DEBUGPRINT_H

// Leightweight library for printing debugging info to the UART

#include <avr/pgmspace.h>

// All the DEBUG() prints can be disabled with this switch
// useful if you're done debugging and want a clean .hex file
#define DEBUGPRINT_ENABLED 1						// Disable ALL debug output

#if DEBUGPRINT_ENABLED
	void debug_hex( uint32_t val, uint8_t digits );	// output `val` as hex number with N digits
	void debug_dec( uint32_t val );					// output `val` as decimal number (variable length)
	// Output val as fixed point number, with a fractional part of`nFract` bits
	void debug_dec_fix( uint32_t val, const uint8_t nFract );
	void debug_putc( uint8_t c );					// output the single character `c`
	#define debug_str(s) debug_str_internal(PSTR(s))// output string `s`, which will be stored in program memory
	void hexDump(uint8_t *buffer, uint16_t nBytes);	// output a pretty hex-dump of `buffer`
#else
	#define debug_hex(val,digits)
	#define debug_dec(val)
	#define debug_dec_fix(val,nFract)
	#define debug_str(s)
	#define debug_putc(c)
	#define hexDump(a,b)
#endif

void debugInit( );									// Initialize debug UART
void debug_str_internal( const char *p );

#endif

*/