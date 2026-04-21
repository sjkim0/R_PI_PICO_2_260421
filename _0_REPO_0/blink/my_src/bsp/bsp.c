#include "bsp.h"


void bspInit(void)
{

}

uint32_t getTickMs(void)
{
    return to_ms_since_boot(get_absolute_time());
}
