/*
   ESP8266 with DHT22

   Requires DHT Sensor Library (https://github.com/adafruit/DHT-sensor-library)
   and Adafruit Unified Sensor Lib (https://github.com/adafruit/Adafruit_Sensor)

*/

#include "DHT.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoOTA.h>
#include "../secrets.h"
#include "../sensor_locations.h"

#define DHTPIN 5
#define LED D4

const SensorLocation LOCATION = SensorLocation::Office;
const char *AREA = locationName(LOCATION);
const char *UNITNAME_DHT22 = "dht22";
const char *UNITNAME_DS18B20 = "ds18b20";
const unsigned long DELAY_TIME = 300000UL; //1000*60*5=300000 (5 min)
const bool SAVE_HUMIDITY = true;
const bool SAVE_TEMPERATURE = true;
const bool DEBUG_MODE = true;
unsigned long lastMeasurement = 0;
bool firstMeasurement = true;
char otaHostname[32];

WiFiClientSecure client;
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  //LED set as output
  pinMode(LED, OUTPUT);

  //TURN OFF LED
  digitalWrite(LED, HIGH);

  if (DEBUG_MODE)
  {
    Serial.begin(9600);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(SSID);
  }

  WiFi.begin(SSID, PASSWORD);
  WiFi.mode(WIFI_STA);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(10000);
    if (DEBUG_MODE)
    {
      Serial.print(".");
    }
  }
  if (DEBUG_MODE)
  {
    Serial.println("");
    Serial.println("WiFi connected");
  }

  snprintf(otaHostname, sizeof(otaHostname), "esp8266-%s", locationName(LOCATION));
  ArduinoOTA.setHostname(otaHostname);
  if (OTA_PASSWORD[0] != '\0')
  {
    ArduinoOTA.setPassword(OTA_PASSWORD);
  }
  ArduinoOTA.begin();

  if (DEBUG_MODE)
  {
    Serial.print("OTA ready at ");
    Serial.println(WiFi.localIP());
  }

  dht.begin();
}

void loop()
{
  ArduinoOTA.handle();

  if (firstMeasurement || millis() - lastMeasurement >= DELAY_TIME)
  {
    lastMeasurement = millis();
    firstMeasurement = false;

    //TURN ON LED
    digitalWrite(LED, LOW);

    if (SAVE_HUMIDITY)
    {
      saveHumidity();
    }

    if (SAVE_TEMPERATURE)
    {
      saveTemperature();
    }

    //TURN OFF LED
    digitalWrite(LED, HIGH);
  }

  delay(1);
}

void saveHumidity()
{
  float m = dht.readHumidity();
  if (isnan(m))
  {
    if (DEBUG_MODE)
    {
      Serial.println(F("Failed to read sensor"));
    }

    //TURN OFF LED
    digitalWrite(LED, HIGH);
    return;
  }

  if (DEBUG_MODE)
  {
    Serial.print(F("Humidity: "));
    Serial.print(m);
  }

  postData(String(UNITNAME_DHT22), String(m, 1));
}

void saveTemperature()
{
  float m = dht.readTemperature();
  if (isnan(m))
  {
    if (DEBUG_MODE)
    {
      Serial.println(F("Failed to read sensor"));
    }

    //TURN OFF LED
    digitalWrite(LED, HIGH);
    return;
  }

  // Fix for negative temperature readings
  // Check if the value is suspiciously large negative (sign bit issue)
  if (m < -200.0 || m > 200.0)
  {
    // This indicates incorrect sign bit handling
    // The DHT22 uses bit 15 as sign, bits 0-14 as magnitude
    // But it was read as two's complement signed integer
    int16_t signed_raw = (int16_t)(m * 10);  // Get back to raw signed value
    uint16_t raw = (uint16_t)(signed_raw + (signed_raw < 0 ? 65536 : 0));  // Convert to unsigned
    
    bool isNegative = (raw & 0x8000) != 0;
    uint16_t magnitude = raw & 0x7FFF;  // Clear sign bit to get magnitude
    
    m = (float)magnitude / 10.0;
    if (isNegative)
    {
      m = -m;
    }
  }

  if (DEBUG_MODE)
  {
    Serial.print(F("%  Temperature: "));
    Serial.print(m);
    Serial.print(F("°C "));
  }

  //TODO fix string
  postData(String(UNITNAME_DS18B20), String(m, 1));
}

void postData(String unitname, String measurement)
{
  client.setInsecure();
  HTTPClient http;
  http.begin(client, String(URL_WS));
  http.addHeader("Content-Type", "application/json");

  int httpCode1 = http.POST("[{\"Unitname\":\"" + unitname + "\",\"Description\":\"" + String(AREA) + "\",\"Value\":\"" + measurement + "\"}]");
  String payload1 = http.getString();

  if (DEBUG_MODE)
  {
    Serial.println(httpCode1);
    Serial.println(payload1);
  }

  http.end();
}
