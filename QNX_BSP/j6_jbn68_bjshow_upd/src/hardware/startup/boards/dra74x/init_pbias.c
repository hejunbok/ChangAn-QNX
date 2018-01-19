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

#include "dra74x_startup.h"

#define	MMCHS_SYSCONFIG			0x110	// System Configuration Register
	#define	SYSCONFIG_SOFTRESET		(1 << 1)	// Software reset
#define	MMCHS_SYSSTATUS			0x114	// System Status Register
	#define	SYSSTATUS_RESETDONE		(1 << 0)	// Reset done
#define	MMCHS_HCTL			0x228	// Host Control register
	#define	HCTL_SDVS1V8			(5 << 9)	// SD bus voltage 1.8V
	#define	HCTL_SDBP_ON			(1 << 8)	// SD bus power
	#define	HCTL_SDBP_OFF			(1 << 7)	// SD bus power
	#define	HCTL_DTW4			(1 << 1)	// Data transfer width 4 bit
	#define	HCTL_DTW1			(0 << 1)	// Data transfer width 1 bit
#define	MMCHS_SYSCTL			0x22C	// SD system control register
	#define	SYSCTL_SRA			(1 << 24)	// Software reset for all
	#define SYSCTL_ICE			(1 << 0)	// Internal Clock Enable
#define	MMCHS_AC12			0x23C	// Auto CMD12 Error Status Register
	#define V1V8_SIGEN			(1 << 19)
#define	MMCHS_CAPA			0x240	// Capabilities Register
	#define VS18				(1 << 26)	// 1.8V support

/*
 * To configure MMC1 I/Os to use 1.8V instead of the default 3.3V the PBIAS and MMC1 registers
 * need to be updated in the sequence below.
 */
void mmc1_select_1_8v()
{
	unsigned reg;

	/* Step 1 - Power down PBIAS I/O */

	/* Power down SDCARD IO */
	reg = in32(DRA74X_CTRL_CORE_CONTROL_PBIAS);
	reg &= ~CTRL_CORE_CONTROL_PBIAS_SDCARD_IO_PWRDNZ;
	out32(DRA74X_CTRL_CORE_CONTROL_PBIAS, reg);

	/* Power down SDCARD BIAS */
	reg &= ~CTRL_CORE_CONTROL_PBIAS_SDCARD_BIAS_PWRDNZ;
	out32(DRA74X_CTRL_CORE_CONTROL_PBIAS, reg);

	/* Configure PBIAS cell to supply 3.0 V */
	reg &= ~CTRL_CORE_CONTROL_PBIAS_SDCARD_BIAS_MASK;
	reg |= CTRL_CORE_CONTROL_PBIAS_SDCARD_BIAS_1_8V;
	out32(DRA74X_CTRL_CORE_CONTROL_PBIAS, reg);

	/* Step 2 - Configure MMC1 to use 1.8V */
	out32(DRA74X_MMC1_BASE + MMCHS_AC12, in32(DRA74X_MMC1_BASE + MMCHS_CAPA) | V1V8_SIGEN);

	/* Soft Reset */
	out32(DRA74X_MMC1_BASE + MMCHS_SYSCONFIG, SYSCONFIG_SOFTRESET);
	while (in32(DRA74X_MMC1_BASE + MMCHS_SYSSTATUS) == 0);
		;

	/* Soft reset all */
	out32(DRA74X_MMC1_BASE + MMCHS_SYSCTL, SYSCTL_SRA);
	while (in32(DRA74X_MMC1_BASE + MMCHS_SYSCTL) != 0);
		;

	/* Enable internal (but not external clock). If internal clock is not enabled MMC1 voltage cannot be changed. */
	out32(DRA74X_MMC1_BASE + MMCHS_SYSCTL, SYSCTL_ICE);

	/* After reset MMC1 is set to 3V by default */
	out32(DRA74X_MMC1_BASE + MMCHS_AC12, V1V8_SIGEN);

	/* Select 1.8V bus voltage and power down host controller */
	out32(DRA74X_MMC1_BASE + MMCHS_HCTL, HCTL_DTW1 | HCTL_SDVS1V8 | HCTL_SDBP_OFF);

	/* Power up host controller */
	out32(DRA74X_MMC1_BASE + MMCHS_HCTL, HCTL_DTW1 | HCTL_SDVS1V8 | HCTL_SDBP_ON);

	/* Configure MMC1 to use 1.8V again since reset would have cleared this setting. */
	out32(DRA74X_MMC1_BASE + MMCHS_AC12, in32(DRA74X_MMC1_BASE + MMCHS_CAPA) | V1V8_SIGEN);
	out32(DRA74X_MMC1_BASE + MMCHS_CAPA, in32(DRA74X_MMC1_BASE + MMCHS_CAPA) | VS18);

	/* Step 3 - Enable PBIAS */

	/* Wait 5us as recommended by TI */
	usec_delay(5);

	/* Power up SDCARD BIAS */
	reg = in32(DRA74X_CTRL_CORE_CONTROL_PBIAS);
	reg |= CTRL_CORE_CONTROL_PBIAS_SDCARD_BIAS_PWRDNZ;
	out32(DRA74X_CTRL_CORE_CONTROL_PBIAS, reg);

	/* Power up SDCARD IO */
	reg |= CTRL_CORE_CONTROL_PBIAS_SDCARD_IO_PWRDNZ;
	out32(DRA74X_CTRL_CORE_CONTROL_PBIAS, reg);

	/* Verify if PBIAS voltage change was successful */
	reg = in32(DRA74X_CTRL_CORE_CONTROL_PBIAS);
	if (reg & CTRL_CORE_CONTROL_PBIAS_SDCARD_BIAS_VMODE_ERROR)
		kprintf("VMODE voltage level != SUPPLY_HI_OUT, MMC1 may not function properly, PBIAS reg value: 0x%x\n",
			in32(DRA74X_CTRL_CORE_CONTROL_PBIAS));
}
#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/dra74x/init_pbias.c $ $Rev: 767266 $")
#endif
