/*
 * $QNXLicenseC:
 * Copyright 2011, QNX Software Systems.
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

int
context_restore_init(omap4430_spi_t *dev)
{
	dev->state.captured = 0;

	dev->mcspi_context_vaddr = mmap_device_io(4, dev->mcspi_context_paddr);
	if (dev->mcspi_context_vaddr == (uintptr_t)MAP_FAILED) {
		fprintf(stderr, "%s: mmap_device_io: %s\n", __FUNCTION__, strerror(errno));
		goto fail1;
	}

	return 0;

fail1:
	return -1;
}

void
context_restore_fini(omap4430_spi_t *dev)
{
	if (dev->mcspi_context_vaddr) {
		munmap_device_io(dev->mcspi_context_vaddr, 4);
	}
}

/*
 * Save the following registers for restoration on context lost:
 *     OMAP4430_MCSPI_SYS_CONFIG
 *     OMAP4430_MCSPI_MODCTRL_OFFSET
 *     OMAP4430_MCSPI_CH1_CONFIG_OFFSET + 0x14*i (where i = 0; i < dev.num_cs)
 */
void
context_restore_save(omap4430_spi_t *dev)
{
	int i;
	omap_clock_enable(dev);
	dev->state.sys_config = in32(dev->vbase + OMAP4430_MCSPI_SYS_CONFIG);
	dev->state.module_ctrl = in32(dev->vbase + OMAP4430_MCSPI_MODCTRL_OFFSET);
	for (i = 0; i < dev->num_cs; i++) {
		dev->state.ch_config[i] = in32(dev->vbase + OMAP4430_MCSPI_CH1_CONFIG_OFFSET + i * 0x14);
	}
	dev->state.captured = 1;
	omap_clock_disable(dev);
}

void
context_restore(omap4430_spi_t *dev)
{
	int i;
	uint32_t val;
	val = in32(dev->mcspi_context_vaddr);
	if ((val & LOSTCONTEXT_DFF) && dev->state.captured) {
		/* Restore the registers needed */
		out32(dev->vbase + OMAP4430_MCSPI_SYS_CONFIG, dev->state.sys_config);
		out32(dev->vbase + OMAP4430_MCSPI_MODCTRL_OFFSET, dev->state.module_ctrl);
		for (i = 0; i < dev->num_cs; i++) {
			out32(dev->vbase + OMAP4430_MCSPI_CH1_CONFIG_OFFSET + i * 0x14, dev->state.ch_config[i]);
		}

		/* Clear context lost */
		out32(dev->mcspi_context_vaddr, val & ~LOSTCONTEXT_DFF);
	}
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/spi/omap4430/context_restore.c $ $Rev: 738420 $")
#endif
