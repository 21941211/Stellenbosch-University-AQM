#include<Arduino.h>
#include "LoRa_Driver.h"
#include "debug.h"

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TOTAL_CYCLE_TIME  600      /* Total cycle time of the AQM 600s = 10 min. Sleep time = total cycle time - total active time*/
#define DEEP_SLEEP_LORA_TEST  120      /* Time ESP32 will go to sleep (in seconds) */
#define LIGHT_SLEEP 5
#define TIMEOUT 60 // During this time, the system tries to reconnect to LoRa gateway after a failed transmission attempt.

extern RTC_DATA_ATTR int bootCount;


void print_wakeup_reason();
void wakeUp(uint8_t measureComplete);
void goSleep(int sleepTime);