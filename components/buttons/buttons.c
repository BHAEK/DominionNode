#include "buttons.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "error_signaling.h"
#include "app.h"

EventGroupHandle_t button_event_group = NULL;

void gpio_button_isr_handler(void* arg);

esp_err_t button_init()
{
    
    esp_err_t ret = ESP_OK;

    gpio_config_t io_conf_btn = 
    {
        .pin_bit_mask = (1ULL << GPIO_BTN_RED) | (1ULL << GPIO_BTN_BLUE),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE,
    };
    
    ret = gpio_config(&io_conf_btn);
    if(ESP_OK != ret)
    {
        ESP_LOGE(__func__, "Error calling gpio_config: %s", esp_err_to_name(ret));
        return ret;
    }

    if(gpio_get_level(GPIO_BTN_RED) == 0 || gpio_get_level(GPIO_BTN_BLUE) == 0)
    {
        ESP_LOGE(__func__, "BUTTON %s IS PRESSED AT STARTUP OR IS DAMAGED!", gpio_get_level(GPIO_BTN_RED) == 0 ? "RED" : "BLUE");
        signal_fatal_error(BUTTON_ERROR);
    }

    button_event_group = xEventGroupCreate();

    if(button_event_group == NULL)
    {
        ESP_LOGE(__func__, "Error creating button_event_group");
        return ESP_FAIL;
    }

    ret = gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    if(ESP_OK != ret)
    {
        ESP_LOGE(__func__, "Error calling gpio_install_isr_service: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ret = gpio_isr_handler_add(GPIO_BTN_RED, gpio_button_isr_handler, (void*)GPIO_BTN_RED);
    if(ESP_OK != ret)
    {
        ESP_LOGE(__func__, "Error calling gpio_isr_handler_add (RED): %s", esp_err_to_name(ret));
        return ret;
    }
    
    ret = gpio_isr_handler_add(GPIO_BTN_BLUE, gpio_button_isr_handler, (void*)GPIO_BTN_BLUE);
    if(ESP_OK != ret)
    {
        ESP_LOGE(__func__, "Error calling gpio_isr_handler_add (BLUE): %s", esp_err_to_name(ret));
        return ret;
    }

    return ret;

}

void IRAM_ATTR gpio_button_isr_handler(void* arg)
{
    
    uint32_t gpio_num = (uint32_t)(uintptr_t)arg;

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (gpio_num == GPIO_BTN_RED) 
    {
        xEventGroupSetBitsFromISR(button_event_group, BTN_RED_EVENT, &xHigherPriorityTaskWoken);
    } 
    else if (gpio_num == GPIO_BTN_BLUE) 
    {
        xEventGroupSetBitsFromISR(button_event_group, BTN_BLUE_EVENT, &xHigherPriorityTaskWoken);
    }

    if (xHigherPriorityTaskWoken) 
    {
        portYIELD_FROM_ISR();
    }

}

void button_task(void* arg)
{
    
    for(;;)
    {
        
        EventBits_t bits = xEventGroupWaitBits(button_event_group,
                                               BTN_RED_EVENT | BTN_BLUE_EVENT,
                                               pdFALSE,    // Do NOT clear bits on exit
                                               pdFALSE,    // Wait any
                                               portMAX_DELAY);

        
        vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_DELAY_MS));
        bits = xEventGroupGetBits(button_event_group);

        bool red_pressed  = bits & BTN_RED_EVENT;
        bool blue_pressed = bits & BTN_BLUE_EVENT;

        TickType_t start_tick = xTaskGetTickCount();

        if (red_pressed && blue_pressed)
        {
            int times = 0;
            do 
            {
                vTaskDelay(pdMS_TO_TICKS(PRESS_INTER_TIME_MS));
                times++;
            } 
            while (gpio_get_level(GPIO_BTN_RED) == 0 && gpio_get_level(GPIO_BTN_BLUE) == 0 && times < PRESS_LONG_MAX_MS/PRESS_INTER_TIME_MS);
            
            TickType_t press_duration = xTaskGetTickCount() - start_tick;
            uint32_t press_time_ms = pdTICKS_TO_MS(press_duration);

            AppEventMessage_t message_event;

            if (press_time_ms < PRESS_SHORT_MAX_MS) 
            {
                message_event.type = APP_EVENT_BTN_BOTH_SHORT;
            } 
            else if (press_time_ms < PRESS_MEDIUM_MAX_MS) 
            {
                message_event.type = APP_EVENT_BTN_BOTH_MEDIUM;
            } 
            else 
            {
                message_event.type = APP_EVENT_BTN_BOTH_LONG;
            }

            xQueueSend(app_event_queue, &message_event, pdMS_TO_TICKS(APP_EVENT_ENQUEUE_TIMEOUT_MS));
        
        }
        else if (red_pressed)
        {
            int times = 0;
            do 
            {
                vTaskDelay(pdMS_TO_TICKS(PRESS_INTER_TIME_MS));
                times++;
            } 
            while (gpio_get_level(GPIO_BTN_RED) == 0 && times < PRESS_LONG_MAX_MS/PRESS_INTER_TIME_MS);

            TickType_t press_duration = xTaskGetTickCount() - start_tick;
            uint32_t press_time_ms = pdTICKS_TO_MS(press_duration);

            AppEventMessage_t message_event;

            if (press_time_ms < PRESS_SHORT_MAX_MS) 
            {
                message_event.type = APP_EVENT_BTN_RED_SHORT;
            } 
            else if (press_time_ms < PRESS_MEDIUM_MAX_MS) 
            {
                message_event.type = APP_EVENT_BTN_RED_MEDIUM;
            } 
            else 
            {
                message_event.type = APP_EVENT_BTN_RED_LONG;
            }

            xQueueSend(app_event_queue, &message_event, pdMS_TO_TICKS(APP_EVENT_ENQUEUE_TIMEOUT_MS));
        
        }
        else if (blue_pressed)
        {
            
            int times = 0;
            do 
            {
                vTaskDelay(pdMS_TO_TICKS(PRESS_INTER_TIME_MS));
                times++;
            }
            while (gpio_get_level(GPIO_BTN_BLUE) == 0 && times < PRESS_LONG_MAX_MS/PRESS_INTER_TIME_MS);

            TickType_t press_duration = xTaskGetTickCount() - start_tick;
            uint32_t press_time_ms = pdTICKS_TO_MS(press_duration);

            AppEventMessage_t message_event;

            if (press_time_ms < PRESS_SHORT_MAX_MS) 
            {
                message_event.type = APP_EVENT_BTN_BLUE_SHORT;
            } 
            else if (press_time_ms < PRESS_MEDIUM_MAX_MS) 
            {
                message_event.type = APP_EVENT_BTN_BLUE_MEDIUM;
            } 
            else 
            {
                message_event.type = APP_EVENT_BTN_BLUE_LONG;
            }

            xQueueSend(app_event_queue, &message_event, pdMS_TO_TICKS(APP_EVENT_ENQUEUE_TIMEOUT_MS));
        
        }

        xEventGroupClearBits(button_event_group, bits);
    
    }

}
