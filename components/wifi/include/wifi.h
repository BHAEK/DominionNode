#ifndef WIFI_H
#define WIFI_H

#define WIFI_SSID "DominionNode"
#define WIFI_PSWD "1234567890"

/**
 * @brief Initialize and start the ESP32 Wi-Fi in Access Point (AP) mode.
 *
 * This function configures the ESP32 as a Wi-Fi Access Point with the SSID and password
 * defined by the macros `WIFI_SSID` and `WIFI_PSWD`. It sets up the necessary network interfaces,
 * event loop, and Wi-Fi settings. Once started, other devices can connect to the ESP32 using
 * the specified credentials.
 *
 * The Access Point allows a maximum of one connected device and uses WPA/WPA2-PSK authentication.
 * The function logs the SSID and password to the console using `ESP_LOGI`.
 *
 * @note Make sure `WIFI_SSID` and `WIFI_PSWD` are defined prior to calling this function.
 *
 * Example usage:
 * ```
 * #define WIFI_SSID "DominionNode"
 * #define WIFI_PSWD "12345678"
 * start_wifi_ap();
 * ```
 */
void start_wifi_ap(void);


#endif // WIFI_H