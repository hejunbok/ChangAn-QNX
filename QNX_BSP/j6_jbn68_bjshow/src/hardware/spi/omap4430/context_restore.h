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

#ifndef CONTEXT_RESTORE_H_
#define CONTEXT_RESTORE_H_

/*
 * Context registers for SPI, used to determine if we've lost
 * context on clock enable.
 */
#define RM_L4PER_MCSPI1_CONTEXT 0x4A3074F4
#define RM_L4PER_MCSPI2_CONTEXT 0x4A3074FC
#define RM_L4PER_MCSPI3_CONTEXT 0x4A307504
#define RM_L4PER_MCSPI4_CONTEXT 0x4A30750C

#define RM_L4PER_MCSPI1_CONTEXT_ES2 0x4AE074F4
#define RM_L4PER_MCSPI2_CONTEXT_ES2 0x4AE074FC
#define RM_L4PER_MCSPI3_CONTEXT_ES2 0x4AE07104
#define RM_L4PER_MCSPI4_CONTEXT_ES2 0x4AE07110

/* base addresses for J6 */
#define J6_RM_L4PER_MCSPI1_CONTEXT 0x4AE074F4
#define J6_RM_L4PER_MCSPI2_CONTEXT 0x4AE074FC
#define J6_RM_L4PER_MCSPI3_CONTEXT 0x4AE07504
#define J6_RM_L4PER_MCSPI4_CONTEXT 0x4AE0750C

#define LOSTCONTEXT_DFF 0x1

int
context_restore_init(omap4430_spi_t *dev);

void
context_restore_fini(omap4430_spi_t *dev);

void
context_restore_save(omap4430_spi_t *dev);

void
context_restore(omap4430_spi_t *dev);

#endif /* CONTEXT_RESTORE_H_ */


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/spi/omap4430/context_restore.h $ $Rev: 738420 $")
#endif
