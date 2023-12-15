#pragma once

//network
#define WIFI_SSID               ""
#define WIFI_PASS               ""
#define IP_ADDRESS_LAST_BYTE    1
#define MDNS_NAME               ""
#define WIFI_CONNECT_TIMEOUT    10000
#define UPDATE_USERNAME         ""
#define UPDATE_PASS             ""

//mqtt
#define MQTT_SERVER                     "192.168.1.200"
#define MQTT_PORT                       1884
#define MQTT_ID                         "mqtt_bath"
#define MQTT_USER                       ""
#define MQTT_PASS                       ""
#define MQTT_RECONNECT_TIMEOUT          10000
#define MQTT_FLOOR_TEMPERATURE_TOPIC    "bath/floor_temp"
#define MQTT_TRIAC_TEMPERATURE_TOPIC    "bath/triac_temp"
#define MQTT_HEATER_STATE_TOPIC         "bath/heater_state"
#define TOPIC_COMMAND                   "bath/"
#define LOG_TOPIC                       "bath/log"

//pins
#define SENSORS_PIN     D4
#define RED_LED_PIN     D6
#define GREEN_LED_PIN   D7
#define HEATER_PIN      D8

//parameters
#define SERIAL_BAUDRATE                     115200
#define TRIAC_MAX_TEMPERATURE               60
#define WIFI_ERROR_BLINK_INTERVAL           500
#define FLOOR_SENSOR_ERROR_BLINK_INTERVAL   100
#define TRIAC_SENSOR_ERROR_BLINK_INTERVAL   500
#define SETTINGS_TEMPERATURE_DEFAULT        25
#define SETTINGS_DELTA_DEFAULT              0.5
#define SETTINGS_INTERVAL_DEFAULT           1000
