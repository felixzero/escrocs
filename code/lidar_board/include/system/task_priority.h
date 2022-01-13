#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define TASK_STACK_SIZE 4096

#define UART_RECEIVER_PRIORITY 20
#define MOTOR_CONTROL_PRIORITY 15
#define OTA_REBOOT_PRIORITY 10

extern TaskHandle_t motor_control_task;
