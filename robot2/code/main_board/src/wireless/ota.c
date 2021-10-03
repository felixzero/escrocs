#include "wireless/ota.h"
#include "system/task_priority.h"

#include <esp_partition.h>
#include <esp_ota_ops.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#define TAG "ota"
#define OTA_BUFFSIZE 1024

static QueueHandle_t reboot_request_queue;

static void restart_in(int ticks_to_wait);
static void rebooter_task(void *parameters);

esp_err_t firmware_upgrade_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Received PUT /firmware.bin request; initiating OTA...");

    // Retrieve OTA partition
    const esp_partition_t *update_partition = esp_ota_get_next_update_partition(NULL);
    assert(update_partition != NULL);
    ESP_LOGI(TAG, "Writing to partition subtype %d at offset 0x%x", update_partition->subtype, update_partition->address);

    // Initiate OTA
    esp_ota_handle_t update_handle = 0;
    esp_err_t err = esp_ota_begin(update_partition, OTA_WITH_SEQUENTIAL_WRITES, &update_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_begin failed (%s)", esp_err_to_name(err));
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failure to initiate OTA");
        return ESP_OK;
    }

    // Download firmware
    int written_length = 0;
    static char ota_write_data[OTA_BUFFSIZE] = { 0 };
    while (written_length < req->content_len) {
        int data_read = httpd_req_recv(req, ota_write_data, OTA_BUFFSIZE);
        if (data_read <= 0) {
            esp_ota_abort(update_handle);
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failure to receive firmware data");
            return ESP_OK;
        }

        err = esp_ota_write(update_handle, (const void*)ota_write_data, data_read);
        if (err != ESP_OK) {
            esp_ota_abort(update_handle);
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failure to write firmware data");
            return ESP_OK;
        }
        written_length += data_read;
        ESP_LOGD(TAG, "Written image length %d", written_length);
    }

    // Validate firmware
    err = esp_ota_end(update_handle);
    if (err != ESP_OK) {
        if (err == ESP_ERR_OTA_VALIDATE_FAILED) {
            ESP_LOGE(TAG, "Image validation failed, image is corrupted");
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Corrupted firmware data");
        } else {
            ESP_LOGE(TAG, "esp_ota_end failed (%s)!", esp_err_to_name(err));
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "esp_ota_end failed");
        }
        return ESP_OK;
    }

    // Reboot on new firmware
    err = esp_ota_set_boot_partition(update_partition);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_set_boot_partition failed (%s)!", esp_err_to_name(err));
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "esp_ota_set_boot_partition failed");
    }
    ESP_LOGI(TAG, "Prepare to restart system!");
    httpd_resp_send(req, "All is fine, captain", HTTPD_RESP_USE_STRLEN);
    restart_in(2000 / portTICK_PERIOD_MS);

    return ESP_OK;
}

void init_ota_rebooter(void)
{
    // Create FreeRTOS task
    TaskHandle_t task;
    reboot_request_queue = xQueueCreate(1, sizeof(int));
    xTaskCreate(rebooter_task, "rebooter_task", TASK_STACK_SIZE, NULL, OTA_REBOOT_PRIORITY, &task);
}

static void restart_in(int ticks_to_wait)
{
    xQueueOverwrite(reboot_request_queue, &ticks_to_wait);
}

static void rebooter_task(void *parameters)
{
    int ticks_to_wait;
    while (xQueueReceive(reboot_request_queue, &ticks_to_wait, 1000 / portTICK_PERIOD_MS) == pdFALSE);
    vTaskDelay(ticks_to_wait);
    esp_restart();
}