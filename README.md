# Skytraq GNSS Module Arduino Library Documentation

## 1. Overview
The **Skytraq Arduino Library** provides an easy-to-use interface for controlling Skytraq GNSS (Global Navigation Satellite System) modules using an Arduino. It supports:

- Configuring GNSS constellations (GPS, GLONASS, Galileo, BeiDou, NavIC)
- SBAS (Satellite-Based Augmentation System) configuration
- Position rate and DOP (Dilution of Precision) configuration
- Position pinning and NMEA talker ID settings
- System restart and factory default commands

This library communicates with Skytraq modules via **UART** (serial interface).

**Note:** This library might not work for all development boards yet. It has been tested with **ESP32 boards** only.

---

## 2. Hardware Requirements
- ESP32 microcontroller (tested with ESP32)
- Skytraq GNSS module
- Connection via UART (TX, RX pins)
- Power supply compatible with the GNSS module

---

## 3. Installation
The library can be installed in one of two ways:

1. **Arduino Library Manager**  
   Search for "Skytraq" in the Arduino Library Manager and install it directly.

2. **Manual Installation**  
   Download the library ZIP from GitHub or another source, and install it via `Sketch -> Include Library -> Add .ZIP Library` in Arduino IDE.

Include the library in your sketch:
```cpp
#include "Skytraq.h"
````

Instantiate the library with TX, RX pins and baud rate:

```cpp
Skytraq gnss(TX_PIN, RX_PIN, 9600);
```

---

## 4. Class Reference

### 4.1 `Skytraq`

**Constructor:**

```cpp
Skytraq(uint8_t txPin, uint8_t rxPin, uint32_t baudRate);
```

* `txPin`: Arduino TX pin connected to GNSS RX
* `rxPin`: Arduino RX pin connected to GNSS TX
* `baudRate`: Serial communication speed (e.g., 9600, 115200)

**Methods:**

| Method                                                                                   | Description                                                                                                                           |
| ---------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------- |
| `begin()`                                                                                | Initializes the UART connection.                                                                                                      |
| `systemRestart(RestartMode mode, ...)`                                                   | Restarts the GNSS module. Supports Hot, Warm, and Cold start. Optional parameters allow setting date, time, and approximate position. |
| `setFactoryDefault()`                                                                    | Restores the module to factory settings.                                                                                              |
| `configSystemPowerMode(uint8_t mode, uint8_t attr)`                                      | Configures power mode (`Normal` or `Power Save`).                                                                                     |
| `configSysPosRate(PositionRate rate, uint8_t attr)`                                      | Configures GNSS position update rate (1–50 Hz).                                                                                       |
| `configPosPinningParams(...)`                                                            | Configures position pinning thresholds for speed, duration, and distance.                                                             |
| `configPosPinning(PosPinning mode, uint8_t attr)`                                        | Enables or disables position pinning.                                                                                                 |
| `configDOPMask(DOPmode mode, uint16_t pdop, uint16_t hdop, uint16_t gdop, uint8_t attr)` | Configures PDOP/HDOP/GDOP mask.                                                                                                       |
| `configNMEATalkerID(NMEAtalkerID id, uint8_t attr)`                                      | Sets the NMEA talker ID (GP, GN, or Auto).                                                                                            |
| `setConstellation(uint8_t constid, uint8_t attr)`                                        | Selects GNSS constellations to use.                                                                                                   |
| `configureSBAS(const SBASConfig &config)`                                                | Configures SBAS settings.                                                                                                             |
| `configureQZSS(uint8_t enable, uint8_t channels, uint8_t attr)`                          | Enables/disables QZSS with channel selection.                                                                                         |
| `setTimeout(uint32_t ms)`                                                                | Sets the timeout for waiting for acknowledgment.                                                                                      |
| `getTimeout()`                                                                           | Returns the current timeout value.                                                                                                    |

---

## 5. Enumerations

### 5.1 `RestartMode`

| Value          | Description              |
| -------------- | ------------------------ |
| `RESTART_HOT`  | System reset, hot start  |
| `RESTART_WARM` | System reset, warm start |
| `RESTART_COLD` | System reset, cold start |

### 5.2 `PositionRate`

Supported GNSS update rates: 1, 2, 4, 5, 8, 10, 20, 25, 40, 50 Hz.

### 5.3 `DOPmode`

| Value          | Description                    |
| -------------- | ------------------------------ |
| `DISABLE_MODE` | DOP masking disabled           |
| `AUTO_MODE`    | PDOP if 3D fix, HDOP if 2D fix |
| `PDOP_ONLY`    | Only PDOP considered           |
| `HDOP_ONLY`    | Only HDOP considered           |
| `GDOP_ONLY`    | Only GDOP considered           |

### 5.4 `PosPinning`

| Value                 | Description              |
| --------------------- | ------------------------ |
| `POS_PINNING_DEFAULT` | Default configuration    |
| `POS_PINNING_ENABLE`  | Enable position pinning  |
| `POS_PINNING_DISABLE` | Disable position pinning |

### 5.5 `NMEAtalkerID`

| Value                 | Description            |
| --------------------- | ---------------------- |
| `NMEA_TALKER_ID_GP`   | GP mode                |
| `NMEA_TALKER_ID_GN`   | GN mode                |
| `NMEA_TALKER_ID_AUTO` | Auto mode (NMEA 4.11+) |

---

## 6. SBAS Configuration Structure

```cpp
struct SBASConfig {
    uint8_t enable;           
    uint8_t ranging;          
    uint8_t rangingURA;       
    uint8_t correction;       
    uint8_t trackingChannels; 
    uint8_t subsystemMask;    
    uint8_t attr;             
};
```

* `enable`: 0 = disable, 1 = enable
* `ranging`: 0 = off, 1 = on, 2 = auto
* `rangingURA`: 0–15, default 8
* `correction`: 0 = off, 1 = on
* `trackingChannels`: 0–3 channels
* `subsystemMask`: bitmask for SBAS systems (WAAS, GAGAN, EGNOS, MSAS, etc.)
* `attr`: 0 = SRAM only, 1 = SRAM & FLASH

---

## 8. Error Handling

* All configuration functions return `true` if the module acknowledges the command (`ACK`), otherwise `false`.
* Timeout for acknowledgment can be adjusted using `setTimeout()`.

---

## 9. Notes

* Always verify the supported position rates and constellations for your Skytraq module in its datasheet.
* SBAS and QZSS support may vary depending on module firmware.
* Tested primarily on ESP32; compatibility with other boards may vary.
