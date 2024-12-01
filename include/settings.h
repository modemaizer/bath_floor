#pragma once

#include "defines/parameters.h"

struct BathSettings {
  float temperature = SETTINGS_TEMPERATURE_DEFAULT;
  float delta = SETTINGS_DELTA_DEFAULT;
  bool on = false;
  uint32_t interval = SETTINGS_INTERVAL_DEFAULT;
};

void settingsInit();
void settingsProcess();

float getSettingsTemperature();
float getSettingsDelta();
bool getDeviceState();
uint32_t getSensorsCheckInterval();
float getTemperatureLowBound();
float getTemperatureHighBound();

void setSettingsTemperature(float value);
void setSettingsDelta(float value);
void setSensorsCheckInterval(uint32_t value);

void toggleDeviceState();
void toggleDeviceState(bool state);

void resetSettings();