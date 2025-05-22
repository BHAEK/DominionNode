#ifndef STORAGE_H
#define STORAGE_H

#include "esp_err.h"
#include "settings_types.h"

#define NVS_NAMESPACE       "config"
#define KEY_CONTROL_POINT   "controlpoint"

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

#endif // STORAGE_H