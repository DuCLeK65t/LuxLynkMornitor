<!-- README.md -->
<h1 align="center">🌤️ LuxLynkMonitor</h1>
<p align="center">
  <em>Lux monitoring IoT system</em>
</p>

---

## 📌 Overview
The **LuxLynkMonitor** project monitors light intensity through GY-30 BH1750 sensor by STM32F103C8T6, transmits BLE data through JDY-31 Module to Gateway ESP32 and displays it visually on Blynk platform

<div align="center">
  <img src="https://github.com/user-attachments/assets/e940f9f9-85ef-4f02-9487-af3bfa0232a7">
</div>

---

## 🛠️ Hardware
| Name                | Description                    | Quantity |
|---------------------|--------------------------------|----------|
| STM32F103C8T6       | Sensor Node                    | 1        |
| ESP32 DEVKITV1      | Gateway                        | 1        |
| GY-30 BH1750        | UART Light (Lux) Sensor        | 1        |
| JDY-31              | I2C BLE Module                 | 1        |

---

## 🔧 Installization

### Software
- [PlatformIO](https://platformio.org/)
- Library:
  ```bash
  lib_deps = 
    blynkkk/Blynk @ ^1.2.0
    espressif/ESP32 BLE Arduino @ ^1.0.1
