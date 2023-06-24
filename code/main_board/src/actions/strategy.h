#pragma once

#include <esp_err.h>
#include <esp_http_server.h>

void init_lua_executor(void);

/**
 * HTTPD handlers to manage /strategy routes
 */
esp_err_t get_strategy_handler(httpd_req_t *req);
esp_err_t put_strategy_handler(httpd_req_t *req);
esp_err_t delete_strategy_handler(httpd_req_t *req);
esp_err_t run_strategy_handler(httpd_req_t *req);
