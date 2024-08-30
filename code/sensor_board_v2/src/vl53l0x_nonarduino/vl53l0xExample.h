#ifndef VL53L0XEXAMPLE_H
#define VL53L0XEXAMPLE_H

//--------------------------------------------------
// Global Defines
//--------------------------------------------------
// GPIO
#define PIN_UART_TX		PD1
#define PIN_I2C_SDA 	PC4
#define PIN_I2C_SCL 	PC5

// Constants
#define BAUD 			115200	// Serial baudrate for user interface

// Pseudo functions for Clearing, Setting and Testing bits
#define SBI(reg, bit) 		( reg |=  ( 1 << bit ) )
#define CBI(reg, bit) 		( reg &= ~( 1 << bit ) )
#define IBI(reg, bit) 		( (reg&(1<<bit)) != 0 )

#endif /* MAIN_H_ */
