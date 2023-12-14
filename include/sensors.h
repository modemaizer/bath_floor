#pragma once

float getFloorTemperature();
float getTriacTemperature();
bool getFloorSensorOk();
bool getTriacSensorOk();

void readFloor();
void readTriac();

void sensorsProcess();