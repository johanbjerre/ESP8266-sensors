#include <ESP8266WiFi.h>


#define MIN_VALUE 25
#define MAX_VALUE 800
#define SENSOR_PIN A0    // select the input pin for the potentiometer

int sensorValue1 = 0; 
int sensorValue2 = 0; 

void setup() {
    Serial.begin(9600);
}

void loop() {
  sensorValue1 = analogRead(SENSOR_PIN);
  sensorValue2 = digitalRead(SENSOR_PIN);
  Serial.print("analog:");
  Serial.print(sensorValue1);
  Serial.print(" digital:");
  Serial.print(sensorValue2);
  Serial.print(" ");
  float level = 100 - ((sensorValue1 - MIN_VALUE) * 100 / (MAX_VALUE - MIN_VALUE));
  
  sensorValue1 = level;
  Serial.print("LUX:");
 Serial.println(sensorValue1);


  delay(100);
}
