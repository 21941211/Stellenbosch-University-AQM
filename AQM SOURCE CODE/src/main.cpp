#include <Arduino.h>
#include "LoRa_Driver.h"
#include "DHT22_Driver.h"
#include "SD_Driver.h"
#include "DeepSleep_Driver.h"
#include "DataProcessing_Driver.h"
#include "BatteryLevel_Driver.h"
#include <Wire.h>
#include "MeasureFlags.h"
#include "driver/rtc_io.h"
#include "debug.h"
#include "PMS5003_Driver.h"
#include "SoftwareSerial.h"
#include <PMS.h>

#define PULLDOWN_GPIO GPIO_NUM_5 // Only RTC IO are allowed

uint8_t SDWRITE_DONE = 0;

uint8_t SDI12_SETUP_COMPLETE = 0;

uint16_t PM1_0[20];
uint16_t PM2_5[20];
uint16_t PM10_0[20];

uint8_t PM_Index = 0;


SoftwareSerial Serial2( PMS5003_TX,PMS5003_RX); // RX, TX

PMS pms(Serial2);
PMS::DATA data;


void setup()
{

  // Slow down CPU for lower power usage
  //setCpuFrequencyMhz(40);
  Serial.begin(115200);
  delay(500);



  Serial.println("Starting");
    Serial.println("Checking flags and boot count: ");
  if (bootCount > 999999)
  {
    bootCount = 0;
  }
  else if(!MEASURE_COMPLETE){
    bootCount++;
  }

    Serial.println("Boot number: " + String(bootCount));
    //Serial.println("SDI12-Connection Status (0 = NC, 1 = C): " +String(SDI12_CONNECTED));
   // Serial.println("SDI12-Connection Type (0 = 60cm, 1 = 90cm, 2 = CS655, 3 = none): "+ String(SDI12_TYPE));
    Serial.println("Measurement transmitted in previous cycle (0 = yes, 1 = no): " + String(MEASURE_COMPLETE));
  Serial.println("******************************************************");


  

  pinMode(SD_ENABLE_PIN, OUTPUT);
  pinMode(LORA_CS_PIN, OUTPUT);
  pinMode(DEBUG_LED_PIN, OUTPUT);
  pinMode(DHT22_ENABLE_PIN, OUTPUT);


//measBat();
//InfiniteStop();

#ifdef ENABLE_SD
  SDSetup();

 

#else
  Serial.println("SD Card Disabled, using default LoRa parameters");
#endif

if(!MEASURE_COMPLETE){
 pms5003_power();
 Serial2.begin(9600);
 for (int i = 0; i < 30; i++)
 {
  Serial.println("Warming up...");
  delay(1000);
 }

 Serial.println("Measuring now...");
}



  if (BATTERY_LOW == 1)
  {
    measBat();
    // this will never run if the battery is low
    BATTERY_LOW = 0;
  }

  if (MEASURE_COMPLETE)
  {
    LoRaSetup();
    decodePayload();
    os_runloop();
  }

// #ifdef ENABLE_MEASURE
//   pinMode(DHT22_ENABLE_PIN, OUTPUT);
// #endif


// #ifdef ENABLE_LORA
//   if (MEASURE_COMPLETE)
//   {
//     LoRaSetup();
//   }
// #endif

// #ifdef ENABLE_LORA_TEST
//   {
//     LoRaSetup();
//   }
// #endif

//   digitalWrite(DEBUG_LED_PIN, HIGH);

// //Code here to start PMS5003 setup

//   digitalWrite(DEBUG_LED_PIN, LOW); // turn the LED off (HIGH is the voltage level)
}

void loop()
{

  if (DHT22_DONE&&PMS_DONE)
  {
    digitalWrite(DHT22_ENABLE_PIN, LOW); // turn the LED on (HIGH is the voltage level)
  }
  else if (digitalRead(DHT22_ENABLE_PIN) == LOW)
  {
    digitalWrite(DHT22_ENABLE_PIN, HIGH); // turn the LED on (HIGH is the voltage level))
    DHTSetup();
    Serial.println("DHT22 now ON");
    Serial.println("******************************************************");
  }

  if (!MEASURE_COMPLETE)
  {

    if (!BATT_DONE)
      measBat();

   if (pms.read(data)&&PM_Index<20)
  {
    Serial.print("PM 1.0 (ug/m3): ");
    Serial.println(data.PM_AE_UG_1_0);
    PM1_0[PM_Index] = data.PM_AE_UG_1_0;

    Serial.print("PM 2.5 (ug/m3): ");
    Serial.println(data.PM_AE_UG_2_5);
    PM2_5[PM_Index] = data.PM_AE_UG_2_5;

    Serial.print("PM 10.0 (ug/m3): ");
    Serial.println(data.PM_AE_UG_10_0);
    PM10_0[PM_Index] = data.PM_AE_UG_10_0;

    Serial.println();

    PM_Index++;
  }
 

  if (PM_Index == 20 && !PMS_DONE)
  {

    pms5003_shutdown();

    for (int i = 0; i < 20; i++)
    {
      Serial.print("PM 1.0 (ug/m3) value " + String(i) + ": ");
      Serial.println(PM1_0[i]);
      Serial.print("PM 2.5 (ug/m3) value " + String(i) + ": ");
      Serial.println(PM2_5[i]);
      Serial.print("PM 10.0 (ug/m3) value " + String(i) + ": ");
      Serial.println(PM10_0[i]);
      Serial.println();
    }

    bubbleSort16(PM1_0, 20);
    bubbleSort16(PM2_5, 20);
    bubbleSort16(PM10_0, 20);

    PM1_0_avg =  trimmedMean16(PM1_0, 20, 10);
   PM2_5_avg = trimmedMean16(PM2_5, 20, 10);
   PM10_0_avg = trimmedMean16(PM10_0, 20, 10);



  Serial.println("PM 1.0 (ug/m3) trimmed mean: " + String(PM1_0_avg));
  Serial.println("PM 2.5 (ug/m3) trimmed mean: " + String(PM2_5_avg));
  Serial.println("PM 10.0 (ug/m3) trimmed mean: " + String(PM10_0_avg));

  // writeToSD();
  // readLastEntry();
  // //LoRaSetup();
  // decodePayload();

  
PMS_DONE =1;

  }

    if (!DHT22_DONE)
      DHT_Measure();
 
    if (BATT_DONE && DHT22_DONE&&PMS_DONE)
    {
      Serial.println("All Measurements Done");
      Serial.println("******************************************************");
     // InfiniteStop();
    

      MEASURE_COMPLETE = 1;
    }
  }
  else
  {

    if (!SDWRITE_DONE)
    {
#ifdef ENABLE_SD

      writeToSD();

#endif
     
      LoRaSetup();

#ifdef ENABLE_SD
      decodePayload();
      
#endif

      SDWRITE_DONE = 1;
     // InfiniteStop();
    }

#ifdef ENABLE_LORA

    os_runloop_once();
   // delay(100);
#endif
  }

}
