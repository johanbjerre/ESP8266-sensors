#include <Ticker.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Wifi
WiFiClient client;
const char *SSID = "";
const char *PASSWORD = "";

const int DELAY_TIME = 5000; //1000*60*5=300000 (5 min)

// Ticker
Ticker ticker;

void setup()
{
  Serial.begin(9600);
  //LED set as output
  pinMode(LED_BUILTIN, OUTPUT);

  //TURN OFF LED
  //digitalWrite(LED, HIGH);

  ticker.attach(0.2,tick);
  setupWifi();
}

void setupWifi()
{
  Serial.begin(9600);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID); 

  WiFi.begin(SSID, PASSWORD);
  WiFi.mode(WIFI_STA);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(10000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}
void loop()
{
  Serial.println();
  ticker.detach();
  delay(DELAY_TIME);
}

void tick()
{
    int state = digitalRead(LED_BUILTIN);    // * Get the current state of GPIO1 pin
    digitalWrite(LED_BUILTIN, !state);       // * Set pin to the opposite state
}
