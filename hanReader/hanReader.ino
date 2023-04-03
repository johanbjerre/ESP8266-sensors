/*
https://github.com/UdoK/esp8266_p1meter_sv/blob/master/esp8266_p1meter/esp8266_p1meter.ino

*/
//#include <SoftwareSerial.h>
#include <HardwareSerial.h>

#include <Ticker.h>

#include "settings.h"
#include "WifiHelper.h"
#include "DecodeHelper.h"

const int DELAY_TIME = 5000;
const int DELAY_AFTER_INIT = 10000;
const int UPDATE_INTERVAL = 20000;
long LAST_UPDATE_SENT = millis();

// Ticker
Ticker ticker;


void setup() {
  // Setup a hw serial connection for communication with the P1 meter and logging (not using inversion)
  Serial.begin(9600, SERIAL_8N1, SERIAL_FULL);
  Serial.println("");
  Serial.println("Swapping UART0 RX to inverted");
  Serial.flush();

  // Invert the RX serialport by setting a register value, this way the TX might continue normally allowing the serial monitor to read println's
  USC0(UART0) = USC0(UART0) | BIT(UCRXI);
  Serial.println("Serial port is ready to recieve.");
  
  pinMode(LED_BUILTIN, OUTPUT);

  ticker.attach(0.2, tick);
  setupWifi();
  ticker.detach();
}

void loop() {
  long now = millis();
  Serial.println("loop now:" + String(now) + ", LAST_UPDATE_SENT :" + String(LAST_UPDATE_SENT) + ", UPDATE_INTERVAL:" + String(UPDATE_INTERVAL));
  if (now - LAST_UPDATE_SENT > UPDATE_INTERVAL) {
    /*
    test
    postDataAPI(2); //test
    LAST_UPDATE_SENT = millis();
    */
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
  if (Serial.available()) {
    memset(telegram, 0, sizeof(telegram));

    while (Serial.available()) {
      ESP.wdtDisable();
      int len = Serial.readBytesUntil('\n', telegram, P1_MAXLINELENGTH);
      ESP.wdtEnable(1);
      processLine(len);
    }
  }
  delay(1000);
}

void processLine(int len) {
  telegram[len] = '\n';
  telegram[len + 1] = 0;
  yield();

  bool result = decode_telegram(len + 1);
  if (result) {
    postDataAPI(CONSUMPTION);
    LAST_UPDATE_SENT = millis();
  }
}
void postDataAPI(long consumption) {
  
  //test with only CONSUMPTION
  HTTPClient http;
  http.begin(client, String(URL_WS_ELECTRICITY));
  http.addHeader("Content-Type", "application/json");
  //[{"Description":"CONSUMPTION","Value": "8"}]
  Serial.println("[{\"Description\":\"CONSUMPTION\",\"Value\":\"" + String(consumption) + "\"}]");
  int httpCode1 = http.POST("[{\"Description\":\"CONSUMPTION\",\"Value\":\"" + String(consumption) + "\"}]");
  String payload1 = http.getString();

  Serial.println(httpCode1);
  Serial.println(payload1);

  http.end();
}