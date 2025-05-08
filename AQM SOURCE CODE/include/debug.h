#include <Arduino.h>

//Debugging definitions
 #define ENABLE_PRINT
#define ENABLE_LORA
#define ENABLE_SD
#define ENABLE_MEASURE
//#define ENABLE_SDI12
//#define ENABLE_SDI12_TESTING
//#define ENABLE_CS655_TESTING
#define ENABLE_LED_DEBUG

//#define ENABLE_DENDRO_TEST

//#define ENABLE_LORA_TEST

#ifdef ENABLE_LORA_TEST
#undef ENABLE_SD
#undef ENABLE_MEASURE
#undef ENABLE_SDI12
#endif

#ifndef ENABLE_PRINT
// disable Serial output
#define Serial SomeOtherwiseUnusedName
static class {
public:
    void begin(...) {}
    void print(...) {}
    void println(...) {}
} Serial;
#endif

void InfiniteStop();