/*
 * $QNXLicenseC:
 * Copyright 2013, 2014 QNX Software Systems.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

#include "dra74x_startup.h"

/*****************************************************************
 * Create a delay function from an ARM counter
 *****************************************************************/


void usec_delay(unsigned usec)
{

    unsigned long long time_start = get_timer_value();
    unsigned diff;
    unsigned long long dly = (usec * 6144 + 999) / 1000;

    for (;;) {
        diff = get_timer_value() - time_start;
        if ((unsigned)diff >= dly)
            break;
    }
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/dra74x/common.c $ $Rev: 763399 $")
#endif
