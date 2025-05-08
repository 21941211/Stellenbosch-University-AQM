#include "LoRa_Driver.h"

using namespace std;

uint8_t mydata[] = "00002360055620238100230860024460244402462024530251902487000001";

float num = 12.45;

uint8_t newNum[] = "00000000255000628000250000099200600002736002727002750002733002793002766005940";

static osjob_t sendjob;

RTC_DATA_ATTR unsigned int LoRaTX_Complete = 0;
uint8_t NoJoin = 0;




#ifdef ENABLE_SD
 u1_t APPEUI[8];
 u1_t DEVEUI[8];
 u1_t APPKEY[16];
 #endif


void LoRa_Power(){
    pinMode(LORA_EN_PIN, OUTPUT);
    digitalWrite(LORA_EN_PIN, HIGH);
    delay(500);
    Serial.println("LoRa Power On");
}

void LoRa_Power_Off(){
    pinMode(LORA_EN_PIN, OUTPUT);
    digitalWrite(LORA_EN_PIN, LOW);
    delay(500);
    Serial.println("LoRa Power Off");
}

void LoRaSetup()
{

LoRa_Power();
 #ifdef ENABLE_SD
    readLastEntry();
 #else
    Serial.println("SD Card Disabled, using default LoRa parameters");

 #endif

Serial.println("Setting up LoRa");

    setSPI(LORA_SPI);

    // LMIC init
    os_init();
     delay(200);
    Serial.println("OS Init Complete");
    //  Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();
     delay(200);
    Serial.println("LMIC_reset complete");
    //  Start job (sending automatically starts OTAA too)
    do_send(&sendjob);
     delay(200);
    Serial.println("do_send complete");
    Serial.println("******************************************************");
}

void printHex2(unsigned v)
{
    v &= 0xff;
    if (v < 16)
        Serial.print('0');
    Serial.print(v, HEX);
}

void onEvent(ev_t ev)
{
    Serial.print(os_getTime());
    Serial.print(": ");
    switch (ev)
    {
    case EV_SCAN_TIMEOUT:
        Serial.println(F("EV_SCAN_TIMEOUT"));
        break;
    case EV_BEACON_FOUND:
        Serial.println(F("EV_BEACON_FOUND"));
        break;
    case EV_BEACON_MISSED:
        Serial.println(F("EV_BEACON_MISSED"));
        break;
    case EV_BEACON_TRACKED:
        Serial.println(F("EV_BEACON_TRACKED"));
        break;
    case EV_JOINING:
        Serial.println(F("EV_JOINING"));
        break;
    case EV_JOINED:
        Serial.println(F("EV_JOINED"));
        {
            u4_t netid = 0;
            devaddr_t devaddr = 0;
            u1_t nwkKey[16];
            u1_t artKey[16];
            LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
            Serial.print("netid: ");
            Serial.println(netid, DEC);
            Serial.print("devaddr: ");
            Serial.println(devaddr, HEX);
            Serial.print("AppSKey: ");
            for (size_t i = 0; i < sizeof(artKey); ++i)
            {
                if (i != 0)
                    Serial.print("-");
                printHex2(artKey[i]);
            }
            Serial.println("");
            Serial.print("NwkSKey: ");
            for (size_t i = 0; i < sizeof(nwkKey); ++i)
            {
                if (i != 0)
                    Serial.print("-");
                printHex2(nwkKey[i]);
            }
            Serial.println();
        }
        // Disable link check validation (automatically enabled
        // during join, but because slow data rates change max TX
        // size, we don't use it in this example.
        LMIC_setLinkCheckMode(0);
        break;
    /*
    || This event is defined but not used in the code. No
    || point in wasting codespace on it.
    ||
    || case EV_RFU1:
    ||     Serial.println(F("EV_RFU1"));
    ||     break;
    */
    case EV_JOIN_FAILED:
        Serial.println(F("EV_JOIN_FAILED"));
        break;
    case EV_REJOIN_FAILED:
        Serial.println(F("EV_REJOIN_FAILED"));
        break;
    case EV_TXCOMPLETE:
        LoRaTX_Complete = 1;
        Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
        if (LMIC.txrxFlags & TXRX_ACK)
            Serial.println(F("Received ack"));
        if (LMIC.dataLen)
        {
            Serial.print("Received ");
            Serial.print(LMIC.dataLen);
            Serial.println(" bytes of payload");
        }
        // Schedule next transmission
        // os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), do_send);
        MEASURE_COMPLETE = 0;

        LMIC_shutdown();
        delay(1000);
            
            goSleep(DEEP_SLEEP);
        break;
    case EV_LOST_TSYNC:
        Serial.println(F("EV_LOST_TSYNC"));
        break;
    case EV_RESET:
        Serial.println(F("EV_RESET"));
        break;
    case EV_RXCOMPLETE:
        // data received in ping slot
        Serial.println(F("EV_RXCOMPLETE"));
        break;
    case EV_LINK_DEAD:
        Serial.println(F("EV_LINK_DEAD"));
        break;
    case EV_LINK_ALIVE:
        Serial.println(F("EV_LINK_ALIVE"));
        break;
    /*
    || This event is defined but not used in the code. No
    || point in wasting codespace on it.
    ||
    || case EV_SCAN_FOUND:
    ||    Serial.println(F("EV_SCAN_FOUND"));
    ||    break;
    */
    case EV_TXSTART:
        Serial.println(F("EV_TXSTART"));
        break;
    case EV_TXCANCELED:
        Serial.println(F("EV_TXCANCELED"));
        break;
    case EV_RXSTART:
        /* do not print anything -- it wrecks timing */
        break;
    case EV_JOIN_TXCOMPLETE:
        Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));

        LMIC_shutdown();
        LoRa_Power_Off();
        delay(1000);
        goSleep(LIGHT_SLEEP);
        break;

    default:
        Serial.print(F("Unknown event: "));
        Serial.println((unsigned)ev);
        break;
    }
}

void do_send(osjob_t *j)
{

    // setSPI(LORA_SPI);
    //  Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND)
    {
        Serial.println(F("OP_TXRXPEND, not sending"));
    }
    else
    {
        // Prepare upstream data transmission at the next possible time.
        // #ifdef ENABLE_LORA_TEST
        //  LMIC_setTxData2(1, mydata,  sizeof(mydata), 0); //dataBuffer

        //  #elseif defined(ENABLE_SD)
        LMIC_setTxData2(1, PayLoadTest.data(), PayLoadTest.size(), 0); // dataBuffer
        // #elseif
        // // LMIC_setTxData2(1, mydata, sizeof(mydata), 0); // dataBuffer
        // #endif
        Serial.println(F("Packet queued"));
    }
    // Next TX is scheduled after TX_COMPLETE event.
}

// For normal use, we require that you edit the sketch to replace FILLMEIN
// with values assigned by the TTN console. However, for regression tests,
// we want to be able to compile these scripts. The regression tests define
// COMPILE_REGRESSION_TEST, and in that case we define FILLMEIN to a non-
// working but innocuous value.
//
#ifdef COMPILE_REGRESSION_TEST
#define FILLMEIN 0
#else
#warning "You must replace the values marked FILLMEIN with real values from the TTN control panel!"
#define FILLMEIN (#dont edit this, edit the lines that use FILLMEIN)
#endif

// lsb/lsb/msb
#ifdef NODE_1
static const u1_t PROGMEM APPEUI[8] = {0x5E, 0x36, 0x2A, 0xFF, 0xFF, 0x40, 0xEE, 0xC0};
static const u1_t PROGMEM DEVEUI[8] = {0x5B, 0x3A, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70};
static const u1_t PROGMEM APPKEY[16] = {0xD6, 0x1D, 0x1F, 0x44, 0xA0, 0xC3, 0x15, 0x03, 0xDB, 0xA0, 0x67, 0x0D, 0x13, 0xE9, 0xAA, 0x24};
#elif defined(NODE_2)
static const u1_t PROGMEM APPEUI[8] = {0x5E, 0x36, 0x2A, 0xFF, 0xFF, 0x40, 0xEE, 0xC0};
static const u1_t PROGMEM DEVEUI[8] = {0x69, 0x7D, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70};
static const u1_t PROGMEM APPKEY[16] = {0x19, 0xEE, 0x0F, 0xEB, 0x7B, 0xBA, 0xCF, 0xBA, 0xDD, 0x2A, 0x49, 0xEB, 0x8B, 0x49, 0x79, 0xC2};
#elif defined(NODE_3)
static const u1_t PROGMEM APPEUI[8] = {0x5E, 0x36, 0x2A, 0xFF, 0xFF, 0x40, 0xEE, 0xC0};
static const u1_t PROGMEM DEVEUI[8] = {0x6A, 0x7D, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70};
static const u1_t PROGMEM APPKEY[16] = {0xCC, 0x2E, 0xAC, 0x49, 0xAB, 0x57, 0x80, 0xF7, 0xE4, 0x55, 0xF0, 0x22, 0xB5, 0xF6, 0x78, 0x78};
#elif defined(NODE_4)
static const u1_t PROGMEM APPEUI[8] = {0x5E, 0x36, 0x2A, 0xFF, 0xFF, 0x40, 0xEE, 0xC0};
static const u1_t PROGMEM DEVEUI[8] = {0x6B, 0x7D, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70};
static const u1_t PROGMEM APPKEY[16] = {0x0E, 0x2F, 0x74, 0x6E, 0xCA, 0xAB, 0xF2, 0x62, 0x22, 0x33, 0x43, 0x23, 0xB5, 0xAC, 0x43, 0x44};
#elif defined(NODE_5)
static const u1_t PROGMEM APPEUI[8] = {0x5E, 0x36, 0x2A, 0xFF, 0xFF, 0x40, 0xEE, 0xC0};
static const u1_t PROGMEM DEVEUI[8] = {0x6C, 0x7D, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70};
static const u1_t PROGMEM APPKEY[16] = {0xD8, 0x5A, 0xDF, 0x2C, 0xFE, 0xCF, 0x62, 0x8A, 0x2B, 0x17, 0x5E, 0x1F, 0xF7, 0x9B, 0xB9, 0xA0};
#elif defined(NODE_6)
static const u1_t PROGMEM APPEUI[8] = {0x5E, 0x36, 0x2A, 0xFF, 0xFF, 0x40, 0xEE, 0xC0};
static const u1_t PROGMEM DEVEUI[8] = {0x6D, 0x7D, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70};
static const u1_t PROGMEM APPKEY[16] = {0x4F, 0xB7, 0x27, 0xFE, 0xE2, 0xE3, 0x05, 0x84, 0x4D, 0x7F, 0x13, 0xF7, 0x15, 0x10, 0xFC, 0x5F};
#elif defined(NODE_7)
static const u1_t PROGMEM APPEUI[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const u1_t PROGMEM DEVEUI[8] = {0x34, 0x87, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70};
static const u1_t PROGMEM APPKEY[16] = {0x43, 0x62, 0xC0, 0xB2, 0xEC, 0x4E, 0xAA, 0xA2, 0x6F, 0xA3, 0xC5, 0x56, 0x76, 0x21, 0x10, 0x24};
#elif defined(NODE_8)
static const u1_t PROGMEM APPEUI[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const u1_t PROGMEM DEVEUI[8] = {0x33, 0x87, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70};
static const u1_t PROGMEM APPKEY[16] = {0x43, 0x62, 0xC0, 0xB2, 0xEC, 0x4E, 0xAA, 0xA2, 0x6F, 0xA3, 0xC5, 0x56, 0x76, 0x21, 0x10, 0x24};
#elif defined(NODE_9)
static const u1_t PROGMEM APPEUI[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const u1_t PROGMEM DEVEUI[8] = {0x32, 0x87, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70};
static const u1_t PROGMEM APPKEY[16] = {0x43, 0x62, 0xC0, 0xB2, 0xEC, 0x4E, 0xAA, 0xA2, 0x6F, 0xA3, 0xC5, 0x56, 0x76, 0x21, 0x10, 0x24};
#elif defined(NODE_10)
static const u1_t PROGMEM APPEUI[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const u1_t PROGMEM DEVEUI[8] = {0x31, 0x87, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70};
static const u1_t PROGMEM APPKEY[16] = {0x43, 0x62, 0xC0, 0xB2, 0xEC, 0x4E, 0xAA, 0xA2, 0x6F, 0xA3, 0xC5, 0x56, 0x76, 0x21, 0x10, 0x24};
#elif defined(NODE_11)
static const u1_t PROGMEM APPEUI[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const u1_t PROGMEM DEVEUI[8] = {0x88, 0x9B, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70};
static const u1_t PROGMEM APPKEY[16] = {0x50, 0x65, 0xA8, 0x37, 0x4D, 0x63, 0xED, 0xB4, 0x4C, 0xCE, 0x7D, 0xE2, 0xED, 0xC3, 0x1C, 0x40};
#else
//#error "No node assigned"
#endif

void os_getArtEui(u1_t *buf) { memcpy_P(buf, APPEUI, 8); }
void os_getDevEui(u1_t *buf) { memcpy_P(buf, DEVEUI, 8); }
void os_getDevKey(u1_t *buf) { memcpy_P(buf, APPKEY, 16); }

// Pin mapping
const lmic_pinmap lmic_pins = {
    .nss = LORA_CS_PIN,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = LORA_RST_PIN,
    .dio = {LORA_DIO0_PIN, LORA_DIO1_PIN, LMIC_UNUSED_PIN},
};
