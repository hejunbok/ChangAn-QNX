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

static const unsigned clkin_freqs[] = {
	0,			// DRA74X_CM_CLKSEL_SYS__UNINITIALIZED
	12000000,	// DRA74X_CM_CLKSEL_SYS__12M
	20000000,	// DRA74X_CM_CLKSEL_SYS__20M
	16800000,	// DRA74X_CM_CLKSEL_SYS__16P8M
	19200000,	// DRA74X_CM_CLKSEL_SYS__19P2M
	26000000,	// DRA74X_CM_CLKSEL_SYS__26M
	0,			// Reserved
	38400000	// DRA74X_CM_CLKSEL_SYS__38P4M
};

const uint32_t dra74x_timer_clkctrl[] = {
	-1,
	DRA74X_CM_WKUPAON_BASE + CM_WKUPAON_TIMER1_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER_TIMER2_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER_TIMER3_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER_TIMER4_CLKCTRL,
	DRA74X_PRCM_BASE + CM_IPU_TIMER5_CLKCTRL,
	DRA74X_PRCM_BASE + CM_IPU_TIMER6_CLKCTRL,
	DRA74X_PRCM_BASE + CM_IPU_TIMER7_CLKCTRL,
	DRA74X_PRCM_BASE + CM_IPU_TIMER8_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER_TIMER9_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER_TIMER10_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER_TIMER11_CLKCTRL,
	-1, /* timer12 */
	DRA74X_PRCM_BASE + CM_L4PER3_TIMER13_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER3_TIMER14_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER3_TIMER15_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER3_TIMER16_CLKCTRL
};

const uint32_t dra74x_i2c_clkctrl[] = {
	-1,
	DRA74X_PRCM_BASE + CM_L4PER_I2C1_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER_I2C2_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER_I2C3_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER_I2C4_CLKCTRL,
	DRA74X_PRCM_BASE + CM_IPU_I2C5_CLKCTRL
};

const uint32_t dra74x_uart_clkctrl[] = {
	-1,
	DRA74X_PRCM_BASE + CM_L4PER_UART1_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER_UART2_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER_UART3_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER_UART4_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER_UART5_CLKCTRL,
	DRA74X_PRCM_BASE + CM_IPU_UART6_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER2_UART7_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER2_UART8_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER2_UART9_CLKCTRL,
	DRA74X_CM_WKUPAON_BASE + CM_WKUPAON_UART10_CLKCTRL
};

const uint32_t dra74x_mmc_clkctrl[] = {
	-1,
	DRA74X_PRCM_BASE + CM_L3INIT_MMC1_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L3INIT_MMC2_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER_MMC3_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER_MMC4_CLKCTRL
};

uint32_t get_sys_clkspeed()
{
	uint32_t sys_clkin_sel;

	sys_clkin_sel = in32(DRA74X_CM_CLKSEL_BASE + CM_CLKSEL_SYS) & CM_CLKSEL_SYS__MASK;
	return clkin_freqs[sys_clkin_sel];
}

static int set_clkstctrl_clock(uint32_t reg, uint32_t mode)
{
	volatile int i = 0;

	sr32(reg, CLKSTCTRL_CLKTRCTRL_OFFSET, CLKSTCTRL_CLKTRCTRL_WIDTH, mode);

	while (((in32(reg) & CLKSTCTRL_CLKACTIVITY_MASK) == CLKSTCTRL_CLKACTIVITY_GATED)
		&& (i++ < LDELAY));

	if (i >= LDELAY) {
		return -1;
	}

	return 0;
}

static unsigned enable_clkctrl_clock(unsigned addr, unsigned mode)
{
	volatile int i = 0;

	/* Enable CLKCTRL and possibly set other clock mode */
	out32(addr, mode);

	while (((in32(addr) & CLKCTRL_IDLEST_MASK) != CLKCTRL_IDLEST_FUNCTIONAL)
		&& (i++ < LDELAY));

	if (i >= LDELAY) {
		return -1;
	}

	return 0;
}

void init_gptimer_clock(int dev_num, int mode)
{
	enable_clkctrl_clock(dra74x_timer_clkctrl[dev_num], CLKCTRL_MODULEMODE_ENABLED | mode);
}

void init_i2c_clock(int dev_num)
{
	enable_clkctrl_clock(dra74x_i2c_clkctrl[dev_num], CLKCTRL_MODULEMODE_ENABLED);
}

void init_uart_clock(int dev_num)
{
	enable_clkctrl_clock(dra74x_uart_clkctrl[dev_num], CLKCTRL_MODULEMODE_ENABLED);
}

void init_mmc_clock(int dev_num, int mode)
{
	enable_clkctrl_clock(dra74x_mmc_clkctrl[dev_num], CLKCTRL_MODULEMODE_ENABLED | mode);
}

void init_wdt_clock()
{
	enable_clkctrl_clock(DRA74X_CM_WKUPAON_BASE + CM_WKUPAON_WD_TIMER2_CLKCTRL, CLKCTRL_MODULEMODE_ENABLED);
}

void init_qspi_clock()
{
	/* QSPI clock comes from DPLL_PER, 192MHz */
	enable_clkctrl_clock(DRA74X_PRCM_BASE + CM_L4PER2_QSPI_CLKCTRL, CLKCTRL_MODULEMODE_ENABLED | (1 << CLKCTRL_CLKSEL_OFFSET));
}

void init_edma_clocks()
{
	enable_clkctrl_clock(DRA74X_PRCM_BASE + CM_L3MAIN1_TPCC_CLKCTRL, CLKCTRL_MODULEMODE_AUTO);
	enable_clkctrl_clock(DRA74X_PRCM_BASE + CM_L3MAIN1_TPTC1_CLKCTRL, CLKCTRL_MODULEMODE_AUTO);
	enable_clkctrl_clock(DRA74X_PRCM_BASE + CM_L3MAIN1_TPTC2_CLKCTRL, CLKCTRL_MODULEMODE_AUTO);
}

unsigned get_mmcsd_clock(int device)
{
	uint32_t	addr;
	uint32_t	freq = 192000000;
	uint32_t	val;

	switch (device) {
		case 1:
			addr = DRA74X_PRCM_BASE + CM_L3INIT_MMC1_CLKCTRL;
			break;
		case 2:
			addr = DRA74X_PRCM_BASE + CM_L3INIT_MMC2_CLKCTRL;
			break;
		case 3:
			addr = DRA74X_PRCM_BASE + CM_L4PER_MMC3_CLKCTRL;
			break;
		case 4:
		default:
			addr = DRA74X_PRCM_BASE + CM_L4PER_MMC4_CLKCTRL;
			break;
	}

	val = in32(addr);
	if (!(val & CLKCTRL_CLKSEL_MSK)) {
		if ((DRA74X_PRCM_BASE + CM_L3INIT_MMC1_CLKCTRL == addr) || (DRA74X_PRCM_BASE + CM_L3INIT_MMC2_CLKCTRL == addr)) {
			freq = 128000000;
		} else {
			freq = 48000000;
		}
	}

	return (freq >> ((val & CLKCTRL_CLKSEL_DIV_MSK) >> CLKCTRL_CLKSEL_DIV_OFF));
}

/* Initialize the basic clocks */
void init_basic_clocks(void)
{
	uint32_t ctrl_base = DRA74X_PRCM_BASE;
	uint32_t wkup_base = DRA74X_CM_WKUPAON_BASE;

	// enable clock domains (place is SW_WPUP mode)
	set_clkstctrl_clock(ctrl_base + CM_L4PER_CLKSTCTRL, CLKSTCTRL_CLKTRCTRL_SW_WKUP);
	set_clkstctrl_clock(ctrl_base + CM_L3INIT_CLKSTCTRL, CLKSTCTRL_CLKTRCTRL_SW_WKUP);
	set_clkstctrl_clock(ctrl_base + CM_EMIF_CLKSTCTRL, CLKSTCTRL_CLKTRCTRL_SW_WKUP);
	set_clkstctrl_clock(ctrl_base + CM_L4CFG_CLKSTCTRL, CLKSTCTRL_CLKTRCTRL_SW_WKUP);

	// enable clock modules - hw auto
	enable_clkctrl_clock(ctrl_base + CM_EMIF_EMIF1_CLKCTRL, CLKCTRL_MODULEMODE_AUTO);
	enable_clkctrl_clock(ctrl_base + CM_EMIF_EMIF2_CLKCTRL, CLKCTRL_MODULEMODE_AUTO);
	enable_clkctrl_clock(ctrl_base + CM_L4CFG_L4_CFG_CLKCTRL, CLKCTRL_MODULEMODE_AUTO);

	// place clock domain in hw_auto mode
	set_clkstctrl_clock(ctrl_base + CM_L4PER_CLKSTCTRL, CLKSTCTRL_CLKTRCTRL_HW_AUTO);
	set_clkstctrl_clock(ctrl_base + CM_L3INIT_CLKSTCTRL, CLKSTCTRL_CLKTRCTRL_HW_AUTO);
	set_clkstctrl_clock(ctrl_base + CM_EMIF_CLKSTCTRL, CLKSTCTRL_CLKTRCTRL_HW_AUTO);
	set_clkstctrl_clock(ctrl_base + CM_L4CFG_CLKSTCTRL, CLKSTCTRL_CLKTRCTRL_HW_AUTO);

	// enable SCRM OPT clocks for PER and CORE DPLL, undocumented register
	sr32(wkup_base + CM_WKUPAON_SCRM_CLKCTRL, 8, 2, 3);

	// enable clock modules - hw auto
	enable_clkctrl_clock(ctrl_base + CM_L3INSTR_L3_MAIN_2_CLKCTRL, CLKCTRL_MODULEMODE_AUTO);
	enable_clkctrl_clock(ctrl_base + CM_L3INSTR_L3_INSTR_CLKCTRL, CLKCTRL_MODULEMODE_AUTO);
	enable_clkctrl_clock(ctrl_base + CM_L3INSTR_OCP_WP_NOC_CLKCTRL, CLKCTRL_MODULEMODE_AUTO);
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/ipl/boards/dra74x/init_clocks.c $ $Rev: 758184 $")
#endif
