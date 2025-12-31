# SmartArduino
SmartArduino is an experimental **ESP32 + Arduino** project focused on integrating embedded devices with the **SmartThings Cloud**.

## What it does
* Communicates with SmartThings via REST API (GET / POST)
* Reads device states and sends commands
* Handles access / refresh tokens
* Uses HTTPS with `NetworkClientSecure`
* Supports BLE features (e.g. HID keyboard, connection tracking)
* Runs on ESP32 with Wi-Fi and FreeRTOS tasks
