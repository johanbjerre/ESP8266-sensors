
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char *AREA = "livingroom";
const char *UNITNAME = "bmp180";
const int DELAY_TIME = 300000; //1000*60*5=300000 (5 min)
#define LED D4

Adafruit_BMP085 bmp;

void setup()
{
  Serial.begin(9600);
  if (!bmp.begin())
  {
    Serial.println("Check wiring!");
    while (1)
    {
    }
  }
  //LED set as output
  pinMode(LED, OUTPUT);

  //TURN OFF LED
  digitalWrite(LED, HIGH);

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
  Serial.println("Temperature = " + String(bmp.readTemperature()) + " *C");

  Serial.println("Pressure = " + String(bmp.readPressure() / 100.0) + " Pa");
  postData(String(bmp.readPressure() / 100.0));

  Serial.println("Altitude = " + String(bmp.readAltitude()) + " meters");

  Serial.println("Pressure at sealevel (calculated) = " + String(bmp.readSealevelPressure()) + " Pa");

  Serial.println("Real altitude = " + String(bmp.readAltitude(102000)) + " meters");

  Serial.println();
  delay(DELAY_TIME);
}

void postData(String measurement)
{
  HTTPClient http;
  http.begin(String(URL_WS));
  http.addHeader("Content-Type", "application/json");

  int httpCode1 = http.POST("[{'Unitname':'" + String(UNITNAME) + "','Description':'" + String(AREA) + "','Value':'" + measurement + "'}]");
  String payload1 = http.getString();

  Serial.println(httpCode1);
  Serial.println(payload1);

  http.end();
}
