#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <esp_err.h>

void init_modbus_rtu_master(void);

esp_err_t modbus_read_coil_status(uint8_t slave_addr, uint16_t starting_coil_address, uint16_t number_of_coils_to_read, bool *output);
esp_err_t modbus_read_input_status(uint8_t slave_addr, uint16_t starting_input_address, uint16_t number_of_inputs_to_read, bool *output);
esp_err_t modbus_read_holding_registers(uint8_t slave_addr, uint16_t starting_address, uint16_t number_of_registers_to_read, uint16_t *output);
esp_err_t modbus_read_input_registers(uint8_t slave_addr, uint16_t starting_address, uint16_t number_of_registers_to_read, uint16_t *output);
esp_err_t modbus_force_single_coil(uint8_t slave_addr, uint16_t address, bool value);
esp_err_t modbus_preset_single_register(uint8_t slave_addr, uint16_t address, uint16_t value);
esp_err_t modbus_force_multiple_coils(uint8_t slave_addr, uint16_t starting_coil_address, uint16_t number_of_coils_to_write, bool *input);
esp_err_t modbus_preset_multiple_registers(uint8_t slave_addr, uint16_t starting_address, uint16_t number_of_registers_to_write, uint16_t *input);
esp_err_t modbus_read_device_identification(uint8_t slave_addr, uint8_t object_id, char *identification, size_t max_length);
