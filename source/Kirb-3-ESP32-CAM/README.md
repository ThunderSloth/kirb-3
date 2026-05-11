# Kirb-3 Camera Node

Arduino IDE should open the sketch from:

- `source/Kirb-3-ESP32-CAM/KirbCameraNode/KirbCameraNode.ino`

Recommended layout:

- `KirbCameraNode/`
- `ci.yml`

The `KirbCameraNode/` folder contains the `.ino` sketch and all local support
files that the Arduino build expects to find next to the sketch, including:

- `camera_httpd.cpp`
- `board_config.h`
- `camera_index.h`
- `camera_pins.h`
- `kirb_config.h`
- `partitions.csv`

Wi-Fi is configured in the shared `source/kirb_wifi.h` header. The camera node
uses the dedicated Kirb router SSID from that file.

`ci.yml` is repository metadata and is not required by the Arduino IDE.
