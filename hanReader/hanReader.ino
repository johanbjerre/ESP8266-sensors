#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Ticker.h>
#include "WiFiSettings.h"

// * Include settings
#include "settings.h"
#include "decode.h"

// * Initiate led blinker library
Ticker ticker;

//Wifi
WiFiClient client;

// **********************************
// * Ticker (System LED Blinker)    *
// **********************************

// * Blink on-board Led
void tick()
{
    // * Toggle state
    int state = digitalRead(LED_BUILTIN);    // * Get the current state of GPIO1 pin
    digitalWrite(LED_BUILTIN, !state);       // * Set pin to the opposite state
}

// **********************************
// * P1                             *
// **********************************



void read_p1_hardwareserial()
{
    if (Serial.available())
    {
        memset(telegram, 0, sizeof(telegram));

        while (Serial.available())
        {
            ESP.wdtDisable();
            int len = Serial.readBytesUntil('\n', telegram, P1_MAXLINELENGTH);
            ESP.wdtEnable(1);

            processLine(len);
        }
    }
}

void processLine(int len) {
    telegram[len] = '\n';
    telegram[len + 1] = 0;
    yield();

    bool result = decode_telegram(len + 1);
    if (result) {
        postDataAPI();
        LAST_UPDATE_SENT = millis();
    }
}

void postDataAPI() {
  
  HTTPClient http;
  http.begin(client, String(URL_WS_ELECTRICITY));
  http.addHeader("Content-Type", "application/json");

  String allJsonData="[{\"Description\":\"CONSUMPTION\",\"Value\":\"" + String(CONSUMPTION) + "\"},"+
    "{\"Description\":\"RETURNDELIVERY\",\"Value\":\"" + String(RETURNDELIVERY) + "\"},"+
    "{\"Description\":\"CONSUMPTION_REACT\",\"Value\":\"" + String(CONSUMPTION_REACT) + "\"},"+
    "{\"Description\":\"RETURNDELIVERY_REACT\",\"Value\":\"" + String(RETURNDELIVERY_REACT) + "\"},"+
    "{\"Description\":\"ACTUAL_CONSUMPTION\",\"Value\":\"" + String(ACTUAL_CONSUMPTION) + "\"},"+
    "{\"Description\":\"ACTUAL_RETURNDELIVERY\",\"Value\":\"" + String(ACTUAL_RETURNDELIVERY) + "\"},"+
    "{\"Description\":\"ACTUAL_CONSUMPTION_REACT\",\"Value\":\"" + String(ACTUAL_CONSUMPTION_REACT) + "\"},"+
    "{\"Description\":\"ACTUAL_RETURNDELIVERY_REACT\",\"Value\":\"" + String(ACTUAL_RETURNDELIVERY_REACT) + "\"},"+
    "{\"Description\":\"L1_INSTANT_POWER_USAGE\",\"Value\":\"" + String(L1_INSTANT_POWER_USAGE) + "\"},"+
    "{\"Description\":\"L1_INSTANT_POWER_DELIVERY\",\"Value\":\"" + String(L1_INSTANT_POWER_DELIVERY) + "\"},"+
    "{\"Description\":\"L2_INSTANT_POWER_USAGE\",\"Value\":\"" + String(L2_INSTANT_POWER_USAGE) + "\"},"+
    "{\"Description\":\"L2_INSTANT_POWER_DELIVERY\",\"Value\":\"" + String(L2_INSTANT_POWER_DELIVERY) + "\"},"+
    "{\"Description\":\"L3_INSTANT_POWER_USAGE\",\"Value\":\"" + String(L3_INSTANT_POWER_USAGE) + "\"},"+
    "{\"Description\":\"L3_INSTANT_POWER_DELIVERY\",\"Value\":\"" + String(L3_INSTANT_POWER_DELIVERY) + "\"},"+
    "{\"Description\":\"L1_REACT_POWER_USAGE\",\"Value\":\"" + String(L1_REACT_POWER_USAGE) + "\"},"+
    "{\"Description\":\"L1_REACT_POWER_DELIVERY\",\"Value\":\"" + String(L1_REACT_POWER_DELIVERY) + "\"},"+
    "{\"Description\":\"L2_REACT_POWER_USAGE\",\"Value\":\"" + String(L2_REACT_POWER_USAGE) + "\"},"+
    "{\"Description\":\"L2_REACT_POWER_DELIVERY\",\"Value\":\"" + String(L2_REACT_POWER_DELIVERY) + "\"},"+
    "{\"Description\":\"L3_REACT_POWER_USAGE\",\"Value\":\"" + String(L3_REACT_POWER_USAGE) + "\"},"+
    "{\"Description\":\"L3_REACT_POWER_DELIVERY\",\"Value\":\"" + String(L3_REACT_POWER_DELIVERY) + "\"},"+
    "{\"Description\":\"L1_INSTANT_POWER_CURRENT\",\"Value\":\"" + String(L1_INSTANT_POWER_CURRENT) + "\"},"+
    "{\"Description\":\"L2_INSTANT_POWER_CURRENT\",\"Value\":\"" + String(L2_INSTANT_POWER_CURRENT) + "\"},"+
    "{\"Description\":\"L3_INSTANT_POWER_CURRENT\",\"Value\":\"" + String(L3_INSTANT_POWER_CURRENT) + "\"},"+
    "{\"Description\":\"L1_VOLTAGE\",\"Value\":\"" + String(L1_VOLTAGE) + "\"},"+
    "{\"Description\":\"L2_VOLTAGE\",\"Value\":\"" + String(L2_VOLTAGE) + "\"},"+
    "{\"Description\":\"L3_VOLTAGE\",\"Value\":\"" + String(L3_VOLTAGE) + "\"}"+
    +"]";
  Serial.println(allJsonData);
  int httpCode1 = http.POST(allJsonData);
  String payload1 = http.getString();

  Serial.println(httpCode1);
  Serial.println(payload1);

  http.end();
}

// **********************************
// * Setup Main                     *
// **********************************

void setup()
{
    // Setup a hw serial connection for communication with the P1 meter and logging (not using inversion)
    Serial.begin(BAUD_RATE, SERIAL_8N1, SERIAL_FULL);
    Serial.println("");
    Serial.println("Swapping UART0 RX to inverted");
    Serial.flush();

    // Invert the RX serialport by setting a register value, this way the TX might continue normally allowing the serial monitor to read println's
    USC0(UART0) = USC0(UART0) | BIT(UCRXI);
    Serial.println("Serial port is ready to recieve.");

    // * Set led pin as output
    pinMode(LED_BUILTIN, OUTPUT);

    // * Start ticker with 0.5 because we start in AP mode and try to connect
    ticker.attach(0.6, tick);

    // * If you get here you have connected to the WiFi
    Serial.println(F("Connected to WIFI..."));

    // * Keep LED on
    ticker.detach();
    digitalWrite(LED_BUILTIN, LOW);

    setupWifi();

}

void setupWifi() {
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

// **********************************
// * Loop                           *
// **********************************

void loop()
{
    long now = millis();
    
    if (now - LAST_UPDATE_SENT > UPDATE_INTERVAL) {
        read_p1_hardwareserial();
    }
}
