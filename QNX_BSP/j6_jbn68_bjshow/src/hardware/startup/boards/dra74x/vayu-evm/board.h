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


/*
 * Vayu EVM board specific definitions
 */

#ifndef __BOARD_H_INCLUDED
#define __BOARD_H_INCLUDED

#define SYS_CLK2_FREQ					22579200

/* IRQ allocation */
#define VIP1_IRQ						122
#define VIP2_IRQ						123
#define VIP3_IRQ						124
#define VPE_IRQ							134
#define CRYPTO_AES_IRQ					137
#define CRYPTO_MD5_IRQ					138

/* Comment this out to disable MMU/Cache in startup */
#define MMU_ENABLED						1

#define IO_EXPANDER_1_I2C_ADDR			0x20	// IO Expander PCF8575, U58 on Vayu
#define IO_EXPANDER_2_I2C_ADDR			0x21	// IO Expander PCF8575, U57 on Vayu
#define IO_EXPANDER_3_I2C_ADDR			0x26	// IO Expander PCF8575, U119 on Vayu
#define TLC59108IPW_1_I2C_ADDR			0x40
#define TLC59108IPW_2_I2C_ADDR			0x41

#define PMIC_CLK32AUDIO_CTRLA			0xD5
#define PMIC_PAD2A						0xFB

/* GPMC configurations */
#define GPMC_CONFIG1_VAL				0x00001803
#define GPMC_CONFIG2_VAL				0x00090A00
#define GPMC_CONFIG3_VAL				0x00090000
#define GPMC_CONFIG4_VAL				0x06000700
#define GPMC_CONFIG5_VAL				0x0007090A
#define GPMC_CONFIG6_VAL				0x0F03000F
#define GPMC_CONFIG7_VAL				0x00000F48

/* Config word 0 to PCF8575 */
#define PCF8575_P0						(1 << 0)
#define PCF8575_P1						(1 << 1)
#define PCF8575_P2						(1 << 2)
#define PCF8575_P3						(1 << 3)
#define PCF8575_P4						(1 << 4)
#define PCF8575_P5						(1 << 5)
#define PCF8575_P6						(1 << 6)
#define PCF8575_P7						(1 << 7)

/* Config word 1 to PCF8575 */
#define PCF8575_P10						(1 << 0)
#define PCF8575_P11						(1 << 1)
#define PCF8575_P12						(1 << 2)
#define PCF8575_P13						(1 << 3)
#define PCF8575_P14						(1 << 4)
#define PCF8575_P15						(1 << 5)
#define PCF8575_P16						(1 << 6)
#define PCF8575_P17						(1 << 7)

extern int wifi;
extern int jamr3;
extern int gpmc_nand;
extern int gpmc_nor;
extern int gpmc_vout3;
extern int abe_dpll_196m;

void init_gpmc(void);

#endif	/* __BOARD_H_INCLUDED */


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/dra74x/vayu-evm/board.h $ $Rev: 766791 $")
#endif
