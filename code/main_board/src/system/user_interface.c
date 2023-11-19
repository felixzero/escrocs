#include "user_interface.h"
#include "peripherals/display.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define OK_MESSAGE_LENGTH_MS    500
#define ERR_MESSAGE_LENGTH_MS   portMAX_DELAY

#define BLINKING_PERIOD         300
#define BLINKING_NUMBER         2

void display_initialization_status(const char *name, esp_err_t status)
{
    if (status == ESP_OK) {
        lcd_printf(0, "%s: OK", name);
        lcd_printf(1, "");
        vTaskDelay(pdMS_TO_TICKS(OK_MESSAGE_LENGTH_MS));
    } else {
        lcd_printf(0, "%s: err", name);
        lcd_printf(1, "Error %d", status);
        vTaskDelay(pdMS_TO_TICKS(ERR_MESSAGE_LENGTH_MS));
    }
}

int menu_pick_item(const char *description, char **menu_entries, size_t menu_length)
{
    int current_picked_item = 0;
    lcd_printf(0, description);

    if(menu_length <= 0) {
        lcd_printf(1, "-- no entries --");
        vTaskDelay(portMAX_DELAY);
        return -1;
    }

    lcd_printf(1, menu_entries[current_picked_item]);

    while (true) {
        rotary_encoder_event_t event = poll_rotary_encoder_event();
        if (event == ROTARY_ENCODER_PRESSED) {
            break;
        } else if (event == ROTARY_ENCODER_TURN_CCW) {
            current_picked_item--;
            if (current_picked_item < 0) {
                current_picked_item = menu_length - 1;
            }
            lcd_printf(1, menu_entries[current_picked_item]);
        } else if (event == ROTARY_ENCODER_TURN_CW) {
            current_picked_item++;
            if (current_picked_item >= menu_length) {
                current_picked_item = 0;
            }
            lcd_printf(1, menu_entries[current_picked_item]);
        }
        vTaskDelay(1);
    }

    lcd_printf(0, "Selected :");
    for (int i = 0; i < BLINKING_NUMBER; ++i) {
        lcd_printf(1, menu_entries[current_picked_item]);
        vTaskDelay(pdMS_TO_TICKS(BLINKING_PERIOD));
        lcd_printf(1, "");
        vTaskDelay(pdMS_TO_TICKS(BLINKING_PERIOD));
    }
    return current_picked_item;
}
