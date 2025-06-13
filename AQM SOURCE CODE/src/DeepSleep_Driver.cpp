#include "DeepSleep_Driver.h"


RTC_DATA_ATTR int CYCLE_TIME = 0;

RTC_DATA_ATTR int bootCount = 0;

RTC_DATA_ATTR uint8_t FirstLightSleep = 1;

RTC_DATA_ATTR int initialActiveTime = 0;

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

void wakeUp(uint8_t measureComplete){
//++bootCount;
  //Serial.println("Boot number: " + String(bootCount));

  //Print the wakeup reason for ESP32
  //print_wakeup_reason();

  /*
  First we configure the wake up source
  We set our ESP32 to wake up every 5 seconds
  */
if (measureComplete)
{
  esp_sleep_enable_timer_wakeup(LIGHT_SLEEP * uS_TO_S_FACTOR);
    Serial.println("Setup ESP32 to sleep for every " + String(LIGHT_SLEEP) +
  " Seconds");
}else
{
   esp_sleep_enable_timer_wakeup(TOTAL_CYCLE_TIME * uS_TO_S_FACTOR);
     Serial.println("Setup ESP32 to sleep for every " + String(TOTAL_CYCLE_TIME) +
  " Seconds");
  LoRaTX_Complete = 0;

}
}

void goSleep(int sleepTime){

if (sleepTime==LIGHT_SLEEP){
 if(!FirstLightSleep){
  Serial.println(millis());
     CYCLE_TIME = CYCLE_TIME + LIGHT_SLEEP + millis()/1000 ;
     Serial.print("Current cycle time:");
     Serial.println(CYCLE_TIME);

  }else{
    esp_sleep_enable_timer_wakeup(sleepTime*uS_TO_S_FACTOR);
    FirstLightSleep = 0;
    initialActiveTime = millis()/1000;
    Serial.println("Initial active time: "+ String(initialActiveTime));
    CYCLE_TIME = initialActiveTime;
    Serial.print("Current cycle time:");
    Serial.println(CYCLE_TIME);
        Serial.println("Going to first light sleep");
  Serial.flush(); 
  esp_deep_sleep_start();
  }
  if (CYCLE_TIME>=(TIMEOUT+initialActiveTime-LIGHT_SLEEP))
  {
    
    Serial.println("Transmission timeout reached, going to DEEP SLEEP now...");
    MEASURE_COMPLETE = 0;
    FirstLightSleep = 1;
   Serial.println("Initial active time: "+ String(initialActiveTime));
  
    int remainingSleepTime = TOTAL_CYCLE_TIME-CYCLE_TIME;
    CYCLE_TIME = 0;
    initialActiveTime = 0;
    Serial.println("Remaining deep sleep time: "+String(remainingSleepTime));
    esp_sleep_enable_timer_wakeup(remainingSleepTime*uS_TO_S_FACTOR);
    Serial.flush(); 
    delay(300);
    esp_deep_sleep_start();
  }else{
    Serial.println("Going to sleep now for "+String(sleepTime)+" seconds");
    esp_sleep_enable_timer_wakeup(sleepTime*uS_TO_S_FACTOR);
    delay(300);
    
  Serial.flush(); 
  esp_deep_sleep_start();
  }
}
else{
 
  FirstLightSleep = 1;
    CYCLE_TIME = CYCLE_TIME + millis()/1000;;
    int remainingSleepTime = (TOTAL_CYCLE_TIME-CYCLE_TIME);
    Serial.print("Current cycle time:");
    Serial.println(CYCLE_TIME);
    Serial.println("Initial active time: "+ String(initialActiveTime));
    CYCLE_TIME = 0;
    initialActiveTime = 0;
    Serial.println("Remaining sleep time: "+ String(remainingSleepTime)+" s");
    delay(300);
    Serial.flush(); 
    esp_sleep_enable_timer_wakeup(remainingSleepTime*uS_TO_S_FACTOR);
    esp_deep_sleep_start();
}

}