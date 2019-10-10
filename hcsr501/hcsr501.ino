
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char *AREA = "livingroom";
const char *UNITNAME = "pir";

#define LED D4
#define SENSOR D7

void setup()
{
  //LED set as output and SENSOR as input
  pinMode(LED, OUTPUT);
  pinMode(SENSOR, INPUT);

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
}

void loop()
{
  Serial.println("Looking");

  long state = digitalRead(SENSOR);
  if (state == HIGH)
  {
    //TURN ON LED
    digitalWrite(LED, LOW);
    Serial.println("MOVEMENT DETECTED");
    //SAVE DATA
    postData();
    delay(10000);
  }
  else
  {
    digitalWrite(LED, HIGH);
    Serial.println("NO MOVEMENT DETECTED");
    delay(100);
  }
}

void postData()
{
  HTTPClient http;
  http.begin(String(URL_WS));
  http.addHeader("Content-Type", "application/json");

  int httpCode1 = http.POST("[{'Unitname':'" + String(UNITNAME) + "','Description':'" + String(AREA) + "','Value':'0'}]");
  String payload1 = http.getString();

  Serial.println(httpCode1);
  Serial.println(payload1);

  http.end();
}
