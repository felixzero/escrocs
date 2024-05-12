#include "actions/strategy.h"
#include "actions/actions_lua_functions.h"
#include "system/spiffs.h"
#include "system/task_priority.h"
#include "peripherals/display.h"
#include "peripherals/motor_board_v3.h"
#include "controllers/motion_control.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_log.h>
#include <stdio.h>
#include <sys/stat.h>

#include "lua/lauxlib.h"
#include "lua/lua.h"
#include "lua/lualib.h"


#define TAG                         "Strategy"
#define DEFAULT_STRATEGY_NAME       "default"
#define MAX_STRATEGY_NAME_LENGTH    32
#define STRATEGY_PATH_PREFIX        "/storage/"
#define BLOCK_SIZE                  512
#define TRIGGER_POLLING_MS          500
#define MATCH_DURATION_MS           89000
#define LUA_ON_INIT_FUNCTION        "on_init"
#define LUA_ON_RUN_FUNCTION         "on_run"
#define LUA_RESUME_LOOP_FUNCTION    "resume_loop"
#define LUA_ON_END_FUNCTION         "on_end"

static QueueHandle_t file_to_execute_queue, on_run_queue, on_end_queue;
static TaskHandle_t lua_executor_task_handle;

static void get_strategy_file_name_from_query(httpd_req_t *req, char *name);
static void lua_executor_task(void *parameters);
static void trigger_timer_task(void *parameters);
static void end_match(void);

static int is_right = -1;

void init_lua_executor(bool is_reversed)
{
    init_lua_action_functions();
    is_right = is_reversed;

    // Create FreeRTOS task
    TaskHandle_t task;
    file_to_execute_queue = xQueueCreate(1, sizeof(char*));
    on_run_queue = xQueueCreate(1, sizeof(int));
    on_end_queue = xQueueCreate(1, sizeof(int));
    xTaskCreatePinnedToCore(lua_executor_task, "lua_executor", TASK_STACK_SIZE, (void*)&is_right, LUA_PRIORITY, &lua_executor_task_handle, LOW_CRITICITY_CORE);
    xTaskCreatePinnedToCore(trigger_timer_task, "trigger_timer", TASK_STACK_SIZE, NULL, TRIGGER_TIMER_PRIORITY, &task, LOW_CRITICITY_CORE);
}

void pick_strategy_by_spiffs_index(int index)
{
    char strategy_name[MAX_STRATEGY_NAME_LENGTH];
    list_spiffs_files(index, strategy_name, MAX_STRATEGY_NAME_LENGTH);
    
    char *strategy_file_name = malloc(sizeof(STRATEGY_PATH_PREFIX) + MAX_STRATEGY_NAME_LENGTH);
    sprintf(strategy_file_name, "%s%s", STRATEGY_PATH_PREFIX, strategy_name);
    ESP_LOGI(TAG, "Picked strategy with name %s (index %d)", strategy_file_name, index);

    struct stat st;
    if (stat(strategy_file_name, &st) == 0) {
        xQueueOverwrite(file_to_execute_queue, &strategy_file_name);
    } else {
        ESP_LOGE(TAG, "No default strategy given; ignoring");
    }
}

static void lua_executor_task(void *is_reversed)
{
    while (true) {
        char *filename;
        while (!xQueueReceive(file_to_execute_queue, &filename, 1000 / portTICK_PERIOD_MS));
        ESP_LOGI(TAG, "Executing strategy file: %s", filename);

        lua_State *L = luaL_newstate();
        luaL_openlibs(L);
        luaL_loadfile(L, filename);
        register_lua_action_functions(L);
        if (lua_pcall(L, 0, LUA_MULTRET, 0) != LUA_OK) {
            ESP_LOGE(TAG, "Lua error: %s", lua_tostring(L, -1));
            lcd_printf(1, "Lua error");
        }

        enable_motors();
        lua_getglobal(L, LUA_ON_INIT_FUNCTION);
        lua_pushboolean(L, *((int*)is_reversed));
        if (lua_pcall(L, 1, LUA_MULTRET, 0) != LUA_OK) {
            ESP_LOGE(TAG, "Lua error on_init: %s", lua_tostring(L, -1));
            lcd_printf(1, "Lua error init");
        }
        disable_motors();

        int queue_buffer;
        xQueueReceive(on_run_queue, &queue_buffer, portMAX_DELAY);

        enable_motors();
        vTaskDelay(pdMS_TO_TICKS(500));
        lua_getglobal(L, LUA_ON_RUN_FUNCTION);
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            ESP_LOGW(TAG, "Missing on_run function in Lua strategy.");
        }

        TickType_t iteration_time = xTaskGetTickCount();
        while (xQueuePeek(on_end_queue, &queue_buffer, 0) == pdFALSE)
        {
            lua_getglobal(L, LUA_RESUME_LOOP_FUNCTION);
            lua_pushinteger(L, pdTICKS_TO_MS(xTaskGetTickCount()));
            //arg is the timestamp in ms, receive the sleep time to wait before calling the coroutine again
            if(lua_pcall(L, 1, 1, 0) != LUA_OK) { //It either returns an error message or the sleep time
                ESP_LOGE(TAG, "Lua error resume_loop: %s", lua_tostring(L, -1));
                lcd_printf(1, "Lua error on main_loop");
            }
            else {
                lua_Integer sleep_time = lua_tointeger(L, -1);
                if (sleep_time == -1) {
                    break;
                }
                xTaskDelayUntil(&iteration_time, pdMS_TO_TICKS(sleep_time));
            }
        }

        free(filename);
    }
}

static void trigger_timer_task(void *parameters)
{
    bool has_key_been_inserted = false;
    while (!has_key_been_inserted) {
        if (read_trigger_key_status()) {
            lcd_printf(1, "Key missing");
        } else {
            lcd_printf(1, "Key inserted");
            has_key_been_inserted = true;
        }
        vTaskDelay(pdMS_TO_TICKS(TRIGGER_POLLING_MS));
    }

    while (!read_trigger_key_status()) {
        vTaskDelay(pdMS_TO_TICKS(TRIGGER_POLLING_MS));
    }

    lcd_printf(0, "00:00");
    lcd_printf(1, "");

    ESP_LOGI(TAG, "Starting key has been removed; starting strategy");
    enable_motors_and_set_timer();

    int queue_buffer = 1;
    xQueueOverwrite(on_run_queue, &queue_buffer);
    TickType_t match_start_time = xTaskGetTickCount();
    while (pdTICKS_TO_MS(xTaskGetTickCount() - match_start_time) < (MATCH_DURATION_MS - 1000)) {
        int elapsed_seconds = pdTICKS_TO_MS(xTaskGetTickCount() - match_start_time) / 1000;
        lcd_printf(0, "%02d:%02d", elapsed_seconds / 60, elapsed_seconds % 60);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    vTaskDelayUntil(&match_start_time, MATCH_DURATION_MS / portTICK_PERIOD_MS);

    while (true) {
        end_match();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

static void end_match(void)
{
    ESP_LOGI(TAG, "Match ended after %d ms", MATCH_DURATION_MS);
    int queue_buffer = 1;
    xQueueOverwrite(on_end_queue, &queue_buffer);
    vTaskDelay(pdMS_TO_TICKS(TRIGGER_POLLING_MS));
    disable_motors();
    lcd_printf(0, "Done");
    set_lua_action_function_enable_status(false);
}

esp_err_t get_strategy_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Received GET /strategy request");
    httpd_resp_set_type(req, "application/octet-stream");

    char strategy_name[MAX_STRATEGY_NAME_LENGTH];
    get_strategy_file_name_from_query(req, strategy_name);
    char strategy_file_name[sizeof(STRATEGY_PATH_PREFIX) + MAX_STRATEGY_NAME_LENGTH];
    sprintf(strategy_file_name, "%s%s", STRATEGY_PATH_PREFIX, strategy_name);
    FILE *strategy_file = fopen(strategy_file_name, "r");
    if (strategy_file == NULL) {
        ESP_LOGE(TAG, "Could not open file %s", strategy_file_name);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    char data_buffer[BLOCK_SIZE];
    size_t data_read;
    do {
        data_read = fread(data_buffer, 1, BLOCK_SIZE, strategy_file);
        httpd_resp_send_chunk(req, data_buffer, data_read);
    } while (data_read > 0);

    fclose(strategy_file);
    return ESP_OK;
}

esp_err_t put_strategy_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Received PUT /strategy request; downloading strategy");

    char strategy_name[MAX_STRATEGY_NAME_LENGTH];
    get_strategy_file_name_from_query(req, strategy_name);
    char strategy_file_name[sizeof(STRATEGY_PATH_PREFIX) + MAX_STRATEGY_NAME_LENGTH];
    sprintf(strategy_file_name, "%s%s", STRATEGY_PATH_PREFIX, strategy_name);
    FILE *strategy_file = fopen(strategy_file_name, "w");
    if (strategy_file == NULL) {
        ESP_LOGE(TAG, "Could not open file %s", strategy_file_name);
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    char data_buffer[BLOCK_SIZE];
    int written_length = 0;
    while (written_length < req->content_len) {
        int data_read = httpd_req_recv(req, data_buffer, BLOCK_SIZE);
        fwrite(data_buffer, 1, data_read, strategy_file);
        written_length += data_read;
    }
    add_spiffs_file_entry(strategy_name);

    httpd_resp_send(req, "All is fine, captain!\n", HTTPD_RESP_USE_STRLEN);
    fclose(strategy_file);
    return ESP_OK;
}

esp_err_t delete_strategy_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Received DELETE /strategy request; formatting everything");
    format_spiffs();
    httpd_resp_send(req, "Storage partition is now wiped\n", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t run_strategy_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Received POST /strategy/run request; executing strategy");
    char strategy_name[MAX_STRATEGY_NAME_LENGTH];
    get_strategy_file_name_from_query(req, strategy_name);
    char *strategy_file_name = malloc(sizeof(STRATEGY_PATH_PREFIX) + MAX_STRATEGY_NAME_LENGTH);
    sprintf(strategy_file_name, "%s%s", STRATEGY_PATH_PREFIX, strategy_name);

    struct stat st;
    if (stat(strategy_file_name, &st) != 0) {
        ESP_LOGW(TAG, "Could not execute strategy, file does not exist");
        httpd_resp_send_404(req);
        return ESP_OK;
    }

    xQueueOverwrite(file_to_execute_queue, &strategy_file_name);
    httpd_resp_send(req, "All is fine, captain!\n", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static void get_strategy_file_name_from_query(httpd_req_t *req, char *name)
{
    char buffer[BLOCK_SIZE];
    httpd_req_get_url_query_str(req, buffer, BLOCK_SIZE);
    if (httpd_query_key_value(buffer, "name", name, MAX_STRATEGY_NAME_LENGTH) == ESP_ERR_NOT_FOUND) {
        strcpy(name, DEFAULT_STRATEGY_NAME);
    }
}