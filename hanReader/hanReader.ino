#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Ticker.h>
#include "WiFiSettings.h"

// * Include settings
#include "settings.h"

// * Initiate led blinker library
Ticker ticker;

//Wifi
WiFiClient client;

// **********************************
// * Ticker (System LED Blinker)    *
// **********************************

// * Blink on-board Led
void tick()
{
    // * Toggle state
    int state = digitalRead(LED_BUILTIN);    // * Get the current state of GPIO1 pin
    digitalWrite(LED_BUILTIN, !state);       // * Set pin to the opposite state
}

// **********************************
// * P1                             *
// **********************************

unsigned int CRC16(unsigned int crc, unsigned char *buf, int len)
{
	for (int pos = 0; pos < len; pos++)
    {
		crc ^= (unsigned int)buf[pos];    // * XOR byte into least sig. byte of crc
                                          // * Loop over each bit
        for (int i = 8; i != 0; i--)
        {
            // * If the LSB is set
            if ((crc & 0x0001) != 0)
            {
                // * Shift right and XOR 0xA001
                crc >>= 1;
				crc ^= 0xA001;
			}
            // * Else LSB is not set
            else
                // * Just shift right
                crc >>= 1;
		}
	}
	return crc;
}

bool isNumber(char *res, int len)
{
    for (int i = 0; i < len; i++)
    {
        if (((res[i] < '0') || (res[i] > '9')) && (res[i] != '.' && res[i] != 0))
            return false;
    }
    return true;
}

int FindCharInArrayRev(char array[], char c, int len)
{
    for (int i = len - 1; i >= 0; i--)
    {
        if (array[i] == c)
            return i;
    }
    return -1;
}

long getValue(char *buffer, int maxlen, char startchar, char endchar)
{
    int s = FindCharInArrayRev(buffer, startchar, maxlen - 2);
    int l = FindCharInArrayRev(buffer, endchar, maxlen - 2) - s - 1;

    char res[16];
    memset(res, 0, sizeof(res));

    if (strncpy(res, buffer + s + 1, l))
    {
        if (endchar == '*')
        {
            if (isNumber(res, l))
                // * Lazy convert float to long
                return (1000 * atof(res));
        }
        else if (endchar == ')')
        {
            if (isNumber(res, l))
                return atof(res);
        }
    }
    return 0;
}
// parsing of telegram according to Swedish ESMR 5.0 implementation //UKR 1220
bool decode_telegram(int len)
{
    int startChar = FindCharInArrayRev(telegram, '/', len);
    int endChar = FindCharInArrayRev(telegram, '!', len);
    bool validCRCFound = false;

    for (int cnt = 0; cnt < len; cnt++) {
        Serial.print(telegram[cnt]);
    }
    Serial.print("\n");

    if (startChar >= 0)
    {
        // * Start found. Reset CRC calculation
        currentCRC = CRC16(0x0000,(unsigned char *) telegram+startChar, len-startChar);
    }
    else if (endChar >= 0)
    {
        // * Add to crc calc
        currentCRC = CRC16(currentCRC,(unsigned char*)telegram+endChar, 1);

        char messageCRC[5];
        strncpy(messageCRC, telegram + endChar + 1, 4);

        messageCRC[4] = 0;   // * Thanks to HarmOtten (issue 5)
        validCRCFound = (strtol(messageCRC, NULL, 16) == currentCRC);

        if (validCRCFound)
            Serial.println(F("CRC Valid!"));
        else
            Serial.println(F("CRC Invalid!"));

        currentCRC = 0;
    }
    else
    {
        currentCRC = CRC16(currentCRC, (unsigned char*) telegram, len);
    }

    // 1-0:1.8.0(000992.992*kWh)
    // 1-0:1.8.0 = Cumulative hourly active import energy (A+) (Q1+Q4)
    if (strncmp(telegram, "1-0:1.8.0", 9) == 0)
    {
        CONSUMPTION = getValue(telegram, len, '(', '*');
    }

    // 1-0:2.8.0(000560.157*kWh)
    // 1-0:2.8.0 = Cumulative hourly active export energy (A-) (Q2+Q3)
    if (strncmp(telegram, "1-0:2.8.0", 9) == 0)
    {
        RETURNDELIVERY = getValue(telegram, len, '(', '*');
    }

    // 1-0:3.8.0(000560.157*kWh)
    // 1-0:3.8.0 = Cumulative hourly reactive import energy (R+) (Q1+Q2)
    if (strncmp(telegram, "1-0:3.8.0", 9) == 0)
    {
        CONSUMPTION_REACT = getValue(telegram, len, '(', '*');
    }

    // 1-0:4.8.0(000560.157*kWh)
    // 1-0:4.8.0 = Cumulative hourly reactive export energy (R-) (Q3+Q4)
    if (strncmp(telegram, "1-0:4.8.0", 9) == 0)
    {
        RETURNDELIVERY_REACT = getValue(telegram, len, '(', '*');
    }

    // 1-0:1.7.0(00.424*kW)
    // 1-0:1.7.x = Momentary Active power+ (Q1+Q4)
    if (strncmp(telegram, "1-0:1.7.0", 9) == 0)
    {
        ACTUAL_CONSUMPTION = getValue(telegram, len, '(', '*');
    }

    // 1-0:2.7.0(00.000*kW) 
    // 1-0:2.7.x = Momentary Active power- (Q2+Q3)
    if (strncmp(telegram, "1-0:2.7.0", 9) == 0)
    {
        ACTUAL_RETURNDELIVERY = getValue(telegram, len, '(', '*');
    }

    // 1-0:3.7.0(00.424*kW)
    // 1-0:3.7.x = Momentary Reactive power + ( Q1+Q2)
    if (strncmp(telegram, "1-0:3.7.0", 9) == 0)
    {
        ACTUAL_CONSUMPTION_REACT = getValue(telegram, len, '(', '*');
    }

    // 1-0:4.7.0(00.000*kW) 
    // 1-0:4.7.x = Momentary Reactive power - ( Q3+Q4)
    if (strncmp(telegram, "1-0:4.7.0", 9) == 0)
    {
        ACTUAL_RETURNDELIVERY_REACT = getValue(telegram, len, '(', '*');
    }

    // 1-0:21.7.0(00.378*kW)
    // 1-0:21.7.0 = Momentary Active power+ (L1)
    if (strncmp(telegram, "1-0:21.7.0", 10) == 0)
    {
        L1_INSTANT_POWER_USAGE = getValue(telegram, len, '(', '*');
    }

    // 1-0:22.7.0(00.378*kW)
    // 1-0:22.7.0 = Momentary Active power- (L1)
    if (strncmp(telegram, "1-0:22.7.0", 10) == 0)
    {
        L1_INSTANT_POWER_DELIVERY = getValue(telegram, len, '(', '*');
    }

    // 1-0:41.7.0(00.378*kW)
    // 1-0:41.7.0 = Momentary Active power+ (L2)
    if (strncmp(telegram, "1-0:41.7.0", 10) == 0)
    {
        L2_INSTANT_POWER_USAGE = getValue(telegram, len, '(', '*');
    }

    // 1-0:42.7.0(00.378*kW)
    // 1-0:42.7.0 = Momentary Active power- (L2)
    if (strncmp(telegram, "1-0:42.7.0", 10) == 0)
    {
        L2_INSTANT_POWER_DELIVERY = getValue(telegram, len, '(', '*');
    }

    // 1-0:61.7.0(00.378*kW)
    // 1-0:61.7.0 = Momentary Active power+ (L3)
    if (strncmp(telegram, "1-0:61.7.0", 10) == 0)
    {
        L3_INSTANT_POWER_USAGE = getValue(telegram, len, '(', '*');
    }

    // 1-0:62.7.0(00.378*kW)
    // 1-0:62.7.0 = Momentary Active power- (L3)
    if (strncmp(telegram, "1-0:62.7.0", 10) == 0)
    {
        L3_INSTANT_POWER_DELIVERY = getValue(telegram, len, '(', '*');
    }

    // 1-0:23.7.0(00.378*kW)
    // 1-0:23.7.0 = Momentary Reactive power+ (L1)
    if (strncmp(telegram, "1-0:23.7.0", 10) == 0)
    {
        L1_REACT_POWER_USAGE = getValue(telegram, len, '(', '*');
    }

    // 1-0:24.7.0(00.378*kW)
    // 1-0:24.7.0 = Momentary Reactive power- (L1)
    if (strncmp(telegram, "1-0:24.7.0", 10) == 0)
    {
        L1_REACT_POWER_DELIVERY = getValue(telegram, len, '(', '*');
    }

    // 1-0:43.7.0(00.378*kW)
    // 1-0:43.7.0 = Momentary Reactive power+ (L2)
    if (strncmp(telegram, "1-0:43.7.0", 10) == 0)
    {
        L2_REACT_POWER_USAGE = getValue(telegram, len, '(', '*');
    }

    // 1-0:44.7.0(00.378*kW)
    // 1-0:44.7.0 = Momentary Reactive power+ (L2)
    if (strncmp(telegram, "1-0:44.7.0", 10) == 0)
    {
        L2_REACT_POWER_DELIVERY = getValue(telegram, len, '(', '*');
    }

    // 1-0:63.7.0(00.378*kW)
    // 1-0:63.7.0 = Momentary Reactive power+ (L3)
    if (strncmp(telegram, "1-0:63.7.0", 10) == 0)
    {
        L3_REACT_POWER_USAGE = getValue(telegram, len, '(', '*');
    }

    // 1-0:64.7.0(00.378*kW)
    // 1-0:64.7.0 = Momentary Reactive power- (L3)
    if (strncmp(telegram, "1-0:64.7.0", 10) == 0)
    {
        L3_REACT_POWER_DELIVERY = getValue(telegram, len, '(', '*');
    }

    // 1-0:31.7.0(002*A)
    // 1-0:31.7.0 = Momentary RMS Current phase L1
    if (strncmp(telegram, "1-0:31.7.0", 10) == 0)
    {
        L1_INSTANT_POWER_CURRENT = getValue(telegram, len, '(', '*');
    }
    // 1-0:51.7.0(002*A)
    // 1-0:51.7.0 = Momentary RMS Current phase L2
    if (strncmp(telegram, "1-0:51.7.0", 10) == 0)
    {
        L2_INSTANT_POWER_CURRENT = getValue(telegram, len, '(', '*');
    }
    
    // 1-0:71.7.0(002*A)
    // 1-0:71.7.0 = Momentary RMS Current phase L3
    if (strncmp(telegram, "1-0:71.7.0", 10) == 0)
    {
        L3_INSTANT_POWER_CURRENT = getValue(telegram, len, '(', '*');
    }

    // 1-0:32.7.0(232.0*V)
    // 1-0:32.7.0 = Momentary RMS Phase voltage L1
    if (strncmp(telegram, "1-0:32.7.0", 10) == 0)
    {
        L1_VOLTAGE = getValue(telegram, len, '(', '*');
    }
    // 1-0:52.7.0(232.0*V)
    // 1-0:52.7.0 = Momentary RMS Phase voltage L2
    if (strncmp(telegram, "1-0:52.7.0", 10) == 0)
    {
        L2_VOLTAGE = getValue(telegram, len, '(', '*');
    }   
    // 1-0:72.7.0(232.0*V)
    // 1-0:72.7.0 = Momentary RMS Phase voltage L3
    if (strncmp(telegram, "1-0:72.7.0", 10) == 0)
    {
        L3_VOLTAGE = getValue(telegram, len, '(', '*');
    }

    return validCRCFound;
}

void read_p1_hardwareserial()
{
    if (Serial.available())
    {
        memset(telegram, 0, sizeof(telegram));

        while (Serial.available())
        {
            ESP.wdtDisable();
            int len = Serial.readBytesUntil('\n', telegram, P1_MAXLINELENGTH);
            ESP.wdtEnable(1);

            processLine(len);
        }
    }
}

void processLine(int len) {
    telegram[len] = '\n';
    telegram[len + 1] = 0;
    yield();

    bool result = decode_telegram(len + 1);
    if (result) {
        postDataAPI();
        LAST_UPDATE_SENT = millis();
    }
}

void postDataAPI() {
  
  HTTPClient http;
  http.begin(client, String(URL_WS_ELECTRICITY));
  http.addHeader("Content-Type", "application/json");

  String jsonData="[{\"Description\":\"CONSUMPTION\",\"Value\":\"" + String(CONSUMPTION) + "\"}"+
  ",{\"Description\":\"RETURNDELIVERY\",\"Value\":\"" + String(RETURNDELIVERY) + "\"}"+
  ",{\"Description\":\"ACTUAL_CONSUMPTION\",\"Value\":\"" + String(ACTUAL_CONSUMPTION) + "\"}"+
  ",{\"Description\":\"ACTUAL_RETURNDELIVERY\",\"Value\":\"" + String(ACTUAL_RETURNDELIVERY) + "\"}"+
  +"]";
  String allJsonData="[{\"Description\":\"CONSUMPTION\",\"Value\":\"" + String(CONSUMPTION) + "\"},"+
    "{\"Description\":\"RETURNDELIVERY\",\"Value\":\"" + String(RETURNDELIVERY) + "\"},"+
    "{\"Description\":\"CONSUMPTION_REACT\",\"Value\":\"" + String(CONSUMPTION_REACT) + "\"},"+
    "{\"Description\":\"RETURNDELIVERY_REACT\",\"Value\":\"" + String(RETURNDELIVERY_REACT) + "\"},"+
    "{\"Description\":\"ACTUAL_CONSUMPTION\",\"Value\":\"" + String(ACTUAL_CONSUMPTION) + "\"},"+
    "{\"Description\":\"ACTUAL_RETURNDELIVERY\",\"Value\":\"" + String(ACTUAL_RETURNDELIVERY) + "\"},"+
    "{\"Description\":\"ACTUAL_CONSUMPTION_REACT\",\"Value\":\"" + String(ACTUAL_CONSUMPTION_REACT) + "\"},"+
    "{\"Description\":\"ACTUAL_RETURNDELIVERY_REACT\",\"Value\":\"" + String(ACTUAL_RETURNDELIVERY_REACT) + "\"},"+
    "{\"Description\":\"L1_INSTANT_POWER_USAGE\",\"Value\":\"" + String(L1_INSTANT_POWER_USAGE) + "\"},"+
    "{\"Description\":\"L1_INSTANT_POWER_DELIVERY\",\"Value\":\"" + String(L1_INSTANT_POWER_DELIVERY) + "\"},"+
    "{\"Description\":\"L2_INSTANT_POWER_USAGE\",\"Value\":\"" + String(L2_INSTANT_POWER_USAGE) + "\"},"+
    "{\"Description\":\"L2_INSTANT_POWER_DELIVERY\",\"Value\":\"" + String(L2_INSTANT_POWER_DELIVERY) + "\"},"+
    "{\"Description\":\"L3_INSTANT_POWER_USAGE\",\"Value\":\"" + String(L3_INSTANT_POWER_USAGE) + "\"},"+
    "{\"Description\":\"L3_INSTANT_POWER_DELIVERY\",\"Value\":\"" + String(L3_INSTANT_POWER_DELIVERY) + "\"},"+
    "{\"Description\":\"L1_REACT_POWER_USAGE\",\"Value\":\"" + String(L1_REACT_POWER_USAGE) + "\"},"+
    "{\"Description\":\"L1_REACT_POWER_DELIVERY\",\"Value\":\"" + String(L1_REACT_POWER_DELIVERY) + "\"},"+
    "{\"Description\":\"L2_REACT_POWER_USAGE\",\"Value\":\"" + String(L2_REACT_POWER_USAGE) + "\"},"+
    "{\"Description\":\"L2_REACT_POWER_DELIVERY\",\"Value\":\"" + String(L2_REACT_POWER_DELIVERY) + "\"},"+
    "{\"Description\":\"L3_REACT_POWER_USAGE\",\"Value\":\"" + String(L3_REACT_POWER_USAGE) + "\"},"+
    "{\"Description\":\"L3_REACT_POWER_DELIVERY\",\"Value\":\"" + String(L3_REACT_POWER_DELIVERY) + "\"},"+
    "{\"Description\":\"L1_INSTANT_POWER_CURRENT\",\"Value\":\"" + String(L1_INSTANT_POWER_CURRENT) + "\"},"+
    "{\"Description\":\"L2_INSTANT_POWER_CURRENT\",\"Value\":\"" + String(L2_INSTANT_POWER_CURRENT) + "\"},"+
    "{\"Description\":\"L3_INSTANT_POWER_CURRENT\",\"Value\":\"" + String(L3_INSTANT_POWER_CURRENT) + "\"},"+
    "{\"Description\":\"L1_VOLTAGE\",\"Value\":\"" + String(L1_VOLTAGE) + "\"},"+
    "{\"Description\":\"L2_VOLTAGE\",\"Value\":\"" + String(L2_VOLTAGE) + "\"},"+
    "{\"Description\":\"L3_VOLTAGE\",\"Value\":\"" + String(L3_VOLTAGE) + "\"}"+
    +"]";
  Serial.println(allJsonData);
  int httpCode1 = http.POST(allJsonData);
  String payload1 = http.getString();

  Serial.println(httpCode1);
  Serial.println(payload1);

  http.end();
}

// **********************************
// * Setup Main                     *
// **********************************

void setup()
{
    // Setup a hw serial connection for communication with the P1 meter and logging (not using inversion)
    Serial.begin(BAUD_RATE, SERIAL_8N1, SERIAL_FULL);
    Serial.println("");
    Serial.println("Swapping UART0 RX to inverted");
    Serial.flush();

    // Invert the RX serialport by setting a register value, this way the TX might continue normally allowing the serial monitor to read println's
    USC0(UART0) = USC0(UART0) | BIT(UCRXI);
    Serial.println("Serial port is ready to recieve.");

    // * Set led pin as output
    pinMode(LED_BUILTIN, OUTPUT);

    // * Start ticker with 0.5 because we start in AP mode and try to connect
    ticker.attach(0.6, tick);

    // * If you get here you have connected to the WiFi
    Serial.println(F("Connected to WIFI..."));

    // * Keep LED on
    ticker.detach();
    digitalWrite(LED_BUILTIN, LOW);

    setupWifi();

}

void setupWifi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);

  WiFi.begin(SSID, PASSWORD);
  WiFi.mode(WIFI_STA);

  while (WiFi.status() != WL_CONNECTED) {
    delay(10000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

// **********************************
// * Loop                           *
// **********************************

void loop()
{
    long now = millis();
    
    if (now - LAST_UPDATE_SENT > UPDATE_INTERVAL) {
        read_p1_hardwareserial();
    }
}
