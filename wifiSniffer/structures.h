#define ETH_MAC_LEN 6

struct beaconinfo
{
  uint8_t bssid[ETH_MAC_LEN];
  uint8_t ssid[33];
  int ssid_len;
  int channel;
  int err;
  signed rssi;
  uint8_t capa[2];
};

/* ==============================================
   Promiscous callback structures, see ESP manual
   ============================================== */
struct RxControl
{
  signed rssi : 8;
  unsigned rate : 4;
  unsigned is_group : 1;
  unsigned : 1;
  unsigned sig_mode : 2;
  unsigned legacy_length : 12;
  unsigned damatch0 : 1;
  unsigned damatch1 : 1;
  unsigned bssidmatch0 : 1;
  unsigned bssidmatch1 : 1;
  unsigned MCS : 7;
  unsigned CWB : 1;
  unsigned HT_length : 16;
  unsigned Smoothing : 1;
  unsigned Not_Sounding : 1;
  unsigned : 1;
  unsigned Aggregation : 1;
  unsigned STBC : 2;
  unsigned FEC_CODING : 1;
  unsigned SGI : 1;
  unsigned rxend_state : 8;
  unsigned ampdu_cnt : 8;
  unsigned channel : 4;
  unsigned : 12;
};

struct LenSeq
{
  uint16_t length;
  uint16_t seq;
  uint8_t address3[6];
};

struct sniffer_buf
{
  struct RxControl rx_ctrl;
  uint8_t buf[36];
  uint16_t cnt;
  struct LenSeq lenseq[1];
};

struct sniffer_buf2
{
  struct RxControl rx_ctrl;
  uint8_t buf[112];
  uint16_t cnt;
  uint16_t len;
};

struct beaconinfo parse_beacon(uint8_t *frame, uint16_t framelen, signed rssi)
{
  struct beaconinfo bi;
  bi.ssid_len = 0;
  bi.channel = 0;
  bi.err = 0;
  bi.rssi = rssi;
  int pos = 36;

  if (frame[pos] == 0x00)
  {
    while (pos < framelen)
    {
      switch (frame[pos])
      {
      case 0x00: //SSID
        bi.ssid_len = (int)frame[pos + 1];
        if (bi.ssid_len == 0)
        {
          memset(bi.ssid, '\x00', 33);
          break;
        }
        if (bi.ssid_len < 0)
        {
          bi.err = -1;
          break;
        }
        if (bi.ssid_len > 32)
        {
          bi.err = -2;
          break;
        }
        memset(bi.ssid, '\x00', 33);
        memcpy(bi.ssid, frame + pos + 2, bi.ssid_len);
        bi.err = 0; // before was error??
        break;
      case 0x03: //Channel
        bi.channel = (int)frame[pos + 2];
        pos = -1;
        break;
      default:
        break;
      }
      if (pos < 0)
        break;
      pos += (int)frame[pos + 1] + 2;
    }
  }
  else
  {
    bi.err = -3;
  }

  bi.capa[0] = frame[34];
  bi.capa[1] = frame[35];
  memcpy(bi.bssid, frame + 10, ETH_MAC_LEN);
  return bi;
}
