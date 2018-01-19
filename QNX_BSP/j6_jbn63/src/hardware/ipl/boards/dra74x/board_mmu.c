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

#include "board.h"

#if defined (CACHE_ENABLED)
#include "arm/arm_tlb.h"

/* Must make sure it's 16K aligned */
unsigned long arm_tlb[TLB_SIZE] __attribute__ ((section(".tlb")));

arm_tlb_t board_tlb[] = {

	/*
	 * By default Cache and Buffer are disbaled on all TLB entries
	 * We only need to list the address space that we want to enable Cache/Buffer
	 */
	{
		.start = DRA74x_OCMC_RAM1,
		.len = PAGE_SIZE,
		.attr = B_ENA | C_ENA,
	},

	/* Reserve one 1M non-cacheable */
	{
		.start = DRA74x_EMIF_CS0_BASE,
		.len = PAGE_SIZE,
		.attr = B_DIS | C_DIS,
	},

	{
		.start = DRA74x_EMIF_CS0_BASE + PAGE_SIZE,
		.len = SDRAM_SIZE - PAGE_SIZE,
		.attr = B_ENA | C_ENA,
	},

	/* mark the last entry */
	{
		-1L, -1L, 0,
	},
};

void enable_cache_mmu()
{
	/* Enable caches and MMU */
	arm_setup_tlb(board_tlb, arm_tlb);

	/* First page (starting at 0x0) is made invalid to avoid speculative
	 * accesses in secure rom. Accessing this region causes security
	 * violations, which we want to avoid.
	 */

	arm_tlb[0] = 0;

	arm_enable_mmu((unsigned long)arm_tlb);
	arm_v7_enable_cache();
}
#endif


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/ipl/boards/dra74x/board_mmu.c $ $Rev: 745291 $")
#endif
