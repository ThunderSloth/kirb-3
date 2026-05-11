# Kirb-3 ESP32 Bridge

Arduino IDE should open the sketch from:

- `source/Kirb-3-ESP32/KirbBridge/KirbBridge.ino`

Recommended layout:

- `KirbBridge/`

The `KirbBridge/` folder contains the Wi-Fi/UI/telemetry bridge sketch and the
local support files it needs, including:

- `bridge_httpd.cpp`
- `bridge_telemetry.cpp`
- `dashboard_index.h`
- `kirb_config.h`
- `partitions.csv`

Wi-Fi is configured in the shared `source/kirb_wifi.h` header. The bridge uses
the dedicated Kirb router SSID from that file.
