#pragma once

#include <esp_http_server.h>

esp_err_t set_pose_target_handler(httpd_req_t *req);
esp_err_t set_motion_control_tuning_handler(httpd_req_t *req);
