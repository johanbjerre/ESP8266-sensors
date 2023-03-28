bool isNumber(char *res, int len) {
  for (int i = 0; i < len; i++) {
    if (((res[i] < '0') || (res[i] > '9')) && (res[i] != '.' && res[i] != 0))
      return false;
  }
  return true;
}

int FindCharInArrayRev(char array[], char c, int len) {
  for (int i = len - 1; i >= 0; i--) {
    if (array[i] == c)
      return i;
  }
  return -1;
}


long getValue(char *buffer, int maxlen, char startchar, char endchar) {
  int s = FindCharInArrayRev(buffer, startchar, maxlen - 2);
  int l = FindCharInArrayRev(buffer, endchar, maxlen - 2) - s - 1;

  char res[16];
  memset(res, 0, sizeof(res));

  if (strncpy(res, buffer + s + 1, l)) {
    if (endchar == '*') {
      if (isNumber(res, l))
        // * Lazy convert float to long
        return (1000 * atof(res));
    } else if (endchar == ')') {
      if (isNumber(res, l))
        return atof(res);
    }
  }
  return 0;
}


unsigned int CRC16(unsigned int crc, unsigned char *buf, int len) {
  for (int pos = 0; pos < len; pos++) {
    crc ^= (unsigned int)buf[pos];  // * XOR byte into least sig. byte of crc
                                    // * Loop over each bit
    for (int i = 8; i != 0; i--) {
      // * If the LSB is set
      if ((crc & 0x0001) != 0) {
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





// parsing of telegram according to Swedish ESMR 5.0 implementation //UKR 1220
bool decode_telegram(int len) {
  int startChar = FindCharInArrayRev(telegram, '/', len);
  int endChar = FindCharInArrayRev(telegram, '!', len);
  bool validCRCFound = false;

  for (int cnt = 0; cnt < len; cnt++) {
    Serial.print(telegram[cnt]);
  }
  Serial.print("\n");

  if (startChar >= 0) {
    // * Start found. Reset CRC calculation
    currentCRC = CRC16(0x0000, (unsigned char *)telegram + startChar, len - startChar);
  } else if (endChar >= 0) {
    // * Add to crc calc
    currentCRC = CRC16(currentCRC, (unsigned char *)telegram + endChar, 1);

    char messageCRC[5];
    strncpy(messageCRC, telegram + endChar + 1, 4);

    messageCRC[4] = 0;  // * Thanks to HarmOtten (issue 5)
    validCRCFound = (strtol(messageCRC, NULL, 16) == currentCRC);

    if (validCRCFound)
      Serial.println(F("CRC Valid!"));
    else
      Serial.println(F("CRC Invalid!"));

    currentCRC = 0;
  } else {
    currentCRC = CRC16(currentCRC, (unsigned char *)telegram, len);
  }

  // 1-0:1.8.0(000992.992*kWh)
  // 1-0:1.8.0 = Cumulative hourly active import energy (A+) (Q1+Q4)
  if (strncmp(telegram, "1-0:1.8.0", 9) == 0) {
    CONSUMPTION = getValue(telegram, len, '(', '*');
  }

  // 1-0:2.8.0(000560.157*kWh)
  // 1-0:2.8.0 = Cumulative hourly active export energy (A-) (Q2+Q3)
  if (strncmp(telegram, "1-0:2.8.0", 9) == 0) {
    RETURNDELIVERY = getValue(telegram, len, '(', '*');
  }

  // 1-0:3.8.0(000560.157*kWh)
  // 1-0:3.8.0 = Cumulative hourly reactive import energy (R+) (Q1+Q2)
  if (strncmp(telegram, "1-0:3.8.0", 9) == 0) {
    CONSUMPTION_REACT = getValue(telegram, len, '(', '*');
  }

  // 1-0:4.8.0(000560.157*kWh)
  // 1-0:4.8.0 = Cumulative hourly reactive export energy (R-) (Q3+Q4)
  if (strncmp(telegram, "1-0:4.8.0", 9) == 0) {
    RETURNDELIVERY_REACT = getValue(telegram, len, '(', '*');
  }

  // 1-0:1.7.0(00.424*kW)
  // 1-0:1.7.x = Momentary Active power+ (Q1+Q4)
  if (strncmp(telegram, "1-0:1.7.0", 9) == 0) {
    ACTUAL_CONSUMPTION = getValue(telegram, len, '(', '*');
  }

  // 1-0:2.7.0(00.000*kW)
  // 1-0:2.7.x = Momentary Active power- (Q2+Q3)
  if (strncmp(telegram, "1-0:2.7.0", 9) == 0) {
    ACTUAL_RETURNDELIVERY = getValue(telegram, len, '(', '*');
  }

  // 1-0:3.7.0(00.424*kW)
  // 1-0:3.7.x = Momentary Reactive power + ( Q1+Q2)
  if (strncmp(telegram, "1-0:3.7.0", 9) == 0) {
    ACTUAL_CONSUMPTION_REACT = getValue(telegram, len, '(', '*');
  }

  // 1-0:4.7.0(00.000*kW)
  // 1-0:4.7.x = Momentary Reactive power - ( Q3+Q4)
  if (strncmp(telegram, "1-0:4.7.0", 9) == 0) {
    ACTUAL_RETURNDELIVERY_REACT = getValue(telegram, len, '(', '*');
  }

  // 1-0:21.7.0(00.378*kW)
  // 1-0:21.7.0 = Momentary Active power+ (L1)
  if (strncmp(telegram, "1-0:21.7.0", 10) == 0) {
    L1_INSTANT_POWER_USAGE = getValue(telegram, len, '(', '*');
  }

  // 1-0:22.7.0(00.378*kW)
  // 1-0:22.7.0 = Momentary Active power- (L1)
  if (strncmp(telegram, "1-0:22.7.0", 10) == 0) {
    L1_INSTANT_POWER_DELIVERY = getValue(telegram, len, '(', '*');
  }

  // 1-0:41.7.0(00.378*kW)
  // 1-0:41.7.0 = Momentary Active power+ (L2)
  if (strncmp(telegram, "1-0:41.7.0", 10) == 0) {
    L2_INSTANT_POWER_USAGE = getValue(telegram, len, '(', '*');
  }

  // 1-0:42.7.0(00.378*kW)
  // 1-0:42.7.0 = Momentary Active power- (L2)
  if (strncmp(telegram, "1-0:42.7.0", 10) == 0) {
    L2_INSTANT_POWER_DELIVERY = getValue(telegram, len, '(', '*');
  }

  // 1-0:61.7.0(00.378*kW)
  // 1-0:61.7.0 = Momentary Active power+ (L3)
  if (strncmp(telegram, "1-0:61.7.0", 10) == 0) {
    L3_INSTANT_POWER_USAGE = getValue(telegram, len, '(', '*');
  }

  // 1-0:62.7.0(00.378*kW)
  // 1-0:62.7.0 = Momentary Active power- (L3)
  if (strncmp(telegram, "1-0:62.7.0", 10) == 0) {
    L3_INSTANT_POWER_DELIVERY = getValue(telegram, len, '(', '*');
  }

  // 1-0:23.7.0(00.378*kW)
  // 1-0:23.7.0 = Momentary Reactive power+ (L1)
  if (strncmp(telegram, "1-0:23.7.0", 10) == 0) {
    L1_REACT_POWER_USAGE = getValue(telegram, len, '(', '*');
  }

  // 1-0:24.7.0(00.378*kW)
  // 1-0:24.7.0 = Momentary Reactive power- (L1)
  if (strncmp(telegram, "1-0:24.7.0", 10) == 0) {
    L1_REACT_POWER_DELIVERY = getValue(telegram, len, '(', '*');
  }

  // 1-0:43.7.0(00.378*kW)
  // 1-0:43.7.0 = Momentary Reactive power+ (L2)
  if (strncmp(telegram, "1-0:43.7.0", 10) == 0) {
    L2_REACT_POWER_USAGE = getValue(telegram, len, '(', '*');
  }

  // 1-0:44.7.0(00.378*kW)
  // 1-0:44.7.0 = Momentary Reactive power+ (L2)
  if (strncmp(telegram, "1-0:44.7.0", 10) == 0) {
    L2_REACT_POWER_DELIVERY = getValue(telegram, len, '(', '*');
  }

  // 1-0:63.7.0(00.378*kW)
  // 1-0:63.7.0 = Momentary Reactive power+ (L3)
  if (strncmp(telegram, "1-0:63.7.0", 10) == 0) {
    L3_REACT_POWER_USAGE = getValue(telegram, len, '(', '*');
  }

  // 1-0:64.7.0(00.378*kW)
  // 1-0:64.7.0 = Momentary Reactive power- (L3)
  if (strncmp(telegram, "1-0:64.7.0", 10) == 0) {
    L3_REACT_POWER_DELIVERY = getValue(telegram, len, '(', '*');
  }

  // 1-0:31.7.0(002*A)
  // 1-0:31.7.0 = Momentary RMS Current phase L1
  if (strncmp(telegram, "1-0:31.7.0", 10) == 0) {
    L1_INSTANT_POWER_CURRENT = getValue(telegram, len, '(', '*');
  }
  // 1-0:51.7.0(002*A)
  // 1-0:51.7.0 = Momentary RMS Current phase L2
  if (strncmp(telegram, "1-0:51.7.0", 10) == 0) {
    L2_INSTANT_POWER_CURRENT = getValue(telegram, len, '(', '*');
  }

  // 1-0:71.7.0(002*A)
  // 1-0:71.7.0 = Momentary RMS Current phase L3
  if (strncmp(telegram, "1-0:71.7.0", 10) == 0) {
    L3_INSTANT_POWER_CURRENT = getValue(telegram, len, '(', '*');
  }

  // 1-0:32.7.0(232.0*V)
  // 1-0:32.7.0 = Momentary RMS Phase voltage L1
  if (strncmp(telegram, "1-0:32.7.0", 10) == 0) {
    L1_VOLTAGE = getValue(telegram, len, '(', '*');
  }
  // 1-0:52.7.0(232.0*V)
  // 1-0:52.7.0 = Momentary RMS Phase voltage L2
  if (strncmp(telegram, "1-0:52.7.0", 10) == 0) {
    L2_VOLTAGE = getValue(telegram, len, '(', '*');
  }
  // 1-0:72.7.0(232.0*V)
  // 1-0:72.7.0 = Momentary RMS Phase voltage L3
  if (strncmp(telegram, "1-0:72.7.0", 10) == 0) {
    L3_VOLTAGE = getValue(telegram, len, '(', '*');
  }

  return validCRCFound;
}