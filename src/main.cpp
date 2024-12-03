#include <Arduino.h>
#include <ESP8266mDNS.h>

#include "wifi_connector.h"
#include "mqtt.h"
#include "http.h"
#include "mdns.h"
#include "main_process.h"
#include "ota.h"

void onConnected() {
  mdnsInit();
  httpInit();
}

static void networkInit() {
  wifiInit();
  mqttInit();
}

static void networkProcess() {
  mdnsProcess();
  httpProcess();
  mqttProcess();
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
