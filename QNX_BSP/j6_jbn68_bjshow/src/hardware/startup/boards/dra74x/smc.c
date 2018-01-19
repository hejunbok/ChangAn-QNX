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

#include "startup.h"

#define _STR(x)				#x
#define MIN_GAS_VER(x)		_STR(x)
#define GAS_VERSION_221		22100

void secure_monitor_call(uint16_t func_id, int p0_in, int p1_in, int * p0_out, int * p1_out)
{
	register uint32_t r12 asm ("r12");
	register uint32_t r0 asm ("r0");
	register uint32_t r1 asm ("r1");

	__asm__ __volatile__("mov %0, %1" : : "r"(r12), "r"(func_id));
	__asm__ __volatile__("mov %0, %1" : : "r"(r0), "r"(p0_in));
	__asm__ __volatile__("mov %0, %1" : : "r"(r1), "r"(p1_in));

	// ROM monitor call clobbers many registers, so manually save/restore them.
	__asm__ __volatile__(
		 "stmdb sp!, {r2-r12, lr}\n"
		 "dsb\n"
		 ".ifdef .gasversion.\n"
		 ".if .gasversion. > " MIN_GAS_VER(GAS_VERSION_221) "\n"
		 "	 .arch_extension sec\n"
		 ".endif\n"
		 ".endif\n"
		 "smc #0\n"
		 "ldmia sp!, {r2-r12, lr}\n" );

	if (p0_out) {
		__asm__ __volatile__("str %0, [%1]" : : "r"(r0), "r"(p0_out));
	}

	if (p1_out) {
		__asm__ __volatile__("str %0, [%1]" : : "r"(r1), "r"(p1_out));
	}
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/dra74x/smc.c $ $Rev: 745291 $")
#endif
