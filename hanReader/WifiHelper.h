#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include "WifiSettings.h"

//Wifi
WiFiClient client;

void setupWifi() {
  Serial.begin(9600);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);

  WiFi.begin(SSID, PASSWORD);
  WiFi.mode(WIFI_STA);

  while (WiFi.status() != WL_CONNECTED) {
    delay(10000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void postData(String temperature)
{
  HTTPClient http;
  http.begin(client, "");// String(URL_WS)
  http.addHeader("Content-Type", "application/json");

  int httpCode1 = http.POST("[{\"Unitname\":\"ds18b20\",\"Description\":\"\",\"Value\":\"" + temperature + "\"}]");
  String payload1 = http.getString();

  Serial.println(httpCode1);
  Serial.println(payload1);

  http.end();
}