#include <Arduino.h>
#include <AutoOTA.h>

#include "defines_ota.h"

AutoOTA otaGithub(CURRENT_VERSION, OTA_GITHUB_CONFIG_PATH);
AutoOTA otaLocal(CURRENT_VERSION, OTA_LOCAL_CONFIG_PATH, OTA_LOCAL_SERVER_HOST, OTA_LOCAL_SERVER_PORT);
uint32_t otaPreviousMillis = 0;

void otaCheck() {
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

void otaProcess() {
  if (!otaLocal.tick()) {
    otaGithub.tick();
  }
}