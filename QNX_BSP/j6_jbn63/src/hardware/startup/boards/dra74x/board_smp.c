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

unsigned board_smp_num_cpu()
{
	unsigned	num;

	// Read CP15 L2CR to get number of processors
	__asm__ __volatile__("mrc	p15, 1, %0, c9, c0, 2" : "=r"(num));
	num = ((num >> 24) & 3) + 1;

	if (debug_flag) {
		kprintf("smp_num_cpus: %d cpus\n", num);
	}
	return num;
}

void board_smp_init(struct smp_entry *smp, unsigned num_cpus)
{
	smp->send_ipi = (void *)&sendipi_gic;
}

int board_smp_start(unsigned cpu, void (*start)(void))
{
	uint32_t	wugen_base = DRA74X_MPU_WUGEN;

	board_disable_mmu();

	out32(wugen_base + AUX_CORE_BOOT_0, 0x0);
	out32(wugen_base + AUX_CORE_BOOT_0, 0x10 << cpu);
	out32(wugen_base + AUX_CORE_BOOT_1, 0x0);
	out32(wugen_base + AUX_CORE_BOOT_1, (uint32_t)start);

	__asm__ __volatile__ ("dsb");

	// send event to slave CPUs to tell them to wake up
	__asm__ __volatile__ ("sev" : : : "memory");

	return 1;
}

unsigned board_smp_adjust_num(unsigned cpu)
{
	return cpu;
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/dra74x/board_smp.c $ $Rev: 738517 $")
#endif
