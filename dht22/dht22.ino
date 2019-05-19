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

const char* ssid = "your ssid";
const char* password = "your password";

#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  //LED set as output
  pinMode(LED, OUTPUT);

  //TURN OFF LED
  digitalWrite(LED, HIGH);

  Serial.begin(9600);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(10000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  dht.begin();
}

void loop() {
  delay(300000); //1000*60*5=300000 (5 min)

  //TURN ON LED
  digitalWrite(LED, LOW);

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read sensor"));

    //TURN OFF LED
    digitalWrite(LED, HIGH);
    return;
  }

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));

  String hString = String(h);
  String tString = String(t);
  postData(hString, tString);

  //TURN OFF LED
  digitalWrite(LED, HIGH);
}

void postData(String humidity, String temperature) {
  HTTPClient http;
  http.begin("http://url");
  http.addHeader("Content-Type", "application/json");

  int httpCode1 = http.POST("[{'Unitname':'dht22','Description':'hall','Value':'" + humidity + "'}]");
  String payload1 = http.getString();

  Serial.println(httpCode1);
  Serial.println(payload1);

  http.end();
}
