/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "my_src/ap.h"


int main()
{
    // apTestGpioInit();
    apTestSerialInit();

    while (true) 
    {
        // apTestGpioLoop();
        apTestSerialLoop();
    }
}
