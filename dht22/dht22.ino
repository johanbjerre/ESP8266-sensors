/*
   ESP8266 with DHT22

   Requires DHT Sensor Library (https://github.com/adafruit/DHT-sensor-library)
   and Adafruit Unified Sensor Lib (https://github.com/adafruit/Adafruit_Sensor)

*/

#include "DHT.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define DHTPIN 5
#define LED D4

const char *AREA = "garage";
const char *UNITNAME_DHT22 = "dht22";
const char *UNITNAME_DS18B20 = "ds18b20";
const int DELAY_TIME = 300000; //1000*60*5=300000 (5 min)
const bool SAVE_HUMIDITY = true;
const bool SAVE_TEMPERATURE = true;
const bool DEBUG_MODE = false;

const char *SSID = "";
const char *PASSWORD = "";
const char *URL_WS = "";

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

  dht.begin();
}

void loop()
{

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

  delay(DELAY_TIME);
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

  postData(String(UNITNAME_DHT22), String(m));
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

  if (DEBUG_MODE)
  {
    Serial.print(F("%  Temperature: "));
    Serial.print(m);
    Serial.print(F("°C "));
  }

  //TODO fix string
  postData(String(UNITNAME_DS18B20), String(m));
}

void postData(String unitname, String measurement)
{
  HTTPClient http;
  http.begin(String(URL_WS));
  http.addHeader("Content-Type", "application/json");

  int httpCode1 = http.POST("[{'Unitname':'" + unitname + "','Description':'" + String(AREA) + "','Value':'" + measurement + "'}]");
  String payload1 = http.getString();

  if (DEBUG_MODE)
  {
    Serial.println(httpCode1);
    Serial.println(payload1);
  }

  http.end();
}
