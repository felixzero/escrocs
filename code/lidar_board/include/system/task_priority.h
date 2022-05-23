#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define TASK_STACK_SIZE 4096

#define UART_RECEIVER_PRIORITY 20
#define I2C_SLAVE_PRIORITY 17
#define TCP_SOCKET_PRIORITY 16
#define MOTOR_CONTROL_PRIORITY 15
#define LOGGER_PRIORITY 12
#define OTA_REBOOT_PRIORITY 10

extern TaskHandle_t motor_control_task;
