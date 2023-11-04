#pragma once

#include <stdint.h>
#include <stdbool.h>

bool read_com_register(uint16_t addr, uint16_t *value);
bool read_com_coil(uint16_t addr, bool *value);
bool write_com_register(uint16_t addr, uint16_t value);
bool write_com_coil(uint16_t addr, bool value);

extern const char modbus_vendor_name[];
extern const char modbus_product_code[];
extern const char modbus_revision[];
