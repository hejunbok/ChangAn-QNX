/*
 * $QNXLicenseC:
 * Copyright 2014, QNX Software Systems.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0

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
#include "dra74x_ipl.h"
#include "qspi.h"

#ifndef	___BOARD_H_INCLUDED
#define ___BOARD_H_INCLUDED

/*
 * We need to set IVA DPLL at OPP_HIGH (532Mhz) for video playback
 * usecase to achieve 1080p@60fps
 */
#define IVA_OPP_HIGH

/*
 * We need to set DSP DPLL at OPP_OD(700Mhz) as per plan of record (POR)
 */
#define DSP_OPP_OD

#define SDRAM_SIZE						0x80000000

/* EFUSE values */
#define EFUSE_1_VAL						0x45145100
#define EFUSE_2_VAL						0x45145100
#define EFUSE_3_VAL						0x45145100
#define EFUSE_4_VAL						0x45145100

/*
 * Define it to enable bench mark of the IPL
 */
#define DEBUG_BOOT_TIMING

/*
 * By enabling i-cache and d-cache in IPL, the performance significantly improved.
 * Please note that if enabling MMU, the d-cache performance will improved significantly,
 * but it doesn't have simple way to handle the cache coherence problem when accessing some devices
 */
#define CACHE_ENABLED

#ifdef HS_AUTH_IFS
/* We don't enable cache for secure booting.. To be revisted later.. */
#undef CACHE_ENABLED
#endif

extern void init_pinmux(void);

#endif /* __BOARD_H_INCLUDED */



#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/ipl/boards/dra74x/arm/vayu-evm.le.v7/board.h $ $Rev: 758539 $")
#endif
