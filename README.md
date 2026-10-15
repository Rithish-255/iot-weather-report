# IoT Weather Reporting - Minor Project

Description
- Collects temperature & humidity from DHT11 and pressure/temperature from BMP280 on an ESP8266.
- Sends readings to a Node.js server.
- Web UI shows real-time values and a chart via Socket.IO.

Repository layout
- server.js           (Node.js server)
- package.json
- public/
  - index.html
  - style.css
- esp/
  - weather_reporter.ino (Arduino sketch for ESP8266)
- simulator/
  - simulator.py       (Python script to simulate sensors)
- README.md

Requirements
- Node.js (16+ recommended) and npm
- For hardware: NodeMCU/ESP8266 or ESP32, DHT11, BMP280, connecting wires
- Arduino IDE + libraries:
  - DHT sensor library (by Adafruit)
  - Adafruit BMP280 library (and Adafruit Unified Sensor)
- (Optional) Python 3 with `requests` to run simulator

Wiring (ESP8266 / NodeMCU)
- DHT11:
  - VCC -> 3.3V
  - GND -> GND
  - Data -> D2 (GPIO4) (use a 10K pull-up to VCC if needed)
- BMP280 (I2C):
  - VIN -> 3.3V
  - GND -> GND
  - SDA -> D2 (GPIO4) OR D1 depending on board (check board pin mapping)
  - SCL -> D1 (GPIO5) OR D2 (match SDA)
  - Note: On some boards SDA=SDA, SCL=SCL; confirm mapping.

Server setup (local)
1. Install dependencies:
   npm install
2. Start server:
   node server.js
3. Open browser to: http://<server-ip>:3000

ESP8266 setup
- Edit `esp/weather_reporter.ino` to set your WiFi SSID, password, and SERVER_IP (or use OTA / mDNS).
- Install required libraries in Arduino IDE.
- Upload to your ESP module.

Simulator (if no hardware)
- Edit simulator/simulator.py to set SERVER_URL.
- Run: python3 simulator/simulator.py

Notes
- The server logs received JSON and broadcasts to any connected browser using Socket.IO.
- Front-end automatically updates numeric cards and the chart for temperature/humidity/pressure.

Sample output is shown at the end of this README (server logs + sample JSON + browser screenshot text).

## Run instructions

1. Install dependencies:
   ```bash
   npm install
   ```
2. Start the server:
   ```bash
   node server.js
   ```
3. Open browser:
   ```text
   http://localhost:3000
   ```
4. If using simulator without hardware:
   ```bash
   python3 simulator/simulator.py --url http://localhost:3000/data
   ```
5. If using ESP8266 hardware:
   - Edit `esp/weather_reporter.ino` with WiFi credentials and server IP.
   - Upload the firmware.

## Example output

```text
Server listening on port 3000
[POST /data] Received: { device: 'simulator-1', timestamp: 1697480000, temperature_dht: 21.4, humidity: 47.3, temperature_bmp: 21.2, pressure_hpa: 1012.6 }
[POST /data] Received: { device: 'esp8266-1', timestamp: 1697480005, temperature_dht: 21.5, humidity: 47.4, temperature_bmp: 21.3, pressure_hpa: 1012.7 }
```

Browser values:
- Temp (DHT): 21.5 °C
- Humidity: 47.4 %
- Temp (BMP): 21.3 °C
- Pressure: 1012.7 hPa
