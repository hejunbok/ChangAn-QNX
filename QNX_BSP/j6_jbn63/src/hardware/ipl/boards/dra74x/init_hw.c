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

#include "board.h"

extern unsigned int chip_revision;

/* Get the device revision */
unsigned int get_omap_rev(void)
{
	return (in32(DRA74X_CTRL_WKUP_BASE + CTRL_WKUP_ID_CODE));
}

void print_omap_revision(void)
{
	switch (chip_revision) {
		case DRA7xx_ID_CODE_ES1_0:
			log_putstr("\nFound DRA7xx ES1.0 SoC\n");
			break;
		case DRA7xx_ID_CODE_ES1_1:
			log_putstr("\nFound DRA7xx ES1.1 SoC\n");
			break;
		default:
			log_putstr("\nUnknown DRA7xx SoC, assuming future rev. of DRA7xx: 0x\n");
			log_puthex(chip_revision);
			log_putstr("\n");
			break;
	}
}
void watchdog_init(void)
{
	out32(DRA74X_WDTIMER2_BASE + WDT_WSPR, DISABLE_SEQ_1);
	while (in32(DRA74X_WDTIMER2_BASE + WDT_WWPS))
		;
	out32(DRA74X_WDTIMER2_BASE + WDT_WSPR, DISABLE_SEQ_2);
	while (in32(DRA74X_WDTIMER2_BASE + WDT_WWPS))
		;
}

void watchdog_toggle(void)
{
	out32(DRA74X_WDTIMER2_BASE + WDT_WTGR, in32(DRA74X_WDTIMER2_BASE + WDT_WTGR) ^ 0xffffffff);
}

void io_settings(void)
{
	unsigned io_settings, mask;
	unsigned ctrl_base = DRA74X_CTRL_CORE_BASE;
	unsigned wkup_base = DRA74X_CTRL_WKUP_BASE;

	// Impedance settings EMMC, C2C 1,2, hsi2
	mask = (DS_MASK << 2) | (DS_MASK << 8) |(DS_MASK << 16) | (DS_MASK << 18);
	io_settings = in32(ctrl_base + CTRL_SMART1IO_PADCONF_0) & (~mask);
	io_settings |= (DS_60_OHM << 8) | (DS_45_OHM << 16) | (DS_45_OHM << 18) | (DS_60_OHM << 2);
	out32(ctrl_base + CTRL_SMART1IO_PADCONF_0, io_settings);

	// Impedance settings Mcspi2
	mask = (DS_MASK << 30);
	io_settings = in32(ctrl_base + CTRL_SMART1IO_PADCONF_1) & (~mask);
	io_settings |= (DS_60_OHM << 30);
	out32(ctrl_base + CTRL_SMART1IO_PADCONF_1, io_settings);

	// Impedance settings C2C 3,4
	mask = (DS_MASK << 14) | (DS_MASK << 16);
	io_settings = in32(ctrl_base + CTRL_SMART1IO_PADCONF_2) & (~mask);
	io_settings |= (DS_45_OHM << 14) | (DS_45_OHM << 16);
	out32(ctrl_base + CTRL_SMART1IO_PADCONF_2, io_settings);

	// Slew rate settings EMMC, C2C 1,2
	mask = (SC_MASK << 8) | (SC_MASK << 16) | (SC_MASK << 18);
	io_settings = in32(ctrl_base + CTRL_SMART2IO_PADCONF_0) & (~mask);
	io_settings |= (SC_FAST << 8) | (SC_NA << 16) | (SC_NA << 18);
	out32(ctrl_base + CTRL_SMART2IO_PADCONF_0, io_settings);

	// Slew rate settings hsi2, Mcspi2
	mask = (SC_MASK << 24) | (SC_MASK << 28);
	io_settings = in32(ctrl_base + CTRL_SMART2IO_PADCONF_1) & (~mask);
	io_settings |= (SC_FAST << 28) | (SC_FAST << 24);
	out32(ctrl_base + CTRL_SMART2IO_PADCONF_1, io_settings);

	// Slew rate settings C2C 3,4
	mask = (SC_MASK << 16) | (SC_MASK << 18);
	io_settings = in32(ctrl_base + CTRL_SMART2IO_PADCONF_2) & (~mask);
	io_settings |= (SC_NA << 16) | (SC_NA << 18);
	out32(ctrl_base + CTRL_SMART2IO_PADCONF_2, io_settings);

	// impedance and slew rate settings for usb
	mask = (USB_I_MASK << 29) | (USB_I_MASK << 26) | (USB_I_MASK << 23) |
		(USB_I_MASK << 20) | (USB_I_MASK << 17) | (USB_I_MASK << 14);
	io_settings = in32(ctrl_base + CTRL_SMART3IO_PADCONF_1) & (~mask);
	io_settings |= (DS_60_OHM << 29) | (DS_60_OHM << 26) |
		(DS_60_OHM << 23) | (SC_FAST << 20) | (SC_FAST << 17) | (SC_FAST << 14);
	out32(ctrl_base + CTRL_SMART3IO_PADCONF_1, io_settings);

	// PBIASLITE control
	out32(ctrl_base + CTRL_CORE_PBIAS, (1 << 27) | (1 << 26) | (1 << 21));

	// DDR3 pins IO settings
	out32(ctrl_base + CTRL_DDR3CH1_0, 0x80808080);
	out32(ctrl_base + CTRL_DDRCH1_0, 0x40404040);
	out32(ctrl_base + CTRL_DDRCH1_1, 0x40404040);

	out32(ctrl_base + CTRL_DDR3CH2_0, 0x80808080);
	out32(ctrl_base + CTRL_DDRCH2_0, 0x40404040);
	out32(ctrl_base + CTRL_DDRCH2_1, 0x40404040);

	out32(ctrl_base + CTRL_DDRIO_0, 0xbae8c631);
	out32(ctrl_base + CTRL_DDRIO_1, 0xb46318d8);
	out32(ctrl_base + CTRL_DDRIO_2, 0x84210000);

	// We does not use lpddr2 on dra74x
	out32(ctrl_base + CTRL_LPDDR2CH1_0, 0x40404040);
	out32(ctrl_base + CTRL_LPDDR2CH1_1, 0x40404040);

	out32(wkup_base + CTRL_WKUP_EMIF1_SDRAM_CONFIG_EXT, 0xb2c00000);
	out32(wkup_base + CTRL_WKUP_EMIF2_SDRAM_CONFIG_EXT, 0xb2c00000);

	out32(ctrl_base + CTRL_DDR_CONTROL_EXT_0, 0xA2000000);

	/* Efuse settings */
	out32(wkup_base + CTRL_WKUP_EFUSE_1, EFUSE_1_VAL);
	out32(wkup_base + CTRL_WKUP_EFUSE_2, EFUSE_2_VAL);
	out32(wkup_base + CTRL_WKUP_EFUSE_3, EFUSE_3_VAL);
	out32(wkup_base + CTRL_WKUP_EFUSE_4, EFUSE_4_VAL);
}

void unlock_ctrl_module(void)
{
	uint32_t	ctrl_base = DRA74X_CTRL_CORE_BASE;

	out32(ctrl_base + CTRL_CORE_MMR_LOCK_1, MMR1_UNLOCK_VALUE);
	out32(ctrl_base + CTRL_CORE_MMR_LOCK_2, MMR2_UNLOCK_VALUE);
	out32(ctrl_base + CTRL_CORE_MMR_LOCK_3, MMR3_UNLOCK_VALUE);
	out32(ctrl_base + CTRL_CORE_MMR_LOCK_4, MMR4_UNLOCK_VALUE);
	out32(ctrl_base + CTRL_CORE_MMR_LOCK_5, MMR5_UNLOCK_VALUE);
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/ipl/boards/dra74x/init_hw.c $ $Rev: 742206 $")
#endif
