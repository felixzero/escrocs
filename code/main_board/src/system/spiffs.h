#pragma once

#include <esp_err.h>
#include <stdbool.h>

/**
 * Mount storage partition
 */
esp_err_t init_spiffs(void);

/**
 * Erase the content of the storage partition
 */
void format_spiffs(void);

/**
 * List all files in the SPIFFS filesystem ("ls" command)
 * @param index Index of the filename to get
 * @param filename Output buffer to which the filename will be written
 * @param max_filename_length If the filename is too long, then it will be truncated to this length
 * @return true if successful, false if it reached the end of the file system
 */
bool list_spiffs_files(int index, char *filename, size_t max_filename_length);

/**
 * Add a new entry to the file system
 * @return true if the file already existed
 */
bool add_spiffs_file_entry(char *filename);
