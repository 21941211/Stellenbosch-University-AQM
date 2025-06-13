#include "SD_Driver.h"
#include <vector>
#include <algorithm>

const byte key[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10};




// Device ID in HEX
String deviceIDHex;

uint8_t endOfFirstPayload = 0;
uint8_t startOfSDI12Payload = 0;
byte dataRead;
uint8_t dataBuffer[];
std::vector<uint8_t> LoRaBuffer;
std::vector<uint8_t> PayLoadTest;
uint8_t LoRaBuffer_SDI12[13] = {0};

// Converts NODE_NUMBER to a string
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#ifdef ENABLE_DENDRO_TEST
#define FILE_NAME "/Measurements.csv"
#else
#define FILE_NAME "/Measurements.csv"
#endif
#define FILE_NAME_SDI12_60 "/SDI_12_60cm_Measurements.csv"
#define FILE_NAME_SDI12_90 "/SDI_12_90cm_Measurements.csv"
#define FILE_NAME_CS655 "/SDI_12_CS655_Measurements.csv"
#define FILE_NAME_DEBUG "/Debug.csv"
#define SYSPARAMS "/parameters.txt"


 const char *fileName = FILE_NAME;
const char *fileName_SDI12;


char *paramFile = SYSPARAMS;

void reverseByteOrder(u1_t arr[8]) {
    std::reverse(arr, arr + 8);
}

void parseHexArray(String hexString, uint8_t *outputArray, int length) {
  Serial.print("Parsing hex string: ");
  Serial.println(hexString);

  hexString.trim(); // Remove extra spaces

  if (hexString.length() != length * 2) {
    Serial.println("Error: Hex string length mismatch!");
    return;
  }

  for (int i = 0; i < length; i++) {
    outputArray[i] = strtol(hexString.substring(i * 2, i * 2 + 2).c_str(), NULL, 16);
  }

}




bool readParametersFromFile(const char *path) {
  File file = SD.open(path);
  if (!file) {
    Serial.println("Failed to open file.");
    return false;
  }

Serial.println("File opened");

 while (file.available()) {
    String line = file.readStringUntil('\n');
    line.trim();

     if (line.startsWith("APPEUI=")) {
      
      parseHexArray(line.substring(7), APPEUI, 8);
    } else if (line.startsWith("DEVEUI=")) {
      parseHexArray(line.substring(7), DEVEUI, 8);
    } else if (line.startsWith("APPKEY=")) {
      parseHexArray(line.substring(7), APPKEY, 16);
    } else if (line.startsWith("DEVID=")) {
      String deviceIDStr = line.substring(6); // Extract value after "DEVID="
    deviceIDStr.trim(); // Remove any whitespace or newline characters
    Serial.print("Device ID (DEC): ");
    Serial.println(deviceIDStr);
    if(deviceIDStr=="5"){
      devID5 = true;
    }
    if (deviceIDStr.length() == 0 || deviceIDStr.toInt() <= 0 || deviceIDStr.toInt() > 255) {
      Serial.println("Error: Invalid or missing DEVID. Using default ID 1.");
      deviceIDStr = "1"; // Default value
    }
    int deviceID = deviceIDStr.toInt();
    deviceIDHex = String(deviceID, HEX);
    if (deviceIDHex.length() < 2) {
      deviceIDHex = "0" + deviceIDHex; // Add leading zero if necessary
    }
    deviceIDHex.toUpperCase();
    Serial.print("Device ID (HEX): ");
    Serial.println(deviceIDHex);
  }
 }

  
  
  file.close();



reverseByteOrder(DEVEUI);

  return true;
}


void writeToDebug(const char *message)
{
  Serial.println("##################################################");
  Serial.print("Writing to debug file: ");

  enableSD_ON();
  setSPI(SD_SPI);
  delay(200);


  while(!SD.begin(SD_CS_PIN, SPI, 80000000))
  {
    Serial.println("Card Mount Failed");

    for (size_t i = 0; i < 2; i++)
    {
    digitalWrite( DEBUG_LED_PIN, HIGH);
    delay(20);
    digitalWrite(DEBUG_LED_PIN,LOW);
    delay(20);
    }
    delay(1000);
  }
 
uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE)
  {
    Serial.println("No SD card attached");
    return;
  }

if(!SD.exists(FILE_NAME_DEBUG))
  {
    Serial.println("Debug file does not exist, creating it now");
    writeFile(SD, FILE_NAME_DEBUG, "Debug file created\n");
  }

  File debugFile = SD.open(FILE_NAME_DEBUG, FILE_APPEND);
  if (!debugFile)
  {
    Serial.println("Failed to open debug file for writing");
    return;
  }
  if (debugFile.println(String(message) + ", Bootcount: " + String(bootCount)))
  {
    // Print a message to the serial monitor to indicate that the debug message was written successfully
    Serial.print("Debug message written: ");
    Serial.println(message + String(", Bootcount: ") + String(bootCount));
  }
  else
  {
    Serial.println("Write to debug file failed");
  }
  debugFile.close();
  enableSD_OFF();
   Serial.println("##################################################");
}

void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root)
  {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory())
  {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file)
  {
    if (file.isDirectory())
    {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels)
      {
        listDir(fs, file.name(), levels - 1);
      }
    }
    else
    {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void createDir(fs::FS &fs, const char *path)
{
  Serial.printf("Creating Dir: %s\n", path);
  if (fs.mkdir(path))
  {
    Serial.println("Dir created");
  }
  else
  {
    Serial.println("mkdir failed");
  }
}

void removeDir(fs::FS &fs, const char *path)
{
  Serial.printf("Removing Dir: %s\n", path);
  if (fs.rmdir(path))
  {
    Serial.println("Dir removed");
  }
  else
  {
    Serial.println("rmdir failed");
  }
}

void readFile(fs::FS &fs, const char *path, uint8_t NormalOrSDI12)
{

  uint8_t newLineFound = 0;
  uint8_t newLinePos = 0;

  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file)
  {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.println("Read from file: ");
  file.seek(file.size());

  while (!newLineFound)
  {
    file.seek(file.size());
    file.seek(file.position() - newLinePos);
    dataRead = file.read();
    newLinePos++;
    // Serial.println(newLinePos);
    if (dataRead == '\n')
    {
      newLineFound++;
    }
  }

  file.seek(file.size());
  file.seek(file.position() - newLinePos + 2);

  uint8_t index = 0;
  if (NormalOrSDI12 == 1)
  {


    index = LoRaBuffer.size();
    startOfSDI12Payload = LoRaBuffer.size();
  }

  while (file.available())
  {
    int temp;
    int currentSize = LoRaBuffer.size();

    temp = file.read() - 48;
    if (temp >= 0)
    {
      //Serial.print("LoRa buffer current size: ");
      //Serial.println(currentSize);
      LoRaBuffer.resize(currentSize + 1);
      LoRaBuffer[index] = temp;
      index++;
    }
  }

  Serial.println("");

  Serial.print("LoRa buffer size: ");
  Serial.println(LoRaBuffer.size());

  Serial.print("LoRa buffer: ");

  for (uint8_t i = 0; i < LoRaBuffer.size(); i++)
  {
    Serial.print(LoRaBuffer[i]);
  }

  Serial.println("");
  file.close();

  if (NormalOrSDI12 == 0)
  {
    int payLoadIndex = 0;


int counter = 0;
   while(counter < LoRaBuffer.size()-1){
      //Serial.println(counter);
        PayLoadTest.resize(PayLoadTest.size() + 1);
        PayLoadTest[payLoadIndex] = LoRaBuffer[counter]*10+LoRaBuffer[counter+1];
        counter= counter+2;
        payLoadIndex++;
    }
  }
else {

int counter = startOfSDI12Payload;
int payLoadIndex = PayLoadTest.size();

  while(counter < LoRaBuffer.size()){
    //  Serial.println(counter);
        PayLoadTest.resize(PayLoadTest.size() + 1);
        PayLoadTest[payLoadIndex] = LoRaBuffer[counter]*10+LoRaBuffer[counter+1];
        counter= counter+2;
        payLoadIndex++;
  }

    // endOfFirstPayload = PayLoadTest.size();
    // Serial.print("End of first payload: ");
    // Serial.println(endOfFirstPayload);
    // Serial.print("End of first LoRa payload: ");
    // Serial.println(endOfFirstLoRaPayload);

// int counter = endOfFirstLoRaPayload;
// int payLoadIndex = endOfFirstPayload;
//       while(counter < LoRaBuffer.size()-1){
//       Serial.println(counter);
//         PayLoadTest.resize(PayLoadTest.size() + 1);
//         PayLoadTest[payLoadIndex] = LoRaBuffer[counter];
//         //PayLoadTest[payLoadIndex] = LoRaBuffer[counter]*10+LoRaBuffer[counter+1];
//         // counter= counter+2;
//         counter++;
//         payLoadIndex++;
//     }
  }

  Serial.print("PayLoadTest size: ");
  Serial.println(PayLoadTest.size());

  Serial.print("Payload: ");
  for (int i = 0; i < PayLoadTest.size(); i++)
  {
    Serial.print(PayLoadTest[i]);
  }
  Serial.println("");
}


void writeFile(fs::FS &fs, const char *path, const char *message)
{
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file)
  {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message))
  {
    Serial.println("File written");
  }
  else
  {
    Serial.println("Write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char *path, const char *message)
{
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file)
  {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message))
  {
    Serial.println("Message appended");
  }
  else
  {
    Serial.println("Append failed");
  }
  file.close();
}

void renameFile(fs::FS &fs, const char *path1, const char *path2)
{
  Serial.printf("Renaming file %s to %s\n", path1, path2);
  if (fs.rename(path1, path2))
  {
    Serial.println("File renamed");
  }
  else
  {
    Serial.println("Rename failed");
  }
}

void deleteFile(fs::FS &fs, const char *path)
{
  Serial.printf("Deleting file: %s\n", path);
  if (fs.remove(path))
  {
    Serial.println("File deleted");
  }
  else
  {
    Serial.println("Delete failed");
  }
}

void testFileIO(fs::FS &fs, const char *path)
{
  File file = fs.open(path);
  static uint8_t buf[512];
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;
  if (file)
  {
    len = file.size();
    size_t flen = len;
    start = millis();
    while (len)
    {
      size_t toRead = len;
      if (toRead > 512)
      {
        toRead = 512;
      }
      file.read(buf, toRead);
      len -= toRead;
    }
    end = millis() - start;
    Serial.printf("%u bytes read for %u ms\n", flen, end);
    file.close();
  }
  else
  {
    Serial.println("Failed to open file for reading");
  }

  file = fs.open(path, FILE_WRITE);
  if (!file)
  {
    Serial.println("Failed to open file for writing");
    return;
  }

  size_t i;
  start = millis();
  for (i = 0; i < 2048; i++)
  {
    file.write(buf, 512);
  }
  end = millis() - start;
  Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
  file.close();
}

void SDSetup()
{
  enableSD_ON();
  setSPI(SD_SPI);
  delay(200);

int LEDFlash = 0;
  while(!SD.begin(SD_CS_PIN, SPI, 80000000)&&LEDFlash<5)
  {
    Serial.println("Card Mount Failed");

    for (size_t i = 0; i < 2; i++)
    {
    digitalWrite( DEBUG_LED_PIN, HIGH);
    delay(20);
    digitalWrite(DEBUG_LED_PIN,LOW);
    delay(20);
    }
    LEDFlash++;
    delay(1000);
    
  }
  
if (LEDFlash ==5)
{
  Serial.println("SD connect timeout reached");
  Serial.println("Going to sleep for 10 seconds to reset");
  goSleep(LIGHT_SLEEP);
}


  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE)
  {
    Serial.println("No SD card attached");
    return;
  }

Serial.println("SD Card mounted successfully");
Serial.println("The following files are available:");
listDir(SD, "/", 0);

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  Serial.println("");

    // Read parameters from file
Serial.println("Reading loRa keys from file");
  if (!readParametersFromFile(paramFile)) {
    Serial.println("Failed to read parameters from file.");
    while (1);
  }


 
  if (!SD.exists(fileName))
  {
    writeFile(SD, fileName, "Air Temperature,Air Humidity,PM1.0,PM2.5,PM10.0,Battery %,Boot count\n");
  }

  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));


  //enableSD_OFF();

   Serial.println("SD Setup complete");
Serial.println("******************************************************");
}

void writeToSD()
{

  uint8_t buffSize = 8;

  char temp_buffer[buffSize];
  char hum_buffer[buffSize];
  char pms1_0_buffer[buffSize];
  char pms2_5_buffer[buffSize];
  char pms10_0_buffer[buffSize];
  char battBuffer[buffSize];
  char BootCountBuffer[buffSize];


Serial.println("Checking data buffers");
  dataToBuff(temp_buffer, tempMedian, buffSize);
  dataToBuff(hum_buffer, humMedian, buffSize);
  dataToBuff16(pms1_0_buffer, PM1_0_avg, buffSize);
  dataToBuff16(pms2_5_buffer, PM2_5_avg, buffSize);
  dataToBuff16(pms10_0_buffer, PM10_0_avg, buffSize);
  dataToBuff(battBuffer, batPercentage, buffSize);
  dataToBuff(BootCountBuffer, float(bootCount)/100.0, buffSize);

 

   Serial.println("******************************************************");


  SDSetup();

  appendFile(SD, fileName, "\n");
  appendFile(SD, fileName, temp_buffer);
  appendFile(SD, fileName, ",");
  appendFile(SD, fileName, hum_buffer);
  appendFile(SD, fileName, ",");
  appendFile(SD, fileName, pms1_0_buffer);
  appendFile(SD, fileName, ",");
  appendFile(SD, fileName, pms2_5_buffer);
  appendFile(SD, fileName, ",");
  appendFile(SD, fileName, pms10_0_buffer);
  appendFile(SD, fileName, ",");
  appendFile(SD, fileName, battBuffer);
  appendFile(SD, fileName, ",");
  appendFile(SD, fileName, BootCountBuffer);
  appendFile(SD, fileName, ",");
  appendFile(SD, fileName, ":");
   

 



  enableSD_OFF();
}

void readLastEntry()
{
  #ifdef ENABLE_SD
  SDSetup();

  readFile(SD, fileName, 0);

#endif
  enableSD_OFF();
}

void enableSD_ON()
{

  digitalWrite(SD_ENABLE_PIN, LOW);
delay(300);
  digitalWrite(SD_ENABLE_PIN, HIGH);
 digitalWrite(LORA_CS_PIN, HIGH); // SET LoRa CS pin HIGH
  delay(300);
}

void enableSD_OFF()
{
  digitalWrite(SD_ENABLE_PIN, LOW);
  digitalWrite(LORA_CS_PIN, LOW); // turn the LED on (HIGH is the voltage level)
  delay(100);
}


void decodePayload() {
    Serial.println("Decoding payload as seen by TTN");

    int i = 0;
    while (i <= PayLoadTest.size() - 3) {  // Ensure we don't go out of bounds
        float value = float(PayLoadTest[i] * 100.0) + 
                      float(PayLoadTest[i + 1]) * 1.0 + 
                      float(PayLoadTest[i + 2] * 0.01);

        Serial.print(value);
        Serial.print(",");

        i += 3;
    }

    Serial.println("");  // New line after printing all values
}


