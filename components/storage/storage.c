#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"

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
    if (control_point <= CONTROL_POINT_NONE || control_point >= CONTROL_POINT_COUNT)
    {
        return ESP_ERR_INVALID_ARG;
    }

    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) 
    {
        ESP_LOGE(__func__, "Error opening nvs!");
        return err;
    }

    err = nvs_set_u8(handle, KEY_CONTROL_POINT, (uint8_t)control_point);
    if (err == ESP_OK)
    {
        err = nvs_commit(handle);
        if (err != ESP_OK)
        {
            ESP_LOGE(__func__, "Error committing data!");
        }
    }
    else
    {
        ESP_LOGE(__func__, "Error setting u8!");
    }

    nvs_close(handle);
    
    return err;

}

esp_err_t storage_get_control_point(ControlPoint_t * control_point)
{
    
    if (!control_point) 
    {
        return ESP_ERR_INVALID_ARG;
    }

    nvs_handle_t handle;
    
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK) 
    {
        ESP_LOGE(__func__, "Error opening nvs!");
        return err;
    }

    uint8_t val;
    
    err = nvs_get_u8(handle, KEY_CONTROL_POINT, &val);
    
    if (err != ESP_OK)
    {
        ESP_LOGE(__func__, "Error committing data!");
    }

    nvs_close(handle);

    if (err == ESP_OK)
    {
        
        if (val > CONTROL_POINT_NONE && val < CONTROL_POINT_COUNT)
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

esp_err_t storage_set_beep(bool on)
{
    
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) 
    {
        ESP_LOGE(__func__, "Error opening nvs!");
        return err;
    }

    err = nvs_set_u8(handle, KEY_BEEP, on ? 1 : 0);
    if (err == ESP_OK)
    {
        
        err = nvs_commit(handle);
        if (err != ESP_OK)
        {
            ESP_LOGE(__func__, "Error committing data!");
        }
    
    }
    else
    {
        ESP_LOGE(__func__, "Error setting u8!");
    }

    nvs_close(handle);
    
    return err;

}

esp_err_t storage_get_beep(bool * on)
{
    
    if (!on)
    {
        return ESP_ERR_INVALID_ARG;
    }

    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK) 
    {
        ESP_LOGE(__func__, "Error opening nvs!");
        return err;
    }

    uint8_t val;
    err = nvs_get_u8(handle, KEY_BEEP, &val);
    if(ESP_OK != err)
    {
        ESP_LOGE(__func__, "Error committing data!");
    }
    
    nvs_close(handle);

    if (err == ESP_OK)
    {
        if (val == 0 || val == 1)
        {
            *on = (val == 1) ? true : false;
        }
        else
        {
            err = ESP_ERR_INVALID_RESPONSE;
        }
    }

    return err;

}