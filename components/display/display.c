#include "display.h"
#include "tm1637.h"
#include "symbols.h"
#include "config.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "string.h"

#define TM1637_AUTO_DELAY 300000

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

esp_err_t display_set_string(Display_t display, char * string)
{
    
    if (string == NULL) 
    {
        return ESP_ERR_INVALID_ARG;
    }

    tm1637_led_t *led = NULL;

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

    tm1637_set_segment_ascii(led, string);
    
    return ESP_OK;

}

esp_err_t display_set_string_dual(char * string)
{

    if (string == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    if (display_red == NULL || display_blue == NULL)
    {
        ESP_LOGE(__func__, "One or both displays not initialized! red=%p blue=%p", display_red, display_blue);
        return ESP_FAIL;
    }

    const int red_len = display_red->segment_max;
    const int blue_len = display_blue->segment_max;
    const int total_len = red_len + blue_len;

    char padded[16] = {0}; // up to 15 + null terminator
    int input_len = strlen(string);

    if (input_len <= total_len)
    {
        
        // Left-align string in full display
        memset(padded, 0x20, total_len);
        strncpy(padded, string, total_len);

        char red_text[8] = {0};
        char blue_text[8] = {0};

        strncpy(red_text, padded, red_len);
        strncpy(blue_text, padded + red_len, blue_len);

        tm1637_set_segment_ascii(display_red, red_text);
        tm1637_set_segment_ascii(display_blue, blue_text);
    
    }
    else
    {
        // Scroll text across both displays
        uint8_t segments[8] = {0};
        for (int i = 0; i < input_len; i++)
        {
            
            // shift left
            for (int j = 0; j < 7; j++)
            {
                segments[j] = segments[j+1];
            }

            int c = string[i];
            segments[7] = ascii_symbols[c];

            tm1637_set_segment_auto(display_red, segments, 4);
            tm1637_set_segment_auto(display_blue, segments + 4, 4);

            vTaskDelay(pdMS_TO_TICKS(TM1637_AUTO_DELAY / 1000));
        
        }

        // Clear out scroll
        for (int i = 0; i < 8; i++)
        {
            
            for (int j = 0; j < 7; j++)
            {
                segments[j] = segments[j+1];
            }
            segments[7] = 0;

            tm1637_set_segment_auto(display_red, segments, 4);
            tm1637_set_segment_auto(display_blue, segments + 4, 4);

            vTaskDelay(pdMS_TO_TICKS(TM1637_AUTO_DELAY / 1000));
        
        }
    
    }

    return ESP_OK;

}

esp_err_t display_set_fw_version(Display_t display, float fw_version)
{
    if (fw_version < 0.0f || fw_version >= 100.0f) 
    {
        ESP_LOGE(__func__, "Invalid firmware version: %.2f", fw_version);
        return ESP_ERR_INVALID_ARG;
    }

    tm1637_led_t *led = NULL;

    switch (display) 
    {
        case DISPLAY_RED:
            if (display_red == NULL) 
            {
                ESP_LOGE(__func__, "Display (RED) not initialized!");
                return ESP_FAIL;
            }
            led = display_red;
            break;

        case DISPLAY_BLUE:
            if (display_blue == NULL) 
            {
                ESP_LOGE(__func__, "Display (BLUE) not initialized!");
                return ESP_FAIL;
            }
            led = display_blue;
            break;

        default:
            ESP_LOGE(__func__, "Invalid DISPLAY: %d", display);
            return ESP_ERR_INVALID_ARG;
    }

    // Convert version float to integer format (e.g., 1.23 -> 123)
    int version_number = (int)(fw_version * 100.0f + 0.5f);  // rounded

    // Dot at position 3 (from right): 1 << 2 == 0x04
    const uint16_t dot_position = 0x04;

    tm1637_set_number(led, version_number, false, dot_position);

    return ESP_OK;
    
}