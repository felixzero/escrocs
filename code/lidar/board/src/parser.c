#include "parser.h"

#include "esp_log.h"

static uint8_t data_length = 1;

static LiDARFrameTypeDef* frames[MAX_NB_FRAMES_FULL_SCAN];
static uint8_t cur_frame_i = 0;

static float total_angle = 0;

static uint8_t CalcCRC8(uint8_t *p, uint8_t len)
{
    uint8_t crc = 0;
    uint16_t i;
    for (i = 0; i < len; i++)
    {
    crc = CrcTable[(crc ^ *p++) & 0xff];
    }
    return crc;
}

bool check_update_full_circle(LiDARFrameTypeDef *frame) {
    if (frame->start_angle > frame->end_angle) {
        total_angle += 36000 - frame->start_angle + frame->end_angle;
    } else {
        total_angle += frame->end_angle - frame->start_angle;
    }
    if (total_angle >= 36000) {
        total_angle = 0;
        return true;
    }
    return false;
}

// Give the expected next length of the data
uint8_t parse_chunk(uint8_t data[], bool *full_scan) {
    if (cur_frame_i >= MAX_NB_FRAMES_FULL_SCAN)
    {
        ESP_LOGI("parser", "Max number of frames reached");
    } 
    
    *full_scan = false;
    switch (state) {
        case HEADER:
            if (data[0] == 0x54) {
                state = LEN;
            }
            data_length = 1;
            break;
        case LEN:
            if (data[0] != 0x2C)
            {
                state = HEADER;
                data_length = 1;
                break;
            }
            state = DATA;
            data_length = 9 + 3 * (data[0] & 0x0E); // 9 + 3 * (12)
            break;
        case DATA:
            frames[cur_frame_i] = (LiDARFrameTypeDef*) malloc(sizeof(LiDARFrameTypeDef));
            LiDARFrameTypeDef *frame = frames[cur_frame_i];

            frame->header = 0x54;
            frame->ver_len = 0x2C;
            frame->speed = data[0] | (data[1] << 8);
            frame->start_angle = data[2] | (data[3] << 8);
            frame->end_angle = data[data_length-5] | (data[data_length-4] << 8);
            frame->timestamp = data[data_length-3] | (data[data_length-2] << 8);
            frame->crc8 = data[data_length-1];

            for (size_t i = 0; i < 12; i++)
            {
                frame->points[i] = data[4 + 3 * i] | (data[4 + 1 + 3 * i] << 8);
                frame->intensities[i] = data[6 + 3 * i];
            }
            
            state = HEADER;
            data_length = 1;
            cur_frame_i++;
            *full_scan = check_update_full_circle(frame);

            break;
    }
    //TODO DATA VALIDATION CRC
    //CalcCRC8(frame, sizeof LiDARFrameTypeDef)
    return data_length;

} 

void parse_frames(raw_lidar_t *out_lidar) {
    out_lidar->count = (1+cur_frame_i) * 12;
    out_lidar->angles = (uint16_t*) malloc(out_lidar->count * sizeof(uint16_t));
    out_lidar->distances = (uint16_t*) malloc(out_lidar->count * sizeof(uint16_t));
    out_lidar->intensities = (uint8_t*) malloc(out_lidar->count * sizeof(uint8_t));
    for (size_t i = 0; i < cur_frame_i; i++)
    {
        float step_angle = (frames[i]->start_angle > frames[i]->end_angle) ?
            (36000 - frames[i]->start_angle + frames[i]->end_angle) / 12.0f :
            (frames[i]->end_angle - frames[i]->start_angle) / 12.0f;

        for (size_t j = 0; j < 12; j++) {
            if (frames[i]->start_angle + j * step_angle >= 36000)
            {
                frames[i]->start_angle = frames[i]->start_angle + j * step_angle - 36000;
            }
            out_lidar->angles[i * 12 + j] = frames[i]->start_angle + j * step_angle;
            out_lidar->distances[i * 12 + j] = frames[i]->points[j];
            out_lidar->intensities[i * 12 + j] = frames[i]->intensities[j];

        }
        free(frames[i]);
    }
    cur_frame_i = 0;
    
}