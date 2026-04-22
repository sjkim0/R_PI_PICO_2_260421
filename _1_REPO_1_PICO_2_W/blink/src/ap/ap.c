#include "ap.h"


void apInit(void) 
{
    apTestInit();
}

void apLoop(void) 
{
    while(true)
    {
        apTestLoop();
    }
}