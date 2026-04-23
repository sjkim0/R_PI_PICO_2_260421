#include "ap.h"


void apInit(void) 
{
    // apTestInit();
    apBtStandaloneServerInit();
}

void apLoop(void) 
{
    while(true)
    {
        // apTestLoop();
        apBtStandaloneServerLoop();
    }
}