#ifndef DISPLAY_H
#define DISPLAY_H

#include "esp_err.h"
#include "stdbool.h"

#define DISPLAY_REFRESH_RATE_MS 500

/**
 * @brief Identifiers for available 7-segment displays.
 *
 * Used to select which display (red or blue) to address in functions.
 */
typedef enum
{   
    DISPLAY_RED,
    DISPLAY_BLUE,
    DISPLAY_COUNT,
} Display_t;

/**
 * @brief Brightness levels for 7-segment displays.
 *
 * TM1637 supports 8 brightness levels (0-7). Aliases like DISPLAY_BRIGHTNESS_MIN and DISPLAY_BRIGHTNESS_MAX
 * are provided for clarity.
 */
typedef enum
{   
    DISPLAY_BRIGHTNESS_0 = 0,
    DISPLAY_BRIGHTNESS_MIN = 0,
    DISPLAY_BRIGHTNESS_1,
    DISPLAY_BRIGHTNESS_2,
    DISPLAY_BRIGHTNESS_3,
    DISPLAY_BRIGHTNESS_4,
    DISPLAY_BRIGHTNESS_MID = 4,
    DISPLAY_BRIGHTNESS_5,
    DISPLAY_BRIGHTNESS_6,
    DISPLAY_BRIGHTNESS_7 = 7,
    DISPLAY_BRIGHTNESS_MAX = 7,
} DisplayBrightness_t;

/**
 * @brief Initializes the two 7-segment displays (red and blue).
 *
 * This function initializes two TM1637-based 7-segment displays
 * connected to the GPIOs defined by `GPIO_7SEG_RED_CLK`, `GPIO_7SEG_RED_SDO`,
 * `GPIO_7SEG_BLUE_CLK`, and `GPIO_7SEG_BLUE_SDO`. It logs the status
 * of each initialization and returns an appropriate result.
 *
 * @note Make sure the GPIO macros are correctly defined before calling this function.
 *
 * @retval ESP_OK  Initialization successful for both displays.
 * @retval ESP_FAIL Failed to initialize one or both displays.
 */
esp_err_t display_init(void);

/**
 * @brief Sets the brightness level of the specified 7-segment display.
 *
 * This function changes the brightness of either the red or blue 7-segment display.
 * It checks for valid input and whether the target display has been properly initialized.
 *
 * @param[in] display The display to configure (DISPLAY_RED or DISPLAY_BLUE).
 * @param[in] brightness The desired brightness level. Must be in the range DISPLAY_BRIGHTNESS_MIN to DISPLAY_BRIGHTNESS_MAX.
 *
 * @retval ESP_OK              Brightness was set successfully.
 * @retval ESP_ERR_INVALID_ARG Invalid display or brightness value passed.
 * @retval ESP_FAIL            The specified display is not initialized.
 */
esp_err_t display_set_brightness(Display_t display, DisplayBrightness_t brightness);

/**
 * @brief Displays a time value in MM:SS format on the specified display.
 *
 * This function takes a number of seconds, converts it to minutes and seconds,
 * and displays it in MM:SS format on a 4-digit 7-segment display using the TM1637 driver.
 * The colon between minutes and seconds can be turned on or off.
 *
 * @param[in] display The display to set (DISPLAY_RED or DISPLAY_BLUE).
 * @param seconds Time in seconds to be displayed (0 to 5999 = 99 minutes 59 seconds).
 * @param colon Whether to show the colon (true = show, false = hide).
 * 
 * @retval ESP_OK              Time was set successfully.
 * @retval ESP_ERR_INVALID_ARG Invalid display or seconds value passed.
 * @retval ESP_FAIL            The specified display is not initialized.
 */
esp_err_t display_set_time(Display_t display, int seconds, bool colon);

/**
 * @brief Displays a string on the specified 7-segment display.
 *
 * This function uses the ASCII segment function to show a string on the TM1637-based display.
 * If the string is longer than the display width, it will scroll automatically.
 *
 * @param display The target display (DISPLAY_RED or DISPLAY_BLUE).
 * @param string The null-terminated string to display.
 * @return esp_err_t
 *         - ESP_OK: Successfully updated the display.
 *         - ESP_ERR_INVALID_ARG: The display is invalid or the string is NULL.
 *         - ESP_FAIL: The corresponding display is not initialized (NULL pointer).
 */
esp_err_t display_set_string(Display_t display, char * string);

/**
 * @brief Displays a string across both RED and BLUE displays as if they are concatenated.
 *
 * This function assumes that the RED display comes first and the BLUE display follows.
 * If the string is shorter than the combined length, it will be right-aligned.
 * If longer, it will scroll across the combined 8 digits.
 *
 * @param string Null-terminated string to display.
 * @return esp_err_t
 *         - ESP_OK: Displayed successfully.
 *         - ESP_ERR_INVALID_ARG: Null pointer passed.
 *         - ESP_FAIL: One or both displays are not initialized.
 */
esp_err_t display_set_string_dual(char * string);

/**
 * @brief Set the firmware version on the specified display.
 *
 * This function formats and displays a firmware version number in the format XX.YY
 * on a 6-digit 7-segment TM1637 display.
 *
 * Example: A version 1.23 will be shown as " 01.23" (with a dot at the 3rd digit from right).
 *
 * @param display The display identifier (DISPLAY_RED or DISPLAY_BLUE).
 * @param fw_version The firmware version as a float (e.g., 1.23).
 * @return esp_err_t ESP_OK on success, or error code on failure.
 */
esp_err_t display_set_fw_version(Display_t display, float fw_version);

#endif // DISPLAY_H