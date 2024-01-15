#pragma once

#define TASK_STACK_SIZE 4096

#define MOTION_CONTROL_PRIORITY     (tskIDLE_PRIORITY + 20)
#define US_BOARD_PRIORITY           (tskIDLE_PRIORITY + 17)
#define TRIGGER_TIMER_PRIORITY      (tskIDLE_PRIORITY + 15)
#define LUA_PRIORITY                (tskIDLE_PRIORITY + 14)
#define DISABLE_MOTOR_PRIORITY      (tskIDLE_PRIORITY + 13)
#define LOGGER_PRIORITY             (tskIDLE_PRIORITY + 10)
#define OTA_REBOOT_PRIORITY         (tskIDLE_PRIORITY + 9)

#define TIME_CRITICAL_CORE          1
#define LOW_CRITICITY_CORE          0