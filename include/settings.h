#pragma once

#include "defines/parameters.h"

struct BathSettings {
  float temperature = SETTINGS_TEMPERATURE_DEFAULT;
  float delta = SETTINGS_DELTA_DEFAULT;
  bool on = false;
  uint32_t sensorsInterval = SETTINGS_SENSORS_INTERVAL_DEFAULT;
  uint32_t mqttInterval = SETTINGS_MQTT_INTERVAL_DEFAULT;
};

void settingsInit();
void settingsProcess();

float getSettingsTemperature();
float getSettingsDelta();
bool getDeviceState();
uint32_t getSensorsCheckInterval();
uint32_t getMqttInterval();
float getTemperatureLowBound();
float getTemperatureHighBound();

void setSettingsTemperature(float value);
void setSettingsDelta(float value);
void setSensorsCheckInterval(uint32_t value);
void setMqttInterval(uint32_t value);

void toggleDeviceState();
void toggleDeviceState(bool state);

void resetSettings();