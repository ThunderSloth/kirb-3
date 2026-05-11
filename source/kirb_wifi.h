#ifndef KIRB_WIFI_H
#define KIRB_WIFI_H

/** @file kirb_wifi.h
 *  @brief Shared Wi-Fi credentials and hostnames for Kirb ESP32 nodes.
 */

/** mDNS hostname used by the ESP32 bridge dashboard. */
#define KIRB_BRIDGE_HOSTNAME "kirb-bridge"

/** mDNS hostname used by the ESP32-CAM camera node. */
#define KIRB_CAMERA_HOSTNAME "kirb-camera"

/** Dedicated Kirb project router SSID. */
#define KIRB_WIFI_SSID "Kirb-WiFi"

/** Dedicated Kirb project router password. */
#define KIRB_WIFI_PASSWORD "01234567"

#endif
