/**
 * BZ40i HTTP Example
 * 
 * Live HTTP status page for BZ40i
 *  
 * Requires:
 * https://github.com/plerup/espsoftwareserial#4.0.0
 * https://github.com/me-no-dev/ESPAsyncTCP
 * https://github.com/me-no-dev/ESPAsyncWebServer
 */

#define READSDMEVERY  5000                                                      //read BZ40i every 5s
//#define USE_STATIC_IP

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

#include <ESPAsyncTCP.h>                                                        //https://github.com/me-no-dev/ESPAsyncTCP
#include <ESPAsyncWebServer.h>                                                  //https://github.com/me-no-dev/ESPAsyncWebServer

#include <SoftwareSerial.h>                                                     //import SoftwareSerial library (if used)
#include <BZ40i.h>                                                              //https://github.com/adlerweb/BZ40i_Energy_Meter/

#include "index_page.h"

#define NBREG 37
#define DEBUG_SERIAL

//------------------------------------------------------------------------------
AsyncWebServer server(80);

SoftwareSerial swSerBZ40i(D1, D2);                                              //config SoftwareSerial (rx->D1 / tx->D2)

BZ40i bz40i(swSerBZ40i, 4800, NOT_A_PIN);                                       //SOFTWARE SERIAL
//BZ40i bz40i(Serial, 9600, NOT_A_PIN, SERIAL_8N1, false);                      //HARDWARE SERIAL

//------------------------------------------------------------------------------
String devicename = "PWRMETER";

#if defined ( USE_STATIC_IP )
IPAddress ip(192, 168, 0, 130);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
#endif

const char* wifi_ssid = "YOUR_SSID";
const char* wifi_password = "YOUR_PASSWORD";

String lastresetreason = "";

unsigned long readtime;
//------------------------------------------------------------------------------
typedef struct {
  String dname;
  float regvalarr;
  const uint16_t regarr;
  byte isSigned;
} bz40i_struct;

bz40i_struct bz40iarr[NBREG] = {
  {"BZ40i_U_SYSTEM",        0.00, BZ40i_U_SYSTEM,       0},
  {"BZ40i_U_LN_P1",         0.00, BZ40i_U_LN_P1,        0},
  {"BZ40i_U_LN_P2",         0.00, BZ40i_U_LN_P2,        0},
  {"BZ40i_U_LN_P3",         0.00, BZ40i_U_LN_P3,        0},
  {"BZ40i_U_LL_P12",        0.00, BZ40i_U_LL_P12,       0},
  {"BZ40i_U_LL_P23",        0.00, BZ40i_U_LL_P23,       0},
  {"BZ40i_U_LL_P31",        0.00, BZ40i_U_LL_P31,       0},
  {"BZ40i_I_SYSTEM",        0.00, BZ40i_I_SYSTEM,       1},
  {"BZ40i_I_P1",            0.00, BZ40i_I_P1,           1},
  {"BZ40i_I_P2",            0.00, BZ40i_I_P2,           1},
  {"BZ40i_I_P3",            0.00, BZ40i_I_P3,           1},
  {"BZ40i_I_N",             0.00, BZ40i_I_N,            0},
  {"BZ40i_PF_SYSTEM",       0.00, BZ40i_PF_SYSTEM,      1},
  {"BZ40i_PF_P1",           0.00, BZ40i_PF_P1,          1},
  {"BZ40i_PF_P2",           0.00, BZ40i_PF_P2,          1},
  {"BZ40i_PF_P3",           0.00, BZ40i_PF_P3,          1},
  {"BZ40i_S_SYSTEM",        0.00, BZ40i_S_SYSTEM,       1},
  {"BZ40i_S_P1",            0.00, BZ40i_S_P1,           1},
  {"BZ40i_S_P2",            0.00, BZ40i_S_P2,           1},
  {"BZ40i_S_P3",            0.00, BZ40i_S_P3,           1},
  {"BZ40i_P_SYSTEM",        0.00, BZ40i_P_SYSTEM,       1},
  {"BZ40i_P_P1",            0.00, BZ40i_P_P1,           1},
  {"BZ40i_P_P2",            0.00, BZ40i_P_P2,           1},
  {"BZ40i_P_P3",            0.00, BZ40i_P_P3,           1},
  {"BZ40i_Q_SYSTEM",        0.00, BZ40i_Q_SYSTEM,       1},
  {"BZ40i_Q_P1",            0.00, BZ40i_Q_P1,           1},
  {"BZ40i_Q_P2",            0.00, BZ40i_Q_P2,           1},
  {"BZ40i_Q_P3",            0.00, BZ40i_Q_P3,           1},
  {"BZ40i_F",               0.00, BZ40i_F,              2},
  {"BZ40i_IMPORT_P",        0.00, BZ40i_IMPORT_P,       0},
  {"BZ40i_IMPORT_Q_LAG",    0.00, BZ40i_IMPORT_Q_LAG,   0},
  {"BZ40i_IMPORT_Q_LEAD",   0.00, BZ40i_IMPORT_Q_LEAD,  0},
  {"BZ40i_IMPORT_S",        0.00, BZ40i_IMPORT_S,       0},
  {"BZ40i_EXPORT_P",        0.00, BZ40i_EXPORT_P,       0},
  {"BZ40i_EXPORT_Q_LAG",    0.00, BZ40i_EXPORT_Q_LAG,   0},
  {"BZ40i_EXPORT_Q_LEAD",   0.00, BZ40i_EXPORT_Q_LEAD,  0},
  {"BZ40i_EXPORT_S",        0.00, BZ40i_EXPORT_S,       0}
};
//------------------------------------------------------------------------------
void jsonrequest(AsyncWebServerRequest *request) {
  String json = F("{\n");
  for (int i = 0; i < NBREG; i++) { 
    json += "  \"";
    json += bz40iarr[i].dname;
    json += "\": " + String(bz40iarr[i].regvalarr,2) + ",\n";
  }
  json += F("  \"heap\": ");
  json += String(ESP.getFreeHeap());
  json += F(",\n  \"reset\": \"");
  json += lastresetreason;
  json += F("\"\n}");
  request->send(200, "application/json", json);
}
//------------------------------------------------------------------------------
void indexrequest(AsyncWebServerRequest *request) {
  request->send_P(200, "text/html", index_page); 
}
//------------------------------------------------------------------------------
void ledOn() {
  digitalWrite(LED_BUILTIN, LOW);
}
//------------------------------------------------------------------------------
void ledOff() {
  digitalWrite(LED_BUILTIN, HIGH);
}
//------------------------------------------------------------------------------
void ledSwap() {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}
//------------------------------------------------------------------------------
void otaInit() {
  ArduinoOTA.setHostname(devicename.c_str());

  ArduinoOTA.onStart([]() {
    ledOn();
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    ledSwap();
  });
  ArduinoOTA.onEnd([]() {
    ledOff();
  });
  ArduinoOTA.onError([](ota_error_t error) {
    ledOff();
  });
  ArduinoOTA.begin();
  #ifdef DEBUG_SERIAL
    Serial.println("OTA ready");
  #endif
}
//------------------------------------------------------------------------------
void serverInit() {
  server.on("/", HTTP_GET, indexrequest);
  server.on("/json", HTTP_GET, jsonrequest);
  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(404);
  });
  server.begin();
  #ifdef DEBUG_SERIAL
    Serial.println("HTTP ready");
  #endif
}
//------------------------------------------------------------------------------
static void wifiInit() {
  WiFi.persistent(false);                                                       // Do not write new connections to FLASH
  WiFi.mode(WIFI_STA);
#if defined ( USE_STATIC_IP )
  WiFi.config(ip, gateway, subnet);                                             // Set fixed IP Address
#endif
  WiFi.begin(wifi_ssid, wifi_password);

  while( WiFi.status() != WL_CONNECTED ) {                                      //  Wait for WiFi connection
    ledSwap();
    delay(100);
  }

  #ifdef DEBUG_SERIAL
    Serial.print("Connected to ");
    Serial.print(wifi_ssid);
    Serial.print(" using legacy IP ");
    Serial.println(WiFi.localIP());
  #endif
}
//------------------------------------------------------------------------------
void bz40iRead() {
  float temp = NAN;

  for (uint8_t i = 0; i < NBREG; i++) {
    temp = bz40i.readVal(bz40iarr[i].regarr, bz40iarr[i].isSigned);

    if(!isnan(temp)) {
      bz40iarr[i].regvalarr = temp;
      #ifdef DEBUG_SERIAL
        Serial.print(bz40iarr[i].dname);
        Serial.print(" -> ");
        Serial.println(bz40iarr[i].regvalarr);
      #endif
    }

    yield();
  }
}
//------------------------------------------------------------------------------
void setup() {
  #ifdef DEBUG_SERIAL
    Serial.begin(115200);
    Serial.println("\nBZ40i HTTP example");
  #endif
  pinMode(LED_BUILTIN, OUTPUT);
  ledOn();

  lastresetreason = ESP.getResetReason();

  wifiInit();
  otaInit();
  serverInit();
  bz40i.begin();

  readtime = millis();

  ledOff();
}
//------------------------------------------------------------------------------
void loop() {
  ArduinoOTA.handle();

  if (millis() - readtime >= READSDMEVERY) {
    bz40iRead();
    readtime = millis();
  }

  yield();
}
