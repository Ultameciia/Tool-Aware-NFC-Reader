# Klipper NFC Spool Manager

A hardware bridge between NFC-tagged filament spools and a Klipper-based Toolchanger fleet. This project allows you to assign specific Spoolman IDs to specific Tool Slots (T0-T4) or MMU Lanes across multiple printers using a single handheld device.

## 🚀 Features
* **Multi-Printer Support:** Manage up to 4+ printers from one device.
* **Toolchanger/MMU Aware:** Supports T0-Tx slot assignment for toolchangers, Lane X-X for MMUs or direct assignment for single-tool printers.
* **Non-Blocking UI:** Rotary encoder navigation that stays responsive even during WiFi handshake.
* **Spoolman Integration:** Automatically updates the "Location" field in your Spoolman database.

## 🛠 Hardware Required
* **Microcontroller:** Wemos D1 Mini (ESP8266)
* **NFC Reader:** PN532 (connected via I2C)
* **Input:** KY-040 Rotary Encoder
* **Display:** SSD1306 128x64 OLED

## 🔌 Wiring Diagram
| Component | Pin | ESP8266 Pin |
| :--- | :--- | :--- |
| **OLED/NFC SCL** | SCL | D1 |
| **OLED/NFC SDA** | SDA | D2 |
| **Encoder SW** | Button | D3 |
| **Encoder CLK** | Phase A | D6 |
| **Encoder DT** | Phase B | D5 |

## 📦 Installation
1. Install the **Encoder** (by Paul Stoffregen), **Adafruit PN532**, **Adafruit SSD1306**, and **Adafruit GFX** libraries in Arduino IDE.
2. Open `config.h` and enter your WiFi credentials and Printer IP addresses.
3. Flash the firmware to your Wemos D1 Mini.
4. Add the provided macros to your Klipper `macros.cfg`.

## 📖 Usage
1. **Select Printer:** Scroll and click your target machine.
2. **Select Tool:** Choose which tool slot you are loading.
3. **Scan:** Click to enter Scan Mode and tap your filament's NFC tag (I'm using NTAG215 ymmv with others).
4. **Success:** The variable is saved to Klipper-TC's `offset_save_file.cfg` or other [save_variables] file and Spoolman is updated.
