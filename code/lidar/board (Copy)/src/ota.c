#include "ota.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_ota_ops.h"
#include <esp_http_server.h>

#define EXAMPLE_ESP_WIFI_SSID      "escroc_lidar"
#define EXAMPLE_ESP_WIFI_PASS      "escrobouille"
#define EXAMPLE_MAX_STA_CONN       4

static const char *TAG = "OTA_UPDATE";

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station join, AID=%d", event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station leave, AID=%d", event->aid);
    }
}


void wifi_init_softap(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Setting IP configuration
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_t* wifi_ap = esp_netif_create_default_wifi_ap();
    esp_netif_ip_info_t ip_info;
    esp_netif_set_ip4_addr(&ip_info.ip, 192, 168, 168, 1);
	esp_netif_set_ip4_addr(&ip_info.gw, 192, 168, 168, 1);
	esp_netif_set_ip4_addr(&ip_info.netmask, 255, 255, 255, 0);
	esp_netif_dhcps_stop(wifi_ap);
	esp_netif_set_ip_info(wifi_ap, &ip_info);
	esp_netif_dhcps_start(wifi_ap);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .channel = 1,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    esp_wifi_set_max_tx_power(8);
    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
}

esp_err_t ota_update_handler(httpd_req_t *req) {
    esp_err_t err;
    esp_ota_handle_t update_handle = 0;
    const esp_partition_t *update_partition = NULL;

    // Get the update partition
    update_partition = esp_ota_get_next_update_partition(NULL);
    if (update_partition == NULL) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "No update partition available");
        return ESP_FAIL;
    }

    // Begin the update process
    err = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &update_handle);
    if (err != ESP_OK) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to begin OTA");
        return ESP_FAIL;
    }

    // Read the update data
    char buf[1024];
    int recv_len = httpd_req_recv(req, buf, sizeof(buf) - 1);
    while (recv_len > 0) {
        if (esp_ota_write(update_handle, buf, recv_len) != ESP_OK) {
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to write OTA data");
            return ESP_FAIL;
        }
        recv_len = httpd_req_recv(req, buf, sizeof(buf) - 1);
    }

    // End the update process
    if (esp_ota_end(update_handle) != ESP_OK) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to end OTA");
        return ESP_FAIL;
    }

    // Set the boot partition
    err = esp_ota_set_boot_partition(update_partition);
    if (err != ESP_OK) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to set boot partition");
        return ESP_FAIL;
    }

    httpd_resp_send(req, "Update successful! Rebooting...", HTTPD_RESP_USE_STRLEN);
    esp_restart();

    return ESP_OK;
}

void start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    // Start the HTTP server
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t ota_uri = {
            .uri = "/firmware.bin",
            .method = HTTP_PUT,
            .handler = ota_update_handler,
            .user_ctx = NULL,
        };
        ESP_ERROR_CHECK(httpd_register_uri_handler(server, &ota_uri));
    } else {
        ESP_LOGE(TAG, "Failed to start HTTP server");
    }
}
