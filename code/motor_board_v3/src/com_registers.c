#include "com_registers.h"
#include "steppers.h"

static int16_t stepper_speed_0 = 0;
static int16_t stepper_speed_1 = 0;
static int16_t stepper_speed_2 = 0;

const char modbus_vendor_name[] = "E.S.C.Ro.C.S.";
const char modbus_product_code[] = "Motor board";
const char modbus_revision[] = "V3";

#define MAX_SPEED_RPM               300
#define NUMBER_OF_TICK_PER_TURN     200
#define MAX_SPEED_PERIOD            (60 * F_CPU / MAX_SPEED_RPM / NUMBER_OF_TICK_PER_TURN / 2)

static void write_motor_channel(uint8_t channel, int16_t speed);

bool read_com_register(uint16_t addr, uint16_t *value)
{
    switch (addr) {
    case COM_REG_MOTOR_SPEED_0:
        *value = stepper_speed_0;
        return true;
    case COM_REG_MOTOR_SPEED_1:
        *value = stepper_speed_1;
        return true;
    case COM_REG_MOTOR_SPEED_2:
        *value = stepper_speed_2;
        return true;
    case COM_REG_MOTOR_STEPS_0:
        *value = encoder_values[0];
        return true;
    case COM_REG_MOTOR_STEPS_1:
        *value = encoder_values[1];
        return true;
    case COM_REG_MOTOR_STEPS_2:
        *value = encoder_values[2];
        return true;
    default:
        return false;
    }
}

bool read_com_coil(uint16_t addr, bool *value)
{
    switch (addr) {
    case COM_COIL_ENABLE_MOTORS:
        *value = is_stepper_enabled();
        return true;
    default:
        return false;
    }
}

bool write_com_register(uint16_t addr, uint16_t value)
{
    switch (addr) {
    case COM_REG_MOTOR_SPEED_0:
        stepper_speed_0 = *(int16_t*)&value;
        write_motor_channel(0, stepper_speed_0);
        return true;
    case COM_REG_MOTOR_SPEED_1:
        stepper_speed_1 = *(int16_t*)&value;
        write_motor_channel(1, stepper_speed_1);
        return true;
    case COM_REG_MOTOR_SPEED_2:
        stepper_speed_2 = *(int16_t*)&value;
        write_motor_channel(2, stepper_speed_2);
        return true;
    default:
        return false;
    }
}

bool write_com_coil(uint16_t addr, bool value)
{
    switch (addr) {
    case COM_COIL_ENABLE_MOTORS:
        set_stepper_enable(value);
        return true;
    default:
        return false;
    }
}

static void write_motor_channel(uint8_t channel, int16_t speed)
{
    if (speed == 0) {
        set_stepper_speed(channel, 0);
        return;
    }
    int32_t period = MAX_SPEED_PERIOD * INT16_MAX / speed;
    set_stepper_speed(channel, period);
}
