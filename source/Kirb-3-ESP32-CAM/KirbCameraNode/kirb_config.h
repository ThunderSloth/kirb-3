#ifndef KIRB_CONFIG_H
#define KIRB_CONFIG_H

/** @file kirb_config.h
 *  @brief ESP32-CAM build-time camera and network settings.
 */

#include "esp_camera.h"

#include "../../kirb_wifi.h"

/** Diagnostic stream frame size. Keep conservative until the stream is stable. */
static constexpr framesize_t KIRB_CAMERA_FRAME_SIZE = FRAMESIZE_QQVGA;

/** JPEG quality setting; higher numbers reduce bandwidth. */
static constexpr int KIRB_CAMERA_JPEG_QUALITY = 24;

/** Camera frame-buffer count. */
static constexpr int KIRB_CAMERA_FB_COUNT = 1;

/** Camera grab mode selected for low latency and stable memory use. */
static constexpr camera_grab_mode_t KIRB_CAMERA_GRAB_MODE = CAMERA_GRAB_WHEN_EMPTY;

/** Browser snapshot polling interval, in milliseconds. */
static constexpr uint32_t KIRB_STREAM_FRAME_INTERVAL_MS = 333;

/** Human-readable frame-rate label used by UI code. */
static constexpr const char *KIRB_STREAM_FRAMERATE = "3";

/** Camera external clock frequency, in hertz. */
static constexpr int KIRB_CAMERA_XCLK_HZ = 20000000;

/** Maximum Wi-Fi association wait at boot, in milliseconds. */
static constexpr uint32_t KIRB_STA_CONNECT_TIMEOUT_MS = 20000;

#endif  // KIRB_CONFIG_H
