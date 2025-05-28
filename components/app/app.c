#include "error_signaling.h"
#include "stdbool.h"
#include "esp_log.h"

#include "app.h"
#include "leds.h"
#include "chrono.h"
#include "storage.h"
#include "settings.h"
#include "display.h"
#include "buzzer.h"

QueueHandle_t app_event_queue = NULL;
TimerHandle_t initial_setup_timer = NULL;
TaskHandle_t display_task_handle = NULL;

// GAME VARIABLES
ControlPoint_t control_point = CONTROL_POINT_NONE;
AppState_t current_state = APP_STATE_INIT;
Chrono_t blue_chrono = CHRONO_DEFAULT();
Chrono_t red_chrono = CHRONO_DEFAULT();

/**
 * @brief Timer callback for the initial setup phase.
 *
 * This function is triggered by a software or hardware timer to signal that
 * the initial setup period has elapsed. It creates an event of type
 * APP_EVENT_TMR_INIT_SETUP and enqueues it into the application's event queue.
 *
 * The event is sent asynchronously using xQueueSend. If the queue is full,
 * the function will wait up to APP_EVENT_ENQUEUE_TIMEOUT_MS milliseconds.
 *
 * @note This function is intended to be used as a FreeRTOS timer callback.
 */
void initial_setup_timer_callback();

/**
 * @brief Enters the SETTINGS state of the application.
 * 
 * This function sets the current state to APP_STATE_SETTINGS
 * and initializes the settings module.
 */
void app_enter_setting(void);

/**
 * @brief Enters the IDLE state of the application.
 * 
 * Resets both chronometers, turns off all LEDs, and clears both displays.
 */
void app_enter_idle(void);

/**
 * @brief Enters the RUNNING state for the specified team.
 * 
 * Starts the selected team's chronometer, stops the other,
 * updates the LEDs accordingly, and starts/resumes the display update task.
 * 
 * @param team The team to set as currently running (APP_TEAM_RED or APP_TEAM_BLUE).
 */
void app_enter_running(Team_t team);

/**
 * @brief Switches the active team during the RUNNING state.
 * 
 * If the application is running with one team, this function switches control
 * to the other team by stopping the current timer and starting the new one.
 * 
 * @param new_team The team to activate (APP_TEAM_RED or APP_TEAM_BLUE).
 */
void app_set_team(Team_t new_team);

/**
 * @brief Enters the FINISHED state of the application.
 * 
 * Stops both chronometers, turns on all LEDs, suspends the display task,
 * and logs the final scores and winning team.
 */
void app_enter_finished(void);

/**
 * @brief Main application FreeRTOS task.
 * 
 * Initializes the application state, retrieves persistent settings (e.g., control point),
 * sets up the initial setup timer, and enters the main event loop.
 * 
 * Inside the loop, it waits for events on the application event queue and
 * handles state transitions accordingly.
 * 
 * @param arg Unused task parameter (set to NULL).
 */
void app_task(void * arg);

/**
 * @brief FreeRTOS task that updates the red and blue displays with the current timer values.
 *
 * Periodically checks the red and blue chronometers and updates the corresponding displays
 * only if the time has changed. The task delays based on DISPLAY_REFRESH_RATE_MS.
 *
 * This task is designed to run concurrently with other system tasks and ensures efficient
 * display updates without unnecessary redraws.
 *
 * @param arg Unused task parameter (set to NULL).
 */
void display_chrono_task(void * arg);

AppState_t get_app_state(void)
{
    return current_state;
}

void app_task(void * arg)
{
    
    current_state = APP_STATE_INIT;

    esp_err_t storage_err = storage_get_control_point(&control_point);
    if(ESP_OK != storage_err)
    {
        ESP_LOGE(__func__, "Error getting control point! %s", esp_err_to_name(storage_err));
        control_point = CONTROL_POINT_ALPHA;
    }

    ESP_LOGI(__func__, "CONTROL POINT: %s", control_point_to_string(control_point));

    app_event_queue = xQueueCreate(10, sizeof(AppEventMessage_t));    
    if (!app_event_queue) 
    {
        ESP_LOGE(__func__, "Error creating app_event_queue");
        signal_fatal_error(INIT_ERROR);
    }

    initial_setup_timer = xTimerCreate("initial_setup", pdMS_TO_TICKS(INITIAL_SETUP_TIME_MS), pdFALSE, NULL, initial_setup_timer_callback);
    if(!initial_setup_timer)
    {
        ESP_LOGE(__func__, "Error creating initial_setup_timer");
        signal_fatal_error(INIT_ERROR);
    }
  
    BaseType_t timer_error = xTimerStart(initial_setup_timer, 0);
    if(pdFAIL == timer_error)
    {
        ESP_LOGE(__func__, "Error starting initial_setup_timer...");
        ESP_LOGE(__func__, "Setting state to APP_STATE_IDLE istantly!");
        current_state = APP_STATE_IDLE;
    }

    display_set_string(DISPLAY_RED, "HI");
    display_set_fw_version(DISPLAY_BLUE, FW_VERSION);

    xEventGroupSetBits(buzzer_event_group, BUZZER_EVENT_HELLO);

    while (true) 
    {
        
        AppEventMessage_t event;
        
        if (xQueueReceive(app_event_queue, &event, portMAX_DELAY)) 
        {

            switch (current_state) 
            {
                
                case APP_STATE_INIT:
                {
                    
                    switch (event.type)
                    {
                        
                        case APP_EVENT_BTN_BLUE_SHORT:
                        case APP_EVENT_BTN_RED_SHORT:
                        case APP_EVENT_BTN_BLUE_MEDIUM:
                        case APP_EVENT_BTN_RED_MEDIUM:
                        case APP_EVENT_BTN_BOTH_SHORT:
                        case APP_EVENT_BTN_BOTH_MEDIUM:
                        case APP_EVENT_TMR_INIT_SETUP:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            if(initial_setup_timer)
                            {
                                xTimerStop(initial_setup_timer, 0);
                            }                            
                            
                            app_enter_idle();
                            
                            break;
                        }
                                             
                        case APP_EVENT_BTN_BLUE_LONG:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Blue long press");
                            break;
                        }
                                                
                        case APP_EVENT_BTN_RED_LONG:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Red long press");
                            break;
                        }
                        
                        case APP_EVENT_BTN_BOTH_LONG:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Both long press");
                            if(initial_setup_timer)
                            {
                                xTimerStop(initial_setup_timer, 0);
                            }

                            app_enter_setting();

                            break;
                        }

                        default:
                        {
                            ESP_LOGE(__func__, "UNEXPECTED TRANSITION! STATE, WRONG EVENT: %d, %d", current_state, event.type);
                            break;
                        }

                    }
                    
                    break;

                }

                case APP_STATE_IDLE:
                {
                    
                    switch (event.type)
                    {
                        
                        case APP_EVENT_BTN_BLUE_SHORT:
                        case APP_EVENT_BTN_BLUE_MEDIUM:
                        {
                            
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Blue short press");
                            
                            app_enter_running(APP_TEAM_BLUE);

                            break;
                        
                        }
                        
                        case APP_EVENT_BTN_BLUE_LONG:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Blue long press");
                            break;
                        }
                        
                        case APP_EVENT_BTN_RED_SHORT:
                        case APP_EVENT_BTN_RED_MEDIUM:
                        {
                            
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Red short press");
                            
                            app_enter_running(APP_TEAM_RED);

                            break;
                        
                        }
                        
                        case APP_EVENT_BTN_RED_LONG:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Red long press");
                            break;
                        }
                        
                        case APP_EVENT_BTN_BOTH_SHORT:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Both short press");
                            break;
                        }
                        
                        case APP_EVENT_BTN_BOTH_MEDIUM:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Both medium press");
                            break;
                        }
                        
                        case APP_EVENT_BTN_BOTH_LONG:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Both long press");
                            break;
                        }

                        default:
                        {
                            ESP_LOGE(__func__, "UNEXPECTED TRANSITION! STATE, WRONG EVENT: %d, %d", current_state, event.type);
                            break;
                        }

                    }
                    
                    break;

                }

                case APP_STATE_RUNNING_BLUE:
                {
                    
                    switch (event.type)
                    {
                        
                        case APP_EVENT_BTN_BLUE_SHORT:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Blue short press");
                            break;
                        }
                        
                        case APP_EVENT_BTN_BLUE_MEDIUM:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Blue medium press");
                            break;
                        }
                        
                        case APP_EVENT_BTN_BLUE_LONG:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Blue long press");
                            break;
                        }
                        
                        case APP_EVENT_BTN_RED_SHORT:
                        case APP_EVENT_BTN_RED_MEDIUM:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Red short press");

                            app_set_team(APP_TEAM_RED);

                            break;
                        }
                                           
                        case APP_EVENT_BTN_RED_LONG:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Red long press");
                            break;
                        }
                        
                        case APP_EVENT_BTN_BOTH_SHORT:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Both short press");
                            break;
                        }
                        
                        case APP_EVENT_BTN_BOTH_MEDIUM:
                        case APP_EVENT_BTN_BOTH_LONG:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Both medium press");

                            app_enter_finished();

                            break;
                        }

                        default:
                        {
                            ESP_LOGE(__func__, "UNEXPECTED TRANSITION! STATE, WRONG EVENT: %d, %d", current_state, event.type);
                            break;
                        }

                    }
                    
                    break;

                }

                case APP_STATE_RUNNING_RED:
                {
                    
                    switch (event.type)
                    {
                        
                        case APP_EVENT_BTN_BLUE_SHORT:
                        case APP_EVENT_BTN_BLUE_MEDIUM:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Blue short press");
                            
                            app_set_team(APP_TEAM_BLUE);

                            break;
                        
                        }
                        
                        case APP_EVENT_BTN_BLUE_LONG:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Blue long press");
                            break;
                        }
                        
                        case APP_EVENT_BTN_RED_SHORT:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Red short press");
                            break;
                        }
                        
                        case APP_EVENT_BTN_RED_MEDIUM:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Red medium press");
                            break;
                        }
                        
                        case APP_EVENT_BTN_RED_LONG:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Red long press");
                            break;
                        }
                        
                        case APP_EVENT_BTN_BOTH_SHORT:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Both short press");
                            break;
                        }
                        
                        case APP_EVENT_BTN_BOTH_MEDIUM:
                        case APP_EVENT_BTN_BOTH_LONG:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Both medium press");
                            
                            app_enter_finished();

                            break;
                        }
                        
                        default:
                        {
                            ESP_LOGE(__func__, "UNEXPECTED TRANSITION! STATE, WRONG EVENT: %d, %d", current_state, event.type);
                            break;
                        }

                    }
                    
                    break;

                }

                case APP_STATE_FINISHED:
                {
                    
                    switch (event.type)
                    {
                        
                        case APP_EVENT_BTN_BLUE_SHORT:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Blue short press");
                            break;
                        }
                        
                        case APP_EVENT_BTN_BLUE_MEDIUM:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Blue medium press");
                            break;
                        }
                        
                        case APP_EVENT_BTN_BLUE_LONG:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Blue long press");
                            break;
                        }
                        
                        case APP_EVENT_BTN_RED_SHORT:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Red short press");
                            break;
                        }
                        
                        case APP_EVENT_BTN_RED_MEDIUM:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Red medium press");
                            break;
                        }
                        
                        case APP_EVENT_BTN_RED_LONG:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Red long press");
                            break;
                        }
                        
                        case APP_EVENT_BTN_BOTH_SHORT:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Both short press");
                            break;
                        }
                        
                        case APP_EVENT_BTN_BOTH_MEDIUM:
                        case APP_EVENT_BTN_BOTH_LONG:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Both medium press");
                            
                            app_enter_idle();

                            break;
                        }

                        default:
                        {
                            ESP_LOGE(__func__, "UNEXPECTED TRANSITION! STATE, WRONG EVENT: %d, %d", current_state, event.type);
                            break;
                        }

                    }
                    
                    break;

                }

                case APP_STATE_SETTINGS:
                {
                    switch (event.type)
                    {
                        
                        case APP_EVENT_BTN_BLUE_SHORT:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Blue short press");
                            settings_next();
                            break;
                        }
                        
                        case APP_EVENT_BTN_BLUE_MEDIUM:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Blue medium press");
                            break;
                        }
                        
                        case APP_EVENT_BTN_BLUE_LONG:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Blue long press");
                            break;
                        }
                        
                        case APP_EVENT_BTN_RED_SHORT:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Red short press");
                            if (settings_get_current() == SETTING_EXIT) 
                            {
                                settings_save();
                                current_state = APP_STATE_IDLE;
                                ESP_LOGI(__func__, "EXITED SETTINGS");                                                            
                                
                                app_enter_idle();

                            } 
                            else 
                            {
                                settings_modify_current();
                            }
                            break;
                        }
                        
                        case APP_EVENT_BTN_RED_MEDIUM:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Red medium press");
                            break;
                        }
                        
                        case APP_EVENT_BTN_RED_LONG:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Red long press");
                            break;
                        }
                        
                        case APP_EVENT_BTN_BOTH_SHORT:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Both short press");
                            break;
                        }
                        
                        case APP_EVENT_BTN_BOTH_MEDIUM:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Both medium press");
                            break;
                        }
                        
                        case APP_EVENT_BTN_BOTH_LONG:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Both long press");
                            break;
                        }

                        default:
                        {
                            ESP_LOGE(__func__, "UNEXPECTED TRANSITION! STATE, WRONG EVENT: %d, %d", current_state, event.type);
                            break;
                        }

                    }
                    
                    break;

                }

                default:
                {
                    ESP_LOGE(__func__, "UNEXPECTED TRANSITION! WRONG STATE, EVENT: %s, %s", app_state_to_string(current_state), app_event_to_string(event.type));
                    break;
                }
                    
            }
        
        }
    
    }

}

void display_chrono_task(void * arg)
{

    int seconds_red = chrono_get_seconds(&red_chrono);
    int seconds_blue = chrono_get_seconds(&blue_chrono);

    while(true)
    {
        
        if(seconds_red != chrono_get_seconds(&red_chrono))
        {
            display_set_time(DISPLAY_RED, chrono_get_seconds(&red_chrono), true);
        }
        
        if(seconds_blue != chrono_get_seconds(&blue_chrono))
        {
            display_set_time(DISPLAY_BLUE, chrono_get_seconds(&blue_chrono), true);
        }

        vTaskDelay(pdMS_TO_TICKS(DISPLAY_REFRESH_RATE_MS));
    
    }

}

void initial_setup_timer_callback()
{
    AppEventMessage_t event;
    event.type = APP_EVENT_TMR_INIT_SETUP;
    xQueueSend(app_event_queue, &event, APP_EVENT_ENQUEUE_TIMEOUT_MS);
}

void app_enter_setting()
{
    current_state = APP_STATE_SETTINGS;
    settings_init();
    ESP_LOGI(__func__, "ENTERED SETTINGS");
}

void app_enter_idle(void)
{
    current_state = APP_STATE_IDLE;
    chrono_reset(&blue_chrono);
    chrono_reset(&red_chrono);
    turn_all_leds_off();
    display_set_time(DISPLAY_RED, 0, true);
    display_set_time(DISPLAY_BLUE, 0, true);
}

void app_enter_running(Team_t team)
{
    switch (team)
    {
        
        case APP_TEAM_RED:
        {
            
            current_state = APP_STATE_RUNNING_RED;
            chrono_start(&red_chrono);
            chrono_stop(&blue_chrono);
            turn_led_on(RED_LED);
            turn_led_off(BLUE_LED);
            
            break;
        
        }

        case APP_TEAM_BLUE:
        {
            
            current_state = APP_STATE_RUNNING_BLUE;
            chrono_start(&blue_chrono);
            chrono_stop(&red_chrono);
            turn_led_on(BLUE_LED);
            turn_led_off(RED_LED);
            
            break;
        
        }
        
        default:
        {
            ESP_LOGE(__func__, "Invalid TEAM: %d", team);
            return;
        }
            
    }
    
    if(!display_task_handle)
    {
        BaseType_t display_task_error = xTaskCreate(display_chrono_task, "display_chrono", DISPLAY_TASK_STACK_DEPTH, NULL, DISPLAY_TASK_PRIORITY, &display_task_handle);
        if(pdPASS != display_task_error)
        {
            ESP_LOGE(__func__, "Error creating display task!");
        }
    }
    else
    {
        vTaskResume(display_task_handle);
    }

    xEventGroupSetBits(buzzer_event_group, BUZZER_EVENT_SWITCH);

}

void app_set_team(Team_t new_team)
{
    switch (new_team)
    {
        
        case APP_TEAM_RED:
        {
            
            if(current_state == APP_STATE_RUNNING_RED)
            {
                // do nothing
                return;
            }
            else if(current_state == APP_STATE_RUNNING_BLUE)
            {
                current_state = APP_STATE_RUNNING_RED;
                chrono_stop(&blue_chrono);
                chrono_start(&red_chrono);
                turn_led_off(BLUE_LED);
                turn_led_on(RED_LED);
            }
            else
            {
                ESP_LOGE(__func__, "Function called in an invalid state: %s", app_state_to_string(current_state));
                return;
            }

            break;
        
        }

        case APP_TEAM_BLUE:
        {
            
            if(current_state == APP_STATE_RUNNING_BLUE)
            {
                // do nothing
                return;
            }
            else if(current_state == APP_STATE_RUNNING_RED)
            {
                current_state = APP_STATE_RUNNING_BLUE;
                chrono_stop(&red_chrono);
                chrono_start(&blue_chrono);
                turn_led_off(RED_LED);
                turn_led_on(BLUE_LED);
            }
            else
            {
                ESP_LOGE(__func__, "Function called in an invalid state: %s", app_state_to_string(current_state));
                return;
            }
            
            break;
            
        }

        default:
        {
            ESP_LOGE(__func__, "Invalid TEAM: %d", new_team);
            return;
        }

    }

    xEventGroupSetBits(buzzer_event_group, BUZZER_EVENT_SWITCH);

}

void app_enter_finished(void)
{
    
    current_state = APP_STATE_FINISHED;
    
    chrono_stop(&blue_chrono);
    chrono_stop(&red_chrono);
    
    turn_all_leds_on();
    
    if(display_task_handle)
    {
        vTaskSuspend(display_task_handle);
    }
    
    int blue_seconds = chrono_get_seconds(&blue_chrono);
    int red_seconds = chrono_get_seconds(&red_chrono);
    
    ESP_LOGI(__func__, "BLUE TEAM: %ds", blue_seconds);
    ESP_LOGI(__func__, "RED TEAM:  %ds", red_seconds);
    ESP_LOGI(__func__, "WIN %s TEAM!", blue_seconds >= red_seconds ? "BLUE" : "RED");
    
    xEventGroupSetBits(buzzer_event_group, BUZZER_EVENT_FINISH);

}

const char * app_state_to_string(AppState_t state)
{
    switch (state)
    {
        case APP_STATE_INIT:
            return "INIT";
        case APP_STATE_IDLE:
            return "IDLE";
        case APP_STATE_SETTINGS:
            return "SETTINGS";
        case APP_STATE_RUNNING_BLUE:
            return "RUNNING_BLUE";
        case APP_STATE_RUNNING_RED:
            return "RUNNING_RED";
        case APP_STATE_FINISHED:
            return "FINISHED";
        default:
            return "UNKNOWN_STATE";
    }
}

const char * app_event_to_string(AppEvent_t event)
{
    switch (event)
    {
        // TIMERS
        case APP_EVENT_TMR_INIT_SETUP:
            return "TMR_INIT_SETUP";

        // BUTTON PRESSION
        case APP_EVENT_BTN_RED_SHORT:
            return "BTN_RED_SHORT";
        case APP_EVENT_BTN_RED_MEDIUM:
            return "BTN_RED_MEDIUM";
        case APP_EVENT_BTN_RED_LONG:
            return "BTN_RED_LONG";

        case APP_EVENT_BTN_BLUE_SHORT:
            return "BTN_BLUE_SHORT";
        case APP_EVENT_BTN_BLUE_MEDIUM:
            return "BTN_BLUE_MEDIUM";
        case APP_EVENT_BTN_BLUE_LONG:
            return "BTN_BLUE_LONG";

        case APP_EVENT_BTN_BOTH_SHORT:
            return "BTN_BOTH_SHORT";
        case APP_EVENT_BTN_BOTH_MEDIUM:
            return "BTN_BOTH_MEDIUM";
        case APP_EVENT_BTN_BOTH_LONG:
            return "BTN_BOTH_LONG";

        // Fallback
        default:
            return "UNKNOWN_EVENT";
    }
}