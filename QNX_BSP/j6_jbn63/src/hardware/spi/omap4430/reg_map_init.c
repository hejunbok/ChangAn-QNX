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

#include "omap4430spi.h"
#include "clock_toggle.h"
#include "context_restore.h"

int
omap_reg_map_init(omap4430_spi_t* dev)
{
	uint32_t	silicon_type;
	uint32_t	silicon_rev;
	uintptr_t	id_code_reg_ptr;
	char		cbuf[64];

	dev->clkctrlpbase = 0;
	dev->clkctrlvbase = 0;
	dev->clkstctrlpbase = 0;
	dev->clkstctrlvbase = 0;
	dev->mcspi_context_paddr = 0;
	dev->mcspi_context_vaddr = 0;

	if (confstr(_CS_MACHINE, cbuf, sizeof(cbuf))) {

		/* We can't differentiate J6 from OMAP5 or OMAP4 through OMAP_CONTROL_ID_CODE register */
		if (strstr(cbuf, "DRA7") || strstr(cbuf, "J6")) {
			dev->clkstctrlpbase = CM_L4PER_CLKSTCTRL_DRA7XX;

			if (OMAP_MCSPI1_PHYSBASE == dev->pbase) {
				dev->clkctrlpbase = CM_L4PER_MCSPI1_CLKCTRL_DRA7XX;
				dev->mcspi_context_paddr = J6_RM_L4PER_MCSPI1_CONTEXT;
			} else if (OMAP_MCSPI2_PHYSBASE == dev->pbase) {
				dev->clkctrlpbase = CM_L4PER_MCSPI2_CLKCTRL_DRA7XX;
				dev->mcspi_context_paddr = J6_RM_L4PER_MCSPI2_CONTEXT;
			} else if (OMAP_MCSPI3_PHYSBASE == dev->pbase) {
				dev->clkctrlpbase = CM_L4PER_MCSPI3_CLKCTRL_DRA7XX;
				dev->mcspi_context_paddr = J6_RM_L4PER_MCSPI3_CONTEXT;
			} else if (OMAP_MCSPI4_PHYSBASE == dev->pbase) {
				dev->clkctrlpbase = CM_L4PER_MCSPI4_CLKCTRL_DRA7XX;
				dev->mcspi_context_paddr = J6_RM_L4PER_MCSPI4_CONTEXT;
			}

		/* OMAP4 or OMAP5 */
		} else {
			/* get silicon revision and type - requires a mapping of the id-code register */
			if ((id_code_reg_ptr = mmap_device_io(4, OMAP_CONTROL_ID_CODE)) == (uintptr_t)MAP_FAILED) {
				fprintf(stderr, "%s: mmap_device_io: %s\n", __FUNCTION__, strerror(errno));
				return -1;
			}

			silicon_rev = (in32(id_code_reg_ptr) & OMAP_CONTROL_ID_CODE_REV_MASK) >> OMAP_CONTROL_ID_CODE_REV_SHIFT;
			silicon_type = (in32(id_code_reg_ptr) & OMAP_CONTROL_ID_CODE_ID_MASK) >> OMAP_CONTROL_ID_CODE_ID_SHIFT;

			/* L4PER register offsets differ between revs and types of OMAP chips */
			if ( (silicon_type == OMAP_CONTROL_ID_CODE_ID_5430 || silicon_type == OMAP_CONTROL_ID_CODE_ID_5432) &&
				 (silicon_rev >= OMAP_CONTROL_ID_CODE_REV_2) )
			{
				dev->clkstctrlpbase = CM_L4PER_CLKSTCTRL_OMAP5_ES2;

				if (OMAP_MCSPI1_PHYSBASE == dev->pbase) {
					dev->clkctrlpbase = CM_L4PER_MCSPI1_CLKCTRL_OMAP5_ES2;
					dev->mcspi_context_paddr = RM_L4PER_MCSPI1_CONTEXT_ES2;
				} else if (OMAP_MCSPI2_PHYSBASE == dev->pbase) {
					dev->clkctrlpbase = CM_L4PER_MCSPI2_CLKCTRL_OMAP5_ES2;
					dev->mcspi_context_paddr = RM_L4PER_MCSPI2_CONTEXT_ES2;
				} else if (OMAP_MCSPI3_PHYSBASE == dev->pbase) {
					dev->clkctrlpbase = CM_L4PER_MCSPI3_CLKCTRL_OMAP5_ES2;
					dev->mcspi_context_paddr = RM_L4PER_MCSPI3_CONTEXT_ES2;
				} else if (OMAP_MCSPI4_PHYSBASE == dev->pbase) {
					dev->clkctrlpbase = CM_L4PER_MCSPI4_CLKCTRL_OMAP5_ES2;
					dev->mcspi_context_paddr = RM_L4PER_MCSPI4_CONTEXT_ES2;
				}
			}
			else /* OMAP543X silicon ES1 or OMAP4430 */
			{
				dev->clkstctrlpbase = CM_L4PER_CLKSTCTRL;

				if (OMAP_MCSPI1_PHYSBASE == dev->pbase) {
					dev->clkctrlpbase = CM_L4PER_MCSPI1_CLKCTRL;
					dev->mcspi_context_paddr = RM_L4PER_MCSPI1_CONTEXT;
				} else if (OMAP_MCSPI2_PHYSBASE == dev->pbase) {
					dev->clkctrlpbase = CM_L4PER_MCSPI2_CLKCTRL;
					dev->mcspi_context_paddr = RM_L4PER_MCSPI2_CONTEXT;
				} else if (OMAP_MCSPI3_PHYSBASE == dev->pbase) {
					dev->clkctrlpbase = CM_L4PER_MCSPI3_CLKCTRL;
					dev->mcspi_context_paddr = RM_L4PER_MCSPI3_CONTEXT;
				} else if (OMAP_MCSPI4_PHYSBASE == dev->pbase) {
					dev->clkctrlpbase = CM_L4PER_MCSPI4_CLKCTRL;
					dev->mcspi_context_paddr = RM_L4PER_MCSPI4_CONTEXT;
				}
			}
		}
	}

	if (!dev->clkctrlpbase || !dev->clkstctrlpbase || !dev->mcspi_context_paddr) {
		fprintf(stderr, "%s: Unrecognized physical address: 0x%llx\n", __FUNCTION__, dev->pbase);
		return -1;
	}

	return 0;
}



#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/spi/omap4430/reg_map_init.c $ $Rev: 738420 $")
#endif
