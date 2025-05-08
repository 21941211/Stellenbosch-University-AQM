#ifndef LORA_H
#define LORA_H

#include "Arduino.h"
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <debug.h>
#include <pinmapping.h>
#include "DataProcessing_Driver.h"
#include "SPI_Driver.h"
#include <bits/stdc++.h> 
#include <sstream>
#include "SD_Driver.h"
#include "DeepSleep_Driver.h"
#include "DHT22_Driver.h"
#include "debug.h"

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 60;

// LoRa keys

static u1_t DEVID[1];
#ifdef ENABLE_SD
extern u1_t APPEUI[8];
extern u1_t DEVEUI[8];
extern u1_t APPKEY[16];

extern u1_t APPEUITEST[8];
extern u1_t DEVEUITEST[8];
extern u1_t APPKEYTEST[16];

#else
 static u1_t APPEUI[8] =  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
 static u1_t DEVEUI[8] = {0x66, 0xE7, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70};
 static u1_t APPKEY[16] = {0xF1, 0x8C, 0xB2, 0x63, 0x5F, 0xAF, 0x2A, 0x9C, 0x8B, 0x4F, 0x5F, 0x23, 0xB4, 0x29, 0xB1, 0x4B};

#endif



extern RTC_DATA_ATTR unsigned int LoRaTX_Complete;
extern uint8_t NoJoin;


void printHex2(unsigned v);
void onEvent (ev_t ev);
void do_send(osjob_t* j);
void LoRaSetup(void);
void os_getArtEui (u1_t* buf);
void os_getDevEui (u1_t* buf) ;
void os_getDevKey (u1_t* buf);
void setMyData(float temp, float humid);

#endif