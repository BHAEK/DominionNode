#ifndef SETTINGS_TYPES_H
#define SETTINGS_TYPES_H

#include "stdbool.h"

/**
 * @brief Enumeration of control points.
 *
 * Defines the possible control points available for selection.
 */
typedef enum
{
    CONTROL_POINT_NONE = -1, /**< No control point selected */
    CONTROL_POINT_ALPHA = 0, /**< Control Point Alpha */
    CONTROL_POINT_BRAVO,     /**< Control Point Bravo */
    CONTROL_POINT_CHARLIE,   /**< Control Point Charlie */
    CONTROL_POINT_DELTA,     /**< Control Point Delta */
    CONTROL_POINT_ECHO,      /**< Control Point Echo */
    CONTROL_POINT_COUNT      /**< Total number of control points */
} ControlPoint_t;

/**
 * @brief Enumeration of setting types.
 *
 * Represents the types of settings available in the settings module.
 */
typedef enum {
    SETTING_CONTROL_POINT, /**< Setting for selecting the control point */
    SETTING_BEEP,          /**< Setting for selecting if enable or not the "beep" */
    SETTING_EXIT,          /**< Exit option from the settings menu */
    SETTING_COUNT          /**< Total number of settings */
} SettingType_t;

/**
 * @brief Structure holding the current settings values.
 *
 * Contains the values for all configurable settings.
 */
typedef struct
{
    ControlPoint_t  control_point; /**< Currently selected control point */
    bool            beep;
} Setting_t;

/**
 * @brief Returns a readable string representation of a control point.
 *
 * This function converts a ControlPoint_t enum value into a constant string,
 * useful for debugging or displaying symbolic names.
 *
 * @param control_point The control point to convert (an enum value of type ControlPoint_t).
 * @return A constant string representing the name of the control point.
 *         Returns "Unknown" if the value is not recognized.
 */
const char * control_point_to_string(ControlPoint_t control_point);

/**
 * @brief Returns a readable string representation of a generic enable setting (ON/OFF).
 *
 * This function converts a bool value into a constant string,
 * useful for debugging or displaying symbolic names.
 *
 * @param enable The bool to convert.
 * @return A constant string representing the name of enabled/disabled setting (ON/OFF).
 */
const char * generic_enable_to_string(bool enable);

#endif // SETTINGS_TYPES_H