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

#define TAG "I2C"
#define I2C_SDA_PIN 33
#define I2C_SCL_PIN 32
#define I2C_TIMEOUT 80000
#define I2C_BUFFER_SIZE 512
#define SLAVE_ADDR 0x0D

struct i2c_query_t {
    pose_t guess_pose;
} __attribute__((packed));

struct i2c_response_t {
    pose_t refined_pose;
    uint8_t has_refined;
    float obstacle_clusters[NUMBER_OF_CLUSTER_ANGLES];
} __attribute__((packed));

static void i2c_slave_task(void *parameters);

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
        int s;
        if ((s = i2c_slave_read_buffer(I2C_NUM_0, (void*)&query, sizeof(query), I2C_TIMEOUT)) <= 0) {
            continue;
            ESP_ERROR_CHECK_WITHOUT_ABORT(s);
        }

        ESP_LOGD(TAG, "Received from I2C master");

        update_estimated_input_pose(&query.guess_pose);
        response.has_refined = get_refined_output_pose(&response.refined_pose);

        for (int i = 0; i < NUMBER_OF_CLUSTER_ANGLES; ++i) {
            response.obstacle_clusters[i] = INFINITY;
        }
        get_obstacle_clusters(response.obstacle_clusters);

        ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_reset_tx_fifo(I2C_NUM_0));
        i2c_slave_write_buffer(I2C_NUM_0, (const void*)&response, sizeof(struct i2c_response_t), 0);
    }
}
