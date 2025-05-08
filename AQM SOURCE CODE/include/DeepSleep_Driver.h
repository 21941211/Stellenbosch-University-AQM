#include<Arduino.h>
#include "LoRa_Driver.h"
#include "debug.h"

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define DEEP_SLEEP  300      /* Time ESP32 will go to sleep (in seconds) 900s = 15 min */
#define DEEP_SLEEP_LORA_TEST  120      /* Time ESP32 will go to sleep (in seconds) */
#define SDI12_DEEP_SLEEP  900      /* Time ESP32 will go to sleep (in seconds) 900s = 15 min */
#define LIGHT_SLEEP 5
#define TIMEOUT 60 // During this time, the system tries to reconnect to LoRa gateway after a failed transmission attempt.

extern RTC_DATA_ATTR int bootCount;


void print_wakeup_reason();
void wakeUp(uint8_t measureComplete);
void goSleep(int sleepTime);