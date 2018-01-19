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


/*
 * DRA74X specific GPMC NAND/NOR flash support.
 */

#include "dra74x_startup.h"
#include "board.h"

static const uint32_t pin_data_vout3[] = {
	GPMC_AD0,	(IEN | PDIS | FSC | M3),
	GPMC_AD1,	(IEN | PDIS | FSC | M3),
	GPMC_AD2,	(IEN | PDIS | FSC | M3),
	GPMC_AD3,	(IEN | PDIS | FSC | M3),
	GPMC_AD4,	(IEN | PDIS | FSC | M3),
	GPMC_AD5,	(IEN | PDIS | FSC | M3),
	GPMC_AD6,	(IEN | PDIS | FSC | M3),
	GPMC_AD7,	(IEN | PDIS | FSC | M3),
	GPMC_AD8,	(IEN | PDIS | FSC | M3),
	GPMC_AD9,	(IEN | PDIS | FSC | M3),
	GPMC_AD10,	(IEN | PDIS | FSC | M3),
	GPMC_AD11,	(IEN | PDIS | FSC | M3),
	GPMC_AD12,	(IEN | PDIS | FSC | M3),
	GPMC_AD13,	(IEN | PDIS | FSC | M3),
	GPMC_AD14,	(IEN | PDIS | FSC | M3),
	GPMC_AD15,	(IEN | PDIS | FSC | M3),
	GPMC_A0,	(IDIS | PDIS | FSC | M3),
	GPMC_A1,	(IDIS | PDIS | FSC | M3),
	GPMC_A2,	(IDIS | PDIS | FSC | M3),
	GPMC_A5,	(IDIS | PDIS | FSC | M3),
	GPMC_A6,	(IDIS | PDIS | FSC | M3),
	GPMC_A7,	(IDIS | PDIS | FSC | M3),
	GPMC_A8,	(IDIS | PDIS | FSC | M3),
	GPMC_A9,	(IDIS | PDIS | FSC | M3),
	GPMC_A10,	(IDIS | PDIS | FSC | M3),
	GPMC_A11,	(IDIS | PDIS | FSC | M3),
};

/*
 * Pin Mux Registers and the Corresponding Values
 * Need to rework hardware in order to enable NOR on Vayu
 */
static const uint32_t pin_data_nor[] = {
	GPMC_AD0,	(IEN | PDIS | FSC | M0),
	GPMC_AD1,	(IEN | PDIS | FSC | M0),
	GPMC_AD2,	(IEN | PDIS | FSC | M0),
	GPMC_AD3,	(IEN | PDIS | FSC | M0),
	GPMC_AD4,	(IEN | PDIS | FSC | M0),
	GPMC_AD5,	(IEN | PDIS | FSC | M0),
	GPMC_AD6,	(IEN | PDIS | FSC | M0),
	GPMC_AD7,	(IEN | PDIS | FSC | M0),
	GPMC_AD8,	(IEN | PDIS | FSC | M0),
	GPMC_AD9,	(IEN | PDIS | FSC | M0),
	GPMC_AD10,	(IEN | PDIS | FSC | M0),
	GPMC_AD11,	(IEN | PDIS | FSC | M0),
	GPMC_AD12,	(IEN | PDIS | FSC | M0),
	GPMC_AD13,	(IEN | PDIS | FSC | M0),
	GPMC_AD14,	(IEN | PDIS | FSC | M0),
	GPMC_AD15,	(IEN | PDIS | FSC | M0),
	GPMC_A0,	(IDIS | PDIS | FSC | M0),
	GPMC_A1,	(IDIS | PDIS | FSC | M0),
	GPMC_A2,	(IDIS | PDIS | FSC | M0),
	GPMC_A5,	(IDIS | PDIS | FSC | M0),
	GPMC_A6,	(IDIS | PDIS | FSC | M0),
	GPMC_A7,	(IDIS | PDIS | FSC | M0),
	GPMC_A8,	(IDIS | PDIS | FSC | M0),
	GPMC_A9,	(IDIS | PDIS | FSC | M0),
	GPMC_A10,	(IDIS | PDIS | FSC | M0),
	GPMC_A11,	(IDIS | PDIS | FSC | M0),
	GPMC_A12,	(IDIS | PDIS | FSC | M0),
	GPMC_A13,	(IDIS | PDIS | FSC | M0),
	GPMC_A14,	(IDIS | PDIS | FSC | M0),
	GPMC_A15,	(IDIS | PDIS | FSC | M0),
	GPMC_A16,	(IDIS | PDIS | FSC | M0),
	GPMC_A17,	(IDIS | PDIS | FSC | M0),
	GPMC_A18,	(IDIS | PDIS | FSC | M0),
	GPMC_A19,	(IDIS | PDIS | FSC | M0),
	GPMC_A20,	(IDIS | PDIS | FSC | M0),
	GPMC_A21,	(IDIS | PDIS | FSC | M0),
	GPMC_A22,	(IDIS | PDIS | FSC | M0),
	GPMC_A23,	(IDIS | PDIS | FSC | M0),
	GPMC_A24,	(IDIS | PDIS | FSC | M0),
	GPMC_A25,	(IDIS | PDIS | FSC | M0),
	GPMC_A26,	(IDIS | PDIS | FSC | M0),
	GPMC_A27,	(IDIS | PDIS | FSC | M0),
	GPMC_CS0,		(IDIS | PEN | PTU | FSC | M0),
	GPMC_OEN_REN,	(IDIS | PDIS | FSC | M0),
	GPMC_WEN,		(IDIS | PDIS | FSC | M0),
};

static void init_gpmc_nor(void)
{
	int32_t	 i, size;
	uint32_t	gpmc_base = DRA74X_GPMC_BASE;

	// configure PINMUX
	size = sizeof(pin_data_nor) / sizeof(uint32_t);
	for (i = 0; i < size; i += 2)
		out32(DRA74X_CONTROL_PADCONF_CORE_BASE + pin_data_nor[i], pin_data_nor[i + 1]);

	// configure access timing
	out32(gpmc_base + GPMC_IRQENABLE, 0);		/* isr's sources masked */
	out32(gpmc_base + GPMC_TIMEOUT_CONTROL, 0); /* timeout disable */
	out32(gpmc_base + GPMC_CONFIG, 0);			/* disable WP */

	/* NOR flash for CS0 */
	out32(gpmc_base + GPMC_CONFIG7(0), 0);
	out32(gpmc_base + GPMC_CONFIG7(1), 0);
	out32(gpmc_base + GPMC_CONFIG7(2), 0);
	out32(gpmc_base + GPMC_CONFIG7(3), 0);
	out32(gpmc_base + GPMC_CONFIG7(4), 0);
	out32(gpmc_base + GPMC_CONFIG7(5), 0);
	out32(gpmc_base + GPMC_CONFIG7(6), 0);
	out32(gpmc_base + GPMC_CONFIG7(7), 0);

	out32(gpmc_base + GPMC_CONFIG1(0), 0x00001010);
	out32(gpmc_base + GPMC_CONFIG2(0), 0x00090880);
	out32(gpmc_base + GPMC_CONFIG3(0), 0x00000000);
	out32(gpmc_base + GPMC_CONFIG4(0), 0x08000800);
	out32(gpmc_base + GPMC_CONFIG5(0), 0x0007090B);
	out32(gpmc_base + GPMC_CONFIG6(0), 0x07000000);

	/* Device base address: 0x08000000, Chip-select size: 64 Mbytes for S29GL512 */
	out32(gpmc_base + GPMC_CONFIG7(0), 0x00000C48);
}

/* Pin Mux Registers and the Corresponding Values */
static const uint32_t pin_data_nand[] = {
	GPMC_AD0,		(IEN | PEN | PTU | FSC | M0),
	GPMC_AD1,		(IEN | PEN | PTU | FSC | M0),
	GPMC_AD2,		(IEN | PEN | PTU | FSC | M0),
	GPMC_AD3,		(IEN | PEN | PTU | FSC | M0),
	GPMC_AD4,		(IEN | PEN | PTU | FSC | M0),
	GPMC_AD5,		(IEN | PEN | PTU | FSC | M0),
	GPMC_AD6,		(IEN | PEN | PTU | FSC | M0),
	GPMC_AD7,		(IEN | PEN | PTU | FSC | M0),
	GPMC_AD8,		(IEN | PEN | PTU | FSC | M0),
	GPMC_AD9,		(IEN | PEN | PTU | FSC | M0),
	GPMC_AD10,		(IEN | PEN | PTU | FSC | M0),
	GPMC_AD11,		(IEN | PEN | PTU | FSC | M0),
	GPMC_AD12,		(IEN | PEN | PTU | FSC | M0),
	GPMC_AD13,		(IEN | PEN | PTU | FSC | M0),
	GPMC_AD14,		(IEN | PEN | PTU | FSC | M0),
	GPMC_AD15,		(IEN | PEN | PTU | FSC | M0),
	GPMC_CS0,		(IDIS | PDIS | FSC | M0),
	GPMC_BEN0,		(IDIS | PEN | PTU | FSC | M0),
	GPMC_ADVN_ALE,	(IDIS | PEN | PTU | FSC | M0),
	GPMC_OEN_REN,	(IDIS | PEN | PTU | FSC | M0),
	GPMC_WEN,		(IDIS | PEN | PTU | FSC | M0),
	GPMC_WAIT0,	 	(IEN | PDIS | FSC | M0),
};

static void init_gpmc_nand(void)
{
	int32_t	 i, size;
	uint32_t	gpmc_base = DRA74X_GPMC_BASE;

	// configure PINMUX
	size = sizeof(pin_data_nand) / sizeof(uint32_t);
	for (i = 0; i < size; i += 2)
		out32(DRA74X_CONTROL_PADCONF_CORE_BASE + pin_data_nand[i], pin_data_nand[i + 1]);

	// configure access timing
	out32(gpmc_base + GPMC_IRQENABLE, 0);		/* isr's sources masked */
	out32(gpmc_base + GPMC_TIMEOUT_CONTROL, 0); /* timeout disable */
	out32(gpmc_base + GPMC_CONFIG, 0);		 /* disable WP */

	/* NAND flash for CS0 */
	out32(gpmc_base + GPMC_CONFIG7(0), 0);
	out32(gpmc_base + GPMC_CONFIG7(1), 0);
	out32(gpmc_base + GPMC_CONFIG7(2), 0);
	out32(gpmc_base + GPMC_CONFIG7(3), 0);
	out32(gpmc_base + GPMC_CONFIG7(4), 0);
	out32(gpmc_base + GPMC_CONFIG7(5), 0);
	out32(gpmc_base + GPMC_CONFIG7(6), 0);
	out32(gpmc_base + GPMC_CONFIG7(7), 0);

	out32(gpmc_base + GPMC_CONFIG1(0), GPMC_CONFIG1_VAL);
	out32(gpmc_base + GPMC_CONFIG2(0), GPMC_CONFIG2_VAL);
	out32(gpmc_base + GPMC_CONFIG3(0), GPMC_CONFIG3_VAL);
	out32(gpmc_base + GPMC_CONFIG4(0), GPMC_CONFIG4_VAL);
	out32(gpmc_base + GPMC_CONFIG5(0), GPMC_CONFIG5_VAL);
	out32(gpmc_base + GPMC_CONFIG6(0), GPMC_CONFIG6_VAL);

	/* Device base address: 0x08000000, Chip-select size: 16Mbytes */
	out32(gpmc_base + GPMC_CONFIG7(0), GPMC_CONFIG7_VAL);
}

static void init_gpmc_vout3(void)
{
	int32_t i, size;
	uint32_t gpmc_base = DRA74X_GPMC_BASE;

	/* configure PINMUX */
	size = sizeof(pin_data_vout3) / sizeof(uint32_t);
	for (i = 0; i < size; i += 2)
		out32(DRA74X_CONTROL_PADCONF_CORE_BASE + pin_data_vout3[i], pin_data_vout3[i + 1]);
}

void init_gpmc(void)
{
	kprintf("gpmc_vout3:%d\n", gpmc_vout3);
	if (gpmc_vout3) {
		init_gpmc_vout3();
	} else if (gpmc_nand) {
		init_gpmc_nand();
	} else if (gpmc_nor) {
		init_gpmc_nor();
	}
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/dra74x/vayu-evm/init_gpmc.c $ $Rev: 764091 $")
#endif
