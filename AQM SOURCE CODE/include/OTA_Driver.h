#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include "pinmapping.h"



extern RTC_DATA_ATTR int OTA_Window_Missed; // RTC memory variable to keep track of OTA window

uint8_t setupOTA(void);

void LoopOTA(void);

void disableWiFi();