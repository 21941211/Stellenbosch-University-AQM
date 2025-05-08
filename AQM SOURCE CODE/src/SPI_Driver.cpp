#include "SPI_Driver.h"
#include <Arduino.h>

void setSPI(uint8_t SPIMode){
 if (SPIMode == LORA_SPI)
 {
     SPI.begin(LORA_SCK_PIN, LORA_MISO_PIN, LORA_MOSI_PIN, LORA_CS_PIN); 
     Serial.println("LoRa SPI Setup done");
     
 } else if (SPIMode == SD_SPI ){
     SPI.begin(SD_SCK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);
      Serial.println("SD SPI Setup done");
 }

}