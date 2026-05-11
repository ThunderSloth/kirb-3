#include "Arduino.h"
#include "esp_http_server.h"
#include <WiFi.h>

#include "bridge_httpd.h"
#include "bridge_telemetry.h"
#include "dashboard_index.h"
#include "kirb_config.h"

/** @file bridge_httpd.cpp
 *  @brief HTTP handlers for the bridge dashboard, telemetry, and commands.
 */

namespace
{

/** Copy the URL query string into a heap buffer owned by the caller. */
static esp_err_t parse_get(httpd_req_t *req, char **obuf)
{
    char *buf = nullptr;
    size_t buf_len = httpd_req_get_url_query_len(req) + 1;

    if (buf_len > 1)
    {
        buf = static_cast<char *>(malloc(buf_len));
        if (buf == nullptr)
        {
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }

        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK)
        {
            *obuf = buf;
            return ESP_OK;
        }

        free(buf);
    }

    httpd_resp_send_404(req);
    return ESP_FAIL;
}

/** Serve the embedded dashboard page. */
static esp_err_t index_handler(httpd_req_t *req)
{
    Serial.println("[http] GET /");
    httpd_resp_set_type(req, "text/html; charset=utf-8");
    return httpd_resp_send(req, dashboard_index_html, HTTPD_RESP_USE_STRLEN);
}

/** Health-check endpoint used during network bring-up. */
static esp_err_t ping_handler(httpd_req_t *req)
{
    Serial.println("[http] GET /ping");
    httpd_resp_set_type(req, "text/plain");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    return httpd_resp_send(req, "pong", HTTPD_RESP_USE_STRLEN);
}

/** Return Wi-Fi status as a small JSON object. */
static esp_err_t net_handler(httpd_req_t *req)
{
    Serial.println("[http] GET /net");
    char json[192];
    snprintf(json, sizeof(json),
             "{\"ssid\":\"%s\",\"ip\":\"%s\",\"gateway\":\"%s\",\"rssi\":%d}",
             WiFi.SSID().c_str(), WiFi.localIP().toString().c_str(),
             WiFi.gatewayIP().toString().c_str(), WiFi.RSSI());
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    return httpd_resp_send(req, json, HTTPD_RESP_USE_STRLEN);
}

/** Return the camera capture URL used by the browser stream loop. */
static esp_err_t camera_url_handler(httpd_req_t *req)
{
    char json[256];
    snprintf(json, sizeof(json), "{\"url\":\"%s\"}", KIRB_CAMERA_STREAM_URL);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, json, HTTPD_RESP_USE_STRLEN);
}

/** Return the latest MSPM0 telemetry snapshot as JSON. */
static esp_err_t telemetry_handler(httpd_req_t *req)
{
    static char json[768];
    static uint32_t lastBuildMs = 0;
    const uint32_t nowMs = millis();

    if (lastBuildMs == 0 || (nowMs - lastBuildMs) >= 50)
    {
        bridgeTelemetryBuildJson(json, sizeof(json));
        lastBuildMs = nowMs;
    }

    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, json, HTTPD_RESP_USE_STRLEN);
}

/** Queue one dashboard command for the MSPM0 command bridge. */
static esp_err_t command_handler(httpd_req_t *req)
{
    char *buf = nullptr;
    char command[128];
    char response[160];

    if (parse_get(req, &buf) != ESP_OK)
    {
        return ESP_FAIL;
    }

    if (httpd_query_key_value(buf, "cmd", command, sizeof(command)) != ESP_OK)
    {
        free(buf);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
    free(buf);

    const bool accepted =
        bridgeTelemetryQueueCommand(command, response, sizeof(response));

    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

    char json[256];
    snprintf(json, sizeof(json), "{\"accepted\":%s,\"message\":\"%s\"}",
             accepted ? "true" : "false", response);
    return httpd_resp_send(req, json, HTTPD_RESP_USE_STRLEN);
}

/** Return console response lines newer than a caller-provided sequence. */
static esp_err_t console_handler(httpd_req_t *req)
{
    char *buf = nullptr;
    char sinceBuf[16];
    uint32_t sinceSeq = 0;

    if (parse_get(req, &buf) == ESP_OK && buf != nullptr)
    {
        if (httpd_query_key_value(buf, "since", sinceBuf, sizeof(sinceBuf)) ==
            ESP_OK)
        {
            sinceSeq = static_cast<uint32_t>(strtoul(sinceBuf, nullptr, 10));
        }
        free(buf);
    }

    char json[1024];
    bridgeTelemetryBuildConsoleJson(sinceSeq, json, sizeof(json));
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, json, HTTPD_RESP_USE_STRLEN);
}

} // namespace

void startBridgeServer()
{
    httpd_handle_t bridge_httpd = nullptr;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 8;
    config.stack_size = 8192;
    config.max_open_sockets = 4;
    config.lru_purge_enable = true;
    config.recv_wait_timeout = 10;
    config.send_wait_timeout = 10;

    httpd_uri_t index_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = index_handler,
        .user_ctx = nullptr,
    };
    httpd_uri_t ping_uri = {
        .uri = "/ping",
        .method = HTTP_GET,
        .handler = ping_handler,
        .user_ctx = nullptr,
    };
    httpd_uri_t net_uri = {
        .uri = "/net",
        .method = HTTP_GET,
        .handler = net_handler,
        .user_ctx = nullptr,
    };
    httpd_uri_t telemetry_uri = {
        .uri = "/telemetry",
        .method = HTTP_GET,
        .handler = telemetry_handler,
        .user_ctx = nullptr,
    };
    httpd_uri_t camera_url_uri = {
        .uri = "/camera_url",
        .method = HTTP_GET,
        .handler = camera_url_handler,
        .user_ctx = nullptr,
    };
    httpd_uri_t command_uri = {
        .uri = "/command",
        .method = HTTP_GET,
        .handler = command_handler,
        .user_ctx = nullptr,
    };
    httpd_uri_t console_uri = {
        .uri = "/console",
        .method = HTTP_GET,
        .handler = console_handler,
        .user_ctx = nullptr,
    };

    if (httpd_start(&bridge_httpd, &config) == ESP_OK)
    {
        Serial.printf("[http] bridge server started on port %u\n",
                      config.server_port);
        httpd_register_uri_handler(bridge_httpd, &index_uri);
        httpd_register_uri_handler(bridge_httpd, &ping_uri);
        httpd_register_uri_handler(bridge_httpd, &net_uri);
        httpd_register_uri_handler(bridge_httpd, &camera_url_uri);
        httpd_register_uri_handler(bridge_httpd, &telemetry_uri);
        httpd_register_uri_handler(bridge_httpd, &command_uri);
        httpd_register_uri_handler(bridge_httpd, &console_uri);
    }
    else
    {
        Serial.println("[http] bridge server failed to start");
    }
}
