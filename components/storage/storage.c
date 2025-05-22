#include "nvs_flash.h"
#include "nvs.h"

#include "storage.h"

esp_err_t storage_init(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    return err;
}

esp_err_t storage_set_control_point(ControlPoint_t control_point)
{
    if (control_point <= CONTROL_POINT_NONE || control_point >= CONTROL_POINT_MAX)
        return ESP_ERR_INVALID_ARG;

    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) return err;

    err = nvs_set_u8(handle, KEY_CONTROL_POINT, (uint8_t)control_point);
    if (err == ESP_OK)
        err = nvs_commit(handle);

    nvs_close(handle);
    return err;
}

esp_err_t storage_get_control_point(ControlPoint_t * control_point)
{
    if (!control_point) return ESP_ERR_INVALID_ARG;

    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK) return err;

    uint8_t val;
    err = nvs_get_u8(handle, KEY_CONTROL_POINT, &val);
    nvs_close(handle);

    if (err == ESP_OK)
    {
        if (val < CONTROL_POINT_MAX)
        {
            *control_point = (ControlPoint_t)val;
        }
        else
        {
            err = ESP_ERR_INVALID_RESPONSE;
        }
    }

    return err;
}

const char *control_point_to_string(ControlPoint_t control_point) 
{
    switch (control_point) 
    {
        case CONTROL_POINT_ALPHA:   return "Alpha";
        case CONTROL_POINT_BRAVO:   return "Bravo";
        case CONTROL_POINT_CHARLIE: return "Charlie";
        case CONTROL_POINT_DELTA:   return "Delta";
        case CONTROL_POINT_ECHO:    return "Echo";
        case CONTROL_POINT_NONE:    return "None";
        default:                    return "Unknown";
    }
}