#ifndef BUZZER_H
#define BUZZER_H

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

extern EventGroupHandle_t buzzer_event_group;

typedef enum 
{
    BUZZER_COMMAND_ON,
    BUZZER_COMMAND_OFF,
    BUZZER_COMMAND_BLINK,
    BUZZER_COMMAND_HELLO,
    BUZZER_COMMAND_OK,
    BUZZER_COMMAND_SWITCH,
    BUZZER_COMMAND_FINISH,
    BUZZER_COMMAND_COUNT,
} BuzzerCommand_t;

typedef enum 
{
    BUZZER_EVENT_ON      = (1 << BUZZER_COMMAND_ON),
    BUZZER_EVENT_OFF     = (1 << BUZZER_COMMAND_OFF),
    BUZZER_EVENT_BLINK   = (1 << BUZZER_COMMAND_BLINK),
    BUZZER_EVENT_HELLO   = (1 << BUZZER_COMMAND_HELLO),
    BUZZER_EVENT_OK      = (1 << BUZZER_COMMAND_OK),
    BUZZER_EVENT_SWITCH  = (1 << BUZZER_COMMAND_SWITCH),
    BUZZER_EVENT_FINISH  = (1 << BUZZER_COMMAND_FINISH),
    BUZZER_EVENT_ALL     = ((1 << BUZZER_COMMAND_COUNT) - 1)
} BuzzerEventBits_t;

esp_err_t buzzer_init();

#endif // BUZZER_H