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

#include "dra74x_startup.h"
#include "board.h"

#if defined (MMU_ENABLED)
#include <arm/arm_tlb.h>

#define SDRAM_BASE_ADDR		0x80000000
#ifndef STARTUP_SDRAM_SIZE
#define STARTUP_SDRAM_SIZE	(128*1024*1024)
#endif

/* Must make sure it's 16K aligned */
unsigned long arm_tlb[TLB_SIZE] __attribute__ ((aligned(16 * 1024)));

arm_tlb_t board_tlb[] = {

	/*
	 * By default Cache and Buffer are disbaled on all TLB entries
	 * We only need to list the address space that we want to enable Cache/Buffer
	 */
	{
		.start = SDRAM_BASE_ADDR,
		.len = STARTUP_SDRAM_SIZE,
		.attr = B_ENA | C_ENA,
	},

	/* mark the last entry */
	{
		-1L, -1L, 0,
	},
};

/*
 * This is to overwrite startup/lib/arm/board_cpu_startup.c
 * enable cache, setup MMU at entrance of startup, and set the L2 cache AUXCR
 */
void board_cpu_startup()
{
	/* Enable caches and MMU */
	arm_setup_tlb(board_tlb, arm_tlb);

	/* First page (starting at 0x0) is made invalid to avoid speculative
	 * accesses in secure rom. Accessing this region causes security
	 * violations, which we want to avoid.
	 */
	arm_tlb[0] = 0;

	/* Enable Cache/MMU on CPU0 only */
	if (!arm_mp_cpuid()) {
		arm_enable_mmu((unsigned long)arm_tlb);
		arm_v7_enable_cache();
	}
}

/*
 * Disable cache/MMU right before jumping to kernel code
 */
void board_disable_mmu()
{
	/*
	 * Flush D-cache and invalidate I-cache, TLB
	 */
	arm_v7_disable_cache_mmu();
}

void board_cpu_startnext()
{
	board_disable_mmu();
}
#else
void board_disable_mmu()
{
}
#endif


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/dra74x/board_mmu.c $ $Rev: 745291 $")
#endif
