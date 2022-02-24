#pragma once

#include <esp_http_server.h>

httpd_uri_t *get_game_actions_uri_handlers();
size_t game_actions_uri_handlers_size();

