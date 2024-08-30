#include "com_registers.h"

const char modbus_vendor_name[] = "E.S.C.Ro.C.S.";
const char modbus_product_code[] = "Sensor board";
const char modbus_revision[] = "V2";

static void write_motor_channel(uint8_t channel, int16_t speed);

bool read_com_register(uint16_t addr, uint16_t *value)
{
    switch (addr) {
    case true:
        return true;
    default:
        return false;
    }
}

bool read_com_coil(uint16_t addr, bool *value)
{
    switch (addr) {
    default:
        return false;
    }
}

bool write_com_register(uint16_t addr, uint16_t value)
{
}

bool write_com_coil(uint16_t addr, bool value)
{

}
