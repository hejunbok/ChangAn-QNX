/*
 * $QNXLicenseC:
 * Copyright 2013, QNX Software Systems.
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

#include "omap4430spi.h"
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
omap_clock_enable(omap4430_spi_t* dev)
{
	uint32_t val;

	if (dev->clkctrlvbase) {
		if (enable_count) {
			enable_count += 1;
			return;
		}

		/* Enable the clock module: explicit enable mode */
		out32(dev->clkctrlvbase, OMAP_CLKCTRL_MODMODE_ENABLE);

		/* Wait for the module mode to have been written */
		if (-1 == poll_for_condition(dev->clkctrlvbase, OMAP_CLKCTRL_MODMODE_MASK, OMAP_CLKCTRL_MODMODE_ENABLE)) {
			fprintf(stderr, "%s: Failed to set module mode to 'enabled'\n", __FUNCTION__);
		}

		/* Wait for the module idle status to report "fully functional" */
		if (-1 == poll_for_condition(dev->clkctrlvbase, OMAP_CLKCTRL_IDLEST_MASK, OMAP_CLKCTRL_IDLEST_FUNCTIONAL)) {
			fprintf(stderr, "%s: Module failed to report 'fully functional'\n", __FUNCTION__);
		}

		/* Set the idle mode to "smart" */
		val = (OMAP_SPI_SYSC_CLKACTIVITY_ALLOW_CUTOFF | OMAP_SPI_SYSC_IDLEMODE_NO | OMAP_SPI_SYSC_AUTOIDLE_ENABLE);
		out32(dev->vbase + OMAP4430_MCSPI_SYS_CONFIG, val);

		context_restore(dev);

		enable_count += 1;
	}
}

void
omap_clock_disable(omap4430_spi_t* dev)
{
	uint32_t val;

	if (dev->clkctrlvbase) {
		if (0 == enable_count) {
			fprintf(stderr, "Calling omap_clock_disable without prior call to omap_clock_enable\n");
			return;
		}

		enable_count -= 1;
		if (enable_count) {
			return;
		}

		/* Set the idle mode to "force" */
		val = (OMAP_SPI_SYSC_CLKACTIVITY_ALLOW_CUTOFF | OMAP_SPI_SYSC_IDLEMODE_FORCE | OMAP_SPI_SYSC_AUTOIDLE_ENABLE);
		out32(dev->vbase + OMAP4430_MCSPI_SYS_CONFIG, val);

		/* Disable the clock */
		out32(dev->clkctrlvbase, OMAP_CLKCTRL_MODMODE_DISABLE);

		/* Wait for the module mode to have been written */
		if (-1 == poll_for_condition(dev->clkctrlvbase, OMAP_CLKCTRL_MODMODE_MASK, OMAP_CLKCTRL_MODMODE_DISABLE)) {
			fprintf(stderr, "%s: Failed to set module mode to 'disabled'\n", __FUNCTION__);
		}
	}
}

int
omap_clock_toggle_init(omap4430_spi_t* dev)
{
	dev->clkctrlvbase = mmap_device_io(4, dev->clkctrlpbase);
	if (dev->clkctrlvbase == (uintptr_t)MAP_FAILED) {
		fprintf(stderr, "%s: mmap_device_io: %s\n", __FUNCTION__, strerror(errno));
		goto fail1;
	}

	if (dev->clkstctrlpbase) {
		dev->clkstctrlvbase = mmap_device_io(4, dev->clkstctrlpbase);

		if (dev->clkstctrlvbase == (uintptr_t)MAP_FAILED) {
			fprintf(stderr, "%s: mmap_device_io: %s\n", __FUNCTION__, strerror(errno));
			goto fail2;
		}
	}

	return 0;

fail2:
	munmap_device_io(dev->clkctrlvbase, 4);
	dev->clkctrlvbase = 0;

fail1:
	return -1;
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/spi/omap4430/clock_toggle.c $ $Rev: 738420 $")
#endif
