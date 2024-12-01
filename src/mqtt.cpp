#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "defines/mqtt.h"
#include "wifi.h"
#include "main_process.h"

static WiFiClient wifiClient;
static PubSubClient mqttClient(wifiClient);
static const char *mqttServer = MQTT_SERVER;
static const int mqttPort = MQTT_PORT;

static void mqttConnect()
{
  if (!isWifiConnected())
    return;

  if (!mqttClient.connected())
  {
    mqttClient.setServer(mqttServer, mqttPort);
    if (mqttClient.connect(MQTT_ID, MQTT_USER, MQTT_PASS))
    {
      mqttClient.setCallback([](char *topic, byte *payload, unsigned int length)
                         { parseIncomingCommand(topic, payload, length); });
      mqttClient.subscribe(MQTT_COMMAND_TOPIC);
    }
    else
    {
      Serial.print("Не подключено. Код ошибки: ");
      Serial.println(mqttClient.state());
    }
  }
}

void mqttProcess()
{
  mqttClient.loop();
}

void mqttInit()
{
  mqttConnect();
}

// static void checkReconnectTimeout()
// {
//   if (millis() > mqttNextConnectMs)
//   {
//     needReconnect = true;
//     mqttNextConnectMs = millis() + MQTT_RECONNECT_TIMEOUT;
//   }
// }

// example: mqttPrintf("topic", "Hello, %s! The answer is %d", "World", 42);
void mqttPrintf(const char *topic, const char *format, ...)
{  
  char buffer[256];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);
  mqttClient.publish(topic, buffer);
}