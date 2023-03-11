#include <Ticker.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Wifi
WiFiClient client;
const char *SSID = "";
const char *PASSWORD = "";

const int DELAY_TIME = 5000;
const int DELAY_AFTER_INIT = 10000;
const int UPDATE_INTERVAL = 20000;
long LAST_UPDATE_DATA = millis();

// Ticker
Ticker ticker;

void setup() {
  Serial.begin(9600);
  //LED set as output
  pinMode(LED_BUILTIN, OUTPUT);

  //TURN OFF LED
  //digitalWrite(LED, HIGH);

  ticker.attach(0.2, tick);
  setupWifi();
}

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
  ticker.detach();
}
void loop() {
  long now = millis();
  Serial.println("loop now:" + String(now) + ", LAST_UPDATE_DATA:" + String(LAST_UPDATE_DATA) + ", UPDATE_INTERVAL:" + String(UPDATE_INTERVAL));
  if (now - LAST_UPDATE_DATA > UPDATE_INTERVAL) {
    readData();
  } else {
    Serial.println("readData NO");
  }
}

void tick() {
  int state = digitalRead(LED_BUILTIN);  // * Get the current state of GPIO1 pin
  digitalWrite(LED_BUILTIN, !state);     // * Set pin to the opposite state
}

void readData() {
  Serial.println("readData YES");
  LAST_UPDATE_DATA = millis();
}