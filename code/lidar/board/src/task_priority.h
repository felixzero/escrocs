#pragma once

#define TASK_STACK_SIZE             4096

#define I2C_TASK_PRIORITY       (tskIDLE_PRIORITY + 10)
#define LIDAR_TASK_PRIORITY     (tskIDLE_PRIORITY + 8)
#define AMALGAME_TASK_PRIORITY  (tskIDLE_PRIORITY + 6)
#define PRINTER_TASK_PRIORITY   (tskIDLE_PRIORITY + 4)


#define TIME_CRITICAL_CORE          1
#define LOW_CRITICITY_CORE          0
