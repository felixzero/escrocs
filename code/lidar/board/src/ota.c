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

#define EXAMPLE_ESP_WIFI_SSID      "escroc_lidar"
#define EXAMPLE_ESP_WIFI_PASS      "escrobouille"
#define EXAMPLE_MAX_STA_CONN       2

static const char *TAG = "OTA_UPDATE";

void wifi_init_softap(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    esp_netif_create_default_wifi_ap();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    esp_netif_t* wifi_ap = esp_netif_create_default_wifi_ap();

    // Setting IP configuration
    esp_netif_ip_info_t ip_info;
    esp_netif_set_ip4_addr(&ip_info.ip, 192, 168, 168, 1);
	esp_netif_set_ip4_addr(&ip_info.gw, 192, 168, 168, 1);
	esp_netif_set_ip4_addr(&ip_info.netmask, 255, 255, 255, 0);
	esp_netif_dhcps_stop(wifi_ap);
	esp_netif_set_ip_info(wifi_ap, &ip_info);
	esp_netif_dhcps_start(wifi_ap);

    esp_wifi_init(&cfg);

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

    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
    esp_wifi_start();

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
}

esp_err_t ota_update_handler(httpd_req_t *req)
{
    esp_err_t err;
    esp_ota_handle_t update_handle = 0;
    const esp_partition_t *update_partition = NULL;

    update_partition = esp_ota_get_next_update_partition(NULL);
    if (update_partition == NULL) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "No OTA partition available");
        return ESP_FAIL;
    }

    err = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &update_handle);
    if (err != ESP_OK) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Error starting OTA update");
        return ESP_FAIL;
    }

    int total_len = req->content_len;
    int recv_len = 0;
    char ota_buf[1024];

    while (recv_len < total_len) {
        int data_len = httpd_req_recv(req, ota_buf, MIN(total_len - recv_len, sizeof(ota_buf)));
        if (data_len > 0) {
            err = esp_ota_write(update_handle, (const void *)ota_buf, data_len);
            if (err != ESP_OK) {
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Error writing OTA data");
                return ESP_FAIL;
            }
            recv_len += data_len;
        } else if (data_len == HTTPD_SOCK_ERR_TIMEOUT) {
            continue;
        } else {
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Error receiving OTA data");
            return ESP_FAIL;
        }
    }

    err = esp_ota_end(update_handle);
    if (err != ESP_OK) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Error ending OTA update");
        return ESP_FAIL;
    }

    err = esp_ota_set_boot_partition(update_partition);
    if (err != ESP_OK) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Error setting boot partition");
        return ESP_FAIL;
    }

    httpd_resp_send(req, "OTA update successful. Rebooting...", HTTPD_RESP_USE_STRLEN);
    esp_restart();
    return ESP_OK;
}

void start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    httpd_uri_t ota_uri = {
        .uri       = "/ota",
        .method    = HTTP_POST,
        .handler   = ota_update_handler,
        .user_ctx  = NULL
    };
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(&config, &ota_uri);
    }
}
