#include <Arduino.h>
#include <ESP8266mDNS.h>
#include <AutoOTA.h>

#include "defines.h"
#include "defines_ota.h"
#include "wifi.h"
#include "mqtt.h"
#include "http.h"
#include "main_process.h"

AutoOTA otaGithub(CURRENT_VERSION, OTA_GITHUB_CONFIG_PATH);
AutoOTA otaLocal(CURRENT_VERSION, OTA_LOCAL_CONFIG_PATH, OTA_LOCAL_SERVER_HOST, OTA_LOCAL_SERVER_PORT);
uint32_t otaPreviousMillis = 0;

static void otaCheck() {
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

static void otaProcess() {
  if (!otaLocal.tick()) {
    otaGithub.tick();
  }
}

static void mdnsInit() {
  if(MDNS.begin(MDNS_NAME)) {
    MDNS.addService("http", "tcp", 80);
  }
}

static void networkInit() {
  wifiInit();
  //mdnsInit();
  httpInit();
  //mqttInit();
  otaCheck();
}

static void networkProcess() {
  wifiProcess();
  //MDNS.update();
  httpProcess();
  //mqttProcess();
  otaProcess();
}

void setup() {
  mainInit();
  networkInit();
}

void loop() {
  mainProcess();
  networkProcess();
}
