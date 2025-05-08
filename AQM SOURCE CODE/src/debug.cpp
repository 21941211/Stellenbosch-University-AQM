#include "debug.h"

void InfiniteStop(){
    Serial.println("Infinite loop");
    while(1);
}