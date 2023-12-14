#include <ESP8266WiFi.h>

#include "defines.h"

static const char *ssid = WIFI_SSID;
static const char *password = WIFI_PASS;

static const IPAddress localIP(192, 168, 1, IP_ADDRESS_LAST_BYTE);
static const IPAddress gateway(192, 168, 1, 1);
static const IPAddress subnet(255, 255, 255, 0);

static WiFiEventHandler wifiConnectHandler;
static WiFiEventHandler wifiDisconnectHandler;

static uint32_t next_connect_ms = 0;
static bool f_first_connect = true;

static void onWiFiConnect(const WiFiEventStationModeGotIP &event)
{
  // Serial.println("Wi-Fi соединение установлено.");
  // Serial.print("IP адрес: ");
  // Serial.println(WiFi.localIP());
  // Serial.print("RSSI: ");
  // Serial.println(WiFi.RSSI());
}

static void onWiFiDisconnect(const WiFiEventStationModeDisconnected &event)
{
  // Serial.println("Wi-Fi соединение потеряно, пытаемся переподключиться...");
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
  WiFi.config(localIP, gateway, subnet);
  WiFi.begin(ssid, password);
  next_connect_ms = millis() + WIFI_CONNECT_TIMEOUT;
  // Serial.println("Попытка подключиться к WiFi...");
  if (WiFi.status() == WL_CONNECTED)
    f_first_connect = false;
  else
  {
    next_connect_ms = millis() + WIFI_CONNECT_TIMEOUT;
    // Serial.print("Wi-Fi соединение не установлено. Код ошибки: ");
    // Serial.println(WiFi.status());
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