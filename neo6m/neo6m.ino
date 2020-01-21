#include <TinyGPS++.h> // library for GPS module
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
TinyGPSPlus gps;
SoftwareSerial ss(4, 5);// The serial connection to the GPS device
float latitude, longitude;
int year, month, date, hour, minute, second;
String date_str, time_str, lat_str, lng_str, coords;
int pm;

const char *AREA = "car";
const int DELAY_TIME = 1000*60*5;
const bool DEBUG_MODE = true;
const char *UNITNAME = "gps";

const char *SSID = "";
const char *PASSWORD = "";
const char *URL_WS = "";
const char *URL_PING = "";

void setup()
{
  Serial.begin(115200);
  ss.begin(9600);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);
  WiFi.begin(SSID, PASSWORD);           //connecting to wifi
  while (WiFi.status() != WL_CONNECTED) // while wifi not connected
  {
    delay(500);
    Serial.print("."); //print "...."
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop()
{
  while (ss.available() > 0)   //while data is available
    if (gps.encode(ss.read())) //read gps data
    {
      if (gps.location.isValid()) //check whether gps location is valid
      {
        latitude = gps.location.lat();
        lat_str = String(latitude, 6); // latitude location is stored in a string
        longitude = gps.location.lng();
        lng_str = String(longitude, 6); //longitude location is stored in a string
      }
      if (gps.date.isValid()) //check whether gps date is valid
      {
        date_str = "";
        date = gps.date.day();
        month = gps.date.month();
        year = gps.date.year();
        if (date < 10)
          date_str = '0';
        date_str += String(date); // values of date,month and year are stored in a string
        date_str += " / ";

        if (month < 10)
          date_str += '0';
        date_str += String(month); // values of date,month and year are stored in a string
        date_str += " / ";
        if (year < 10)
          date_str += '0';
        date_str += String(year); // values of date,month and year are stored in a string
      }
      if (gps.time.isValid()) //check whether gps time is valid
      {
        time_str = "";
        hour = gps.time.hour();
        minute = gps.time.minute();
        second = gps.time.second();
        minute = (minute + 30); // converting to IST
        if (minute > 59)
        {
          minute = minute - 60;
          hour = hour + 1;
        }
        hour = (hour + 5);
        if (hour > 23)
          hour = hour - 24; // converting to IST
        if (hour >= 12)     // checking whether AM or PM
          pm = 1;
        else
          pm = 0;
        hour = hour % 12;
        if (hour < 10)
          time_str = '0';
        time_str += String(hour); //values of hour,minute and time are stored in a string
        time_str += " : ";
        if (minute < 10)
          time_str += '0';
        time_str += String(minute); //values of hour,minute and time are stored in a string
        time_str += " : ";
        if (second < 10)
          time_str += '0';
        time_str += String(second); //values of hour,minute and time are stored in a string
        if (pm == 1)
          time_str += " PM ";
        else
          time_str += " AM ";
      }
      if (gps.location.isValid() && gps.date.isValid() && gps.satellites.isValid() && gps.time.isValid())
      {
        //location
        Serial.print("lat_str:" + lat_str+", lng_str:" + lng_str);

        coords=lng_str + "," +lat_str;
        postData(String(UNITNAME),lng_str,lat_str);

        //date
        Serial.print(", date_str:" + date_str);
        
        //location
        Serial.print(", gps.satellites:" + String(gps.satellites.value()));

        //time
        Serial.print(", time_str:" + time_str);
        Serial.println("");
        delay(DELAY_TIME);
      }
    }

}

void postData(String unitname, String longStr, String latStr)
{
  HTTPClient http;
  http.begin(String(URL_WS));
  http.addHeader("Content-Type", "application/json");

Serial.println("[{\"Unitname\":\"" + unitname + "\",\"Description\":\"" + String(AREA) + "\",\"Lat\":\"" + latStr + "\",\"Long\":\"" + longStr + "\"}]");
  int httpCode1 = http.POST("[{\"Unitname\":\"" + unitname + "\",\"Description\":\"" + String(AREA) + "\",\"Lat\":\"" + latStr + "\",\"Long\":\"" + longStr + "\"}]");
  String payload1 = http.getString();

  if (DEBUG_MODE)
  {
    Serial.println(httpCode1);
    Serial.println(payload1);
  }

  http.end();
}
