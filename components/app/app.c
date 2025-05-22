#include "error_signaling.h"
#include "stdbool.h"
#include "esp_log.h"

#include "app.h"
#include "leds.h"
#include "chrono.h"
#include "storage.h"

QueueHandle_t app_event_queue = NULL;
TimerHandle_t initial_setup_timer = NULL;

// GAME VARIABLES
ControlPoint_t control_point = CONTROL_POINT_NONE;
AppState_t current_state = APP_STATE_INIT;
Chrono_t blue_chrono = CHRONO_DEFAULT();
Chrono_t red_chrono = CHRONO_DEFAULT();

void initial_setup_timer_callback();

AppState_t get_app_state(void)
{
    return current_state;
}

void app_task(void* arg)
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
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Blue short press");
                            if(initial_setup_timer)
                            {
                                xTimerStop(initial_setup_timer, 0);
                            }
                            current_state = APP_STATE_IDLE;
                            break;
                        }
                        
                        case APP_EVENT_BTN_BLUE_MEDIUM:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Blue medium press");
                            current_state = APP_STATE_IDLE;
                            if(initial_setup_timer)
                            {
                                xTimerStop(initial_setup_timer, 0);
                            }
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
                            current_state = APP_STATE_IDLE;
                            if(initial_setup_timer)
                            {
                                xTimerStop(initial_setup_timer, 0);
                            }
                            break;
                        }
                        
                        case APP_EVENT_BTN_RED_MEDIUM:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Red medium press");
                            current_state = APP_STATE_IDLE;
                            if(initial_setup_timer)
                            {
                                xTimerStop(initial_setup_timer, 0);
                            }
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
                            current_state = APP_STATE_IDLE;
                            if(initial_setup_timer)
                            {
                                xTimerStop(initial_setup_timer, 0);
                            }
                            break;
                        }
                        
                        case APP_EVENT_BTN_BOTH_MEDIUM:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Both medium press");
                            current_state = APP_STATE_IDLE;
                            if(initial_setup_timer)
                            {
                                xTimerStop(initial_setup_timer, 0);
                            }
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
                            current_state = APP_STATE_SETTINGS_CONTROL_POINT;
                            ESP_LOGI(__func__, "SETTINGS - CONTROL POINT");
                            break;
                        }

                        case APP_EVENT_TMR_INIT_SETUP:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Init setup timer expired! Entering APP_STATE_IDLE...");
                            current_state = APP_STATE_IDLE;
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
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Blue short press");
                            current_state = APP_STATE_RUNNING_BLUE;
                            chrono_start(&blue_chrono);
                            chrono_stop(&red_chrono);
                            turn_led_on(BLUE_LED);
                            turn_led_off(RED_LED);
                            break;
                        }
                        
                        case APP_EVENT_BTN_BLUE_MEDIUM:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Blue medium press");
                            current_state = APP_STATE_RUNNING_BLUE;
                            chrono_start(&blue_chrono);
                            chrono_stop(&red_chrono);
                            turn_led_on(BLUE_LED);
                            turn_led_off(RED_LED);
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
                            current_state = APP_STATE_RUNNING_RED;
                            chrono_stop(&blue_chrono);
                            chrono_start(&red_chrono);
                            turn_led_off(BLUE_LED);
                            turn_led_on(RED_LED);
                            break;
                        }
                        
                        case APP_EVENT_BTN_RED_MEDIUM:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Red medium press");
                            current_state = APP_STATE_RUNNING_RED;
                            chrono_stop(&blue_chrono);
                            chrono_start(&red_chrono);
                            turn_led_off(BLUE_LED);
                            turn_led_on(RED_LED);
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
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Red short press");
                            current_state = APP_STATE_RUNNING_RED;
                            chrono_stop(&blue_chrono);
                            chrono_start(&red_chrono);
                            turn_led_off(BLUE_LED);
                            turn_led_on(RED_LED);
                            break;
                        }
                        
                        case APP_EVENT_BTN_RED_MEDIUM:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Red medium press");
                            current_state = APP_STATE_RUNNING_RED;
                            chrono_stop(&blue_chrono);
                            chrono_start(&red_chrono);
                            turn_led_off(BLUE_LED);
                            turn_led_on(RED_LED);
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
                            current_state = APP_STATE_FINISHED;
                            chrono_stop(&blue_chrono);
                            chrono_stop(&red_chrono);
                            turn_all_leds_on();
                            int blue_seconds = chrono_get_seconds(&blue_chrono);
                            int red_seconds = chrono_get_seconds(&red_chrono);
                            ESP_LOGI(__func__, "BLUE TEAM: %ds", blue_seconds);
                            ESP_LOGI(__func__, "RED TEAM:  %ds", red_seconds);
                            ESP_LOGI(__func__, "WIN %s TEAM!", blue_seconds >= red_seconds ? "BLUE" : "RED");
                            break;
                        }
                        
                        case APP_EVENT_BTN_BOTH_LONG:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Both long press");
                            current_state = APP_STATE_FINISHED;
                            chrono_stop(&blue_chrono);
                            chrono_stop(&red_chrono);
                            turn_all_leds_on();
                            int blue_seconds = chrono_get_seconds(&blue_chrono);
                            int red_seconds = chrono_get_seconds(&red_chrono);
                            ESP_LOGI(__func__, "BLUE TEAM: %ds", blue_seconds);
                            ESP_LOGI(__func__, "RED TEAM:  %ds", red_seconds);
                            ESP_LOGI(__func__, "WIN %s TEAM!", blue_seconds >= red_seconds ? "BLUE" : "RED");
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
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Blue short press");
                            current_state = APP_STATE_RUNNING_BLUE;
                            chrono_start(&blue_chrono);
                            chrono_stop(&red_chrono);
                            turn_led_on(BLUE_LED);
                            turn_led_off(RED_LED);
                            break;
                        }
                        
                        case APP_EVENT_BTN_BLUE_MEDIUM:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Blue medium press");
                            current_state = APP_STATE_RUNNING_BLUE;
                            chrono_start(&blue_chrono);
                            chrono_stop(&red_chrono);
                            turn_led_on(BLUE_LED);
                            turn_led_off(RED_LED);
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
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Both medium press");
                            current_state = APP_STATE_FINISHED;
                            chrono_stop(&blue_chrono);
                            chrono_stop(&red_chrono);
                            turn_all_leds_on();
                            int blue_seconds = chrono_get_seconds(&blue_chrono);
                            int red_seconds = chrono_get_seconds(&red_chrono);
                            ESP_LOGI(__func__, "BLUE TEAM: %ds", blue_seconds);
                            ESP_LOGI(__func__, "RED TEAM:  %ds", red_seconds);
                            ESP_LOGI(__func__, "WIN %s TEAM!", blue_seconds >= red_seconds ? "BLUE" : "RED");
                            break;
                        }
                        
                        case APP_EVENT_BTN_BOTH_LONG:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Both long press");
                            current_state = APP_STATE_FINISHED;
                            chrono_stop(&blue_chrono);
                            chrono_stop(&red_chrono);
                            turn_all_leds_on();
                            int blue_seconds = chrono_get_seconds(&blue_chrono);
                            int red_seconds = chrono_get_seconds(&red_chrono);
                            ESP_LOGI(__func__, "BLUE TEAM: %ds", blue_seconds);
                            ESP_LOGI(__func__, "RED TEAM:  %ds", red_seconds);
                            ESP_LOGI(__func__, "WIN %s TEAM!", blue_seconds >= red_seconds ? "BLUE" : "RED");
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
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Both medium press");
                            current_state = APP_STATE_IDLE;
                            chrono_reset(&blue_chrono);
                            chrono_reset(&red_chrono);
                            turn_all_leds_off();
                            break;
                        }
                        
                        case APP_EVENT_BTN_BOTH_LONG:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Both long press");
                            current_state = APP_STATE_IDLE;
                            chrono_reset(&blue_chrono);
                            chrono_reset(&red_chrono);
                            turn_all_leds_off();
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

                case APP_STATE_SETTINGS_CONTROL_POINT:
                {
                    switch (event.type)
                    {
                        
                        case APP_EVENT_BTN_BLUE_SHORT:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Blue short press");
                            current_state = APP_STATE_SETTINGS_EXIT;
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
                            current_state = APP_STATE_SETTINGS_CP_ALPHA;
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
                            current_state = APP_STATE_IDLE;
                            break;
                        }
                        
                        case APP_EVENT_BTN_BOTH_LONG:
                        {
                            ESP_LOGI(__func__, "STATE, EVENT: %d, %d", current_state, event.type);
                            ESP_LOGI(__func__, "Both long press");
                            current_state = APP_STATE_IDLE;
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
                    ESP_LOGE(__func__, "UNEXPECTED TRANSITION! WRONG STATE, EVENT: %d, %d", current_state, event.type);
                    break;
                }
                    
            }
        
        }
    
    }

}

void initial_setup_timer_callback()
{
    AppEventMessage_t event;
    event.type = APP_EVENT_TMR_INIT_SETUP;
    xQueueSend(app_event_queue, &event, APP_EVENT_ENQUEUE_TIMEOUT_MS);
}