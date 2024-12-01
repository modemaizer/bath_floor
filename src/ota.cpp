#include <Arduino.h>
#include <AutoOTA.h>

#include "defines/ota.h"
#include "wifi.h"

AutoOTA otaGithub(CURRENT_VERSION, OTA_GITHUB_CONFIG_PATH);
AutoOTA otaLocal(CURRENT_VERSION, OTA_LOCAL_CONFIG_PATH);
uint32_t ota_tmr = 0;

void otaCheck() {
  if (isWifiConnected() && (!ota_tmr || millis() - ota_tmr > OTA_INTERVAL)) {
    ota_tmr = millis();
    
    Serial.print("current version (local OTA): ");
    Serial.println(otaLocal.version());
    Serial.print("current version (Github OTA): ");
    Serial.println(otaGithub.version());
    Serial.println("checking updates");
    String ver, notes;
    if (otaLocal.checkUpdate(&ver, &notes)) {
        Serial.print("found local: ");
        Serial.println(ver);
        Serial.println(notes);
        otaLocal.update();
    } else if (otaGithub.checkUpdate(&ver, &notes)) {
          Serial.print("found on Github: ");
          Serial.println(ver);
          Serial.println(notes);
          otaGithub.update();
    } else {
      Serial.println("no any updates");
    }
  }
}

void otaProcess() {
  otaCheck();
  if (!otaLocal.tick()) {
    otaGithub.tick();
  }
}