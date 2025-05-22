#include "stdint.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define APP_EVENT_ENQUEUE_TIMEOUT_MS    100
#define INITIAL_SETUP_TIME_MS           30000

typedef enum 
{
    APP_STATE_INIT,
    APP_STATE_IDLE,
    // SETTINGS
    APP_STATE_SETTINGS_CONTROL_POINT,
    APP_STATE_SETTINGS_CP_ALPHA,
    APP_STATE_SETTINGS_CP_BRAVO,
    APP_STATE_SETTINGS_CP_CHARLIE,
    APP_STATE_SETTINGS_CP_DELTA,
    APP_STATE_SETTINGS_CP_ECHO,
    APP_STATE_SETTINGS_CP_EXIT,
    APP_STATE_SETTINGS_EXIT,
    // RUNNING
    APP_STATE_RUNNING_BLUE,
    APP_STATE_RUNNING_RED,
    // FINISHED
    APP_STATE_FINISHED,
} AppState_t;

typedef enum 
{
    // TIMERS
    APP_EVENT_TMR_INIT_SETUP,
    // BUTTON PRESSION
    APP_EVENT_BTN_RED_SHORT,
    APP_EVENT_BTN_RED_MEDIUM,
    APP_EVENT_BTN_RED_LONG,
    APP_EVENT_BTN_BLUE_SHORT,
    APP_EVENT_BTN_BLUE_MEDIUM,
    APP_EVENT_BTN_BLUE_LONG,
    APP_EVENT_BTN_BOTH_SHORT,
    APP_EVENT_BTN_BOTH_MEDIUM,
    APP_EVENT_BTN_BOTH_LONG,
    // ...
} AppEvent_t;

typedef struct 
{
    AppEvent_t type;
    uint8_t * data;
    uint16_t data_len;
} AppEventMessage_t;

extern QueueHandle_t app_event_queue;

void app_task(void* arg);
AppState_t get_app_state(void);