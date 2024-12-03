#include <Arduino.h>
#include <AutoOTA.h>
#include <sstream>

#include "defines/mqtt.h"
#include "defines/ota.h"
#include "wifi_connector.h"
#include "mqtt.h"

AutoOTA otaGithub(CURRENT_VERSION, OTA_GITHUB_CONFIG_PATH);
AutoOTA otaLocal(CURRENT_VERSION, OTA_LOCAL_CONFIG_PATH);
uint32_t ota_tmr = 0;

bool isNewerVersion(const String version) {
    // Split versions into arrays of integers
    std::vector<int> vNew, vCurrent;
    std::stringstream ssNew(version.c_str()), ssCurrent(CURRENT_VERSION);
    int temp;

    while (ssNew >> temp) vNew.push_back(temp);
    while (ssCurrent >> temp) vCurrent.push_back(temp);

    // Make versions of equal length by padding with zeros
    size_t maxLen = std::max(vNew.size(), vCurrent.size());
    vNew.resize(maxLen, 0);
    vCurrent.resize(maxLen, 0);

    // Compare elements
    for (size_t i = 0; i < maxLen; ++i) {
        if (vNew[i] > vCurrent[i]) return true;
        if (vNew[i] < vCurrent[i]) return false;
    }

    // If we reach here, versions are equal
    return false;
}

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
        if(isNewerVersion(ver)) {
          Serial.print("Newer version, updating");
          mqttPrintf(MQTT_LOG_TOPIC, "%s", "Updating from local server");
          otaLocal.update();
        }
    } else if (otaGithub.checkUpdate(&ver, &notes)) {
        Serial.print("found on Github: ");
        Serial.println(ver);
        Serial.println(notes);
        if(isNewerVersion(ver)) {
          Serial.print("Newer version, updating");
          mqttPrintf(MQTT_LOG_TOPIC, "%s", "Updating from Github");
          otaGithub.update();
        }
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