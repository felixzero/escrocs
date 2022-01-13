#include <driver/uart.h>
#include <esp_log.h>

#include "motor_control.h"
#include "uart_receiver.h"
#include "wireless/wifi.h"
#include "wireless/httpd.h"

void app_main()
{
    init_wifi_system();
    init_http_server();
    init_motor_control();
    init_uart_receiver();

    while (1) {
        vTaskDelay(1);
    }
}
