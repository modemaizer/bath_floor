#define WIFI_SSID "Keenetic-0531"
#define WIFI_PASS "eSVjAJEV"
#define MDNS_NAME "bath"
#define GREEN_LED D7
#define RED_LED D6
#define SENSOR_INTERVAL 500
#define WIFI_ERROR_INTERVAL 500
#define SENSOR_ERROR_INTERVAL 100
#define HEATER D8
#define SENSORS_PIN D4
#define TRIAC_SENSOR_ADDRESS "282d8f75d0013caf";
#define FLOOR_SENSOR_ADDRESS "2835e675d0013cb7";
#define TRIAC_MAX_TEMPERATURE 60

#include <Arduino.h>
#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>
#include <EspHtmlTemplateProcessor.h>
#include <microDS18B20.h>
#include <FileData.h>
#include <ArduinoJson.h>

struct BathSettings {
  float temperature = 20;
  float delta = 0.5;
  bool on = false;
};
BathSettings settings;

FileData data(&LittleFS, "/settings.dat", 'B', &settings, sizeof(settings));

IPAddress localIP(192, 168, 1, 221);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);

unsigned long sensorPreviousMillis = 0;  // last time sensors were checked
unsigned long wifiErrorPreviousMillis = 0;
unsigned long sensorErrorPreviousMillis = 0;

float floorTemp = 0;
float triacTemp = 0;
bool floorSensorError = false;
bool triacSensorError = false;
bool heater = false;
bool wifiError = false;

uint8_t floorSensorAddress[8] = { 0x28, 0x35, 0xE6, 0x75, 0xD0, 0x01, 0x3C, 0xB7 };
uint8_t triacSensorAddress[8] = { 0x28, 0x2D, 0x8F, 0x75, 0xD0, 0x01, 0x3C, 0xAF };

WiFiClient client;
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdateServer;
EspHtmlTemplateProcessor templateProcessor(&httpServer);

MicroDS18B20<SENSORS_PIN, floorSensorAddress> floorSensor;
MicroDS18B20<SENSORS_PIN, triacSensorAddress> triacSensor;

String indexKeyProcessor(const String& key)
{
  if (key == "FLOOR_TEMPERATURE") return floorSensorError ? "Sensor error" : String(floorTemp);
  else if (key == "HEATER_COLOR") return heater ? "red" : "blue";
  else if (key == "TRIAC_TEMPERATURE") return triacSensorError ? "Sensor error" : String(triacTemp);
  else if (key == "SWITCH_CHECKED") return settings.on ? "checked" : "";

  return "Key not found";
}
String settingsMessage = "";
String settingsKeyProcessor(const String& key)
{
  if (key == "TEMPERATURE") return String(settings.temperature);
  else if (key == "DELTA") return String(settings.delta);
  else if (key == "MESSAGE") return settingsMessage;

  return "Key not found";
}

void getMainPage() {
  templateProcessor.processAndSend("/index.html", indexKeyProcessor);
}

void getSettingsForm() {
  if(templateProcessor.processAndSend("/settings.html", settingsKeyProcessor)) {
    settingsMessage = "";
  }
}

void getSettings() {
  DynamicJsonDocument doc(512);
  doc["t"] = settings.temperature;
  doc["d"] = settings.delta;
  doc["on"] = settings.on;
  String buf;
  serializeJson(doc, buf);
  httpServer.send(200, "application/json", buf);
}

void getData() {
  DynamicJsonDocument doc(512);
  doc["floorTemperature"] = floorTemp;
  doc["floorSensorOk"] = !floorSensorError;
  doc["triacTemperature"] = triacTemp;
  doc["triacSensorOk"] = !triacSensorError;
  doc["heaterEnabled"] = heater;
  doc["enabled"] = settings.on;
  String buf;
  serializeJson(doc, buf);
  httpServer.send(200, "application/json", buf);
}

void updateSettings() {
  if (httpServer.method() != HTTP_POST) {
    httpServer.send(405, "text/plain", "Method Not Allowed");
  } 
  else {
    bool changed = false;
    if(httpServer.hasArg("t")) {
      settings.temperature = httpServer.arg("t").toFloat();
      changed = true;
    }
    if(httpServer.hasArg("d")) {
      settings.delta = httpServer.arg("d").toFloat();
      changed = true;
    }
    if(changed) {
      data.update();
      settingsMessage = "Settings updated";
    }
    getSettingsForm();
  }
}

void updateSettings_old() {
  bool changed = false;
  if (httpServer.hasArg("t")){
    settings.temperature = httpServer.arg("t").toFloat();
    changed = true;
  }
  if (httpServer.hasArg("d")){
    settings.delta = httpServer.arg("d").toFloat();
    changed = true;
  }
  if (httpServer.hasArg("on")){
    settings.on = httpServer.arg("on").toInt() == 1;
    changed = true;
  }  
  if(changed) {
    data.updateNow();
  }
  httpServer.send(200, "text/html", "Settings updated");
}

void switchState() {
  settings.on = !settings.on;
  data.update();
  httpServer.send(200, "text/plain", String(settings.on));
}

File f;
String uploadError = "Upload success";

void restServerRouting() {
  httpServer.on(F("/"), HTTP_GET, getMainPage);
  httpServer.on(F("/data"), HTTP_GET, getData);
  httpServer.on(F("/settings"), HTTP_GET, getSettingsForm);
  httpServer.on(F("/settings"), HTTP_POST, updateSettings);
  httpServer.on(F("/switch"), HTTP_GET, switchState);
  

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

void handleNotFound() {
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

void setupWiFi() {
  Serial.println("Setup WiFi");
  WiFi.mode(WIFI_STA);
  if (!WiFi.config(localIP, gateway, subnet)) {
    wifiError = true;
  }
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  int retries = 0;
  while ((WiFi.status() != WL_CONNECTED) && (retries < 15)) {
    retries++;
    delay(500);
  }
  if (retries > 14) {
    wifiError = true;
  }
}

void setupHttp() {
  restServerRouting();
  httpServer.onNotFound(handleNotFound);
  httpUpdateServer.setup(&httpServer, "bath", "bath");
  httpServer.begin();
}

void setupMdns() {
  if(MDNS.begin(MDNS_NAME)) {
    MDNS.addService("http", "tcp", 80);
  }
}

void setupNetwork() {
  setupWiFi();
  setupMdns();
  setupHttp();
}

void readFloor() {
  if (floorSensor.readTemp()) {
    floorTemp = floorSensor.getTemp();
    floorSensorError = false;
  } 
  else {
    floorTemp = 0;
    floorSensorError = true;
  }
  floorSensor.setResolution(10);
  floorSensor.requestTemp();
}

void readTriac() {
  if (triacSensor.readTemp()) {
    triacTemp = triacSensor.getTemp();
    triacSensorError = false;
  } 
  else {
    triacTemp = 0;
    triacSensorError = true;
  }
  triacSensor.setResolution(10);
  triacSensor.requestTemp();
}

float getTemperatureHighBound() {
  return settings.temperature + settings.delta;
}

float getTemperatureLowBound() {
  return settings.temperature - settings.delta;
}

void switchHeater(bool enable) {
  digitalWrite(HEATER, enable);
  heater = enable;
}

void processHeater() {
  if(!settings.on || floorSensorError || triacSensorError || floorTemp >= getTemperatureHighBound() || triacTemp > TRIAC_MAX_TEMPERATURE) {
    switchHeater(false);
    return;
  }
  if(settings.on && floorTemp <= getTemperatureLowBound()) {
    switchHeater(true);
  }
}

void processSensors() {

  readFloor();
  readTriac();
  
  processHeater();
}

void resetSettings() {
  settings.temperature = 20;
  settings.delta = 0.5;
  settings.on = false;
  data.updateNow();
}

void invertPin(uint8_t pin){
  digitalWrite(pin, !digitalRead(pin));
}

void blink(uint8_t pin, unsigned long &counter, unsigned long interval) {
  unsigned long current = millis();
  if(current - counter >= interval) {
    invertPin(pin);
    counter = current;
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Start");
  if(LittleFS.begin()) {
    Serial.println("LittleFS OK");
  }
  else {
    Serial.println("LittleFS ERR");
  }
  
  // прочитать данные из файла в переменную
  // при первом запуске в файл запишутся данные из структуры
  FDstat_t stat = data.read();
  switch (stat) {
    case FD_FS_ERR: Serial.println("FS Error");
      break;
    case FD_FILE_ERR: Serial.println("Error");
      break;
    case FD_WRITE: Serial.println("Data Write");
      break;
    case FD_ADD: Serial.println("Data Add");
      break;
    case FD_READ: Serial.println("Data Read");
      break;
    default:
      break;
  }
  pinMode(HEATER, OUTPUT);
  digitalWrite(HEATER, LOW);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(RED_LED, LOW);
  pinMode(GREEN_LED, OUTPUT);
  digitalWrite(GREEN_LED, LOW);

  setupNetwork();
}

void processLeds() {
  if(wifiError) {
    blink(GREEN_LED, wifiErrorPreviousMillis, WIFI_ERROR_INTERVAL);
  }
  else {    
    digitalWrite(GREEN_LED, HIGH);
  }

  if(floorSensorError) {
    blink(RED_LED, sensorErrorPreviousMillis, SENSOR_ERROR_INTERVAL);
  }
  else if(heater) {
    digitalWrite(RED_LED, HIGH);
  }
  else {
    digitalWrite(RED_LED, LOW);
  }
}

void loop() {
  MDNS.update();
  data.tick();
  
  processLeds();
  
  unsigned long currentMillis = millis();
  if (currentMillis - sensorPreviousMillis >= SENSOR_INTERVAL) {
    sensorPreviousMillis = currentMillis;
    processSensors();
  }

  httpServer.handleClient();
}

