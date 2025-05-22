/******************************************************************************
 *  Project      : DominionNode
 *  File         : main.c
 *  Author       : Gabriele Bertone
 *  Created on   : 05/2025
 *  Description  : Firmware module for DominionNode device in airsoft control game.
 *                 Handles game logic, input buttons, LED control, display updates
 *                 and communication with the master node.
 *
 *  Platform     : ESP32 (ESP-IDF + FreeRTOS)
 *  Target       : Dominion Node - Field Unit
 *
 *  License      : MIT License
 ******************************************************************************/


#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "config.h"
#include "error_signaling.h"
#include "leds.h"
#include "buttons.h"
#include "app.h"
#include "storage.h"

esp_err_t app_init()
{
    
    ESP_LOGI(__func__, "Initializing the app...");

    esp_err_t partial_err = ESP_FAIL;
    bool error = false;

    // LED INITIALIZATION
    partial_err = led_init();
    if(ESP_OK != partial_err)
    {
        ESP_LOGE(__func__, "Error calling led: %s", esp_err_to_name(partial_err));
        error = true;
    }
    else
    {
        ESP_LOGI(__func__, "LED INIT OK");
    }

    // BUTTON INITIALIZATION
    partial_err = button_init();
    if(ESP_OK != partial_err)
    {
        ESP_LOGE(__func__, "Error calling button_init: %s", esp_err_to_name(partial_err));
        error = true;
    }
    else
    {
        ESP_LOGI(__func__, "BUTTON INIT OK");
    }

    partial_err = storage_init();
    if(ESP_OK != partial_err)
    {
        ESP_LOGE(__func__, "Error calling storage_init: %s", esp_err_to_name(partial_err));
        error = true;
    }
    else
    {
        ESP_LOGI(__func__, "STORAGE INIT OK");
    }

    if(error)
    {
        ESP_LOGE(__func__, "Error initializing the app");
        return ESP_FAIL;
    }
    else
    {
        ESP_LOGI(__func__, "App initialized OK");
        return ESP_OK;
    }

}

void app_main(void)
{
    
    esp_err_t init_error = app_init();
    if(ESP_OK != init_error)
    {
        signal_fatal_error(INIT_ERROR);
    }

    BaseType_t button_task_error = xTaskCreate(button_task, "button", BUTTON_TASK_STACK_DEPTH, NULL, BUTTON_TASK_PRIORITY, NULL);
    if(pdPASS != button_task_error)
    {
        signal_fatal_error(INIT_ERROR);
    }

    BaseType_t app_task_error = xTaskCreate(app_task, "app", APP_TASK_STACK_DEPTH, NULL, APP_TASK_PRIORITY, NULL);
    if(pdPASS != app_task_error)
    {
        signal_fatal_error(INIT_ERROR);
    }

}
