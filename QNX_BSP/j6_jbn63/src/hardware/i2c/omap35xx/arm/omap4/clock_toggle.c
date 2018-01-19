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

#include <stdio.h>
#include <string.h>

#include "proto.h"
#include "clock_toggle.h"
#include "context_restore.h"

/* We use the enable_count to allow nesting of enables/disables */
static int enable_count = 0;

static const unsigned sleep_slice_us = 1000; /* 1ms */
static const unsigned max_sleep_us = 100000; /* 100ms */

static int
poll_for_condition(uintptr_t addr, uint_t mask, uint_t desired_val)
{
	uint32_t val;
	unsigned slept_us = 0;

	val = (in32(addr) & mask);
	while (val != desired_val) {
		if (0 == usleep(sleep_slice_us)) {
			slept_us += sleep_slice_us;

			if (slept_us >= max_sleep_us) {
				return -1;
			}
		}
		val = (in32(addr) & mask);
	}

	return 0;
}

void
omap_clock_enable(omap_dev_t* dev)
{
	uint32_t val;
	uint32_t timeout = 0;

	// Skip power management if specify, this means clocks are enable
	// by default.
	if (dev->no_powmgm)
		return;
		
	if (dev->clkctrl_base) {
		if (enable_count) {
			enable_count += 1;
			return;
		}

		/* Enable the clock */
		out32(dev->clkctrl_base, OMAP_CLKCTRL_MODMODE_ENABLE);

		/* Wait for the module mode to have been written */
		if (-1 == poll_for_condition(dev->clkctrl_base, OMAP_CLKCTRL_MODMODE_MASK, OMAP_CLKCTRL_MODMODE_ENABLE)) {
			fprintf(stderr, "%s(0x%08x): Failed to set module mode to 'enabled'\n", __FUNCTION__, dev->clkctrl_phys);
			timeout++;
		}

		/* Wait for the module idle status to report "fully functional" */
		if (-1 == poll_for_condition(dev->clkctrl_base, OMAP_CLKCTRL_IDLEST_MASK, OMAP_CLKCTRL_IDLEST_FUNCTIONAL)) {
			fprintf(stderr, "%s(0x%08x): Module failed to report 'fully functional'\n", __FUNCTION__, dev->clkctrl_phys);
			timeout++;
		}

		/* Set the idle mode to "smart" */
		val = (OMAP_I2C_SYSC_CLKACTIVITY_KEEP_SYSCLK | OMAP_I2C_SYSC_IDLEMODE_SMART_WKUP | OMAP_I2C_SYSC_ENWAKEUP | OMAP_I2C_SYSC_AUTOIDLE_ENABLE);
		out32(dev->regbase + OMAP_I2C_SYSC, val);

		context_restore(dev);
		enable_count += 1;
		if( !timeout ) {
			dev->clkctrl_disabled = 0;
			InterruptUnmask(dev->intr, dev->iid);
		}
	}
}

void
omap_clock_disable(omap_dev_t* dev)
{
	uint32_t val;

	// Skip power management if specify, this means clocks are enable
	// by default.
	if (dev->no_powmgm)
		return;

	if (dev->clkctrl_base) {
		if (0 == enable_count) {
			fprintf(stderr, "Calling omap_clock_disable without prior call to omap_clock_enable\n");
			return;
		}

		enable_count -= 1;
		if (enable_count) {
			return;
		}
		dev->clkctrl_disabled = 1;
		InterruptMask(dev->intr, dev->iid);

                /* assume that the context will be lost, so save it here */
		context_restore_save(dev);

		/* Set the idle mode to "force" */
		val = (OMAP_I2C_SYSC_CLKACTIVITY_ALLOW_CUTOFF | OMAP_I2C_SYSC_IDLEMODE_FORCE | OMAP_I2C_SYSC_AUTOIDLE_ENABLE);
		out32(dev->regbase + OMAP_I2C_SYSC, val);

		/* Disable the clock */
		out32(dev->clkctrl_base, OMAP_CLKCTRL_MODMODE_DISABLE);

		/* Wait for the module mode to have been written */
		if (-1 == poll_for_condition(dev->clkctrl_base, OMAP_CLKCTRL_MODMODE_MASK, OMAP_CLKCTRL_MODMODE_DISABLE)) {
			fprintf(stderr, "%s(0x%08x): Failed to set module mode to 'disabled'\n", __FUNCTION__, dev->clkctrl_phys);
		}
	}
}

int
omap_clock_toggle_init(omap_dev_t* dev)
{
	dev->clkctrl_disabled = 0;
	dev->clkctrl_base = mmap_device_io(4, dev->clkctrl_phys);
	if (dev->clkctrl_base == (uintptr_t)MAP_FAILED) {
		fprintf(stderr, "%s(0x%08x): mmap_device_io: %s\n", __FUNCTION__, dev->clkctrl_phys, strerror(errno));
		goto fail1;
	}

	omap_clock_enable(dev);
	out32(dev->regbase + OMAP_I2C_WE, OMAP_I2C_WE_ALL);
	omap_clock_disable(dev);

	return 0;

fail1:
	return -1;

}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/i2c/omap35xx/arm/omap4/clock_toggle.c $ $Rev: 736699 $")
#endif
