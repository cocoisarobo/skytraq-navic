# Skytraq NavIC Library

A comprehensive Arduino library for communicating with GNSS modules based on the Skytraq chipset, with extensive support for the Bharat Pi NavIC module and advanced configuration options.

## 🌟 Features

- 🛰️ **Multi-Constellation Support**: GPS, GLONASS, Galileo, BeiDou, NavIC
- 🌐 **SBAS Configuration**: WAAS, EGNOS, MSAS, GAGAN, SDCM, BDSBAS
- 🗾 **QZSS Support**: Japanese satellite system
- 🔄 **System Management**: Hot/Warm/Cold restart with assisted data
- ⚡ **Power Management**: Normal and power-save modes
- 📊 **Position Control**: Configurable update rates (1-50 Hz)
- 🎯 **Position Pinning**: Static position detection and filtering
- 📡 **DOP Masking**: Quality filtering with PDOP/HDOP/VDOP/GDOP
- 📝 **NMEA Configuration**: Talker ID management (GP/GN/Auto modes)
- ✅ **Reliable Communication**: ACK/NACK response handling

## 📚 API Reference

### System Management

#### **System Restart**
```cpp
bool systemRestart(RestartMode mode = RESTART_WARM, uint16_t year = 0, 
                   uint8_t month = 0, uint8_t day = 0, uint8_t hour = 0, 
                   uint8_t minute = 0, uint8_t second = 0, 
                   int16_t lat = 0, int16_t lon = 0, int16_t alt = 0);
```

**Restart Modes:**
- `RESTART_HOT` - Fast start with recent data (~5-15s)
- `RESTART_WARM` - Medium start with known position (~15-30s) 
- `RESTART_COLD` - Full reset, longest time (~30-60s)

#### **Factory Reset**
```cpp
bool setFactoryDefault();  // Reset all settings to factory defaults
```

#### **Power Management**
```cpp
bool configSystemPowerMode(uint8_t mode = CONFIG_POWER_MODE_NORMAL, 
                          uint8_t attr = CONFIG_ATTR_UPDATE_TO_SRAM);
```

### Position Configuration

#### **Update Rate**
```cpp
bool configSysPosRate(PositionRate posRate = POS_RATE_10HZ, 
                     uint8_t attr = CONFIG_ATTR_UPDATE_TO_SRAM);
```

**Available Rates:** 1, 2, 4, 5, 8, 10, 20, 25, 40, 50 Hz

#### **Position Pinning**
```cpp
bool configPosPinning(PosPinning mode = POS_PINNING_DEFAULT, 
                     uint8_t attr = CONFIG_ATTR_UPDATE_TO_SRAM);

bool configPosPinningParams(uint16_t p_speed, uint16_t p_cnt, uint16_t up_speed, 
                           uint16_t up_cnt, uint16_t up_dist, 
                           uint8_t attr = CONFIG_ATTR_UPDATE_TO_SRAM);
```

### Quality Control

#### **DOP Masking**
```cpp
bool configDOPMask(DOPmode mode, uint16_t pdop, uint16_t hdop, uint16_t vdop, 
                  uint8_t attr = CONFIG_ATTR_UPDATE_TO_SRAM);
```

**DOP Modes:**
- `DISABLE_MODE` - No DOP filtering
- `AUTO_MODE` - PDOP for 3D, HDOP for 2D
- `PDOP_ONLY` - Position DOP only
- `HDOP_ONLY` - Horizontal DOP only  
- `GDOP_ONLY` - Geometric DOP only

### NMEA Configuration

#### **Talker ID**
```cpp
bool configNMEATalkerID(NMEAtalkerID id, uint8_t attr = CONFIG_ATTR_UPDATE_TO_SRAM);
```

**Talker IDs:**
- `NMEA_TALKER_ID_GP` - GPS mode (GP prefix)
- `NMEA_TALKER_ID_GN` - Combined GNSS mode (GN prefix)
- `NMEA_TALKER_ID_AUTO` - Auto mode (NMEA 4.11)

### Legacy Functions

#### **Constellation Setup**
```cpp
bool setConstellation(uint8_t constid = CONFIG_GNSS_GPS, uint8_t attr = 0);
bool configureSBAS(const SBASConfig &config);
bool configureQZSS(uint8_t enable, uint8_t channels = 1, uint8_t attr = 0);
```

## 🏭 NavIC Module Hardware Settings

| Parameter | Value |
|-----------|-------|
| **Receiver** | NavIC L5, GAGAN/GPS L1 C/A Phoenix engine |
| **Constellations** | All GNSS systems enabled |
| **SBAS** | Enabled (2 tracking channels) |
| **Update Rate** | 1 Hz |
| **Baud Rate** | 115200 |
| **Output** | NMEA 0183 |
| **Power Mode** | Normal |
| **Byte Order** | Big-endian |

## 🔄 Start Modes Explained

- **🔥 Hot Start**: Power cycled within seconds, all data valid (fastest)
- **🌡️ Warm Start**: Hours/days offline, position known but ephemeris outdated  
- **❄️ Cold Start**: Complete reset, no stored data (slowest but most reliable)

## 📋 Configuration Attributes

- `CONFIG_ATTR_UPDATE_TO_SRAM` (0x00) - Temporary, lost on power cycle
- `CONFIG_ATTR_UPDATE_TO_SRAM_AND_FLASH` (0x01) - Permanent storage
- `CONFIG_ATTR_TEMPORARILY_ENABLED` (0x02) - Temporary enable

## DOP Masking  
Filters out poor-quality position fixes based on Dilution of Precision values:
- **PDOP**: 3D position accuracy
- **HDOP**: Horizontal accuracy  
- **VDOP**: Vertical accuracy
- **GDOP**: Overall geometric accuracy

## 🌏 Regional SBAS Systems

| System | Region | Constant |
|--------|--------|----------|
| WAAS | North America | `CONFIG_SBAS_WAAS` |
| EGNOS | Europe | `CONFIG_SBAS_EGNOS` |
| MSAS | Japan | `CONFIG_SBAS_MSAS` |
| GAGAN | India | `CONFIG_SBAS_GAGAN` |
| SDCM | Russia | `CONFIG_SBAS_SDCM` |
| BDSBAS | China | `CONFIG_SBAS_BDSBAS` |

## Development status

This library is under active development and currently only supports project specific configuration.  
Additional commands and features will be added in future updates.  
