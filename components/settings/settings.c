#include "settings.h"
#include "storage.h"
#include "esp_log.h"
#include "display.h"

static SettingType_t current_setting_type;
static Setting_t setting;

const char * setting_type_to_string(SettingType_t setting)
{
    switch (setting)
    {
        case SETTING_CONTROL_POINT:
            return "SETTING_CONTROL_POINT";
        case SETTING_BEEP:
            return "SETTING_BEEP";
        case SETTING_EXIT:
            return "SETTING_EXIT";
        case SETTING_COUNT:
            return "SETTING_COUNT";
        default:
            return "UNKNOWN";
    }
}

const char * setting_type_to_short_string(SettingType_t setting)
{
    switch (setting)
    {
        case SETTING_CONTROL_POINT:
            return "CP";     // Control Point
        case SETTING_BEEP:
            return "BEEP";
        case SETTING_EXIT:
            return "EXIT";
        case SETTING_COUNT:
            return "CNT";    // Count
        default:
            return "UNK";    // Unknown
    }
}

const char * control_point_to_short_string(ControlPoint_t control_point)
{
    switch (control_point)
    {
        case CONTROL_POINT_NONE:
            return "NONE";
        case CONTROL_POINT_ALPHA:
            return "A";
        case CONTROL_POINT_BRAVO:
            return "B";
        case CONTROL_POINT_CHARLIE:
            return "C";
        case CONTROL_POINT_DELTA:
            return "D";
        case CONTROL_POINT_ECHO:
            return "E";
        case CONTROL_POINT_COUNT:
            return "CNT";
        default:
            return "UNK";
    }
}

void settings_init(void) 
{
    current_setting_type = SETTING_CONTROL_POINT;
    settings_load();
    ESP_LOGI(__func__, "Actual setting: %s", setting_type_to_string(current_setting_type));
    display_set_string(DISPLAY_RED, setting_type_to_short_string(current_setting_type));
    display_set_string(DISPLAY_BLUE, control_point_to_short_string(setting.control_point));
}

esp_err_t settings_load(void)
{
    
    esp_err_t ret = ESP_FAIL;
    
    ret = storage_get_control_point(&setting.control_point);
    if(ESP_OK != ret)
    {
        ESP_LOGE(__func__, "Impossible to load actual settings due to storage_get_control_point error: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = storage_get_beep(&setting.beep);
    if(ESP_OK != ret)
    {
        ESP_LOGE(__func__, "Impossible to load actual settings due to storage_get_beep error: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(__func__, "Settings loaded successfully");
    
    return ESP_OK;

}

esp_err_t settings_save(void)
{
    esp_err_t ret = ESP_FAIL;
    
    ret = storage_set_control_point(setting.control_point);
    if(ESP_OK != ret)
    {
        ESP_LOGE(__func__, "Impossible to save actual settings due to storage_set_control_point error: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = storage_set_beep(setting.beep);
    if(ESP_OK != ret)
    {
        ESP_LOGE(__func__, "Impossible to save actual settings due to storage_set_beep error: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(__func__, "Settings saved successfully");
    
    return ESP_OK;

}

void settings_next(void) 
{
    
    current_setting_type = (current_setting_type + 1) % SETTING_COUNT;
    ESP_LOGI(__func__, "Actual setting: %s", setting_type_to_string(current_setting_type));
    display_set_string(DISPLAY_RED, setting_type_to_short_string(current_setting_type));
    
    switch (current_setting_type)
    {
        
        case SETTING_CONTROL_POINT:
        {
            ESP_LOGI(__func__, "Actual value: %s", control_point_to_string(setting.control_point));
            display_set_string(DISPLAY_BLUE, control_point_to_short_string(setting.control_point));
            break;
        }

        case SETTING_BEEP:
        {
            ESP_LOGI(__func__, "Actual value: %s", generic_enable_to_string(setting.beep));
            display_set_string(DISPLAY_BLUE, generic_enable_to_string(setting.beep));
            break;
        }

        case SETTING_EXIT:
        {
            display_set_string(DISPLAY_BLUE, "EXIT");
            break;
        }
        
        default:
        {
            break;
        }
    
    }

}

void settings_modify_current(void) 
{
    
    switch (current_setting_type) 
    {
        
        case SETTING_CONTROL_POINT:
        {
            setting.control_point = (setting.control_point + 1) % CONTROL_POINT_COUNT;
            ESP_LOGI(__func__, "Actual value: %s", control_point_to_string(setting.control_point));
            display_set_string(DISPLAY_BLUE, control_point_to_short_string(setting.control_point));
            break;
        }

        case SETTING_BEEP:
        {
            setting.beep = !setting.beep;
            ESP_LOGI(__func__, "Actual value: %s", generic_enable_to_string(setting.beep));
            display_set_string(DISPLAY_BLUE, generic_enable_to_string(setting.beep));
            break;
        }

        case SETTING_EXIT:
        {
            // Do nothing; handled externally (e.g. save and exit)
            break;
        }

        default:
        {
            break;
        }
    
    }

}

SettingType_t settings_get_current(void) 
{
    return current_setting_type;
}
