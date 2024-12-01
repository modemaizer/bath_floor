#include <Arduino.h>
#include <ESP8266mDNS.h>

#include "defines.h"
#include "wifi.h"
#include "mqtt.h"
#include "http.h"
#include "main_process.h"
#include "ota.h"

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
