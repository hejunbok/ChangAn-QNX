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
#include "context_restore.h"
#include "clock_toggle.h"

#ifndef TBL_SIZE
	#define TBL_SIZE(tbl)   (sizeof(tbl)/sizeof(tbl[0]))
#endif

// this is the registers database, per device and per block
typedef struct pm_info_s {
    uint32_t    physbase;       // I2C module physical base address
    uint32_t    clkctrl_phys;   // PRCM clkctrl physical address
    uint32_t    context_phys;   // PRCM context register physical address
}pm_info_t;

typedef struct soc_info_s {
    pm_info_t*  pm_info_tbl;        // Pointer to SoC PM configuration table
    unsigned    size;               // Size of the configuration table
}soc_info_t;

// Definition of OMAP4 PM info
pm_info_t      omap4_info[] = {
   {.physbase = OMAP_I2C1_PHYSBASE, .clkctrl_phys = CM_L4PER_I2C1_CLKCTRL, .context_phys = RM_L4PER_I2C1_CONTEXT},
   {.physbase = OMAP_I2C2_PHYSBASE, .clkctrl_phys = CM_L4PER_I2C2_CLKCTRL, .context_phys = RM_L4PER_I2C2_CONTEXT},
   {.physbase = OMAP_I2C3_PHYSBASE, .clkctrl_phys = CM_L4PER_I2C3_CLKCTRL, .context_phys = RM_L4PER_I2C3_CONTEXT},
   {.physbase = OMAP_I2C4_PHYSBASE, .clkctrl_phys = CM_L4PER_I2C4_CLKCTRL, .context_phys = RM_L4PER_I2C4_CONTEXT},
};

// Definition of OMAP5 ES1 PM info
pm_info_t      omap5_es1_info[] = {
   {.physbase = OMAP5_I2C1_PHYSBASE, .clkctrl_phys = OMAP5_ES1_CM_L4PER_I2C1_CLKCTRL, .context_phys = OMAP5_ES1_RM_L4PER_I2C1_CONTEXT},
   {.physbase = OMAP5_I2C2_PHYSBASE, .clkctrl_phys = OMAP5_ES1_CM_L4PER_I2C2_CLKCTRL, .context_phys = OMAP5_ES1_RM_L4PER_I2C2_CONTEXT},
   {.physbase = OMAP5_I2C3_PHYSBASE, .clkctrl_phys = OMAP5_ES1_CM_L4PER_I2C3_CLKCTRL, .context_phys = OMAP5_ES1_RM_L4PER_I2C3_CONTEXT},
   {.physbase = OMAP5_I2C4_PHYSBASE, .clkctrl_phys = OMAP5_ES1_CM_L4PER_I2C4_CLKCTRL, .context_phys = OMAP5_ES1_RM_L4PER_I2C4_CONTEXT},
   {.physbase = OMAP5_I2C5_PHYSBASE, .clkctrl_phys = OMAP5_ES1_CM_L4PER_I2C5_CLKCTRL, .context_phys = OMAP5_ES1_RM_L4PER_I2C5_CONTEXT},
};

// Definition of OMAP5 ES2 PM info
pm_info_t      omap5_es2_info[] = {
   {.physbase = OMAP5_I2C1_PHYSBASE, .clkctrl_phys = OMAP5_ES2_CM_L4PER_I2C1_CLKCTRL, .context_phys = OMAP5_ES2_RM_L4PER_I2C1_CONTEXT},
   {.physbase = OMAP5_I2C2_PHYSBASE, .clkctrl_phys = OMAP5_ES2_CM_L4PER_I2C2_CLKCTRL, .context_phys = OMAP5_ES2_RM_L4PER_I2C2_CONTEXT},
   {.physbase = OMAP5_I2C3_PHYSBASE, .clkctrl_phys = OMAP5_ES2_CM_L4PER_I2C3_CLKCTRL, .context_phys = OMAP5_ES2_RM_L4PER_I2C3_CONTEXT},
   {.physbase = OMAP5_I2C4_PHYSBASE, .clkctrl_phys = OMAP5_ES2_CM_L4PER_I2C4_CLKCTRL, .context_phys = OMAP5_ES2_RM_L4PER_I2C4_CONTEXT},
   {.physbase = OMAP5_I2C5_PHYSBASE, .clkctrl_phys = OMAP5_ES2_CM_L4PER_I2C5_CLKCTRL, .context_phys = OMAP5_ES2_RM_L4PER_I2C5_CONTEXT},
};

pm_info_t      j6_info[] = {
   {.physbase = OMAP5_I2C1_PHYSBASE, .clkctrl_phys = J6_CM_L4PER_I2C1_CLKCTRL, .context_phys = J6_RM_L4PER_I2C1_CONTEXT},
   {.physbase = OMAP5_I2C2_PHYSBASE, .clkctrl_phys = J6_CM_L4PER_I2C2_CLKCTRL, .context_phys = J6_RM_L4PER_I2C2_CONTEXT},
   {.physbase = OMAP5_I2C3_PHYSBASE, .clkctrl_phys = J6_CM_L4PER_I2C3_CLKCTRL, .context_phys = J6_RM_L4PER_I2C3_CONTEXT},
   {.physbase = OMAP5_I2C4_PHYSBASE, .clkctrl_phys = J6_CM_L4PER_I2C4_CLKCTRL, .context_phys = J6_RM_L4PER_I2C4_CONTEXT},
   {.physbase = OMAP5_I2C5_PHYSBASE, .clkctrl_phys = J6_CM_IPU_I2C5_CLKCTRL, .context_phys = J6_RM_IPU_I2C5_CONTEXT},
};

soc_info_t      soc_list[] = {
   {.pm_info_tbl = omap4_info, .size = TBL_SIZE(omap4_info)},
   {.pm_info_tbl = omap5_es1_info, .size = TBL_SIZE(omap5_es1_info)},
   {.pm_info_tbl = omap5_es2_info, .size = TBL_SIZE(omap5_es2_info)},
   {.pm_info_tbl = j6_info, .size = TBL_SIZE(j6_info)}
};

unsigned soc_list_size = TBL_SIZE(soc_list);

int
omap_reg_map_init(omap_dev_t* dev)
{
	unsigned		idx;
	soc_info_t*		soc = &soc_list[dev->soc_version];

	// Walk through the list I2C module to find proper PM info
	dev->clkctrl_phys = -1;

	for (idx=0; idx<soc->size; idx++) {
		if (soc->pm_info_tbl[idx].physbase == dev->physbase) {
			dev->clkctrl_phys = soc->pm_info_tbl[idx].clkctrl_phys;
			dev->i2c_context_paddr = soc->pm_info_tbl[idx].context_phys;
		}
	}

	if (dev->clkctrl_phys == -1) {
		fprintf(stderr, "%s(0x%08x): Unrecognized physical address.\n", __FUNCTION__, dev->physbase);
		dev->clkctrl_phys = 0;
		dev->clkctrl_base = 0;
		dev->clkstctrl_base = 0;
		return EINVAL;
	}

	return EOK;
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/i2c/omap35xx/arm/omap4/reg_map_init.c $ $Rev: 739568 $")
#endif
