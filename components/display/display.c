#include "display.h"
#include "tm1637.h"
#include "symbols.h"
#include "config.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"


static tm1637_led_t * display_red;
static tm1637_led_t * display_blue;

esp_err_t display_init(void)
{

    ESP_LOGI(__func__, "Initializing 7 segment displays...");

    display_red = tm1637_init(GPIO_7SEG_RED_CLK, GPIO_7SEG_RED_SDO);
    if(!display_red)
    {
        ESP_LOGE(__func__, "Error initializing 7seg display (RED)!");
        return ESP_FAIL;
    }

    display_blue = tm1637_init(GPIO_7SEG_BLUE_CLK, GPIO_7SEG_BLUE_SDO);
    if(!display_blue)
    {
        ESP_LOGE(__func__, "Error initializing 7seg display (BLUE)!");
        return ESP_FAIL;
    }
    
    return ESP_OK;

}

esp_err_t display_set_brightness(Display_t display, DisplayBrightness_t brightness)
{
    if(brightness < DISPLAY_BRIGHTNESS_MIN || brightness > DISPLAY_BRIGHTNESS_MAX)
    {
        ESP_LOGE(__func__, "Passed invalid brighness: %d", brightness);
        return ESP_ERR_INVALID_ARG;
    }

    switch (display)
    {
        
        case DISPLAY_RED:
        {
            
            if(!display_red)
            {
                ESP_LOGE(__func__, "Display RED is not initialized!");
                return ESP_FAIL;
            }
            else
            {
                tm1637_set_brightness(display_red, brightness);
            }
            
            break;
        
        }

        case DISPLAY_BLUE:
        {
            
            if(!display_blue)
            {
                ESP_LOGE(__func__, "Display BLUE is not initialized!");
                return ESP_FAIL;
            }
            else
            {
                tm1637_set_brightness(display_blue, brightness);
            }
            
            break;
        
        }
        
        default:
        {
            
            ESP_LOGE(__func__, "Invalid DISPLAY: %d", display);
            return ESP_ERR_INVALID_ARG;
            
            break;
    
        }
    
    }

    return ESP_OK;

}

esp_err_t display_set_time(Display_t display, int seconds, bool colon) 
{
    
    if (seconds < 0 || seconds > 5999)
    { 
        return ESP_ERR_INVALID_ARG;
    }

    tm1637_led_t * led = NULL;

    switch (display) 
    {
        
        case DISPLAY_RED:
        {
            
            if (display_red == NULL) 
            {
                ESP_LOGE(__func__, "Display (RED) not initialized!");
                return ESP_FAIL;
            }

            led = display_red;
            
            break;
        
        }
        
        case DISPLAY_BLUE:
        {
            
            if (display_blue == NULL) 
            {
                ESP_LOGE(__func__, "Display (BLUE) not initialized!");
                return ESP_FAIL;
            }

            led = display_blue;
            
            break;
        
        }

        default:
        {
            ESP_LOGE(__func__, "Invalid DISPLAY: %d", display);
            return ESP_ERR_INVALID_ARG;
        }

            
    }

    int minutes = seconds / 60;
    int secs = seconds % 60;

    int m1 = minutes / 10;
    int m2 = minutes % 10;
    int s1 = secs / 10;
    int s2 = secs % 10;

    uint8_t seg1 = numerical_symbols[m1];
    uint8_t seg2 = numerical_symbols[m2];
    uint8_t seg3 = numerical_symbols[s1];
    uint8_t seg4 = numerical_symbols[s2];

    if (colon) 
    {
        seg2 |= 0x80;
    }

    tm1637_set_segment_fixed(led, 0, seg1);
    tm1637_set_segment_fixed(led, 1, seg2);
    tm1637_set_segment_fixed(led, 2, seg3);
    tm1637_set_segment_fixed(led, 3, seg4);

    return ESP_OK;

}