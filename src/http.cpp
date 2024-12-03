#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

#include "defines/network.h"
#include "defines/ota.h"
#include "wifi_connector.h"
#include "main_process.h"
#include "sensors.h"
#include "settings.h"

static WiFiClient wifiClient;
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdateServer;

static void getSettingsEndpoint() {
  DynamicJsonDocument doc(512);
  doc["temperature"] = getSettingsTemperature();
  doc["delta"] = getSettingsDelta();
  doc["state"] = getDeviceState();
  doc["sensors_interval"] = getSensorsCheckInterval();
  doc["mqtt_interval"] = getMqttInterval();
  doc["version"] = CURRENT_VERSION;
  String buf;
  serializeJson(doc, buf);
  httpServer.send(200, "application/json", buf);
}

static void getDataEndpoint() {
  DynamicJsonDocument doc(512);
  doc["floorTemperature"] = getFloorTemperature();
  doc["floorSensorOk"] = getFloorSensorOk();
  doc["triacTemperature"] = getTriacTemperature();
  doc["triacSensorOk"] = getTriacSensorOk();
  doc["heaterEnabled"] = getHeaterState();
  doc["enabled"] = getDeviceState();
  String buf;
  serializeJson(doc, buf);
  httpServer.send(200, "application/json", buf);
}

static void updateSettingsEndpoint() {
  if (httpServer.method() != HTTP_POST) {
    httpServer.send(405, "text/plain", "Method Not Allowed");
  } 
  else {
    if(httpServer.hasArg("t")) {
      setSettingsTemperature(httpServer.arg("t").toFloat());
    }
    if(httpServer.hasArg("d")) {
      setSettingsDelta(httpServer.arg("d").toFloat());
    }
    if(httpServer.hasArg("i")) {
      setSensorsCheckInterval(httpServer.arg("i").toInt());
    }
    if(httpServer.hasArg("m")) {
      setMqttInterval(httpServer.arg("m").toInt());
    }

    getSettingsEndpoint();
  }
}

static void switchDeviceStateEndpoint() {
  toggleDeviceState();
  httpServer.send(200, "text/plain", String(getDeviceState()));
}

static void restServerRouting() {
  httpServer.serveStatic("/", LittleFS, "/index.html");
  httpServer.serveStatic("/settings", LittleFS, "/settings.html");
  httpServer.serveStatic("/settings.css", LittleFS, "/settings.css");
  httpServer.serveStatic("/settings.js", LittleFS, "/settings.js");
  httpServer.on(F("/data"), HTTP_GET, getDataEndpoint);
  httpServer.on(F("/settings/data"), HTTP_GET, getSettingsEndpoint);
  httpServer.on(F("/settings/update"), HTTP_POST, updateSettingsEndpoint);
  httpServer.on(F("/switch"), HTTP_GET, switchDeviceStateEndpoint);
}

static void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += httpServer.uri();
  message += "\nMethod: ";
  message += (httpServer.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += httpServer.args();
  message += "\n";
  for (uint8_t i = 0; i < httpServer.args(); i++) {
    message += " " + httpServer.argName(i) + ": " + httpServer.arg(i) + "\n";
  }
  httpServer.send(404, "text/plain", message);
}

void httpInit() {
  restServerRouting();
  httpServer.onNotFound(handleNotFound);
  httpUpdateServer.setup(&httpServer, UPDATE_USERNAME, UPDATE_PASS);
  httpServer.begin();
}

void httpProcess() {
  httpServer.handleClient();
}