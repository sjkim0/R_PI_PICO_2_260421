#include "ap.h"


void apInit(void) 
{
    // apTestGpioInit();
    apTestSerialInit();
    apAdcInit();
}

void apLoop(void) 
{
    while(true)
    {
        // apTestGpioLoop();
        apTestSerialLoop();
        apAdcLoop();
    }
}