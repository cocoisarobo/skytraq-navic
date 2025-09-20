#include <stdint.h>
#include "Skytraq.h"

Skytraq::Skytraq(uint8_t txPin, uint8_t rxPin, uint32_t baudRate) : _tx(txPin), _rx(rxPin), baud(baudRate) {}

void Skytraq::begin()
{
  Serial1.begin(baud, SERIAL_8N1, _rx, _tx);
}

bool Skytraq::systemRestart(RestartMode mode, uint16_t year, uint8_t month,
                            uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, int16_t lat, int16_t lon, int16_t alt)
{
  uint8_t payload[15] = {0x01, mode,
                         (year >> 8) & 0xFF, year & 0xFF,
                         month, day, hour, minute, second,
                         (lat >> 8) & 0xFF, lat & 0xFF,
                         (lon >> 8) & 0xFF, lon & 0xFF,
                         (alt >> 8) & 0xFF, alt & 0xFF};

  sendCommand(payload, sizeof(payload));

  delay(100);

  return getAck();
}

bool Skytraq::setFactoryDefault()
{
  uint8_t payload[2] = {0x04, 0x01};
  sendCommand(payload, sizeof(payload));

  delay(100);

  return getAck();
}

bool Skytraq::configSystemPowerMode(uint8_t mode, uint8_t attr)
{
  if (mode > 1)
    return false;

  uint8_t payload[3] = {0x0C, mode, attr};
  sendCommand(payload, sizeof(payload));

  delay(100);

  return getAck();
}

bool Skytraq::configSysPosRate(PositionRate posRate, uint8_t attr)
{

  uint8_t payload[3] = {0x0E, posRate, attr};
  sendCommand(payload, sizeof(payload));

  delay(100);

  return getAck();
}

bool Skytraq::configPosPinningParams(uint16_t p_speed, uint16_t p_cnt, uint16_t up_speed, uint16_t up_cnt, uint16_t up_dist, uint8_t attr)
{
  uint8_t payload[12] = {0x3B,
                         (p_speed >> 8) & 0xFF,
                         p_speed & 0xFF,
                         (p_cnt >> 8) & 0xFF,
                         p_cnt & 0xFF,
                         (up_speed >> 8) & 0xFF,
                         up_speed & 0xFF,
                         (up_cnt >> 8) & 0xFF,
                         up_cnt & 0xFF,
                         (up_dist >> 8) & 0xFF,
                         up_dist & 0xFF,
                         attr};

  sendCommand(payload, sizeof(payload));

  delay(100);

  return getAck();
}

bool Skytraq::configPosPinning(PosPinning mode, uint8_t attr)
{
  if (mode > 2)
    return false;

  uint8_t payload[3] = {0x39, mode, attr};
  sendCommand(payload, sizeof(payload));

  delay(100);

  return getAck();
}

bool Skytraq::configDOPMask(DOPmode mode, uint16_t pdop, uint16_t hdop, uint16_t gdop, uint8_t attr)
{
  if (mode > GDOP_ONLY)
    return false;

  if (pdop < 5 || pdop > 300 || hdop < 5 || hdop > 300 || gdop < 5 || gdop > 300)
    return false;

  uint8_t payload[9] = {0x2A,
                        mode,
                        (pdop >> 8) & 0xFF,
                        pdop & 0xFF,
                        (hdop >> 8) & 0xFF,
                        hdop & 0xFF,
                        (gdop >> 8) & 0xFF,
                        gdop & 0xFF,
                        attr};

  sendCommand(payload, sizeof(payload));

  delay(100);

  return getAck();
}

bool Skytraq::configNMEATalkerID(NMEAtalkerID id, uint8_t attr)
{
  if (id > 2)
    return false;

  uint8_t payload[3] = {0x4B, id, attr};
  sendCommand(payload, sizeof(payload));

  delay(100);

  return getAck();
}

bool Skytraq::setConstellation(uint8_t constid, uint8_t attr)
{
  uint8_t payload[5] = {0x64, 0x19, 0x00, constid, attr};
  sendCommand(payload, sizeof(payload));

  delay(100);

  return getAck();
}

bool Skytraq::configureSBAS(const SBASConfig &config)
{
  if (config.enable > 1)
    return false;
  if (config.ranging > 2)
    return false;
  if (config.rangingURA > 15)
    return false;
  if (config.trackingChannels > 3)
    return false;

  uint8_t payload[9] = {0x62, 0x01,
                        config.enable,
                        config.ranging,
                        config.rangingURA,
                        config.correction,
                        config.trackingChannels,
                        config.subsystemMask,
                        config.attr};

  sendCommand(payload, sizeof(payload));

  delay(100);

  return getAck();
}

bool Skytraq::configureQZSS(uint8_t enable, uint8_t channels, uint8_t attr)
{
  if (enable > 1)
    return false;
  if (channels > 3)
    return false;

  uint8_t payload[5] = {0x62, 0x03, enable, channels, attr};
  sendCommand(payload, sizeof(payload));

  delay(100);

  return getAck();
}

bool Skytraq::sendCommand(uint8_t *payload, size_t len)
{
  if (len + 7 > sizeof(_packetBuffer))
    return false;

  uint8_t checksum = calcChecksum(payload, len);

  // uint8_t packet[32];
  size_t idx = 0;

  _packetBuffer[idx++] = 0xA0;
  _packetBuffer[idx++] = 0xA1;

  _packetBuffer[idx++] = (len >> 8) & 0xFF;
  _packetBuffer[idx++] = len & 0xFF;

  for (size_t i = 0; i < len; i++)
  {
    _packetBuffer[idx++] = payload[i];
  }

  _packetBuffer[idx++] = checksum;

  _packetBuffer[idx++] = 0x0D;
  _packetBuffer[idx++] = 0x0A;

  Serial1.write(_packetBuffer, idx);

  return true;
}

uint8_t Skytraq::calcChecksum(uint8_t *msg, size_t len)
{
  uint8_t cs = 0;
  for (size_t i = 0; i < len; i++)
  {
    cs ^= msg[i];
  }
  return cs;
}

bool Skytraq::getAck()
{
  unsigned long startTime = millis();
  // const unsigned long _timeout = 5000;

  uint8_t state = 0; // 0=sync1, 1=sync2, 2=len_h, 3=len_l, 4=payload, 5=end
  uint16_t expectedLen = 0;
  uint16_t bytesRead = 0;
  uint8_t ackNack = 0;

  while (millis() - startTime < _timeout)
  {
    if (!Serial1.available())
    {
      delay(1);
      continue;
    }

    uint8_t byte = Serial1.read();

    switch (state)
    {
    case 0:
      if (byte == 0xA0)
        state = 1;
      break;

    case 1:
      if (byte == 0xA1)
        state = 2;
      else
        state = 0;
      break;

    case 2:
      expectedLen = byte << 8;
      state = 3;
      break;

    case 3:
      expectedLen |= byte;
      bytesRead = 0;
      state = 4;
      break;

    case 4:
      if (bytesRead == 0)
      {
        ackNack = byte;
      }
      bytesRead++;
      if (bytesRead >= expectedLen + 1)
      { // +1 for checksum
        state = 5;
      }
      break;

    case 5:
      if (byte == 0x0A)
      {
        return (ackNack == 0x83);
      }
      break;
    }
  }

  return false;
}