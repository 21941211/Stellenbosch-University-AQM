
#include "DHT.h"
#include <debug.h>
#include <pinmapping.h>
#include <Arduino.h>
#include "LoRa_Driver.h"
#include "MeasureFlags.h"


void DHTSetup();
void DHT_Measure();


extern float tempMedian;
extern float humMedian;


