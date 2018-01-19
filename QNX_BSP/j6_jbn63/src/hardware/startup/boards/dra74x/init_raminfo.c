/*
 * $QNXLicenseC:
 * Copyright 2013-2014, QNX Software Systems.
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


/*
 * init_raminfo.c
 *	Tell syspage about our RAM configuration
 */
#include "dra74x_startup.h"

extern int ddr_size;
void init_raminfo()
{
	if (ddr_size) {
		add_ram(0x80000000, ddr_size);
	} else {
		uintptr_t dmm_base = DRA74X_DMM_BASE;
		int	i;

		for (i = 0; i < 4; ++i) {
			uint32_t lisa_map;
			uint32_t mem_base;
			uint32_t map_size;
			int	is_mapped;

			lisa_map = in32(dmm_base + DMM_LISA_MAP_0 + (i * 4));
			mem_base = (lisa_map & DMM_LISA_MAP_SYS_ADDR_MASK);
			is_mapped = ((lisa_map & DMM_LISA_MAP_SDRC_MAP_MASK) >> DMM_LISA_MAP_SDRC_MAP_SHIFT) != DMM_LISA_MAP_SDRC_MAP_UNMAPPED;

			// FIXME: figure out what this mapped section is for
			if (is_mapped && (mem_base != 0xff000000)) {
				map_size = (lisa_map & DMM_LISA_MAP_SYS_SIZE_MASK) >> DMM_LISA_MAP_SYS_SIZE_SHIFT;
				map_size = (1 << map_size) * MEG(16);
				add_ram(mem_base, map_size);
			} else if (mem_base == 0xff000000) {
				alloc_ram(mem_base, MEG(16), 1);
			}
		}
	}
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/dra74x/init_raminfo.c $ $Rev: 738517 $")
#endif
