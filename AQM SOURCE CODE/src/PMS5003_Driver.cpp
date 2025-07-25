#include <PMS5003_Driver.h>

//SoftwareSerial pmsSerial(PMS5003_TX,PMS5003_RX,false);

// struct pms5003data {
//   uint16_t framelen;
//   uint16_t pm10_standard, pm25_standard, pm100_standard;
//   uint16_t pm10_env, pm25_env, pm100_env;
//   uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
//   uint16_t unused;
//   uint16_t checksum;
// };


// struct pms5003data data;

uint16_t PM1_0_avg =0;
 uint16_t PM2_5_avg=0;
uint16_t PM10_0_avg=0;

bool devID5 = false; // default value


void pms5003_power(){
    //pinMode(PMS5003_EN_PIN,OUTPUT);
    digitalWrite(PMS5003_EN_PIN, HIGH);
    if(devID5){
        Serial.println("DEVID 5 detected, powering PMS5003 through DHT22 VCC and GND pins");
        digitalWrite(DHT22_ENABLE_PIN, HIGH); // turn the DHT22 off
    }
    //Serial.println("PMS5003 now on...");
   //delay(40000);
}

// void pms5003_setup(){
//     pmsSerial.begin(9600);
//     delay(500);
// }


void pms5003_shutdown(){
    digitalWrite(PMS5003_EN_PIN, LOW);
    digitalWrite(DHT22_ENABLE_PIN,LOW);
    delay(300); 
}


// void pms5003_WriteToSerial(){
//     if (pms5003_Read(&pmsSerial)) {
//     // reading data was successful!
//     Serial.println();
//     Serial.println("---------------------------------------");
//     Serial.println("Concentration Units (standard)");
//     Serial.print("PM 1.0: "); Serial.print(data.pm10_standard);
//     Serial.print("\t\tPM 2.5: "); Serial.print(data.pm25_standard);
//     Serial.print("\t\tPM 10: "); Serial.println(data.pm100_standard);
//     Serial.println("---------------------------------------");
//     Serial.println("Concentration Units (environmental)");
//     Serial.print("PM 1.0: "); Serial.print(data.pm10_env);
//     Serial.print("\t\tPM 2.5: "); Serial.print(data.pm25_env);
//     Serial.print("\t\tPM 10: "); Serial.println(data.pm100_env);
//     Serial.println("---------------------------------------");
//     Serial.print("Particles > 0.3um / 0.1L air:"); Serial.println(data.particles_03um);
//     Serial.print("Particles > 0.5um / 0.1L air:"); Serial.println(data.particles_05um);
//     Serial.print("Particles > 1.0um / 0.1L air:"); Serial.println(data.particles_10um);
//     Serial.print("Particles > 2.5um / 0.1L air:"); Serial.println(data.particles_25um);
//     Serial.print("Particles > 5.0um / 0.1L air:"); Serial.println(data.particles_50um);
//     Serial.print("Particles > 10.0 um / 0.1L air:"); Serial.println(data.particles_100um);
//     Serial.println("---------------------------------------");
//   }
// }

// boolean pms5003_Read(Stream *s){
//    if (! s->available()) {
//     return false;
//   }
  
//   // Read a byte at a time until we get to the special '0x42' start-byte
//   if (s->peek() != 0x42) {
//     s->read();
//     return false;
//   }
 
//   // Now read all 32 bytes
//   if (s->available() < 32) {
//     return false;
//   }
    
//   uint8_t buffer[32];    
//   uint16_t sum = 0;
//   s->readBytes(buffer, 32);
 
//   // get checksum ready
//   for (uint8_t i=0; i<30; i++) {
//     sum += buffer[i];
//   }
 

//   for (uint8_t i=2; i<32; i++) {
//     Serial.print("0x"); Serial.print(buffer[i], HEX); Serial.print(", ");
//   }
//   Serial.println();

  
//   // The data comes in endian'd, this solves it so it works on all platforms
//   uint16_t buffer_u16[15];
//   for (uint8_t i=0; i<15; i++) {
//     buffer_u16[i] = buffer[2 + i*2 + 1];
//     buffer_u16[i] += (buffer[2 + i*2] << 8);
//   }
 
//   // put it into a nice struct :)
//   memcpy((void *)&data, (void *)buffer_u16, 30);
 
//   if (sum != data.checksum) {
//     Serial.println("Checksum failure");
//     return false;
//   }
//   // success!
//   return true; 
// }