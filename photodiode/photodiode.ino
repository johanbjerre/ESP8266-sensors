#include <ESP8266WiFi.h>


#define SENSOR_PIN A0    // select the input pin for the potentiometer

int sensorValue = 0;

void setup() {
    Serial.begin(9600);
}

void loop() {
  sensorValue = analogRead(SENSOR_PIN);
  Serial.print("analog:");
  Serial.println(sensorValue);
  
  delay(100);
}
