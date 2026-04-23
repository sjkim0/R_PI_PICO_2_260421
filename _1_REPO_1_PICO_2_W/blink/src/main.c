/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ap.h"
#include "bsp.h"

int main() 
{
    bspInit();
    apInit();
    
    apLoop();
}
