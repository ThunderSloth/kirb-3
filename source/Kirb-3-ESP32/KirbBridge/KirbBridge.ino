#include <Arduino.h>
#include <ESPmDNS.h>
#include <WiFi.h>

#include "bridge_httpd.h"
#include "bridge_telemetry.h"
#include "kirb_config.h"

/** @file KirbBridge.ino
 *  @brief ESP32 bridge firmware for Wi-Fi, dashboard HTTP, and MSPM0 UART.
 */

namespace
{

/** Convert Arduino Wi-Fi status codes into compact log strings. */
const char *wifiStatusName(wl_status_t status)
{
    switch (status)
    {
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
void logWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info)
{
    switch (event)
    {
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        Serial.println("[wifi] STA connected to AP");
        break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        Serial.print("[wifi] STA got IP: ");
        Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));
        break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        Serial.printf("[wifi] STA disconnected, reason=%d\n",
                      info.wifi_sta_disconnected.reason);
        break;
    default:
        break;
    }
}

/** Connect the bridge to the dedicated Kirb Wi-Fi network. */
bool initWifi()
{
    WiFi.onEvent(logWiFiEvent);
    WiFi.persistent(false);
    WiFi.setSleep(false);
    WiFi.setHostname(KIRB_BRIDGE_HOSTNAME);

    Serial.printf("[wifi] configured SSID: %s\n", KIRB_WIFI_SSID);

    WiFi.disconnect(true, true);
    WiFi.mode(WIFI_STA);
    WiFi.begin(KIRB_WIFI_SSID, KIRB_WIFI_PASSWORD);

    Serial.printf("[wifi] connecting to STA SSID '%s'\n", KIRB_WIFI_SSID);
    const uint32_t startMs = millis();
    wl_status_t lastStatus = WL_IDLE_STATUS;

    while (WiFi.status() != WL_CONNECTED &&
           (millis() - startMs) < KIRB_STA_CONNECT_TIMEOUT_MS)
    {
        const wl_status_t status = WiFi.status();
        if (status != lastStatus)
        {
            Serial.printf("[wifi] STA status: %s\n", wifiStatusName(status));
            lastStatus = status;
        }
        delay(250);
    }

    if (WiFi.status() != WL_CONNECTED)
    {
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

/** Start bridge mDNS advertisement and HTTP service record. */
bool initMdns()
{
    if (!MDNS.begin(KIRB_BRIDGE_HOSTNAME))
    {
        Serial.printf("[mdns] failed to start hostname '%s.local'\n",
                      KIRB_BRIDGE_HOSTNAME);
        return false;
    }
    MDNS.addService("http", "tcp", 80);
    Serial.printf("[mdns] bridge hostname: http://%s.local\n",
                  KIRB_BRIDGE_HOSTNAME);
    return true;
}

/** Print build-time network settings at boot. */
void logBootBanner()
{
    Serial.println();
    Serial.println("KirbBridge boot");
    Serial.printf("[build] Wi-Fi SSID: %s\n", KIRB_WIFI_SSID);
    Serial.printf("[build] camera stream URL: %s\n", KIRB_CAMERA_STREAM_URL);
}

} // namespace

/** Arduino setup: initialize serial, network, telemetry, and HTTP server. */
void setup()
{
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    logBootBanner();

    if (!initWifi())
    {
        return;
    }

    (void)initMdns();

    bridgeTelemetryBegin();
    startBridgeServer();

    Serial.println("[bridge] network ready");
    Serial.print("[bridge] connected SSID: ");
    Serial.println(KIRB_WIFI_SSID);
    Serial.print("[bridge] gateway: ");
    Serial.println(WiFi.gatewayIP());
    Serial.print("[bridge] RSSI: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    Serial.print("[bridge] open http://");
    Serial.println(WiFi.localIP());
    Serial.print("[bridge] test http://");
    Serial.print(WiFi.localIP());
    Serial.println("/ping");
}

/** Arduino loop: keep the MSPM0 UART bridge drained. */
void loop()
{
    bridgeTelemetryPoll();
    delay(2);
}
