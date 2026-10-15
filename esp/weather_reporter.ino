/*
  Weather Reporter
  - Reads DHT11 (temp, humidity) and BMP280 (pressure, temperature)
  - Sends JSON POST to server at http://SERVER_IP:3000/data
  - Uses ESP8266WiFi and HTTPClient
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include "DHT.h"

// ======== CONFIGURE HERE ========
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";
const char* SERVER_IP = "192.168.1.100"; // set to your server IP
const uint16_t SERVER_PORT = 3000;
const unsigned long SEND_INTERVAL_MS = 5000; // send every 5s

// Pin definitions
#define DHTPIN D4        // NodeMCU D4 (GPIO2) or change as wired
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP280 bmp; // I2C

unsigned long lastSend = 0;

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("Weather Reporter starting...");

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 20000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Connected. IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi connect failed.");
  }

  dht.begin();

  if (!bmp.begin(0x76)) {
    Serial.println("Could not find BMP280 sensor at 0x76/0x77");
  } else {
    Serial.println("BMP280 initialized.");
  }
}

void loop() {
  if (millis() - lastSend < SEND_INTERVAL_MS) {
    delay(100);
    return;
  }
  lastSend = millis();

  float humidity = dht.readHumidity();
  float temp_dht = dht.readTemperature();
  float temp_bmp = NAN;
  float pressure = NAN;

  if (isnan(temp_dht) || isnan(humidity)) {
    Serial.println("DHT read failed");
  }

  if (bmp.begin(0x76) || bmp.begin(0x77)) {
    temp_bmp = bmp.readTemperature();
    pressure = bmp.readPressure() / 100.0F;
  }

  String payload = "{";
  payload += "\"device\":\"esp8266-1\",";
  payload += "\"timestamp\":" + String((unsigned long)(millis() / 1000)) + ",";
  payload += "\"temperature_dht\":" + (isnan(temp_dht) ? String("null") : String(temp_dht, 1)) + ",";
  payload += "\"humidity\":" + (isnan(humidity) ? String("null") : String(humidity, 1)) + ",";
  payload += "\"temperature_bmp\":" + (isnan(temp_bmp) ? String("null") : String(temp_bmp, 1)) + ",";
  payload += "\"pressure_hpa\":" + (isnan(pressure) ? String("null") : String(pressure, 1));
  payload += "}";

  Serial.print("Payload: ");
  Serial.println(payload);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String("http://") + SERVER_IP + ":" + String(SERVER_PORT) + "/data";
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST(payload);
    if (httpCode > 0) {
      String resp = http.getString();
      Serial.printf("POST %d, response: %s\n", httpCode, resp.c_str());
    } else {
      Serial.printf("POST failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  } else {
    Serial.println("WiFi not connected; skipping send.");
  }
}
