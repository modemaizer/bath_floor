#include <ESP8266mDNS.h>

#include "defines/network.h"

void mdnsInit() {
  if(MDNS.begin(MDNS_NAME)) {
    MDNS.addService("http", "tcp", 80);
  }
}

void mdnsProcess() {
  MDNS.update();
}