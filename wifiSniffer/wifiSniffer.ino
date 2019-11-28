#include <ESP8266WiFi.h>
#include "./functions.h"

#define disable 0
#define enable 1
unsigned int channel = 1;

void setup()
{
  Serial.begin(57600);

  //Promiscuous works only with station mode
  wifi_set_opmode(STATION_MODE);
  wifi_set_channel(channel);
  wifi_promiscuous_enable(disable);
  //Set up promiscuous callback
  wifi_set_promiscuous_rx_cb(promisc_cb);
  wifi_promiscuous_enable(enable);
}

void loop()
{
  channel = 1;
  wifi_set_channel(channel);
  while (true)
  {
    //Array is not finite, check bounds and adjust if required
    nothing_new++;
    if (nothing_new > 100)
    {
      nothing_new = 0;
      channel++;
      //Only scan channels 1 to 14
      if (channel == 15)
        break;
      wifi_set_channel(channel);
    }
    //critical processing timeslice for NONOS SDK! No delay(0) yield()
    delay(1);
  }
}
