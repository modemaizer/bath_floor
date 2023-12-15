#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

#include "defines.h"
#include "wifi.h"
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
  doc["interval"] = getSensorsCheckInterval();
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

    getSettingsEndpoint();
  }
}

static void switchDeviceStateEndpoint() {
  toggleDeviceState();
  httpServer.send(200, "text/plain", String(getDeviceState()));
}

// File f;
// String uploadError = "Upload success";

static void restServerRouting() {
  httpServer.serveStatic("/", LittleFS, "/index.html");
  httpServer.serveStatic("/settings", LittleFS, "/settings.html");
  httpServer.serveStatic("/settings.css", LittleFS, "/settings.css");
  httpServer.serveStatic("/settings.js", LittleFS, "/settings.js");
  httpServer.on(F("/data"), HTTP_GET, getDataEndpoint);
  httpServer.on(F("/settings/data"), HTTP_GET, getSettingsEndpoint);
  httpServer.on(F("/settings/update"), HTTP_POST, updateSettingsEndpoint);
  httpServer.on(F("/switch"), HTTP_GET, switchDeviceStateEndpoint);
  

  // Upload new HTML files
  // This responds to commands like:
  //    curl -F "file=@css/dropdown.css;filename=/css/dropdown.css" 192.168.4.1/update
  //    curl -F "file=@index.html;filename=/index.html" 192.168.4.1/update
  // To upload web pages. This is using the underlying SPIFF filesystem, but doesn't require
  // a completely new upload
  // httpServer.onFileUpload([](){
     
  //     if(httpServer.uri() != "/update") return;
  //     HTTPUpload& upload = httpServer.upload();
  //     if(upload.status == UPLOAD_FILE_START){
  //       Serial.setDebugOutput(true);
  //       WiFiUDP::stopAll();
  //       Serial.printf("Update: %s\n", upload.filename.c_str());

  //       //  (1) Rename the old file
  //       if (LittleFS.exists(upload.filename.c_str()))
  //       {
  //         LittleFS.rename(upload.filename.c_str(),(upload.filename+".BAK").c_str());
  //       }
  //       //  (2) Create the new file
  //       f = LittleFS.open(upload.filename.c_str(), "w+");
  //       uploadError = "";
       
  //     } else if(upload.status == UPLOAD_FILE_WRITE){
  //       // (1) Append this buffer to the end of the open file
  //       if (f.write(upload.buf, upload.currentSize) != upload.currentSize){
  //         uploadError = "Error writing file chunk";
  //       }
  //       else
  //       {
  //         Serial.printf("Wrote bytes: %d\n", upload.currentSize);
  //       }
       
  //     } else if(upload.status == UPLOAD_FILE_END){

  //     // Close the file
  //       f.close();
  //       // (1) Check if the update was successful
  //       // (2) If Successful, close the file and delete the renamed one
  //       // (3) If failed, close and delete the new file and move the renamed one back in place
  //       if (uploadError == "")
  //       {
  //         Serial.println("Upload succesful");
  //         LittleFS.remove((upload.filename+".BAK").c_str());
  //       }
  //       else
  //       {
  //         Serial.printf("Error uploading new file putting old file back in place: %s\n", upload.filename.c_str());
  //         LittleFS.remove((upload.filename).c_str());
  //         LittleFS.rename((upload.filename+".BAK").c_str(), upload.filename.c_str());
  //       }
       
  //       Serial.setDebugOutput(false);
  //     }
  //     yield();
  //   });

  //   httpServer.on("/update", HTTP_POST, [](){
  //     httpServer.sendHeader("Connection", "close");
  //     httpServer.sendHeader("Access-Control-Allow-Origin", "*");

  //     // TODO: Send back better information based on whether the upload was successful.
  //     httpServer.send(200, "text/plain", uploadError);
     
  //     //ESP.restart(); // I don't think we need to restart after every file upload
     
  //   });
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