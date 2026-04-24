#include "ap.h"


void apInit(void) 
{
    // apTestInit();
    apBtStandaloneServerInit();
    // apBtStandaloneClientInit();
}

void apLoop(void) 
{
    while(true)
    {
        // apTestLoop();
        apBtStandaloneServerLoop();
        // apBtStandaloneClientLoop();
    }
}