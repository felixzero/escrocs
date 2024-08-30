#include "modbus_rtu_slave.h"
#include "i2c_bus.h"
#include "soft_serial.h"
#include "vl53l0x_nonarduino/VL53L0X.h"

#include <avr/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <math.h>
#define CLAMP_ABS(x, clamp) ((fabsf(x) > (clamp)) ? (clamp) * (x) / fabsf(x) : (x))

//TODO :
/*
Init soft serial
Init I2C
init distance sensor library
Communication modbus pour lecture de la distance

MANAGE HANGOUT I2C
*/


int main()
{
    init_modbus_rtu_slave();
    init_soft_serial();
    //init_i2c();

    send_serial("test avr");
    while(1) {
        poll_modbus_rtu_operations();
        //poll_i2c_operations();
        _delay_ms(100);
        //start_i2c();
        //addr_i2c(address, 1);
        //write_i2c(0x50);
        //close_i2c();

    }
}
