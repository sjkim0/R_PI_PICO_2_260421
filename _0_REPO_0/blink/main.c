/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "my_src/ap.h"


int main()
{
    bspInit();
    apInit();
    apLoop();

    return 0;
}
