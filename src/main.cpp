#include <Arduino.h>
#include <ESP8266mDNS.h>
#include <AutoOTA.h>

#include "defines.h"
#include "wifi.h"
#include "mqtt.h"
#include "http.h"
#include "main_process.h"

AutoOTA ota("1.2", "modemaizer/bath_floor/master/project.json");
uint32_t otaPreviousMillis = 0;

static void otaProcess() {
  uint32_t currentMillis = millis();
  if (currentMillis - otaPreviousMillis >= OTA_INTERVAL) {
    otaPreviousMillis = currentMillis;
    String ver, notes;
    Serial.print("check updates, current version: ");
    Serial.println(ota.version());
    if (ota.checkUpdate(&ver, &notes)) {
        Serial.println(ver);
        Serial.println(notes);
        ota.updateNow();
    }
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
  Serial.println(ota.version());
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
