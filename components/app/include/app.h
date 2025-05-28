#ifndef APP_H
#define APP_H

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
    APP_STATE_SETTINGS,
    // RUNNING
    APP_STATE_RUNNING_BLUE,
    APP_STATE_RUNNING_RED,
    // FINISHED
    APP_STATE_FINISHED,
} AppState_t;

typedef enum
{  
    APP_TEAM_INVALID = -1,
    APP_TEAM_RED,
    APP_TEAM_BLUE,
    APP_TEAM_COUNT,
} Team_t;

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


/**
 * @brief Converts an AppState_t value to a human-readable string.
 *
 * This function returns a string representation of the given application state.
 * It is mainly used for logging and debugging purposes.
 *
 * @param state The application state to convert.
 * @return A constant string describing the application state.
 */
const char * app_state_to_string(AppState_t state);

/**
 * @brief Converts an AppEvent_t value to a human-readable string.
 *
 * This function returns a string representation of the given application event.
 * It is useful for logging and debugging to easily identify events.
 *
 * @param event The application event to convert.
 * @return A constant string describing the application event.
 */
const char * app_event_to_string(AppEvent_t event);

void app_task(void* arg);

/**
 * @brief Returns the current application state.
 *
 * This function provides read-only access to the current state of the application
 * state machine. It can be used by external modules for monitoring or decision making.
 *
 * @return The current AppState_t value representing the application's state.
 */
AppState_t get_app_state(void);

#endif //APP_H