#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "defines/mqtt.h"
#include "wifi.h"
#include "main_process.h"

static WiFiClient wifiClient;
static PubSubClient mqttClient(wifiClient);
static const char* mqttServer = MQTT_SERVER;
static const int mqttPort = MQTT_PORT;

uint32_t mqtt_tmr = 0;

boolean reconnect() {
  if (mqttClient.connect(MQTT_ID, MQTT_USER, MQTT_PASS)) {
    mqttClient.subscribe(MQTT_COMMAND_TOPIC);
  }
  return mqttClient.connected();
}

void mqttProcess()
{
  if (!mqttClient.connected() && (!mqtt_tmr || millis() - mqtt_tmr > 5000)) {
    mqtt_tmr = millis();
    reconnect();
  }
  mqttClient.loop();
}

void mqttInit()
{
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback([](char *topic, byte *payload, unsigned int length)
             { parseIncomingCommand(topic, payload, length); });
}

// example: mqttPrintf("topic", "Hello, %s! The answer is %d", "World", 42);
void mqttPrintf(const char *topic, const char *format, ...)
{
  uint8_t bufferSize = 250;
  char* buffer = new char[bufferSize];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, bufferSize, format, args);
  va_end(args);
  mqttClient.publish(topic, buffer);
  delete [] buffer;
}