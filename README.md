# IMU-OSC

[M5StickC Plus2](https://shop.m5stack.com/products/m5stickc-plus2-esp32-mini-iot-development-kit) → WiFi → OSC → Pure Data.

Portable IMU (accelerometer + gyroscope) streaming over WiFi using OSC protocol.

## Sketches

| Environment | File | Description |
|-------------|------|-------------|
| `slash` | `src/main_slash.cpp` | Machete gesture detection → triggers sound in Pd |
| `imu` | `src/main_imu.cpp` | Continuous accelerometer + gyroscope streaming |

## Setup

1. Install [VS Code](https://code.visualstudio.com/) + **PlatformIO IDE** extension (`platformio.platformio-ide`)
2. Open this folder in VS Code — PlatformIO will download all dependencies automatically
3. Copy `src/config.example.h` to `src/config.h` and fill in your WiFi and OSC settings

## Build & Upload

```bash
pio run -e slash -t upload    # gesture detection
pio run -e imu -t upload      # IMU streaming
```

## Pure Data

Open `pd/Untitled-1.pd` in Pure Data vanilla (0.51+), enable DSP.

## OSC Messages (UDP port 9000)

| Env | Address | Payload |
|-----|---------|---------|
| `slash` | `/slash` | `int` 1 or 2 (gesture direction) |
| `imu` | `/imu` | `float` x6 (ax ay az gx gy gz) |
