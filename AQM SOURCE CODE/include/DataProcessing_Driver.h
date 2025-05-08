
#include<Arduino.h>
//#include "SDI-12_Driver.h"

extern uint8_t temp[5];
extern uint8_t humidity[5];
extern uint8_t volume[5];


void bubbleSort(float array[], int size);
void bubbleSort16(uint16_t array[], int size);
float trimmedMean(float array[], int size, int trimCount);
uint16_t trimmedMean16(uint16_t arr[], int size, int trimCount);
void dataToBuff(char buff[], float data, uint8_t buffSize);
void dataToBuff16(char buff[], uint16_t data, uint8_t buffSize);
void extractValuesFromStringSDI12(String &input, double *array, String &DevAddress, uint8_t bufferSize);