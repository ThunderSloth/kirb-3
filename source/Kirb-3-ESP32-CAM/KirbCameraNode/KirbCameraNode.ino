#include <Arduino.h>
#include <ESPmDNS.h>
#include "esp_camera.h"
#include <WiFi.h>

// ===========================
// Select camera model in board_config.h
// ===========================
#include "board_config.h"
#include "kirb_config.h"

/** @file KirbCameraNode.ino
 *  @brief ESP32-CAM firmware for camera capture, Wi-Fi, and HTTP endpoints.
 */

/** Start the ESP32-CAM HTTP server. */
void startCameraNodeServer();

/** Configure the camera module flash LED if this board exposes one. */
void setupLedFlash();

namespace {

/** Convert Arduino Wi-Fi status codes into compact log strings. */
const char *wifiStatusName(wl_status_t status) {
  switch (status) {
    case WL_IDLE_STATUS:
      return "IDLE";
    case WL_NO_SSID_AVAIL:
      return "NO_SSID";
    case WL_SCAN_COMPLETED:
      return "SCAN_COMPLETED";
    case WL_CONNECTED:
      return "CONNECTED";
    case WL_CONNECT_FAILED:
      return "CONNECT_FAILED";
    case WL_CONNECTION_LOST:
      return "CONNECTION_LOST";
    case WL_DISCONNECTED:
      return "DISCONNECTED";
    default:
      return "UNKNOWN";
  }
}

/** Log major Wi-Fi station events. */
void logWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      Serial.println("[wifi] STA connected to AP");
      break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.print("[wifi] STA got IP: ");
      Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.printf("[wifi] STA disconnected, reason=%d\n", info.wifi_sta_disconnected.reason);
      break;
    default:
      break;
  }
}

/** Build the esp_camera driver configuration for the selected board. */
camera_config_t buildCameraConfig() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = KIRB_CAMERA_XCLK_HZ;
  config.frame_size = KIRB_CAMERA_FRAME_SIZE;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = KIRB_CAMERA_GRAB_MODE;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = KIRB_CAMERA_JPEG_QUALITY;
  config.fb_count = KIRB_CAMERA_FB_COUNT;

  if (config.pixel_format == PIXFORMAT_JPEG) {
    if (!psramFound()) {
      config.fb_location = CAMERA_FB_IN_DRAM;
      config.fb_count = 1;
      config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
      if (config.frame_size > FRAMESIZE_QVGA) {
        config.frame_size = FRAMESIZE_QVGA;
      }
      if (config.jpeg_quality < 12) {
        config.jpeg_quality = 12;
      }
      Serial.println("[camera] PSRAM not found, using reduced DRAM-safe settings");
    }
  } else {
    config.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
    config.fb_count = 2;
#endif
  }

  return config;
}

/** Initialize the camera driver and apply sensor-specific defaults. */
bool initCamera() {
  camera_config_t config = buildCameraConfig();

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  Serial.printf(
    "[camera] init ssid=%s frame_size=%d jpeg_quality=%d fb_count=%d grab_mode=%d\n",
    KIRB_WIFI_SSID,
    static_cast<int>(config.frame_size),
    config.jpeg_quality,
    config.fb_count,
    static_cast<int>(config.grab_mode));

  const esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("[camera] init failed: 0x%x\n", err);
    return false;
  }

  sensor_t *s = esp_camera_sensor_get();
  if (s == nullptr) {
    Serial.println("[camera] sensor handle unavailable after init");
    return false;
  }

  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);
    s->set_brightness(s, 1);
    s->set_saturation(s, -2);
  }

  if (config.pixel_format == PIXFORMAT_JPEG) {
    s->set_framesize(s, config.frame_size);
    s->set_quality(s, config.jpeg_quality);
  }

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

#if defined(CAMERA_MODEL_ESP32S3_EYE)
  s->set_vflip(s, 1);
#endif

  Serial.println("[camera] init success");
  return true;
}

/** Connect the camera node to the dedicated Kirb Wi-Fi network. */
bool initWifi() {
  WiFi.onEvent(logWiFiEvent);
  WiFi.persistent(false);
  WiFi.setSleep(false);
  WiFi.setHostname(KIRB_CAMERA_HOSTNAME);

  Serial.printf("[wifi] configured SSID: %s\n", KIRB_WIFI_SSID);

  WiFi.disconnect(true, true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(KIRB_WIFI_SSID, KIRB_WIFI_PASSWORD);

  Serial.printf("[wifi] connecting to STA SSID '%s'\n", KIRB_WIFI_SSID);
  const uint32_t startMs = millis();
  wl_status_t lastStatus = WL_IDLE_STATUS;
  while (WiFi.status() != WL_CONNECTED && (millis() - startMs) < KIRB_STA_CONNECT_TIMEOUT_MS) {
    const wl_status_t status = WiFi.status();
    if (status != lastStatus) {
      Serial.printf("[wifi] STA status: %s\n", wifiStatusName(status));
      lastStatus = status;
    }
    delay(250);
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.printf("[wifi] STA connection failed after %lu ms, status=%s\n",
                  static_cast<unsigned long>(millis() - startMs),
                  wifiStatusName(WiFi.status()));
    return false;
  }

  Serial.print("[wifi] STA ready, IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("[wifi] gateway: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("[wifi] subnet: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("[wifi] RSSI: ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
  return true;
}

/** Start camera-node mDNS advertisement and HTTP service record. */
bool initMdns() {
  if (!MDNS.begin(KIRB_CAMERA_HOSTNAME)) {
    Serial.printf("[mdns] failed to start hostname '%s.local'\n",
                  KIRB_CAMERA_HOSTNAME);
    return false;
  }

  MDNS.addService("http", "tcp", 80);
  Serial.printf("[mdns] camera hostname: http://%s.local\n",
                KIRB_CAMERA_HOSTNAME);
  return true;
}

/** Print build-time camera and network settings at boot. */
void logBootBanner() {
  Serial.println();
  Serial.println("KirbCameraNode boot");
  Serial.printf("[build] Wi-Fi SSID: %s\n", KIRB_WIFI_SSID);
  Serial.printf("[build] stream frame_size=%d jpeg_quality=%d fb_count=%d xclk=%d stream_interval_ms=%lu\n",
                static_cast<int>(KIRB_CAMERA_FRAME_SIZE),
                KIRB_CAMERA_JPEG_QUALITY,
                KIRB_CAMERA_FB_COUNT,
                KIRB_CAMERA_XCLK_HZ,
                static_cast<unsigned long>(KIRB_STREAM_FRAME_INTERVAL_MS));
}

}  // namespace

/** Arduino setup: initialize camera, Wi-Fi, mDNS, and HTTP server. */
void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  logBootBanner();

  if (!initCamera()) {
    return;
  }

#if defined(LED_GPIO_NUM)
  setupLedFlash();
#endif

  if (!initWifi()) {
    return;
  }

  (void)initMdns();
  startCameraNodeServer();

  Serial.println("[camera] network ready");
  Serial.print("[camera] connected SSID: ");
  Serial.println(KIRB_WIFI_SSID);
  Serial.print("[camera] gateway: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("[camera] RSSI: ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
  Serial.print("[camera] open http://");
  Serial.println(WiFi.localIP());
  Serial.print("[camera] test http://");
  Serial.print(WiFi.localIP());
  Serial.println("/ping");
}

/** Arduino loop: keep the sketch alive between HTTP requests. */
void loop() {
  delay(2);
}
