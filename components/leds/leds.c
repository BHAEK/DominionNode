#include "leds.h"
#include "driver/gpio.h"
#include "esp_log.h"

esp_err_t led_init()
{

    esp_err_t ret = ESP_OK;

    gpio_config_t io_conf_led = 
    {
        .pin_bit_mask = (1ULL << GPIO_LED_RED) | (1ULL << GPIO_LED_BLUE),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    
    ret = gpio_config(&io_conf_led);
    if(ESP_OK != ret)
    {
        ESP_LOGE(__func__, "Error calling gpio_config: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = gpio_set_level(GPIO_LED_RED, 0);
    if(ESP_OK != ret)
    {
        ESP_LOGE(__func__, "Error calling gpio_set_level(%d): %s", GPIO_LED_RED, esp_err_to_name(ret));
        return ret;
    }

    ret = gpio_set_level(GPIO_LED_BLUE, 0);
    if(ESP_OK != ret)
    {
        ESP_LOGE(__func__, "Error calling gpio_set_level(%d): %s", GPIO_LED_BLUE, esp_err_to_name(ret));
        return ret;
    }

    return ret;

}

esp_err_t turn_led_on(led_t led)
{
    return gpio_set_level(led, ON);
}

esp_err_t turn_led_off(led_t led)
{
    return gpio_set_level(led, OFF);
}

esp_err_t turn_all_leds_off()
{
    esp_err_t ret = gpio_set_level(BLUE_LED, OFF);
    ret = ret | gpio_set_level(RED_LED, OFF);
    return ret;
}

esp_err_t turn_all_leds_on()
{
    esp_err_t ret = gpio_set_level(BLUE_LED, ON);
    ret = ret | gpio_set_level(RED_LED, ON);
    return ret;
}