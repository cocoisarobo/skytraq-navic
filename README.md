# skytraq-navic

An Arduino library (WIP) for communicating with GNSS modules based on the Skytraq chipset.

## Current Features

Configure GNSS constellation types (GPS, GLONASS, Galileo, BeiDou, NavIC)

SBAS system configuration (WAAS, EGNOS, MSAS, GAGAN, SDCM, BDSBAS)

QZSS satellite system support

ACK/NACK response handling

Easy-to-use Arduino interface


## Hardware set-up

Bharat Pi NavIC module connected to ESP32 over UART

## Module specifications (NavIC)

    Receiver Type: NavIC L5, GAGAN/GPS L1 C/A code Phoenix engine
    Constellations: All GNSS constellations enabled for navigation
    SBAS: Enabled (2 tracking channels active)
    Baud Rate: 115200
    Output Format: NMEA
    Update Rate: 1 Hz
    Byte order : Big-endian

## Start modes

Cold Start: Factory reset, clears all stored data (~30-60s fix time)

Warm Start: Last position available, outdated ephemeris (~15-30s fix time)

Hot Start: Recent data valid, power cycled within seconds (~5-15s fix time)

## Development status

To be updated

This library is under active development and currently only supports project specific configuration.  
Additional commands and features will be added in future updates.  