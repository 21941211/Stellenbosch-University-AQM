
#include "DHT22_Driver.h"




#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define DHT_SAMPLE_SIZE 10
#define DHT_TRIM_COUNT 5


float tempMedian = 0;
float humMedian = 0;

int sampleCounterDHT22 = 0;
long lastMillisDHT22 = 0;


float arrTemp[DHT_SAMPLE_SIZE] = {0};
float arrHum[DHT_SAMPLE_SIZE] = {0};

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

void DHTSetup() {
  dht.begin();
}

void DHT_Measure() {

  long currentMillis = millis();
  if (currentMillis >= (lastMillisDHT22 + 750)){
arrTemp[sampleCounterDHT22] = dht.readTemperature();
arrHum[sampleCounterDHT22] = dht.readHumidity();


lastMillisDHT22 = millis();
sampleCounterDHT22++;
  }
else {
  //Serial.println("DHT22 Not Ready!");
return;
}
if (sampleCounterDHT22==DHT_SAMPLE_SIZE)
{
bubbleSort(arrTemp, DHT_SAMPLE_SIZE);
bubbleSort(arrHum, DHT_SAMPLE_SIZE);

Serial.println("DHT22 Done:");

tempMedian = trimmedMean(arrTemp, DHT_SAMPLE_SIZE, DHT_TRIM_COUNT);
humMedian = trimmedMean(arrHum, DHT_SAMPLE_SIZE, DHT_TRIM_COUNT);

if (isnan(tempMedian) || isnan(humMedian)){
  Serial.println("DHT22 Error: NaN");
  tempMedian = 9999.99;
  humMedian = 9999.99;
  DHT22_DONE = 1;
  return;
}
else if (tempMedian < -40 || tempMedian > 80 || humMedian < 0 || humMedian > 100)
{
  Serial.println("DHT22 Error: Out of Range");
  tempMedian = 1111.11;
  humMedian = 1111.11;
  DHT22_DONE = 1;
  return;
}
else
{
  Serial.println("DHT22 Measurement Successful");
  Serial.print(F("Temperature: "));
  Serial.print(tempMedian);
  Serial.print(F(" C   Humidity: "));
  Serial.print(humMedian);
  Serial.println(F("%"));
}
  Serial.print(F("Humidity: "));
  Serial.print(humMedian);
  Serial.print(F("%  Temperature: "));
  Serial.print(tempMedian);
  Serial.println(F(" C "));
  Serial.println("******************************************************");
  DHT22_DONE = 1;
  return;
}

}



// for (uint8_t i = 0; i < DHT_SAMPLE_SIZE; i++)
// {
//   arrTemp[i] = dht.readTemperature();
//    arrHum[i] = dht.readHumidity();
   
//    Serial.println(arrTemp[i]);
//      Serial.println(arrHum[i]);

//    delay(750);
// }


