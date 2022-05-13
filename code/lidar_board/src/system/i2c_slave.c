#include "system/i2c_slave.h"
#include "system/task_priority.h"
#include "localization/pose.h"
#include "localization/pose_refinement.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <esp_err.h>
#include <driver/i2c.h>
#include <math.h>
#include <string.h>

#define TAG "I2C"
#define I2C_SDA_PIN 33
#define I2C_SCL_PIN 32
#define I2C_TIMEOUT 200
#define I2C_BUFFER_SIZE 512
#define SLAVE_ADDR 0x0D
#define HEADER_VALUE 0xBC

struct i2c_query_t {
    uint8_t header;
    int16_t guess_pose_x_mm;
    int16_t guess_pose_y_mm;
    int16_t guess_pose_theta_mrad;
    uint8_t checksum;
} __attribute__((packed));

struct i2c_response_t {
    uint8_t header;
    int16_t refined_pose_x_mm;
    int16_t refined_pose_y_mm;
    int16_t refined_pose_theta_mrad;
    uint8_t has_refined;
    uint8_t obstacle_clusters[NUMBER_OF_CLUSTER_ANGLES];
    uint8_t checksum;
} __attribute__((packed));

static void i2c_slave_task(void *parameters);
static uint8_t compute_checksum(uint8_t *payload, size_t length);

void init_i2c_slave(void)
{
    ESP_LOGI(TAG, "Initializing I2C slave...");

    i2c_config_t conf_slave = {
        .sda_io_num = I2C_SDA_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_SCL_PIN,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .mode = I2C_MODE_SLAVE,
        .slave.addr_10bit_en = 0,
        .slave.slave_addr = SLAVE_ADDR,
    };
    i2c_param_config(I2C_NUM_0, &conf_slave);
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, conf_slave.mode, I2C_BUFFER_SIZE, I2C_BUFFER_SIZE, 0));
    i2c_set_timeout(I2C_NUM_0, I2C_TIMEOUT);

    TaskHandle_t task;
    xTaskCreate(i2c_slave_task, "i2c_slave", TASK_STACK_SIZE, NULL, I2C_SLAVE_PRIORITY, &task);

    ESP_LOGI(TAG, "Done initializing I2C slave.");
}

static void i2c_slave_task(void *parameters)
{
    struct i2c_query_t query;
    struct i2c_response_t response;

    while (true) {
        int read;
        if ((read = i2c_slave_read_buffer(I2C_NUM_0, (void*)&query, sizeof(query), I2C_TIMEOUT)) <= 0) {
            continue;
            ESP_ERROR_CHECK_WITHOUT_ABORT(read);
        }

        ESP_LOGD(TAG, "Received from I2C master");
        memset(&response, 0, sizeof(response));
        response.header = HEADER_VALUE;
        if ((compute_checksum((uint8_t*)&query, sizeof(query)) != query.checksum) || (query.header != HEADER_VALUE)) {
            ESP_LOGW(TAG, "Warning: bad checksum on I2C query from main board");
            ESP_LOG_BUFFER_HEX_LEVEL(TAG, (void*)&query, sizeof(query), ESP_LOG_INFO);
            int flushed = i2c_slave_read_buffer(I2C_NUM_0, (void*)&query, sizeof(query), 0);
            ESP_LOGI(TAG, "Flushed %d bytes", flushed);
            ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_reset_tx_fifo(I2C_NUM_0));
            continue;
        }

        pose_t guess_pose, refined_pose;
        guess_pose.x = query.guess_pose_x_mm;
        guess_pose.y = query.guess_pose_y_mm;
        // Note the minus sign, due to difference in sign convention between boards
        // I am a moron
        guess_pose.theta = -(float)query.guess_pose_theta_mrad / 1000.0;
        ESP_LOGD(TAG, "Estimated pose from main board: %f %f %f", guess_pose.x, guess_pose.y, guess_pose.theta);
        update_estimated_input_pose(&guess_pose);

        response.has_refined = get_refined_output_pose(&refined_pose);
        if (response.has_refined) {
            response.refined_pose_x_mm = refined_pose.x;
            response.refined_pose_y_mm = refined_pose.y;
            response.refined_pose_theta_mrad = -refined_pose.theta * 1000.0;
        }
        float obstacle_clusters[NUMBER_OF_CLUSTER_ANGLES];
        get_obstacle_clusters(obstacle_clusters);
        for (int i = 0; i < NUMBER_OF_CLUSTER_ANGLES; ++i) {
            if (isnanf(obstacle_clusters[i]) || isinff(obstacle_clusters[i])) {
                response.obstacle_clusters[i] = 0xFF;
            } else {
                response.obstacle_clusters[i] = (uint8_t)floorf(obstacle_clusters[i] / 10.0);
            }
        }
        
        response.checksum = compute_checksum((uint8_t*)&response, sizeof(response));

        // Flush RX buffer
        i2c_slave_read_buffer(I2C_NUM_0, (void*)&query, sizeof(query), 0);
        ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_reset_tx_fifo(I2C_NUM_0));
        int written = 0;
        do {
            written += i2c_slave_write_buffer(I2C_NUM_0, (const void*)&response + written, sizeof(response) - written, I2C_TIMEOUT);
        } while (written < sizeof(response));
    }
}

static uint8_t compute_checksum(uint8_t *payload, size_t length)
{
    uint8_t result = 0x00;
    for (size_t i = 0; i < length - 1; ++i) {
        result ^= payload[i];
    }
    return result;
}