#include <Arduino.h>
#include <ESP8266mDNS.h>

#include "wifi.h"
#include "mqtt.h"
#include "http.h"
#include "mdns.h"
#include "main_process.h"

static void networkInit() {
  wifiInit();
}

static void networkProcess() {
  mdnsProcess();
  httpProcess();
  mqttProcess();
}

void setup() {
  mainInit();
  networkInit();
}

void loop() {
  mainProcess();
  networkProcess();
}
