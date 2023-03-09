
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const int oneWireBus = 4;

WiFiClient client;
OneWire oneWire(oneWireBus);

DallasTemperature sensors(&oneWire);

const char *AREA = "outside";
const char *SSID = "";
const char *PASSWORD = "";
const char *URL_WS = "";

#define LED D4
void setup()
{
  //LED set as output
  pinMode(LED, OUTPUT);

  //TURN OFF LED
  digitalWrite(LED, HIGH);

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

  postData(String(temperatureC));
  //TURN OFF LED
  digitalWrite(LED, HIGH);

  delay(300000); //1000*60*5=300000 (5 min)
}

void postData(String temperature)
{
  HTTPClient http;
  http.begin(client, String(URL_WS));
  http.addHeader("Content-Type", "application/json");

  int httpCode1 = http.POST("[{\"Unitname\":\"ds18b20\",\"Description\":\"" + String(AREA) + "\",\"Value\":\"" + temperature + "\"}]");
  String payload1 = http.getString();

  Serial.println(httpCode1);
  Serial.println(payload1);

  http.end();
}
