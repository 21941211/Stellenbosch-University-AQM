#include "BatteryLevel_Driver.h"


#define BAT_SAMPLE_SIZE 20
#define BAT_TRIM_SIZE 5

float vCalibration = 3.38;

float correctionFactor = 1.00;

RTC_DATA_ATTR uint8_t BATTERY_LOW = 0;

   const uint8_t dataSize = 100; // Number of data points
    
    // Data
    const uint8_t SOC[dataSize] = {
        100, 99, 98, 98, 97, 96, 96, 95, 94, 92,
        91, 90, 89, 88, 86, 85, 84, 83, 82, 81,
        80, 79, 78, 77, 76, 75, 74, 73, 72, 71,
        70, 69, 68, 67, 66, 65, 64, 63, 62, 61,
        60, 59, 58, 57, 55, 54, 53, 52, 51, 50,
        49, 48, 47, 46, 45, 44, 43, 41, 40, 39,
        38, 37, 36, 35, 33, 32, 31, 30, 29, 27,
        26, 25, 23, 22, 21, 19, 18, 17, 15, 14,
        13, 12, 10, 10, 9, 8, 7, 6, 5, 5, 4, 3, 2, 2, 1, 1, 0, 0, 0, 0
    };

    const float VOC[dataSize] = {
        4.2, 4.161, 4.135, 4.103, 4.075, 4.056, 4.035, 4.009, 3.991, 3.97,
        3.949, 3.936, 3.918, 3.902, 3.894, 3.884, 3.873, 3.865, 3.86, 3.852,
        3.847, 3.844, 3.837, 3.834, 3.826, 3.818, 3.816, 3.813, 3.808, 3.808,
        3.803, 3.797, 3.795, 3.795, 3.79, 3.787, 3.784, 3.779, 3.776, 3.771,
        3.769, 3.763, 3.761, 3.753, 3.753, 3.753, 3.748, 3.745, 3.742, 3.737,
        3.735, 3.732, 3.729, 3.727, 3.722, 3.722, 3.716, 3.708, 3.708, 3.703,
        3.698, 3.701, 3.698, 3.693, 3.69, 3.688, 3.685, 3.685, 3.68, 3.675,
        3.669, 3.664, 3.664, 3.656, 3.651, 3.646, 3.635, 3.63, 3.62, 3.607,
        3.596, 3.586, 3.57, 3.557, 3.533, 3.523, 3.499, 3.473, 3.447, 3.421,
        3.39, 3.35, 3.316, 3.288, 3.259, 3.222, 3.178, 3.12, 3.073, 3
    };



    // Print the contents of the array to verify


float arrBatt[BAT_SAMPLE_SIZE]={0};
int batMedian;
uint8_t batPercentage;

// Function to perform linear interpolation
uint8_t linearInterpolation(float target) {
    for (uint8_t i = 0; i < dataSize - 1; i++) {

        if (target<=VOC[i]&&target>=VOC[i+1]) {  
            return SOC[i];    
        }
        
    }
    // Return 0 if target is outside the range of floatData
    return 0;
}


void measBat() {

 double voltage;

pinMode(BATTERY_ENABLE_PIN, OUTPUT);

digitalWrite(BATTERY_ENABLE_PIN, HIGH);

delay(500);


for (uint8_t i = 0; i < BAT_SAMPLE_SIZE; i++)
{
    arrBatt[i] = analogRead(BATTERY_ADC_PIN);
    Serial.println(arrBatt[i]);
   delay(100);
}

bubbleSort(arrBatt, BAT_SAMPLE_SIZE);
batMedian=trimmedMean(arrBatt,BAT_SAMPLE_SIZE, BAT_TRIM_SIZE);

 Serial.print("ADC Value ");
  Serial.println(batMedian);



voltage = (double(batMedian)*2600)/8191.0;

// Piecewise linear calibration

Serial.print("Voltage: ");
Serial.print(voltage); // read sensor
Serial.println(" mV");


// voltage = correctVoltage(voltage);

// if (voltage > 1865.0){
//     voltage = 1858.0;
// }

Serial.println("Battery measurement done:");

 double scaledVoltage = (voltage*1.68)/1000.0;

  Serial.print("Scaled Voltage before correction: ");
 Serial.print(scaledVoltage); 
Serial.println(" V");
Serial.print("Voltage after correction: ");


// Apply the correction factor
scaledVoltage = scaledVoltage * correctionFactor;

Serial.print(scaledVoltage);
Serial.println(" V");

//scaledVoltage = VoltageCorrection(scaledVoltage);


if (scaledVoltage > 4.2){
    scaledVoltage = 4.2;
}

//Serial.print("Scaled Voltage after correction: ");
 //Serial.print(scaledVoltage);
//Serial.println(" V");
  



batPercentage = linearInterpolation(scaledVoltage);

  Serial.print("Remaining battery: ");
 Serial.print(batPercentage); 
Serial.println(" %");

Serial.println("******************************************************");

digitalWrite(BATTERY_ENABLE_PIN, LOW);

BATT_DONE = 1;

if (batPercentage<10)
{

  Serial.println("Battery Low");

 BATTERY_LOW = 1;
 delay(100);
 goSleep(DEEP_SLEEP);
 
}

delay(100);
//breakFloat(SM_Vol, volume);

}

int MedBat(){

int value;
int sum = 0;
int avg;

for (int i = 0; i < BAT_SAMPLE_SIZE; i++)
{
    
    delay(100); // slight delay between readings
}

avg = sum/10;

return avg;
}


float VoltageCorrection(float voltage) {
    float scalingFactor = 3.83/3.76;
    return voltage * scalingFactor;

}
