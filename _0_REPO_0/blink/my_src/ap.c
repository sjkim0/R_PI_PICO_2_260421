#include "ap.h"


void apInit(void) 
{
    // apTestGpioInit();
    apTestSerialInit();
    // apAdcInit();
    apAdcDmaInit();
}

void apLoop(void) 
{
    while(true)
    {
        // apTestGpioLoop();
        apTestSerialLoop();
        // apAdcLoop();
        apAdcDmaLoop();
    }
}