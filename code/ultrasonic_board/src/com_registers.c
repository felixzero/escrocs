#include "com_registers.h"
#include "controller.h"

const char modbus_vendor_name[] = "E.S.C.Ro.C.S.";
const char modbus_product_code[] = "Ultrasonic board";
const char modbus_revision[] = "V1";

bool read_com_register(uint16_t addr, uint16_t *value)
{
    if (addr == MODBUS_CONFIG_CRITICAL_DISTANCE) {
        *value = critical_threshold_distance;
        return true;
    } else if (addr == MODBUS_CONFIG_SAFE_DISTANCE) {
        *value = safe_distance;
        return true;
    } else if (addr == MODBUS_CONFIG_REPETITION_PERIOD) {
        *value = repetition_period;
        return true;
    } else if (addr == MODBUS_CONFIG_TIMEOUT) {
        *value = timeout_distance;
        return true;
    } else if (addr >= MODBUS_DISTANCE_INPUT_REG_START && addr < MODBUS_DISTANCE_INPUT_REG_END) {
        uint8_t channel = addr - MODBUS_DISTANCE_INPUT_REG_START;
        *value = ultrasound_distances[channel];
        return true;
    } else {
        return false;
    }
}

bool read_com_coil(uint16_t addr, bool *value)
{
    if (addr >= MODBUS_COIL_CHANNELS_START && addr < MODBUS_COIL_CHANNELS_END) {
        uint8_t channel = addr - MODBUS_COIL_CHANNELS_START;
        *value = enabled_channels[channel];
        return true;
    } else if (addr == MODBUS_OBSTRUCTED_STATUS) {
        *value = is_path_obstructed();
        return true;
    } else {
        return false;
    }
}

bool write_com_register(uint16_t addr, uint16_t value)
{
    if (addr == MODBUS_CONFIG_CRITICAL_DISTANCE) {
        critical_threshold_distance = value;
        return true;
    } else if (addr == MODBUS_CONFIG_SAFE_DISTANCE) {
        safe_distance = value;
        return true;
    } else if (addr == MODBUS_CONFIG_REPETITION_PERIOD) {
        repetition_period = value;
        return true;
    } else if (addr == MODBUS_CONFIG_TIMEOUT) {
        timeout_distance = value;
        return true;
    } else {
        return false;
    }
}

bool write_com_coil(uint16_t addr, bool value)
{
    if (addr >= MODBUS_COIL_CHANNELS_START && addr < MODBUS_COIL_CHANNELS_END) {
        uint8_t channel = addr - MODBUS_COIL_CHANNELS_START;
        enabled_channels[channel] = value;
        return true;
    } else if (addr == MODBUS_COIL_PULSE) {
        controller_state = value ? ULTRASOUND_BOARD_SCANNING : ULTRASOUND_BOARD_IDLE;
        return true;
    } else if (addr == MODBUS_COIL_UPDATE_LED) {
        controller_state = value ? ULTRASOUND_BOARD_UPDATING_LED : ULTRASOUND_BOARD_IDLE;
        return true;
    } else if (addr == MODBUS_COIL_PULSE_AND_UPDATE) {
        controller_state = value ? ULTRASOUND_BOARD_FULL_UPDATE : ULTRASOUND_BOARD_IDLE;
        return true;
    } else {
        return false;
    }
}
