#include <Arduino.h>

#include "main_process.h"
#include "defines.h"
#include "sensors.h"
#include "mqtt.h"
#include "settings.h"
#include "wifi.h"

unsigned long sensorPreviousMillis = 0;
unsigned long wifiErrorPreviousMillis = 0;
unsigned long sensorErrorPreviousMillis = 0;

bool getHeaterState() {
    return digitalRead(HEATER_PIN);
}

void toggleHeater() {
    digitalWrite(HEATER_PIN, !getHeaterState());
}

void toggleHeater(bool enable) {
    digitalWrite(HEATER_PIN, enable);
}

static void heaterProcess() {
  if(!getDeviceState() || !getFloorSensorOk() || !getTriacSensorOk() || 
    getFloorTemperature() >= getTemperatureHighBound() || 
    getTriacTemperature() > TRIAC_MAX_TEMPERATURE) {
    toggleHeater(false);
    return;
  }
  if(getDeviceState() && getFloorTemperature() <= getTemperatureLowBound()) {
    toggleHeater(true);
  }
}

static void invertPin(uint8_t pin){
  digitalWrite(pin, !digitalRead(pin));
}

static void blink(uint8_t pin, unsigned long &counter, unsigned long interval) {
  unsigned long current = millis();
  if(current - counter >= interval) {
    invertPin(pin);
    counter = current;
  }
}

static void processLeds() {
  if(!isWifiConnected()) {
    blink(GREEN_LED_PIN, wifiErrorPreviousMillis, WIFI_ERROR_BLINK_INTERVAL);
  }
  else {    
    digitalWrite(GREEN_LED_PIN, HIGH);
  }

  if(!getFloorSensorOk()) {
    blink(RED_LED_PIN, sensorErrorPreviousMillis, FLOOR_SENSOR_ERROR_BLINK_INTERVAL);
  }
  else if(!getTriacSensorOk()) {
    blink(RED_LED_PIN, sensorErrorPreviousMillis, TRIAC_SENSOR_ERROR_BLINK_INTERVAL);
  }
  else if(getHeaterState()) {
    digitalWrite(RED_LED_PIN, HIGH);
  }
  else {
    digitalWrite(RED_LED_PIN, LOW);
  }
}

void mainInit() {
    Serial.begin(SERIAL_BAUDRATE);
    Serial.println("Start");
    settingsInit();
    pinMode(HEATER_PIN, OUTPUT);
    digitalWrite(HEATER_PIN, LOW);
    pinMode(RED_LED_PIN, OUTPUT);
    digitalWrite(RED_LED_PIN, LOW);
    pinMode(GREEN_LED_PIN, OUTPUT);
    digitalWrite(GREEN_LED_PIN, LOW);
}

void mainProcess() {
    settingsProcess();
    processLeds();
    unsigned long currentMillis = millis();
    if (currentMillis - sensorPreviousMillis >= getSensorsCheckInterval()) {
        sensorPreviousMillis = currentMillis;
        sensorsProcess();
        heaterProcess();
    }
}

void parseIncomingCommand(char *topic, byte *payload, unsigned int length)
{
    char command[length + 1];
    for (uint32_t i = 0; i < length; i++)
        command[i] = (char)payload[i];

    command[length] = '\0';

    if (strcmp(command, "on") == 0)
        toggleDeviceState(true);
    else if (strcmp(command, "off") == 0)
        toggleDeviceState(false);
    else if (strcmp(command, "temp ") == 0) {
        setSettingsTemperature(atof(&command[5]));
    }
    else if (strcmp(command, "delta") == 0) {
        setSettingsDelta(atof(&command[6]));
    }
    
    mqttPrintf(LOG_TOPIC, "%s", command);
}
