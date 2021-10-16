#include "motion/gyroscope.h"
#include "system/i2c_master.h"

#include <esp_log.h>
#include <math.h>
#include <string.h>

#include "motion/mpu6050_dmp.h"

#define GYROSCOPE_I2C_ADDR 0x68
#define FIFO_PACKET_SIZE 28
#define UPLOAD_CHUNK_SIZE 16
#define PI 3.1415926535
#define TAG "Gyro"

#define _write_gyroscope_register(reg, value) write_i2c_register(I2C_PORT_PERIPH, GYROSCOPE_I2C_ADDR, reg, value)
#define _read_gyroscope_register(reg) read_i2c_register(I2C_PORT_PERIPH, GYROSCOPE_I2C_ADDR, reg)

static esp_err_t upload_memory_block(const uint8_t *data, size_t length);

static TickType_t initialization_time;
static float angle_offset[3], angle_drift[3];

void init_gyroscope(void)
{
    ESP_LOGI(TAG, "Init gyroscope system...");
    _write_gyroscope_register(0x6B, _read_gyroscope_register(0x6B) | (1 << 7)); // Perform device reset
    vTaskDelay(100 / portTICK_PERIOD_MS);
    _write_gyroscope_register(0x6A, _read_gyroscope_register(0x6A) | 0b111); // Perform FIFO, I2C master & Sig cond reset
    vTaskDelay(100 / portTICK_PERIOD_MS);
    _write_gyroscope_register(0x6B, 0x01); // PWR_MGMT_1: Clock Source Select PLL_X_gyro
    _write_gyroscope_register(0x38, 0x00); // INT_ENABLE: No Interrupt
    _write_gyroscope_register(0x23, 0x00); // MPU FIFO_EN: (all off) Using DMP's FIFO instead
    _write_gyroscope_register(0x1C, 0x00); // ACCEL_CONFIG: 0 =  Accel Full Scale Select: 2g
    _write_gyroscope_register(0x37, 0x80); // INT_PIN_CFG
	_write_gyroscope_register(0x6B, 0x01); // PWR_MGMT_1: Clock Source Select PLL_X_gyro
	_write_gyroscope_register(0x19, 0x04); // SMPLRT_DIV: Divides the internal sample rate
	_write_gyroscope_register(0x1A, 0x01); // CONFIG: Digital Low Pass Filter (DLPF) Configuration 188HZ

	ESP_ERROR_CHECK_WITHOUT_ABORT(upload_memory_block(mpu6060_dmp_microcode, sizeof(mpu6060_dmp_microcode)));

	_write_gyroscope_register(0x70, 0x04);
	_write_gyroscope_register(0x71, 0x00); // DMP Program Start Address
	_write_gyroscope_register(0x1B, 0x18); // GYRO_CONFIG: 3 = +2000 Deg/sec
	_write_gyroscope_register(0x6A, 0xC0); // USER_CTRL: Enable Fifo and Reset Fifo
	_write_gyroscope_register(0x38, 0x02); // INT_ENABLE: RAW_DMP_INT_EN on
	_write_gyroscope_register(0x6A, _read_gyroscope_register(0x6A) | (1 << 2)); // Reset FIFO one last time just for kicks
	vTaskDelay(1000 / portTICK_PERIOD_MS);

	ESP_LOGI(TAG, "Gyroscope chip initialized");
}

void read_gyroscope(float *yaw, float *pitch, float *roll)
{
	// Reset FIFO and wait for next packet
	_write_gyroscope_register(0x6A, _read_gyroscope_register(0x6A) | (1 << 2));
	while ((_read_gyroscope_register(0x72) << 8 | _read_gyroscope_register(0x73)) < FIFO_PACKET_SIZE);

	// Read first 16 bits of packets (quaternion data)
 	uint8_t packet[16];
	uint8_t reg = 0x74;
	send_to_i2c(I2C_PORT_PERIPH, GYROSCOPE_I2C_ADDR, &reg, 1);
	request_from_i2c(I2C_PORT_PERIPH, GYROSCOPE_I2C_ADDR, packet, 16);

	// Extract quaternion
    float w = (float)(int16_t)((packet[0x0] << 8) | packet[0x1]) / 16384.0f;
    float x = (float)(int16_t)((packet[0x4] << 8) | packet[0x5]) / 16384.0f;
    float y = (float)(int16_t)((packet[0x8] << 8) | packet[0x9]) / 16384.0f;
    float z = (float)(int16_t)((packet[0xc] << 8) | packet[0xd]) / 16384.0f;

	// Convert quaternion to Euler angles
	*yaw = atan2(2 * x * y - 2 * w * z, 2 * w * w + 2 * x * x - 1) * 180. / PI;
	*pitch = -asin(2 * x * z + 2 * w * y) * 180. / PI;
	*roll = atan2(2 * y * z - 2 * w * x, 2 * w * w + 2 * z * z - 1) * 180. / PI;
}

static esp_err_t upload_memory_block(const uint8_t *data, size_t length)
{
    ESP_LOGI(TAG, "Writing DMP code to MPU memory banks");
    ESP_LOGI(TAG, "Total DMP code size: %d", length);

    uint16_t offset = 0;
    while(offset < length) {
		// The chip used 8-bit addresses to the higher byte activate a bank
        uint8_t bank = (offset >> 8) & 0x1F;
        uint8_t address = offset & 0xFF;

        uint8_t chunk_size = UPLOAD_CHUNK_SIZE;
        if (offset + chunk_size > length) {
            chunk_size = length - offset;
        }

		// Write chunk
        _write_gyroscope_register(0x6D, bank);
        _write_gyroscope_register(0x6E, address);
		uint8_t buffer[UPLOAD_CHUNK_SIZE + 1];
		buffer[0] = 0x6F;
		memcpy(buffer + 1, data + offset, chunk_size);
		send_to_i2c(I2C_PORT_PERIPH, GYROSCOPE_I2C_ADDR, buffer, chunk_size + 1);

		// Read back chunk
        _write_gyroscope_register(0x6D, bank);
        _write_gyroscope_register(0x6E, address);
		send_to_i2c(I2C_PORT_PERIPH, GYROSCOPE_I2C_ADDR, buffer, 1);
		request_from_i2c(I2C_PORT_PERIPH, GYROSCOPE_I2C_ADDR, buffer, chunk_size);

		// Check for discrepencies
		if (memcmp(buffer, data + offset, chunk_size) != 0) {
			ESP_LOGE(TAG, "DMP buffer read back does not match");
			return ESP_ERR_INVALID_CRC;
		}

        offset += chunk_size;
    }
    ESP_LOGI(TAG, "DMP code uploaded");

	return ESP_OK;
}

float get_gyroscope_corrected_angle(size_t channel)
{
	assert(channel < 3);

	float angles[3];
	TickType_t current_time = xTaskGetTickCount();
	read_gyroscope(&angles[0], &angles[1], &angles[2]);
	float corrected_angle = angles[channel] - (current_time - initialization_time) * angle_drift[channel] - angle_offset[channel];

	return corrected_angle;
}

void calibrate_gyroscope_drift(TickType_t calibration_time)
{
	ESP_LOGI(TAG, "Performing gyroscope calibration");
	float before_value[3], after_value[3];
	initialization_time = xTaskGetTickCount();
	read_gyroscope(&before_value[0], &before_value[1], &before_value[2]);
	vTaskDelayUntil(&initialization_time, calibration_time);
	read_gyroscope(&after_value[0], &after_value[1], &after_value[2]);
	angle_drift[0] = (after_value[0] - before_value[0]) / calibration_time;
	angle_drift[1] = (after_value[1] - before_value[1]) / calibration_time;
	angle_drift[2] = (after_value[2] - before_value[2]) / calibration_time;
	ESP_LOGI(TAG, "Gyroscope calibration done. Drift: %f, %f, %f", angle_drift[0], angle_drift[1], angle_drift[2]);
}

void zero_out_gyroscope(void)
{
	float angles[3];
	read_gyroscope(&angles[0], &angles[1], &angles[2]);

	angle_offset[0] = angles[0];
	angle_offset[1] = angles[1];
	angle_offset[2] = angles[2];
}