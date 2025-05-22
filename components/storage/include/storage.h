#pragma once

#include "esp_err.h"

#define NVS_NAMESPACE       "config"
#define KEY_CONTROL_POINT   "controlpoint"

typedef enum
{
    CONTROL_POINT_NONE = -1,
    CONTROL_POINT_ALPHA = 0,
    CONTROL_POINT_BRAVO,
    CONTROL_POINT_CHARLIE,
    CONTROL_POINT_DELTA,
    CONTROL_POINT_ECHO,
    CONTROL_POINT_MAX
} ControlPoint_t;


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
 * @brief Converts a ControlPoint_t enum value to its corresponding string representation.
 *
 * @param control_point The control point enum value to convert.
 * @return A pointer to a string representing the name of the control point.
 *         Returns "Unknown" if the value is not a valid control point.
 */
const char *control_point_to_string(ControlPoint_t control_point);