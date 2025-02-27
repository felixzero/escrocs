#include "collision_handler.h"

amalgame_t* current_amalgames;
static uint8_t cur_amalg_count;
uint16_t max_amalg_length;
esp_err_t init_collision(amalgame_t* pointer_to_amalgames, uint16_t maximum_amalg_length) {
    current_amalgames = pointer_to_amalgames;
    max_amalg_length = maximum_amalg_length;
    return ESP_OK;
}
int16_t read_all_amalgames(amalgame_t* amalgames) {
    return 0;
}

bool has_obstacle() {
    for (size_t i = 0; i < max_amalg_length; i++)
    {
        /* code */
    }
    
    return true;
}
uint16_t closest_obstacle() {
    return 0;
}
esp_err_t update_cone(uint16_t angle) {
    return 0;
}
esp_err_t update_dist(uint16_t new_obstacle_trig_dist) {
    obstacle_trig_dist = new_obstacle_trig_dist;
    return ESP_OK;
}