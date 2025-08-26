#include "skytraq.h"

Skytraq gnss(17, 16, 115200);

void setup()
{
  Serial.begin(115200);
  gnss.begin();

  delay(1000);

  // Example: enable GPS and NavIC
  if (gnss.setConstellation(CONFIG_GNSS_GPS | CONFIG_GNSS_NAVIC, CONFIG_ATTR_UPDATE_TO_SRAM))
  {
    Serial.println("Ack received");
  }
  else
  {
    Serial.println("Invalid configuration or constellation not supported");
  }

  delay(1000);

  SBASConfig sbas; // default settings
  if (gnss.configureSBAS(sbas))
  {
    Serial.println("SBAS Configured");
  }
  else
  {
    Serial.println("Failed to configure SBAS");
  }

  // SBASConfig sbas;
  // sbas.enable = 1;
  // sbas.ranging = 2;
  // sbas.correction = 1;
  // sbas.trackingChannels = 2;
  // sbas.subsystemMask = CONFIG_SBAS_GAGAN;
  // sbas.attr = 0;

  // if (gps.setSBAS(sbas))
  // {
  //   Serial.println("Custom SBAS config applied");
  // } else {
  //   Serial.println("Failed to apply custom SBAS config. Check parameters.");
  // }

  delay(1000);

  if (gnss.configureQZSS(1, 2, CONFIG_ATTR_UPDATE_TO_SRAM))
  {
    Serial.println("QZSS Configured");
  }
  else
  {
    Serial.println("Failed to configure QZSS. Check parameters.");
  }
}

void loop()
{
  while (Serial1.available())
  {
    char c = Serial1.read();
    Serial.write(c);
  }
}