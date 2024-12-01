#include <ESP8266WiFi.h>

#include "defines/network.h"
#include "wifi.h"
#include "mdns.h"
#include "http.h"
#include "mqtt.h"
#include "ota.h"

static const char *ssid = WIFI_SSID;
static const char *password = WIFI_PASS;

uint32_t wifi_tmr = 0;
static WiFiEventHandler wifiConnectHandler;
static WiFiEventHandler wifiDisconnectHandler;

// static uint32_t next_connect_ms = 0;
// static bool f_first_connect = true;

static void onWiFiConnect(const WiFiEventStationModeGotIP &event)
{
  Serial.println("Wi-Fi соединение установлено.");
  Serial.print("IP адрес: ");
  Serial.println(WiFi.localIP());
  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());
  mdnsInit();
  httpInit();
}

void wifiReconnect() {
  wifiConnectHandler = WiFi.onStationModeGotIP(onWiFiConnect);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
}

void wifiInit()
{
  wifiConnectHandler = WiFi.onStationModeGotIP(onWiFiConnect);
  wifiReconnect();
  // if (!mqttClient.connected() && (!mqtt_tmr || millis() - mqtt_tmr > 5000)) {
  //   mqtt_tmr = millis();
  //   reconnect();
  // }
  // if (WiFi.status() != WL_CONNECTED || millis() < next_connect_ms)
  //   return;

  // wifiConnectHandler = WiFi.onStationModeGotIP(onWiFiConnect);
  // wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWiFiDisconnect);

  // WiFi.mode(WIFI_STA);
  // WiFi.begin(ssid, password);
  // next_connect_ms = millis() + WIFI_CONNECT_TIMEOUT;
  // if (WiFi.status() == WL_CONNECTED) {
  //   f_first_connect = false;
  // } else {
  //   next_connect_ms = millis() + WIFI_CONNECT_TIMEOUT;
  // }
}

void wifiProcess()
{
  if (!isWifiConnected() && (!wifi_tmr || millis() - wifi_tmr > WIFI_CONNECT_TIMEOUT)) {
    wifi_tmr = millis();
    wifiReconnect();
  }
}

bool isWifiConnected()
{
  return WiFi.status() == WL_CONNECTED;
}