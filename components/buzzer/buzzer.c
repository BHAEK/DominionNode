#include "buzzer.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

EventGroupHandle_t buzzer_event_group;

esp_err_t buzzer_init()
{
    
    esp_err_t ret = ESP_OK;

    gpio_config_t io_conf_buzzer = 
    {
        .pin_bit_mask = (1ULL << GPIO_BUZZER),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    
    ret = gpio_config(&io_conf_buzzer);
    if(ESP_OK != ret)
    {
        ESP_LOGE(__func__, "Error calling gpio_config: %s", esp_err_to_name(ret));
        return ret;
    }

    buzzer_event_group = xEventGroupCreate();
    if (buzzer_event_group == NULL) 
    {
        ESP_LOGE(__func__, "Failed to create buzzer_event_group!");
        return ESP_FAIL;
    }

    return ret;

}

void buzzer_on()
{

    gpio_set_level(GPIO_BUZZER, 1);

}

void buzzer_off()
{

    gpio_set_level(GPIO_BUZZER, 0);

}

void buzzer_task(void * arg)
{
    
    while(true)
    {

        EventBits_t bits = xEventGroupWaitBits(buzzer_event_group, BUZZER_EVENT_ALL, pdTRUE, pdFALSE, portMAX_DELAY);

        switch (bits)
        {
            
            case BUZZER_EVENT_ON:
            {
                // Handle ON
                break;
            }

            case BUZZER_EVENT_OFF:
            {
                // Handle OFF
                break;
            }

            case BUZZER_EVENT_BLINK:
            {
                // Handle BLINK
                break;
            }

            case BUZZER_EVENT_HELLO:
            {
                // Handle HELLO
                break;
            }

            case BUZZER_EVENT_OK:
            {
                // Handle OK
                break;
            }

            case BUZZER_EVENT_SWITCH:
            {
                // Handle SWITCH
                break;
            }

            case BUZZER_EVENT_FINISH:
            {
                // Handle FINISH
                break;
            }

            default:
            {
                // Unknown event
                break;
            }
        
        }

    }

}