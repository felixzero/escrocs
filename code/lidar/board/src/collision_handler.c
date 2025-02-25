#include "collision_handler.h"

esp_err_t init_collision(amalgame_t* pointer_to_amalgames, uint16_t maximum_amalg_length) {
    current_amalgames = pointer_to_amalgames;
    max_amalg_length = maximum_amalg_length;
    return ESP_OK;
}
int16_t read_all_amalgames(amalgame_t* amalgames) {
    return 0;
}

bool has_obstacle() {
    return true;
}
uint16_t closest_obstacle() {
    return 0;
}
esp_err_t update_cone(uint16_t angle) {
    return 0;
}
esp_err_t update_dist(uint16_t new_obstacle_trig_dist) {
    return 0;
}