#include <ESP8266WiFi.h>

#include "defines/network.h"
#include "wifi.h"
#include "mdns.h"
#include "http.h"
#include "mqtt.h"

static const char *ssid = WIFI_SSID;
static const char *password = WIFI_PASS;

static WiFiEventHandler wifiConnectHandler;
static WiFiEventHandler wifiDisconnectHandler;

static uint32_t next_connect_ms = 0;
static bool f_first_connect = true;

static void onWiFiConnect(const WiFiEventStationModeGotIP &event)
{
  Serial.println("Wi-Fi соединение установлено.");
  Serial.print("IP адрес: ");
  Serial.println(WiFi.localIP());
  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());
  mdnsInit();
  httpInit();
  mqttInit();
}

static void onWiFiDisconnect(const WiFiEventStationModeDisconnected &event)
{
  WiFi.disconnect();
  WiFi.begin(ssid, password);
}

void wifiInit()
{
  if (WiFi.status() == WL_CONNECTED || millis() < next_connect_ms)
    return;

  wifiConnectHandler = WiFi.onStationModeGotIP(onWiFiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWiFiDisconnect);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  next_connect_ms = millis() + WIFI_CONNECT_TIMEOUT;
  if (WiFi.status() == WL_CONNECTED) {
    f_first_connect = false;
  } else {
    next_connect_ms = millis() + WIFI_CONNECT_TIMEOUT;
  }
}

void wifiProcess()
{
  wifiInit();
}

bool isWifiConnected()
{
  return WiFi.status() == WL_CONNECTED;
}