#ifndef SKYTRAQ_H
#define SKYTRAQ_H

#include <Arduino.h>

#define CONFIG_GNSS_GPS 0x01
#define CONFIG_GNSS_GLONASS 0x02
#define CONFIG_GNSS_GALILEO 0x04
#define CONFIG_GNSS_BEIDOU 0x08
#define CONFIG_GNSS_NAVIC 0x10
#define CONFIG_GNSS_GPS_NAVIC 0x11

#define CONFIG_ATTR_UPDATE_TO_SRAM 0x00
#define CONFIG_ATTR_UPDATE_TO_SRAM_AND_FLASH 0x01

#define CONFIG_SBAS_WAAS 0x01
#define CONFIG_SBAS_EGNOS 0x02
#define CONFIG_SBAS_MSAS 0x04
#define CONFIG_SBAS_GAGAN 0x08
#define CONFIG_SBAS_SDCM 0x10
#define CONFIG_SBAS_BDSBAS 0x20

struct SBASConfig {
  uint8_t enable;            // 0: disable SBAS system 1: enable SBAS system
  uint8_t ranging;           // 0: disable SBAS for navigation 1: enable SBAS for navigation 2: auto-mode
  uint8_t rangingURA;        // Ranging URA Mask Default:8, range 0~15
  uint8_t correction;        // 0: disable correction 1: enable correction
  uint8_t trackingChannels;  // Value: 0~3 Set how many channels are reserved for SBAS tracking
  uint8_t subsystemMask;     // Bit mask for WAAS/EGNOS/MSAS/GAGAN/SDCM/BDSBAS
  uint8_t attr;              // 0=SRAM only, 1=SRAM & FLASH

  SBASConfig(
    uint8_t en = 1,
    uint8_t ranging = 2,
    uint8_t r_ura = 8,
    uint8_t corr = 1,
    uint8_t trackingCh = 3,
    uint8_t subsystemMask = 0x07,
    uint8_t attr = 0)
    : enable(en), ranging(ranging), rangingURA(r_ura),
      correction(corr), trackingChannels(trackingCh),
      subsystemMask(subsystemMask), attr(attr) {}
};

class Skytraq {
public:
  Skytraq(uint8_t txPin, uint8_t rxPin, uint32_t baudRate);

  void begin();

  bool setConstellation(uint8_t constid = CONFIG_GNSS_GPS, uint8_t attr = 0);

  bool configureSBAS(const SBASConfig &config);

  bool configureQZSS(uint8_t enable, uint8_t channels = 1, uint8_t attr = 0);

private:
  uint8_t _tx, _rx;
  uint32_t baud;

  bool sendCommand(uint8_t *payload, size_t len);
  uint8_t calcChecksum(uint8_t *msg, size_t len);
  bool getAck();
};

#endif
