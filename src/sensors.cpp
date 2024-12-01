#include <microDS18B20.h>

#include "defines/pins.h"
#include "defines/mqtt.h"
#include "mqtt.h"

static float floorTemp = 0;
static float triacTemp = 0;
static bool floorSensorOk = true;
static bool triacSensorOk = true;

uint8_t floorSensorAddress[8] = { 0x28, 0x35, 0xE6, 0x75, 0xD0, 0x01, 0x3C, 0xB7 };
uint8_t triacSensorAddress[8] = { 0x28, 0x2D, 0x8F, 0x75, 0xD0, 0x01, 0x3C, 0xAF };

MicroDS18B20<SENSORS_PIN, floorSensorAddress> floorSensor;
MicroDS18B20<SENSORS_PIN, triacSensorAddress> triacSensor;

float getFloorTemperature() {
    return floorTemp;
}
float getTriacTemperature() {
    return triacTemp;
}

bool getFloorSensorOk() {
    return floorSensorOk;
}
bool getTriacSensorOk() {
    return triacSensorOk;
}

void readFloor() {
  if (floorSensor.readTemp()) {
    floorTemp = floorSensor.getTemp();
    floorSensorOk = true;
  } 
  else {
    floorTemp = 0;
    floorSensorOk = false;
  }
  floorSensor.setResolution(10);
  floorSensor.requestTemp();
  mqttPrintf(MQTT_FLOOR_TEMPERATURE_TOPIC, "%.2f", floorTemp);
}

void readTriac() {
  if (triacSensor.readTemp()) {
    triacTemp = triacSensor.getTemp();
    triacSensorOk = true;
  } 
  else {
    triacTemp = 0;
    triacSensorOk = false;
  }
  triacSensor.setResolution(10);
  triacSensor.requestTemp();
  mqttPrintf(MQTT_TRIAC_TEMPERATURE_TOPIC, "%.2f", triacTemp);
}

void sensorsProcess() {
    readFloor();
    readTriac();
}
