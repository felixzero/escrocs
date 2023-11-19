#include "system/spiffs.h"

#include <esp_log.h>
#include <esp_spiffs.h>
#include <string.h>

#define TAG                             "spiffs"
#define PARTITION_LABEL                 "storage"
#define FS_INDEX_FILE                   "/storage/index.txt"
#define FS_INDEX_FILENAME_MAX_LENGTH    32

static void create_fs_index_file(void);

esp_err_t init_spiffs(void)
{
    esp_err_t err = ESP_OK;
    ESP_LOGI(TAG, "Init SPIFFS partition...");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/storage",
        .partition_label = PARTITION_LABEL,
        .max_files = 5,
        .format_if_mount_failed = true
    };

    err = esp_vfs_spiffs_register(&conf);
    if (err) {
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        return err;
    }

    size_t total = 0, used = 0;
    err = esp_spiffs_info(conf.partition_label, &total, &used);
    if (err) {
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        return err;
    }
    ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);

    return ESP_OK;
}

void format_spiffs(void)
{
    esp_vfs_spiffs_unregister(PARTITION_LABEL);
    esp_spiffs_format(PARTITION_LABEL);
    init_spiffs();
    create_fs_index_file();
}

bool list_spiffs_files(int index, char *filename, size_t max_filename_length)
{
    FILE *f = fopen(FS_INDEX_FILE, "rb");
    fseek(f, FS_INDEX_FILENAME_MAX_LENGTH * index, SEEK_SET);
    if (max_filename_length > FS_INDEX_FILENAME_MAX_LENGTH) {
        max_filename_length = FS_INDEX_FILENAME_MAX_LENGTH;
    }
    if (fread(filename, max_filename_length, 1, f) <= 0) {
        ESP_LOGI(TAG, "End of index file reached at index %d", index);
        fclose(f);
        return false;
    }
    fclose(f);
    ESP_LOGI(TAG, "Found file %s at index %d", filename, index);
    return true;
}

bool add_spiffs_file_entry(char *filename)
{
    FILE *f = fopen(FS_INDEX_FILE, "r+b");
    char buffer[FS_INDEX_FILENAME_MAX_LENGTH];

    fseek(f, 0, SEEK_SET);
    while (true) {
        if (fread(buffer, FS_INDEX_FILENAME_MAX_LENGTH, 1, f) <= 0) {
            ESP_LOGI(TAG, "File %s is a new file to SPIFFS, adding", filename);
            memset(buffer, 0, FS_INDEX_FILENAME_MAX_LENGTH);
            strncpy(buffer, filename, FS_INDEX_FILENAME_MAX_LENGTH);
            fwrite(buffer, FS_INDEX_FILENAME_MAX_LENGTH, 1, f);
            fclose(f);
            return false;
        }

        if (!strncmp(buffer, filename, FS_INDEX_FILENAME_MAX_LENGTH)) {
            ESP_LOGI(TAG, "File %s already exists in SPIFFS, skipping", filename);
            fclose(f);
            return true;
        }
    }
}

static void create_fs_index_file(void)
{
    FILE *f = fopen(FS_INDEX_FILE, "wb");
    fclose(f);
}
