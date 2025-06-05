#ifndef STORAGE_H
#define STORAGE_H

#include "esp_err.h"
#include "settings_types.h"

#define NVS_NAMESPACE       "config"
#define KEY_CONTROL_POINT   "controlpoint"
#define KEY_BEEP            "beep"

/**
 * @brief Initialize the storage system (NVS).
 *
 * @return ESP_OK on success, error code otherwise.
 */
esp_err_t storage_init(void);

/**
 * @brief Save the current device control point to NVS.
 *
 * @param control_point Control point to save.
 * @return ESP_OK on success, error code otherwise.
 */
esp_err_t storage_set_control_point(ControlPoint_t control_point);

/**
 * @brief Load the current device control point from NVS.
 *
 * @param control_point Pointer to control point output variable.
 * @return ESP_OK if value found and valid, ESP_ERR_NOT_FOUND if not set,
 *         or other esp_err_t on internal error.
 */
esp_err_t storage_get_control_point(ControlPoint_t * control_point);

/**
 * @brief Save the current beep setting (ON/OFF) to NVS.
 *
 * @param on Set to ON (true) to enable beep, or OFF (false) to disable.
 * @return ESP_OK on success, error code otherwise.
 */
esp_err_t storage_set_beep(bool on);

/**
 * @brief Load the current beep setting (ON/OFF) from NVS.
 *
 * @param on Pointer to boolean output variable. Will be set to ON (true) or OFF (false).
 * @return ESP_OK if value found and valid, ESP_ERR_NOT_FOUND if not set,
 *         or other esp_err_t on internal error.
 */
esp_err_t storage_get_beep(bool * on);

#endif // STORAGE_H