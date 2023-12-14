#pragma once

void settingsInit();
void settingsProcess();

float getSettingsTemperature();
float getSettingsDelta();
bool getDeviceState();
long unsigned int getSensorsCheckInterval();
float getTemperatureLowBound();
float getTemperatureHighBound();

void setSettingsTemperature(float value);
void setSettingsDelta(float value);
void setSensorsCheckInterval(long unsigned int value);

void toggleDeviceState();
void toggleDeviceState(bool state);

void resetSettings();