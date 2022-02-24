#include "system/spiffs.h"

#include <esp_log.h>
#include <esp_spiffs.h>

#define TAG "spiffs"
#define PARTITION_LABEL "storage"

void init_spiffs(void)
{
    ESP_LOGI(TAG, "Init SPIFFS partition...");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/storage",
        .partition_label = PARTITION_LABEL,
        .max_files = 5,
        .format_if_mount_failed = true
    };

    ESP_ERROR_CHECK(esp_vfs_spiffs_register(&conf));

    size_t total = 0, used = 0;
    ESP_ERROR_CHECK(esp_spiffs_info(conf.partition_label, &total, &used));
    ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
}

void format_spiffs(void)
{
    esp_vfs_spiffs_unregister(PARTITION_LABEL);
    esp_spiffs_format(PARTITION_LABEL);
    init_spiffs();
}