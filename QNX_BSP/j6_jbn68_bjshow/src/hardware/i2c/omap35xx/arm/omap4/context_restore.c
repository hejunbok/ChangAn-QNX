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

#include "proto.h"
#include "clock_toggle.h"
#include "context_restore.h"

int
context_restore_init(omap_dev_t *dev)
{
	dev->state.captured = 0;

	// I2C context is always 4 bytes.
	dev->i2c_context_vaddr = mmap_device_io(4, dev->i2c_context_paddr);
	if (dev->i2c_context_vaddr == (uintptr_t)MAP_FAILED) {
		fprintf(stderr, "%s(0x%08x): mmap_device_io: %s\n", __FUNCTION__, dev->i2c_context_paddr, strerror(errno));
		goto fail1;
	}

	return 0;
fail1:
	return -1;
}

void
context_restore_fini(omap_dev_t *dev)
{
	if (dev->i2c_context_vaddr) {
		munmap_device_io(dev->i2c_context_vaddr, 4);
	}
}

void
context_restore_save(omap_dev_t *dev)
{
	/*
	 * This capture is required only for power management,
	 * but since this is fairly insignificant we'll do it
	 * anyway.
	 */
	dev->state.psc = in16(dev->regbase + OMAP_I2C_PSC);
	dev->state.scll = in16(dev->regbase + OMAP_I2C_SCLL);
	dev->state.sclh = in16(dev->regbase + OMAP_I2C_SCLH);
	dev->state.buf = in16(dev->regbase + OMAP_I2C_BUF);
	dev->state.sysc = in16(dev->regbase + OMAP_I2C_SYSC);
	dev->state.we = in16(dev->regbase + OMAP_I2C_WE);
	dev->state.ie = in16(dev->regbase + OMAP_I2C_IE);
	dev->state.captured = 1;
}

void
context_restore(omap_dev_t *dev) {
	uint32_t val;
#if 0
	if (dev->state.captured) {
		if (dev->state.psc != in16(dev->regbase + OMAP_I2C_PSC)) {
			perror("PSC state inconsistent");
		}
		if (dev->state.scll != in16(dev->regbase + OMAP_I2C_SCLL)) {
			perror("SCLL state inconsistent");
		}
		if (dev->state.sclh != in16(dev->regbase + OMAP_I2C_SCLH)) {
			perror("SCLH state inconsistent");
		}
		if (dev->state.buf != in16(dev->regbase + OMAP_I2C_BUF)) {
			perror("BUF state inconsistent");
		}
		if (dev->state.sysc != in16(dev->regbase + OMAP_I2C_SYSC)) {
			perror("SYSC state inconsistent");
		}
		if (dev->state.we != in16(dev->regbase + OMAP_I2C_WE)) {
			perror("WE state inconsistent");
		}
	}
#endif
	/*
	 * Determine if context was lost.
	 */
	val = (in32(dev->i2c_context_vaddr));
	if ((val & (LOSTCONTEXT_RFF_MASK | LOSTCONTEXT_DFF_MASK)) && dev->state.captured) {
		out32(dev->regbase + OMAP_I2C_CON,  0);
		out32(dev->regbase + OMAP_I2C_PSC,  dev->state.psc);
		out32(dev->regbase + OMAP_I2C_SCLL, dev->state.scll);
		out32(dev->regbase + OMAP_I2C_SCLH, dev->state.sclh);
		out32(dev->regbase + OMAP_I2C_BUF,  dev->state.buf);
		out32(dev->regbase + OMAP_I2C_SYSC, dev->state.sysc);
		out32(dev->regbase + OMAP_I2C_WE,   dev->state.we);
		out32(dev->regbase + OMAP_I2C_IE,   dev->state.ie);
		out32(dev->regbase + OMAP_I2C_CON,  OMAP_I2C_CON_EN);
		/* Clear context lost. */
		out32(dev->i2c_context_vaddr, (dev->i2c_context_paddr == RM_L4PER_I2C1_CONTEXT) ? LOSTCONTEXT_RFF_MASK : LOSTCONTEXT_DFF_MASK);
	}
	return;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/i2c/omap35xx/arm/omap4/context_restore.c $ $Rev: 680332 $")
#endif
