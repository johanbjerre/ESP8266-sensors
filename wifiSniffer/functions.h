// This-->tab == "functions.h"

// Expose Espressif SDK functionality
extern "C" {
#include "user_interface.h"
  typedef void (*freedom_outside_cb_t)(uint8 status);
  int  wifi_register_send_pkt_freedom_cb(freedom_outside_cb_t cb);
  void wifi_unregister_send_pkt_freedom_cb(void);
  int  wifi_send_pkt_freedom(uint8 *buf, int len, bool sys_seq);
}

#include <ESP8266WiFi.h>
#include "./structures.h"

#define MAX_APS_TRACKED 100
#define MAX_CLIENTS_TRACKED 200

int aps_known_count = 0;                                  // Number of known APs
int nothing_new = 0;
int clients_known_count = 0;                              // Number of known CLIENTs

void printMac(uint8_t *buf){
    // Signal strength is in byte 0
    Serial.printf("%i ",int8_t(buf[0]));
    
  if((buf[22]==0x00)&&(buf[22+1]==0x27)&&(buf[22+2]==0x10)&&(buf[22+3]==0x7D)&&(buf[22+4]==0xc9)&&(buf[22+5]==0xd0)){
    Serial.printf("|%-50s|","T410 - 00:27:10:7D:C9:D0");
  }
  else if((buf[22]==0x6c)&&(buf[22+1]==0xad)&&(buf[22+2]==0xf8)&&(buf[22+3]==0x97)&&(buf[22+4]==0x50)&&(buf[22+5]==0x9e)){
    Serial.printf("|%-50s|","Chromecast - 6c:ad:f8:97:50:9e");
  }
  else if((buf[22]==0xb4)&&(buf[22+1]==0xcd)&&(buf[22+2]==0x27)&&(buf[22+3]==0xa1)&&(buf[22+4]==0x12)&&(buf[22+5]==0xcb)){
    Serial.printf("|%-50s|","HUAWEI - b4:cd:27:a1:12:cb");
  }
  else if((buf[22]==0x34)&&(buf[22+1]==0x97)&&(buf[22+2]==0xf6)&&(buf[22+3]==0x63)&&(buf[22+4]==0x39)&&(buf[22+5]==0xc8)){
    Serial.printf("|%-50s|","ROUTER - 34:97:F6:63:39:C8");
  }
  else if((buf[22]==0xd0)&&(buf[22+1]==0x17)&&(buf[22+2]==0xc2)&&(buf[22+3]==0x61)&&(buf[22+4]==0x49)&&(buf[22+5]==0x78)){
    Serial.printf("|%-50s|","Previously seen unknown - d0:17:c2:61:49:78");
  }
  else if((buf[22]==0x00)&&(buf[22+1]==0x16)&&(buf[22+2]==0xa6)&&(buf[22+3]==0x1b)&&(buf[22+4]==0xda)&&(buf[22+5]==0x58)){
    Serial.printf("|%-50s|","Previously seen unknown - 00:16:a6:1b:da:58");
  }
  else if((buf[22]==0xec)&&(buf[22+1]==0x08)&&(buf[22+2]==0x6b)&&(buf[22+3]==0x83)&&(buf[22+4]==0xf4)&&(buf[22+5]==0x7f)){
    Serial.printf("|%-50s|","Previously seen unknown - 08:6b:83:f4:7f");
  }
  else if((buf[22]==0xa0)&&(buf[22+1]==0x6f)&&(buf[22+2]==0xaa)&&(buf[22+3]==0x7c)&&(buf[22+4]==0x86)&&(buf[22+5]==0xa8)){
    Serial.printf("|%-50s|","Previously seen unknown - a0:6f:aa:7c:86:a8");
  }
  else if((buf[22]==0xf4)&&(buf[22+1]==0xf5)&&(buf[22+2]==0xe8)&&(buf[22+3]==0x32)&&(buf[22+4]==0xca)&&(buf[22+5]==0x34)){
    Serial.printf("|%-50s|","Previously seen unknown - f4:f5:e8:32:ca:34");
  }
  else if((buf[22]==0xa4)&&(buf[22+1]==0x77)&&(buf[22+2]==0x33)&&(buf[22+3]==0x6b)&&(buf[22+4]==0x78)&&(buf[22+5]==0xa6)){
    Serial.printf("|%-50s|","Previously seen unknown - a4:77:33:6b:78:a6");
  }
  else if((buf[22]==0x2c)&&(buf[22+1]==0x4d)&&(buf[22+2]==0x54)&&(buf[22+3]==0xd9)&&(buf[22+4]==0x7c)&&(buf[22+5]==0x08)){
    Serial.printf("|%-50s|","Previously seen unknown - 2c:4d:54:d9:7c:08");
  }
  else if((buf[22]==0x20)&&(buf[22+1]==0xdf)&&(buf[22+2]==0xb9)&&(buf[22+3]==0x12)&&(buf[22+4]==0x35)&&(buf[22+5]==0xaa)){
    Serial.printf("|%-50s|","Previously seen unknown - 20:df:b9:12:35:aa");
  }
  else if((buf[22]==0xa8)&&(buf[22+1]==0xdb)&&(buf[22+2]==0x03)&&(buf[22+3]==0x24)&&(buf[22+4]==0x9c)&&(buf[22+5]==0xfa)){
    Serial.printf("|%-50s|","Previously seen unknown - a8:db:03:24:9c:fa");
  }
  else if((buf[22]==0xbc)&&(buf[22+1]==0xdd)&&(buf[22+2]==0xc2)&&(buf[22+3]==0x9d)&&(buf[22+4]==0x30)&&(buf[22+5]==0xce)){
    Serial.printf("|%-50s|","ESP outside - BC:DD:C2:9D:30:CE");
  }
  else if((buf[22]==0xbc)&&(buf[22+1]==0xdd)&&(buf[22+2]==0xc2)&&(buf[22+3]==0x23)&&(buf[22+4]==0x60)&&(buf[22+5]==0x20)){
    Serial.printf("|%-50s|","ESP air pressure - BC:DD:C2:23:60:20");
  }
  else if((buf[22]==0xbc)&&(buf[22+1]==0xdd)&&(buf[22+2]==0xc2)&&(buf[22+3]==0x9d)&&(buf[22+4]==0x2b)&&(buf[22+5]==0x8f)){
    Serial.printf("|%-50s|","ESP livingroom - BC:DD:C2:9D:2B:8F");
  }
  else if((buf[22]==0xbc)&&(buf[22+1]==0xdd)&&(buf[22+2]==0xc2)&&(buf[22+3]==0x9d)&&(buf[22+4]==0xf1)&&(buf[22+5]==0x79)){
    Serial.printf("|%-50s|","ESP PIR living room BC:DD:C2:9D:F1:79");
  }
  else if((buf[22]==0xec)&&(buf[22+1]==0xfa)&&(buf[22+2]==0xbc)&&(buf[22+3]==0x28)&&(buf[22+4]==0x67)&&(buf[22+5]==0x65)){
    Serial.printf("|%-50s|","ESP guest room EC:FA:BC:28:67:65");
  }
  else if((buf[22]==0xbc)&&(buf[22+1]==0xdd)&&(buf[22+2]==0xc2)&&(buf[22+3]==0x9a)&&(buf[22+4]==0xd4)&&(buf[22+5]==0x13)){
    Serial.printf("|%-50s|","ESP laundry room BC:DD:C2:9A:D4:13");
  }
    
    else{
      Serial.printf("|");
      for(int i=0;i<5;i++) {
        Serial.printf("%02x:",buf[22+i]);
      }
      Serial.printf("%02x  ",buf[22+5]);
      Serial.printf("%-31s|","");
    }
      //data type
    Serial.printf("%s", "Type:");
    Serial.printf("%02x|",buf[12]);
    
     if(buf[12]==0x40) Serial.printf("|%-20s|","Disconnected: ");
     else if(buf[12]==0x08) Serial.printf("|%-20s|","Data: ");
     else if(buf[12]==0x88) Serial.printf("|%-20s|","QOS: ");
     else Serial.printf("|%-20s|","Unknown type");
    // Origin MAC address starts at byte 22
    // Print MAC address
    Serial.printf("\n");

    
}

void promisc_cb(uint8_t *buf, uint16_t len)
{
  signed potencia;
  if (len == 12) {
    struct RxControl *sniffer = (struct RxControl*) buf;
    potencia = sniffer->rssi;
  } else if (len == 128) {
    struct sniffer_buf2 *sniffer = (struct sniffer_buf2*) buf;
    struct beaconinfo beacon = parse_beacon(sniffer->buf, 112, sniffer->rx_ctrl.rssi);
    potencia = sniffer->rx_ctrl.rssi;
  } else {
    struct sniffer_buf *sniffer = (struct sniffer_buf*) buf;
    potencia = sniffer->rx_ctrl.rssi;
  }

  // Position 12 in the array is where the packet type number is located
  // For info on the different packet type numbers check:
  // https://stackoverflow.com/questions/12407145/interpreting-frame-control-bytes-in-802-11-wireshark-trace
  // https://supportforums.cisco.com/document/52391/80211-frames-starter-guide-learn-wireless-sniffer-traces
  // https://ilovewifi.blogspot.mx/2012/07/80211-frame-types.html
  if((buf[12]==0x88)||(buf[12]==0x40)||(buf[12]==0x94)||(buf[12]==0xa4)||(buf[12]==0xb4)||(buf[12]==0x08))
  {
    printMac(buf);

    // Enable this lines if you want to scan for a specific MAC address
    // Specify desired MAC address on line 10 of structures.h
    /*int same = 1;
    for(int i=0;i<6;i++)
    {
      if(buf[22+i]!=desired[i])
      {
        same=0;
        break;
      }
    }
    if(same)
    {

    }
    //different device
    else
    {

    }*/
  }
  //Different packet type numbers
  else
  {

  }
}
