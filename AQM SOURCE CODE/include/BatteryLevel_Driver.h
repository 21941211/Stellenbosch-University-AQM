#include "Arduino.h"
#include "pinmapping.h"
#include "DataProcessing_Driver.h"
#include "DeepSleep_Driver.h"
#include "ADC_Driver.h"
#include "MeasureFlags.h"
#include "debug.h"

void measBat();
int avgBat();
uint8_t linearInterpolation(float target);
extern uint8_t batPercentage;
extern RTC_DATA_ATTR uint8_t BATTERY_LOW;
float VoltageCorrection(float voltage);