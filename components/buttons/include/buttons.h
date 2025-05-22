#ifndef BUTTONS_H
#define BUTTONS_H

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "config.h"

#define ESP_INTR_FLAG_DEFAULT 0

#define PRESS_INTER_TIME_MS     10
#define PRESS_SHORT_MAX_MS      2000
#define PRESS_MEDIUM_MAX_MS     4000
#define PRESS_LONG_MAX_MS       10000

extern EventGroupHandle_t button_event_group;
esp_err_t button_init();
void button_task(void* arg);

#endif // BUTTONS_H