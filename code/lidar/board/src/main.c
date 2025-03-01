#include <esp_err.h>
#include <esp_log.h>

#include "parser.h"
#include "collision_handler.h"
#include "lidar.h"
#include "ota.h"

#include "../loca_lidar/amalgame.h"
#include "../loca_lidar/loca_lidar.h"
#include "../loca_lidar/pose_refinement.h"

#include "esp_heap_caps.h"
#include <sys/time.h>

#include "esp_wifi.h"


#define TAG "MAIN"
void app_main() {
    init_uart();

}