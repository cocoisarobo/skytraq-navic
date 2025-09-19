#ifndef SKYTRAQ_H
#define SKYTRAQ_H

#include <Arduino.h>

#define CONFIG_POWER_MODE_NORMAL 0x00
#define CONFIG_POWER_MODE_POWER_SAVE 0x01

#define CONFIG_GNSS_GPS 0x01
#define CONFIG_GNSS_GLONASS 0x02
#define CONFIG_GNSS_GALILEO 0x04
#define CONFIG_GNSS_BEIDOU 0x08
#define CONFIG_GNSS_NAVIC 0x10
#define CONFIG_GNSS_GPS_NAVIC 0x11
#define CONFIG_GNSS_ALL (CONFIG_GNSS_GPS | CONFIG_GNSS_GLONASS | CONFIG_GNSS_GALILEO | CONFIG_GNSS_BEIDOU | CONFIG_GNSS_NAVIC)

#define CONFIG_ATTR_UPDATE_TO_SRAM 0x00
#define CONFIG_ATTR_UPDATE_TO_SRAM_AND_FLASH 0x01
#define CONFIG_ATTR_TEMPORARILY_ENABLED 0x02

#define CONFIG_SBAS_WAAS 0x01
#define CONFIG_SBAS_EGNOS 0x02
#define CONFIG_SBAS_MSAS 0x04
#define CONFIG_SBAS_GAGAN 0x08
#define CONFIG_SBAS_SDCM 0x10
#define CONFIG_SBAS_BDSBAS 0x20
#define CONFIG_SBAS_ALL (CONFIG_SBAS_WAAS | CONFIG_SBAS_EGNOS | CONFIG_SBAS_MSAS | CONFIG_SBAS_GAGAN | CONFIG_SBAS_SDCM | CONFIG_SBAS_BDSBAS)

struct SBASConfig
{
  uint8_t enable;           // 0: disable SBAS system 1: enable SBAS system
  uint8_t ranging;          // 0: disable SBAS for navigation 1: enable SBAS for navigation 2: auto-mode
  uint8_t rangingURA;       // Ranging URA Mask Default:8, range 0~15
  uint8_t correction;       // 0: disable correction 1: enable correction
  uint8_t trackingChannels; // Value: 0~3 Set how many channels are reserved for SBAS tracking
  uint8_t subsystemMask;    // Bit mask for WAAS/EGNOS/MSAS/GAGAN/SDCM/BDSBAS
  uint8_t attr;             // 0=SRAM only, 1=SRAM & FLASH

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

/*
SYSTEM RESTART
00 = Reserved
01 = System Reset, Hot start
02 = System Reset, Warm start
03 = System Reset, Cold start
04 = Reserved
*/
enum RestartMode : uint8_t
{
  RESTART_HOT = 0x01,
  RESTART_WARM = 0x02,
  RESTART_COLD = 0x03
};

/*
CONFIGURE SYSTEM POSITION RATE
Value with 1, 2, 4, 5, 8, 10, 20, 25, 40, 50
Check with your module's datasheet for supported rates
*/
enum PositionRate : uint8_t
{
  POS_RATE_1HZ = 1,
  POS_RATE_2HZ = 2,
  POS_RATE_4HZ = 4,
  POS_RATE_5HZ = 5,
  POS_RATE_8HZ = 8,
  POS_RATE_10HZ = 10,
  POS_RATE_20HZ = 20,
  POS_RATE_25HZ = 25,
  POS_RATE_40HZ = 40,
  POS_RATE_50HZ = 50
};

/*
CONFIGURE DOP MASK
00 : Disable
01 : Auto mode, PDOP when 3-D fix and HDOP when 2-D fix
02 : PDOP only
03 : HDOP only
04 : GDOP only
*/
enum DOPmode : uint8_t
{
  DISABLE_MODE = 0x00,
  AUTO_MODE = 0x01,
  PDOP_ONLY = 0x02,
  HDOP_ONLY = 0x03,
  GDOP_ONLY = 0x04
};

/*
CONFIGURE POSITION PINNING
0: default
1: enable
2: disable
*/
enum PosPinning : uint8_t
{
  POS_PINNING_DEFAULT = 0x00,
  POS_PINNING_ENABLE = 0x01,
  POS_PINNING_DISABLE = 0x02
};

/*
CONFIGURE NMEA TALKER ID
0: GP mode
1: GN mode
2: Auto mode (supported only in NMEA version 4.11): according to NMEA 4.11 to combine GNSS system solution to output GN, GP, GL, GA, GB or GI appropriately.
*/
enum NMEAtalkerID : uint8_t
{
  NMEA_TALKER_ID_GP = 0x00,
  NMEA_TALKER_ID_GN = 0x01,
  NMEA_TALKER_ID_AUTO = 0x02
};

class Skytraq
{
public:
  Skytraq(uint8_t txPin, uint8_t rxPin, uint32_t baudRate);

  void begin();

  bool systemRestart(RestartMode mode = RESTART_WARM, uint16_t year = 0000, uint8_t month = 00, uint8_t day = 00, uint8_t hour = 00, uint8_t minute = 00, uint8_t second = 00, int16_t lat = 0000, int16_t lon = 0000, int16_t alt = 0000);

  bool setFactoryDefault();

  // bool configSerialPort(uint32_t baudRate);

  bool configSystemPowerMode(uint8_t mode = CONFIG_POWER_MODE_NORMAL, uint8_t attr = CONFIG_ATTR_UPDATE_TO_SRAM);

  bool configSysPosRate(PositionRate posRate = POS_RATE_10HZ, uint8_t attr = CONFIG_ATTR_UPDATE_TO_SRAM);

  /*
  Configure Position Pinning Parameters
  @param p_speed Position pinning speed in Km/Hr
  @param p_cnt Position pinning count in second
  @param up_speed Unpinning speed in Km/Hr
  @param up_cnt Unpinning count in second
  @param up_dist Unpinning distance in meters
  @param attr Configuration attribute (0=SRAM only, 1=SRAM & FLASH)
  @return true if the command was acknowledged, false otherwise
  */
  bool configPosPinningParams(uint16_t p_speed, uint16_t p_cnt, uint16_t up_speed, uint16_t up_cnt, uint16_t up_dist, uint8_t attr = CONFIG_ATTR_UPDATE_TO_SRAM);

  bool configPosPinning(PosPinning mode = POS_PINNING_DEFAULT, uint8_t attr = CONFIG_ATTR_UPDATE_TO_SRAM);

  bool configDOPMask(DOPmode mode, uint16_t pdop, uint16_t hdop, uint16_t vdop, uint8_t attr = CONFIG_ATTR_UPDATE_TO_SRAM);

  bool configNMEATalkerID(NMEAtalkerID id, uint8_t attr = CONFIG_ATTR_UPDATE_TO_SRAM);

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
