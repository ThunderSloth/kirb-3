#ifndef KIRB_CONFIG_H
#define KIRB_CONFIG_H

/** @file kirb_config.h
 *  @brief ESP32 bridge build-time network settings.
 */

#include "../../kirb_wifi.h"

/** Camera snapshot endpoint consumed by the bridge dashboard. */
#define KIRB_CAMERA_STREAM_URL "http://" KIRB_CAMERA_HOSTNAME ".local/capture"

/** Maximum Wi-Fi association wait at boot, in milliseconds. */
static constexpr uint32_t KIRB_STA_CONNECT_TIMEOUT_MS = 20000;

#endif
