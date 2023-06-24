#pragma once

/**
 * Mount storage partition
 */
void init_spiffs(void);

/**
 * Erase the content of the storage partition
 */
void format_spiffs(void);
