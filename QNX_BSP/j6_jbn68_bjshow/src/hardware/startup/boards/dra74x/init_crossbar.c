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

static uint16_t get_mpu_offset(uint16_t irq_no)
{
	uint16_t	offset;

	if (irq_no == 4) {
		offset = 0x00;
	} else if (irq_no == 7) {
		offset = 0x02;
	} else if (irq_no <= 130) {
		offset = (irq_no - 6) * 2;
	} else if ((irq_no <= 159) && (irq_no >= 133)) {
		offset = (irq_no - 8) * 2;
	} else {
		offset = -1;
	}

	return offset;
}

static inline uint16_t get_dma_offset(uint16_t irq_no)
{
	return (irq_no * 2);
}

void dra74x_set_mpu_crossbar(unsigned base, struct crossbar_entry const *array, int size)
{
	uint16_t	i, offset;
	struct crossbar_entry *irq = (struct crossbar_entry *)array;

	for (i = 0; i < size; i++, irq++) {
		offset = get_mpu_offset(irq->module_irq);

		if (offset != -1) {
			out16(base + offset, irq->crossbar_irq);
		}
	}
}

void dra74x_set_edma_crossbar(unsigned base, struct crossbar_entry const *array, int size)
{
	uint16_t	i, offset;
	struct crossbar_entry *irq = (struct crossbar_entry *)array;

	for (i = 0; i < size; i++, irq++) {
		offset = get_dma_offset(irq->module_irq);

		if (offset != -1) {
			out16(base + offset, irq->crossbar_irq);
		}
	}
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/dra74x/init_crossbar.c $ $Rev: 738517 $")
#endif
