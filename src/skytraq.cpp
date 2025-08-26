#include <stdint.h>
#include "Skytraq.h"

Skytraq::Skytraq(uint8_t txPin, uint8_t rxPin, uint32_t baudRate) : _tx(txPin), _rx(rxPin), baud(baudRate) {}

void Skytraq::begin()
{
  Serial1.begin(baud, SERIAL_8N1, _rx, _tx);
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
  uint8_t checksum = calcChecksum(payload, len);

  uint8_t packet[32];
  size_t idx = 0;

  packet[idx++] = 0xA0;
  packet[idx++] = 0xA1;

  packet[idx++] = (len >> 8) & 0xFF;
  packet[idx++] = len & 0xFF;

  for (size_t i = 0; i < len; i++)
  {
    packet[idx++] = payload[i];
  }

  packet[idx++] = checksum;

  packet[idx++] = 0x0D;
  packet[idx++] = 0x0A;

  Serial1.write(packet, idx);

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
  const unsigned long timeout = 5000;

  uint8_t state = 0; // 0=sync1, 1=sync2, 2=len_h, 3=len_l, 4=payload, 5=end
  uint16_t expectedLen = 0;
  uint16_t bytesRead = 0;
  uint8_t ackNack = 0;

  while (millis() - startTime < timeout)
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