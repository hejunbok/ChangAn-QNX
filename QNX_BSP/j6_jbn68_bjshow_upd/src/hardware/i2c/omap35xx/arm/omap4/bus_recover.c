/*
 * $QNXLicenseC:
 * Copyright 2014, QNX Software Systems.
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

#include "proto.h"

/* The I2C bus error recovering function by togglling SCL line to recover
 * the SDA line that has been hold to low by a slave device
 */
int omap_i2c_bus_recover(omap_dev_t *dev) {
	int clk_cnt = 0;

	omap_clock_enable(dev);

	/* Enable system test mode, Loop back mode + SDA/SCL IO mode */
	out16(dev->regbase + OMAP_I2C_SYSTEST,
		in16(dev->regbase + OMAP_I2C_SYSTEST) | OMAP_I2C_SYSTEST_ST_EN | OMAP_I2C_SYSTEST_TMODE(3));

	/* set SCL and SDA to high */
	out16(dev->regbase + OMAP_I2C_SYSTEST,
		in16(dev->regbase + OMAP_I2C_SYSTEST) | OMAP_I2C_SYSTEST_SCL_O | OMAP_I2C_SYSTEST_SDA_O);

	/* check the SDA line; if SDA has been hold to low by a slave device, toggle SCL line */
	while (0 == (in16(dev->regbase + OMAP_I2C_SYSTEST) & OMAP_I2C_SYSTEST_SDA_I))  {
		/* set SCL to high */
		out16(dev->regbase + OMAP_I2C_SYSTEST,
			in16(dev->regbase + OMAP_I2C_SYSTEST) | OMAP_I2C_SYSTEST_SCL_O);
        nanospin_ns(5000);
		/* reset SCL to low */
		out16(dev->regbase + OMAP_I2C_SYSTEST,
			in16(dev->regbase + OMAP_I2C_SYSTEST) & ~OMAP_I2C_SYSTEST_SCL_O);
        nanospin_ns(5000);
        clk_cnt++;

		if (clk_cnt > 20) {		// CAN NOT RECOVER I2C BUS ERROR
			/* set SCL and SDA to high */
			out16(dev->regbase + OMAP_I2C_SYSTEST,
				in16(dev->regbase + OMAP_I2C_SYSTEST) | OMAP_I2C_SYSTEST_SCL_O | OMAP_I2C_SYSTEST_SDA_O);
			/* Disable system test mode and back to Normal mode */
			out16(dev->regbase + OMAP_I2C_SYSTEST,
				in16(dev->regbase + OMAP_I2C_SYSTEST) & ~OMAP_I2C_SYSTEST_ST_EN);
			omap_clock_disable(dev);
			return (-1);
		}
    }

    /* send a STOP signal */
	out16(dev->regbase + OMAP_I2C_SYSTEST,							// reset SDA to low
		in16(dev->regbase + OMAP_I2C_SYSTEST) & ~OMAP_I2C_SYSTEST_SDA_O);
    nanospin_ns(5000);
	out16(dev->regbase + OMAP_I2C_SYSTEST,							// set SCL to high
		in16(dev->regbase + OMAP_I2C_SYSTEST) | OMAP_I2C_SYSTEST_SCL_O);
    nanospin_ns(5000);
	out16(dev->regbase + OMAP_I2C_SYSTEST,							// set SDA to high
		in16(dev->regbase + OMAP_I2C_SYSTEST) | OMAP_I2C_SYSTEST_SDA_O);
    nanospin_ns(5000);
    fprintf(stderr, "omap_i2c_recover: Release SDA line by sending %d SCK and a STOP\n", clk_cnt);

    /* Disable system test mode and back to Normal mode */
	out16(dev->regbase + OMAP_I2C_SYSTEST,
		in16(dev->regbase + OMAP_I2C_SYSTEST) & ~OMAP_I2C_SYSTEST_ST_EN);

	omap_clock_disable(dev);
	return 0;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/i2c/omap35xx/arm/omap4/bus_recover.c $ $Rev: 736699 $")
#endif
