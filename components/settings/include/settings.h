#ifndef SETTINGS_H
#define SETTINGS_H

#include "esp_err.h"
#include "settings_types.h"

/**
 * @brief Convert a SettingType_t enum value to a human-readable string.
 *
 * This function returns a constant string representing the name of the setting type.
 * It is useful for debugging, logging, or displaying the current setting.
 *
 * @param setting The SettingType_t value to convert.
 * @return A constant null-terminated string describing the setting type.
 *         Returns "UNKNOWN" if the value is out of range.
 */
const char * setting_type_to_string(SettingType_t setting);

/**
 * @brief Initialize the settings module.
 *
 * This function sets the current setting type to the default (SETTING_CONTROL_POINT)
 * and loads the saved settings from persistent storage.
 */
void settings_init(void);

/**
 * @brief Load the settings from persistent storage.
 *
 * This function attempts to retrieve the current settings values from the storage backend.
 * If the loading fails, an error is logged and the error code is returned.
 *
 * @return ESP_OK on success, or an esp_err_t error code on failure.
 */
esp_err_t settings_load(void);

/**
 * @brief Save the current settings to persistent storage.
 *
 * This function writes the current settings values to the storage backend.
 * If the saving operation fails, an error is logged and the error code is returned.
 *
 * @return ESP_OK on success, or an esp_err_t error code on failure.
 */
esp_err_t settings_save(void);

/**
 * @brief Advance to the next setting in the list.
 *
 * This function cycles the current setting type to the next one,
 * wrapping around to the first setting after the last one.
 */
void settings_next(void);

/**
 * @brief Modify the value of the current setting.
 *
 * Depending on the current setting type, this function updates the setting's value.
 * For SETTING_CONTROL_POINT, it cycles through possible control points.
 * For SETTING_EXIT and other types, no modification is performed here;
 * exit handling should be managed externally.
 */
void settings_modify_current(void);

/**
 * @brief Get the currently selected setting type.
 *
 * @return The current SettingType_t representing the selected setting.
 */
SettingType_t settings_get_current(void);

#endif // SETTINGS_H