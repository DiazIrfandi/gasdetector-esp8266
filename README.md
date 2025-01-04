# 🌫️ Gas Detector ESP8266

[![Arduino](https://img.shields.io/badge/Arduino-IDE-00979C?style=for-the-badge&logo=arduino&logoColor=white)](https://www.arduino.cc/)
[![ESP8266](https://img.shields.io/badge/ESP8266-Supported-blue?style=for-the-badge)](https://www.espressif.com/en/products/socs/esp8266)
[![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)](LICENSE)

A real-time gas detection system using **ESP8266** and the **MQ2 gas sensor**. This project sends live gas concentration data via a web interface hosted on the ESP8266. Perfect for monitoring environments for gas leaks or ensuring safety in hazardous areas. 🚨

---

## 🚀 Features
- 📊 **Live Monitoring**: View gas concentration in real-time via a responsive web interface.
- 🔥 **MQ2 Sensor Integration**: Detect gases like LPG, CO, and smoke.
- 🌐 **Web Dashboard**: Built with **ESPAsyncWebServer** and optimized with **AsyncTCP**.
- 🛠️ **Configurable Settings**: JSON-based configuration stored locally.

---

## 📦 Requirements
Before running this project, make sure you have the following:
- **Hardware**:
  - ESP8266 Module (e.g., NodeMCU or Wemos D1 Mini)
  - MQ2 Gas Sensor
  - Breadboard, Jumper Wires, and a 5V Power Source
- **Software**:
  - Arduino IDE with the following libraries installed:
    - [ArduinoJSON](https://github.com/bblanchon/ArduinoJson)
    - [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
    - [AsyncTCP](https://github.com/me-no-dev/AsyncTCP)

---

## 🛠️ Installation
Follow these steps to set up the project:
![Cable Blueprint](https://raw.githubusercontent.com/DiazIrfandi/gasdetector-esp8266/refs/heads/main/how-to-connect-the-cable.png)

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/username/gas-detector-esp8266.git
   cd gas-detector-esp8266
