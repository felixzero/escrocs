#pragma once

#include <esp_err.h>

void display_initialization_status(const char *name, esp_err_t status);

int menu_pick_item(const char *description, char **menu_entries, size_t menu_length);
