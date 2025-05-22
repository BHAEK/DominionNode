#include "error_signaling.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "stdbool.h"
#include "leds.h"

const char * app_error_to_string(App_error_t error)
{
    switch (error)
    {
        case INIT_ERROR:
        {
            return "INITIALIZATION ERROR";
            break;
        }
    
        default:
        {
            return "UNKNOWN ERROR";
            break;
        }
    
    }

}

_Noreturn void signal_fatal_error(App_error_t error)
{
    
    ESP_LOGE(__func__, "A fatal error occurred: %s", app_error_to_string(error));

    switch (error)
    {
        
        case INIT_ERROR:
        {
            
            turn_all_leds_off();

            while(true)
            {
                turn_led_on(RED_LED);
                turn_led_off(BLUE_LED);
                vTaskDelay(pdMS_TO_TICKS(1000));
                turn_led_on(BLUE_LED);
                turn_led_off(RED_LED);
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
            
            break;
        
        }

        case BUTTON_ERROR:
        {
            
            turn_all_leds_off();

            while(true)
            {
                turn_led_on(RED_LED);
                turn_led_on(BLUE_LED);
                vTaskDelay(pdMS_TO_TICKS(100));
                turn_led_off(BLUE_LED);
                turn_led_off(RED_LED);
                vTaskDelay(pdMS_TO_TICKS(100));
            }
            
            break;
        
        }
    
        default:
        {
            turn_all_leds_off();

            while(true)
            {
                turn_led_on(RED_LED);
                turn_led_off(BLUE_LED);
                vTaskDelay(pdMS_TO_TICKS(3000));
                turn_led_on(BLUE_LED);
                turn_led_off(RED_LED);
                vTaskDelay(pdMS_TO_TICKS(3000));
            }
            
            break;
        
        }
    
    }

}