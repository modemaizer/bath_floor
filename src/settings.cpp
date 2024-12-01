#include <FileData.h>
#include <LittleFS.h>

#include "defines/parameters.h"
#include "settings.h"

static BathSettings settings;

FileData data(&LittleFS, "/settings.dat", 'B', &settings, sizeof(settings));

void settingsInit() {
    if(LittleFS.begin()) {
        Serial.println("LittleFS OK");
    }
    else {
        Serial.println("LittleFS ERR");
    }
    data.setTimeout(1000);
    data.read();
}

void settingsProcess() {
    data.tick();
}

float getSettingsTemperature() {
    return settings.temperature;
}
float getSettingsDelta() {
    return settings.delta;
}
bool getDeviceState() {
    return settings.on;
}
uint32_t getSensorsCheckInterval() {
    return settings.interval;
}
float getTemperatureLowBound() {
    return settings.temperature - settings.delta;
}
float getTemperatureHighBound() {
    return settings.temperature + settings.delta;
}

void setSettingsTemperature(float value) {
    Serial.println(value);
    settings.temperature = value;
    data.update();
}
void setSettingsDelta(float value) {
    settings.delta = value;
    data.update();
}
void setSensorsCheckInterval(uint32_t value) {
    settings.interval = value;
    data.update();
}

void toggleDeviceState() {
    settings.on = !settings.on;
    data.update();
}

void toggleDeviceState(bool state) {
  if(settings.on != state) {
    toggleDeviceState();
  }  
}

void resetSettings() {
  settings.temperature = 20;
  settings.delta = 0.5;
  settings.on = false;
  data.updateNow();
}
