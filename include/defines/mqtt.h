#pragma once

#define MQTT_SERVER                     "192.168.13.200"
#define MQTT_PORT                       1883
#define MQTT_ID                         "mqtt_bath"
#define MQTT_USER                       "bath"
#define MQTT_PASS                       "0112358"
#define MQTT_RECONNECT_TIMEOUT          10000
#define MQTT_FLOOR_TEMPERATURE_TOPIC    "bath/floor_temp"
#define MQTT_TRIAC_TEMPERATURE_TOPIC    "bath/triac_temp"
#define MQTT_HEATER_STATE_TOPIC         "bath/heater_state"
#define MQTT_COMMAND_TOPIC              "bath/"
#define MQTT_LOG_TOPIC                  "bath/log"