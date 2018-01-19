#ifdef VARIANT_v7
/*
 * $QNXLicenseC:
 * Copyright 2015, QNX Software Systems.
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
#include "libfdt.h"

static inline int psci_hvc(int cmd, unsigned arg1, unsigned arg2) {
	asm volatile(
		"mov r0,%0;"
		"mov r1,%2;"
		"mov r2,%3;"
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 6)
		".arch_extension virt\n"
		"hvc #0;"				// psci
#else
		".word 0xe1400070;"	// hvc #0 // psci
#endif
		: "=&r" (cmd)
		: "0" (cmd), "r" (arg1), "r" (arg2)
		: "r0", "r1", "r2"
	);
	return cmd;
}

static inline int psci_smc(int cmd, unsigned arg1, unsigned arg2) {
	asm volatile(
		"mov r0,%0;"
		"mov r1,%2;"
		"mov r2,%3;"
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 6)
		".arch_extension sec\n"
		"smc #0;"				// psci
#else
		".word 0xe1600070;"	// smc #0 // psci
#endif
		: "=&r" (cmd)
		: "0" (cmd), "r" (arg1), "r" (arg2)
		: "r0", "r1", "r2"
	);
	return cmd;
}

int psci_smp_start(unsigned cpu, void (*start)(void))
{
	void						*f = fdt;
	int							node;
	const struct fdt_property	*prop;
	const fdt32_t				*data;
	int							cpu_on;

	if(!f || (node = fdt_node_offset_by_compatible(f, -1, "arm,psci")) <= 0) {
		return 1;
	}
	prop = fdt_get_property(f, node, "cpu_on", 0);
	if(fdt32_to_cpu(prop->len) < sizeof(fdt32_t)) {
		return 0;
	}
	data = (const fdt32_t *)prop->data;
	cpu_on = fdt32_to_cpu(data[0]);

	prop = fdt_get_property(f, node, "method", 0);
	if(fdt32_to_cpu(prop->len) == sizeof("smc") && !strncmp(prop->data, "smc", sizeof("smc"))) {
		if(psci_smc(cpu_on, cpu, (uintptr_t)start) < 0) {
			return 0;
		}
	} else if(fdt32_to_cpu(prop->len) == sizeof("hvc") && !strncmp(prop->data, "hvc", sizeof("hvc"))) {
		if(psci_hvc(cpu_on, cpu, (uintptr_t)start) < 0) {
			return 0;
		}
	} else {
		return 0;
	}
	return 1;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/dra74x/board_smp.c $ $Rev: 738517 $")
#endif
#endif /* VARIANT_v7 */
