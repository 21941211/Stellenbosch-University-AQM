#include <Arduino.h>
#include <SoftwareSerial.h>
#include <pinmapping.h>
#include "PMS.h"

void pms5003_WriteToSerial();

void pms5003_power();
void pms5003_shutdown();
void pms5003_setup();
boolean pms5003_Read(Stream *s);

  extern uint16_t PM1_0_avg;
  extern uint16_t PM2_5_avg;
  extern uint16_t PM10_0_avg;
