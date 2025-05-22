#include "config.h"
#include "esp_err.h"

typedef enum
{
    BLUE_LED = GPIO_LED_BLUE,
    RED_LED = GPIO_LED_RED,
} led_t ;

#define OFF     0
#define ON      1

esp_err_t led_init();
esp_err_t turn_led_on(led_t led);
esp_err_t turn_led_off(led_t led);
esp_err_t turn_all_leds_on();
esp_err_t turn_all_leds_off();