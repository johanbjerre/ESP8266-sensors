
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const int oneWireBus = 4;

OneWire oneWire(oneWireBus);

DallasTemperature sensors(&oneWire);

const char *ssid = "your ssid";
const char *password = "your password";
const char *area = "outside";

void setup()
{
  //LED set as output
  pinMode(LED, OUTPUT);

  //TURN OFF LED
  digitalWrite(LED, HIGH);

  Serial.begin(9600);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(10000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  sensors.begin();
}

void loop()
{

  //TURN ON LED
  digitalWrite(LED, LOW);

  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);
  Serial.print(temperatureC);
  Serial.println("ºC");

  postData(temperatureC);
  //TURN OFF LED
  digitalWrite(LED, HIGH);

  delay(300000); //1000*60*5=300000 (5 min)
}

void postData(String temperature)
{
  HTTPClient http;
  http.begin("http://81.4.102.148:81/api/Weather/saveNow");
  http.addHeader("Content-Type", "application/json");

  int httpCode1 = http.POST("[{'Unitname':'ds18b20','Description':'" + area + "','Value':'" + temperature + "'}]");
  String payload1 = http.getString();

  Serial.println(httpCode1);
  Serial.println(payload1);

  http.end();
}