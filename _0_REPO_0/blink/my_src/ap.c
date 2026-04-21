#include "ap.h"


void apInit(void) 
{
    // apTestGpioInit();
    apTestSerialInit();
}

void apLoop(void) 
{
    while(true)
    {
        // apTestGpioLoop();
        apTestSerialLoop();
    }
}