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

uintptr_t	prcm_base = DRA74X_PRCM_BASE;
uintptr_t	cm_wkupaon_base = DRA74X_CM_WKUPAON_BASE;

const uint32_t clkin_freqs[] = {
	0,		// DRA74X_CM_CLKSEL_SYS__UNINITIALIZED
	12000000, // DRA74X_CM_CLKSEL_SYS__12M
	20000000, // DRA74X_CM_CLKSEL_SYS__20M
	16800000, // DRA74X_CM_CLKSEL_SYS__16P8M
	19200000, // DRA74X_CM_CLKSEL_SYS__19P2M
	26000000, // DRA74X_CM_CLKSEL_SYS__26M
	0,
	38400000	// DRA74X_CM_CLKSEL_SYS__38P4M
};

const uint32_t dra74x_dpll_base[] = {
	DRA74X_PRCM_BASE + CM_CLKMODE_DPLL_PER,
	DRA74X_PRCM_BASE + CM_CLKMODE_DPLL_CORE,
	DRA74X_PRCM_BASE + CM_CLKMODE_DPLL_ABE,
	DRA74X_PRCM_BASE + CM_CLKMODE_DPLL_MPU,
	DRA74X_PRCM_BASE + CM_CLKMODE_DPLL_IVA,
	DRA74X_PRCM_BASE + CM_CLKMODE_DPLL_USB,
	DRA74X_PRCM_BASE + CM_CLKMODE_DPLL_DSP,
	DRA74X_PRCM_BASE + CM_CLKMODE_DPLL_GMAC,
	DRA74X_PRCM_BASE + CM_CLKMODE_DPLL_GPU,
	DRA74X_PRCM_BASE + CM_CLKMODE_DPLL_DDRPHY,
	DRA74X_PRCM_BASE + CM_CLKMODE_DPLL_PCIE_REF,
	DRA74X_PRCM_BASE + CM_CLKMODE_DPLL_EVE
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
	DRA74X_CM_WKUPAON_BASE + CM_WKUPAON_TIMER12_CLKCTRL,
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

const uint32_t dra74x_mcspi_clkctrl[] = {
	-1,
	DRA74X_PRCM_BASE + CM_L4PER_MCSPI1_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER_MCSPI2_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER_MCSPI3_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER_MCSPI4_CLKCTRL
};

const uint32_t dra74x_mcasp_clkctrl[] = {
	-1,
	DRA74X_PRCM_BASE + CM_IPU_MCASP1_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER2_MCASP2_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER2_MCASP3_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER2_MCASP4_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER2_MCASP5_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER2_MCASP6_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER2_MCASP7_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER2_MCASP8_CLKCTRL
};

const uint32_t dra74x_mmc_clkctrl[] = {
	-1,
	DRA74X_PRCM_BASE + CM_L3INIT_MMC1_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L3INIT_MMC2_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER_MMC3_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER_MMC4_CLKCTRL
};

const uint32_t dra74x_gpio_clkctrl[] = {
	-1,
	DRA74X_CM_WKUPAON_BASE + CM_WKUPAON_GPIO1_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER_GPIO2_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER_GPIO3_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER_GPIO4_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER_GPIO5_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER_GPIO6_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER_GPIO7_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L4PER_GPIO8_CLKCTRL
};

const uint32_t dra74x_vip_clkctrl[] = {
	-1,
	DRA74X_PRCM_BASE + CM_CAM_VIP1_CLKCTRL,
	DRA74X_PRCM_BASE + CM_CAM_VIP2_CLKCTRL,
	DRA74X_PRCM_BASE + CM_CAM_VIP3_CLKCTRL
};

const uint32_t dra74x_usb_ss_clkctrl[] = {
	-1,
	DRA74X_PRCM_BASE + CM_L3INIT_USB_OTG_SS1_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L3INIT_USB_OTG_SS2_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L3INIT_USB_OTG_SS3_CLKCTRL,
	DRA74X_PRCM_BASE + CM_L3INIT_USB_OTG_SS4_CLKCTRL
};

const uint32_t dra74x_usb_phy_clkctrl[] = {
	-1,
	DRA74X_PRCM_BASE + CM_COREAON_USB_PHY1_CORE_CLKCTRL,
	DRA74X_PRCM_BASE + CM_COREAON_USB_PHY2_CORE_CLKCTRL,
	DRA74X_PRCM_BASE + CM_COREAON_USB_PHY3_CORE_CLKCTRL
};

uint32_t get_sys_clkspeed()
{
	static uint32_t	sys_clkspeed = 0;
	uint32_t	sys_clkin_sel;

	if (0 == sys_clkspeed) {
		sys_clkin_sel = in32(DRA74X_CM_CLKSEL_BASE + CM_CLKSEL_SYS) & CM_CLKSEL_SYS__MASK;
		if (sys_clkin_sel == CM_CLKSEL_SYS__UNINITIALIZED) {
			crash("Error: CM_SYS_CLKSEL is not configured.\n");
		}
		sys_clkspeed = clkin_freqs[sys_clkin_sel];
	}

	return sys_clkspeed;
}

#define OFF_DPLL_REG(r) (CM_##r##_DPLL_CORE - CM_CLKMODE_DPLL_CORE)

/* Return the locked DPLL frequency */
unsigned calc_dpll_freq(dpll_type_t type)
{
	uint32_t	dpll_mult = 0, dpll_div = 0;
	uint64_t	mn_output = 0;
	uint32_t	sys_clkspeed = get_sys_clkspeed();
	uint32_t	dpll;
	uint32_t	base = dra74x_dpll_base[type];

	if (base == (DRA74X_PRCM_BASE + CM_CLKMODE_DPLL_ABE)) {
		if (in32(DRA74X_CM_CLKSEL_BASE + CM_CLKSEL_ABE_PLL_REF)) {
			sys_clkspeed = 32 * 1024;
		} else if (in32(DRA74X_CM_CLKSEL_BASE + CM_CLKSEL_ABE_PLL_SYS)) {
			sys_clkspeed = get_sys_clk2_freq();
		}
	}

	/* If not locked */
	if (!(in32(base + OFF_DPLL_REG(IDLEST)) & ST_DPLL_CLK_MASK)) {
		return 0;
	}

	dpll = in32(base + OFF_DPLL_REG(CLKSEL));

	dpll_mult = (dpll & CM_CLKSEL_MULT_MASK) >> CM_CLKSEL_MULT_SHIFT;
	dpll_div = (dpll & CM_CLKSEL_DPLL_DIV_MASK) >> CM_CLKSEL_DPLL_DIV_SHIFT;

	mn_output = ((uint64_t)sys_clkspeed * (uint64_t)dpll_mult);
	mn_output = (mn_output / (dpll_div + 1));

	if ((in32(base + OFF_DPLL_REG(CLKSEL)) & CM_CLKSEL_DCC_EN_MASK) == 0) {
		mn_output *= 2;
	}

	if ((base == (DRA74X_PRCM_BASE + CM_CLKMODE_DPLL_ABE)) &&
		(in32(base + OFF_DPLL_REG(CLKSEL)) & DPLL_REGM4XEN)) {
		mn_output *= 4;
	}

	return mn_output;
}

static uint32_t calc_type_b_dpll_freq_clkout(dpll_type_t type)
{
	uint32_t m2, freq;

	if ((freq = calc_dpll_freq(type))) {
		m2 = in32(dra74x_dpll_base[type] + OFF_DPLL_REG(DIV_M2)) & DIVHS_MASK;
		return (freq / (m2 * 2));
	}
	return 0;
}

static uint32_t calc_dpll_post_freq_m2(dpll_type_t type)
{
	uint32_t m2, freq;
	uint32_t base = dra74x_dpll_base[type];

	if ((freq = calc_dpll_freq(type))) {
		m2 = in32(base + OFF_DPLL_REG(DIV_M2)) & DIVHS_MASK;

		if (((in32(base + OFF_DPLL_REG(CLKSEL)) & CM_CLKSEL_DCC_EN_MASK) == 0) || (m2 != 1)) {
			m2 *= 2;
		}

		return freq	/ m2;
	}
	return 0;
}

static uint32_t calc_dpll_post_freq_m3(dpll_type_t type)
{
	uint32_t freq;

	if ((freq = calc_dpll_freq(type))) {
		return (freq / (in32(dra74x_dpll_base[type] + OFF_DPLL_REG(DIV_M3)) & DIVHS_MASK));
	}
	return 0;
}

static uint32_t calc_dpll_post_freq_h11(dpll_type_t type)
{
	uint32_t freq;

	if ((freq = calc_dpll_freq(type))) {
		return (freq / (in32(dra74x_dpll_base[type] + OFF_DPLL_REG(DIV_H11)) & Hnm_DIVHS_MASK));
	}
	return 0;
}

static uint32_t calc_dpll_post_freq_h12(dpll_type_t type)
{
	uint32_t freq;

	if ((freq = calc_dpll_freq(type))) {
		return (freq / (in32(dra74x_dpll_base[type] + OFF_DPLL_REG(DIV_H12)) & Hnm_DIVHS_MASK));
	}
	return 0;
}

static uint32_t calc_dpll_post_freq_h13(dpll_type_t type)
{
	uint32_t freq;

	if ((freq = calc_dpll_freq(type))) {
		return (freq / (in32(dra74x_dpll_base[type] + OFF_DPLL_REG(DIV_H13)) & Hnm_DIVHS_MASK));
	}
	return 0;
}

static uint32_t calc_dpll_post_freq_h14(dpll_type_t type)
{
	uint32_t freq;

	if ((freq = calc_dpll_freq(type))) {
		return (freq / (in32(dra74x_dpll_base[type] + OFF_DPLL_REG(DIV_H14)) & Hnm_DIVHS_MASK));
	}
	return 0;
}

static uint32_t calc_dpll_post_freq_h21(dpll_type_t type)
{
	uint32_t freq;

	if ((freq = calc_dpll_freq(type))) {
		return (freq / (in32(dra74x_dpll_base[type] + OFF_DPLL_REG(DIV_H21)) & Hnm_DIVHS_MASK));
	}
	return 0;
}

static uint32_t calc_dpll_post_freq_h22(dpll_type_t type)
{
	uint32_t freq;

	if ((freq = calc_dpll_freq(type))) {
		return (freq / (in32(dra74x_dpll_base[type] + OFF_DPLL_REG(DIV_H22)) & Hnm_DIVHS_MASK));
	}
	return 0;
}

static uint32_t calc_dpll_post_freq_h23(dpll_type_t type)
{
	uint32_t freq;

	if ((freq = calc_dpll_freq(type))) {
		return (freq / (in32(dra74x_dpll_base[type] + OFF_DPLL_REG(DIV_H23)) & Hnm_DIVHS_MASK));
	}
	return 0;
}

static uint32_t calc_dpll_post_freq_h24(dpll_type_t type)
{
	uint32_t freq;

	if ((freq = calc_dpll_freq(type))) {
		return (freq / (in32(dra74x_dpll_base[type] + OFF_DPLL_REG(DIV_H24)) & Hnm_DIVHS_MASK));
	}
	return 0;
}

unsigned calc_omap_dpll_and_set_freq(unsigned *processor_speed_ptr, unsigned *max_speed_ptr)
{
	if (processor_speed_ptr != NULL) {
		*processor_speed_ptr = calc_dpll_post_freq_m2(DPLL_MPU);
	}

	/* Also pass back the max possible speed for this device */
	if (max_speed_ptr != NULL) {
		*max_speed_ptr = 1500 * 1000 * 1000;
	}

	return 1;
}

static void check_clkstctrl_activity(uint32_t reg, const char *msg)
{
	volatile int i = 0;

	while (((in32(reg) & CLKSTCTRL_CLKACTIVITY_MASK) == CLKSTCTRL_CLKACTIVITY_GATED)
			&& (i++ < LDELAY));

	if (i >= LDELAY) {
		kprintf("Error: failed to enable %s clock. CLKSTCTRL (0x%x): 0x%x\n", msg, reg, in32(reg));
		crash("\n");
	}
}

static void check_clkctrl_idlest_functional(uint32_t reg, const char *msg)
{
	volatile int i = 0;

	while (((in32(reg) & CLKCTRL_IDLEST_MASK) != CLKCTRL_IDLEST_FUNCTIONAL)
			&& (i++ < LDELAY));

	if (i >= LDELAY) {
		kprintf("Error: failed to enable %s clock. CLKCTRL (0x%x): 0x%x\n", msg, reg, in32(reg));
		crash("\n");
	}
}

static void check_clkctrl_idlest_disabled(uint32_t reg, const char *msg)
{
	volatile int i = 0;

	while (((in32(reg) & CLKCTRL_IDLEST_MASK) == CLKCTRL_IDLEST_DISABLED)
			&& (i++ < LDELAY));

	if (i >= LDELAY) {
		kprintf("Error: failed to enable %s clock. CLKCTRL: 0x%x\n", msg, reg, in32(reg));
		crash("\n");
	}
}

void init_ipu(void)
{
	/* Enable IPU clocks */
	out32(prcm_base + CM_IPU_CLKSTCTRL, CLKSTCTRL_CLKTRCTRL_SW_WKUP);

	/* Set IPU powerstate to ON */
	sr32(PM_IPU_PWRSTCTRL, PWRSTCTRL_POWERSTATE_OFFSET, PWRSTCTRL_POWERSTATE_WIDTH, PWRSTCTRL_POWERSTATE_ON);
}

void init_ipu1(void)
{
	out32(prcm_base + CM_IPU1_CLKSTCTRL, CLKSTCTRL_CLKTRCTRL_SW_WKUP);
	out32(prcm_base + CM_IPU1_IPU1_CLKCTRL, CLKCTRL_MODULEMODE_AUTO);
	check_clkstctrl_activity(prcm_base + CM_IPU1_CLKSTCTRL, "IPU1");
	out32(prcm_base + CM_IPU1_CLKSTCTRL, CLKCTRL_MODULEMODE_AUTO);
}

void init_ipu2(void)
{
	/* Enable IPU2 clocks */
	out32(prcm_base + CM_IPU2_CLKSTCTRL, CLKSTCTRL_CLKTRCTRL_SW_WKUP);
	out32(prcm_base + CM_IPU2_IPU2_CLKCTRL, CLKCTRL_MODULEMODE_AUTO);
	out32(prcm_base + CM_IPU2_CLKSTCTRL, CLKSTCTRL_CLKTRCTRL_HW_AUTO);
	check_clkstctrl_activity(prcm_base + CM_IPU2_CLKSTCTRL, "IPU2");
}

void init_gptimer_clock(int dev_num, int mode)
{
	out32(dra74x_timer_clkctrl[dev_num], CLKCTRL_MODULEMODE_ENABLED | mode);
	check_clkctrl_idlest_functional(dra74x_timer_clkctrl[dev_num], "general purpose timer");
}

static int gptimer_clock_enabled(int dev_num)
{
	if ((dra74x_timer_clkctrl[dev_num] == -1) ||
		((in32(dra74x_timer_clkctrl[dev_num]) & CLKCTRL_MODULEMODE_MASK) == CLKCTRL_MODULEMODE_DISABLED)) {
		return 0;
	}
	return 1;
}

void init_i2c_clock(int dev_num)
{
	out32(dra74x_i2c_clkctrl[dev_num], CLKCTRL_MODULEMODE_ENABLED);
	check_clkctrl_idlest_functional(dra74x_i2c_clkctrl[dev_num], "I2C");
}

static int i2c_clock_enabled(int dev_num)
{
	if ((dra74x_i2c_clkctrl[dev_num] == -1) ||
		((in32(dra74x_i2c_clkctrl[dev_num]) & CLKCTRL_MODULEMODE_MASK) == CLKCTRL_MODULEMODE_DISABLED)) {
		return 0;
	}
	return 1;
}

void init_uart_clock(int dev_num)
{
	out32(dra74x_uart_clkctrl[dev_num], CLKCTRL_MODULEMODE_ENABLED);
	check_clkctrl_idlest_functional(dra74x_uart_clkctrl[dev_num], "UART");
}

static unsigned get_uart_clock(int dev_num)
{
	if ((dra74x_uart_clkctrl[dev_num] == -1) ||
		((in32(dra74x_uart_clkctrl[dev_num]) & CLKCTRL_MODULEMODE_MASK) == CLKCTRL_MODULEMODE_DISABLED)) {
		return 0;
	}

	if (in32(dra74x_uart_clkctrl[dev_num]) & CLKCTRL_CLKSEL_MSK) {
		return CLOCK_192MHz;
	} else {
		return CLOCK_48MHz;
	}
}

void init_mcspi_clock(int dev_num)
{
	out32(dra74x_mcspi_clkctrl[dev_num], CLKCTRL_MODULEMODE_ENABLED);
	check_clkctrl_idlest_functional(dra74x_mcspi_clkctrl[dev_num], "MCSPI");
}

static int mcspi_clock_enabled(int dev_num)
{
	if ((dra74x_mcspi_clkctrl[dev_num] == -1) ||
		((in32(dra74x_mcspi_clkctrl[dev_num]) & CLKCTRL_MODULEMODE_MASK) == CLKCTRL_MODULEMODE_DISABLED)) {
		return 0;
	}
	return 1;
}

void init_mcasp_clock(int dev_num)
{
	sr32(dra74x_mcasp_clkctrl[dev_num], CLKCTRL_MODULEMODE_OFFSET, CLKCTRL_MODULEMODE_WIDTH, CLKCTRL_MODULEMODE_ENABLED);
	check_clkctrl_idlest_functional(dra74x_mcasp_clkctrl[dev_num], "MCASP");
}

/* Return the AUX clk */
unsigned get_mcasp_clock(int dev_num)
{
	uint32_t	val, freq = 0;
	static const char *ahclkx_name[16] = { "ABE_24M_GFCLK", "ABE_SYS_CLK", "FUNC_24M_GFCLK", "ATL CLK3",
			"ATL CLK2", "ATL CLK1", "ATL CLK0", "SYS_CLK2",
			"XREF_CLK0", "XREF_CLK1", "XREF_CLK2", "XREF_CLK3",
			"MLB_CLK", "MLBP_CLK", "RESERVED", "RESERVED" };
	static const char *aux_name[4] = { "PER_ABE_X1_GFCLK", "VIDEO1 CLK", "HDMI CLK", "VIDEO2 CLK" };

	val = in32(dra74x_mcasp_clkctrl[dev_num]);

	kprintf("McASP%d clock        : ", dev_num);
	if ((val & CLKCTRL_MODULEMODE_MASK) == CLKCTRL_MODULEMODE_DISABLED) {
		kprintf("Disabled\n");
		return 0;
	}

	kprintf("AHCLKX(%s), ", ahclkx_name[(val & CM_MCASP_CLKSEL_AHCLKX_MSK) >> CM_MCASP_CLKSEL_AHCLKX_OFF]);
	kprintf("AUX_CLK(%s)", aux_name[(val & CM_MCASP_CLKSEL_AUX_MSK) >> CM_MCASP_CLKSEL_AUX_OFF]);

	/* We assume it's from PER_ABE only */
	if ((val & CM_MCASP_CLKSEL_AUX_MSK) == CM_MCASP_CLKSEL_AUX_ABE) {
		freq = calc_dpll_post_freq_m2(DPLL_ABE);
		freq = freq >> in32(DRA74X_CM_CLKSEL_BASE + CM_CLKSEL_PER_ABE_X1_GFCLK_MCASP_AUX);
		kprintf(" %dHz\n", freq);
	} else {
		kprintf("\n");
	}

	return freq;
}

void init_mmc_clock(int dev_num, int mode)
{
	out32(dra74x_mmc_clkctrl[dev_num], mode | CLKCTRL_MODULEMODE_ENABLED);
	check_clkctrl_idlest_functional(dra74x_mmc_clkctrl[dev_num], "MMC");
}

unsigned get_mmcsd_clock(int dev_num)
{
	uint32_t	freq = CLOCK_192MHz;
	uint32_t	val;

	val = in32(dra74x_mmc_clkctrl[dev_num]);

	if ((val & CLKCTRL_MODULEMODE_MASK) == CLKCTRL_MODULEMODE_DISABLED) {
		return 0;
	}

	/* MMC clock parents are different for MMC1/MMC2 versus MMC3/MMC4 */
	if (!(val & CLKCTRL_CLKSEL_MSK)) {
		if ((dev_num == 1) || (dev_num == 2))
			freq = CLOCK_128MHz;
		else
			freq = CLOCK_48MHz;
	}

	freq = freq >> ((val & CLKCTRL_CLKSEL_DIV_MSK) >> CLKCTRL_CLKSEL_DIV_OFF);

	return freq;
}

void init_gmac_clock()
{
	sr32(prcm_base + CM_GMAC_GMAC_CLKCTRL, CLKCTRL_MODULEMODE_OFFSET, CLKCTRL_MODULEMODE_WIDTH, CLKCTRL_MODULEMODE_ENABLED);
	check_clkctrl_idlest_functional(prcm_base + CM_GMAC_GMAC_CLKCTRL, "GMAC");
	out32(prcm_base + CM_GMAC_CLKSTCTRL, CLKSTCTRL_CLKTRCTRL_HW_AUTO);
}

static int gmac_clock_enabled()
{
	if ((in32(prcm_base + CM_GMAC_GMAC_CLKCTRL) & CLKCTRL_MODULEMODE_MASK) == CLKCTRL_MODULEMODE_DISABLED) {
		return 0;
	}

	return 1;
}

void init_gpio_clock(int dev_num, int mode)
{
	out32(dra74x_gpio_clkctrl[dev_num], mode | CLKCTRL_MODULEMODE_AUTO);
	check_clkctrl_idlest_functional(dra74x_gpio_clkctrl[dev_num], "GPIO");
}

static int gpio_clock_enabled(int dev_num)
{
	if ((dra74x_gpio_clkctrl[dev_num] == -1) ||
		((in32(dra74x_gpio_clkctrl[dev_num]) & CLKCTRL_MODULEMODE_MASK) == CLKCTRL_MODULEMODE_DISABLED)) {
		return 0;
	}
	return 1;
}

void init_qspi_clock()
{
	/* QSPI clocks, functional clk derived from PER_QSPI_CLK*/
	out32(prcm_base + CM_L4PER2_QSPI_CLKCTRL, CLKCTRL_MODULEMODE_ENABLED | (1 << CLKCTRL_CLKSEL_OFFSET));
	check_clkctrl_idlest_functional(prcm_base + CM_L4PER2_QSPI_CLKCTRL, "QSPI");
}

unsigned get_qspi_clock()
{
	uint32_t val;
	uint32_t freq = 0;

	val = in32(prcm_base + CM_L4PER2_QSPI_CLKCTRL);

	if ((val & CLKCTRL_MODULEMODE_MASK) == CLKCTRL_MODULEMODE_DISABLED) {
		return 0;
	}

	if (!(val & CLKCTRL_CLKSEL_MSK)) {
		freq = CLOCK_128MHz;
	} else {
		/* PER_DPLL H13 */
		freq = calc_dpll_post_freq_h13(DPLL_PER);
	}

	freq = freq >> ((val & CLKCTRL_CLKSEL_DIV_MSK) >> CLKCTRL_CLKSEL_DIV_OFF);

	return freq;
}

void init_kdb_clock()
{
	out32(cm_wkupaon_base + CM_WKUPAON_KBD_CLKCTRL, CLKCTRL_MODULEMODE_ENABLED);
	check_clkctrl_idlest_functional(cm_wkupaon_base + CM_WKUPAON_KBD_CLKCTRL, "KBD");
}

void init_gpmc_clock()
{
	out32(prcm_base + CM_L3MAIN1_GPMC_CLKCTRL, CLKCTRL_MODULEMODE_AUTO);
	check_clkctrl_idlest_functional(prcm_base + CM_L3MAIN1_GPMC_CLKCTRL, "GPMC");
}

static int gpmc_clock_enabled()
{
	if ((in32(prcm_base + CM_L3MAIN1_GPMC_CLKCTRL) & CLKCTRL_MODULEMODE_MASK) == CLKCTRL_MODULEMODE_DISABLED) {
		return 0;
	}
	return 1;
}

void init_vin_clocks(int dev_num)
{
	out32(prcm_base + CM_CAM_CLKSTCTRL, CLKSTCTRL_CLKTRCTRL_SW_WKUP);

	/* VIN1 */
	if (dev_num & VIP_DEVICE_1) {
		out32(prcm_base + CM_CAM_VIP1_CLKCTRL, CLKCTRL_MODULEMODE_AUTO);
		check_clkctrl_idlest_functional(prcm_base + CM_CAM_VIP1_CLKCTRL, "VIP1");
	}

	/* VIN2 */
	if (dev_num & VIP_DEVICE_2) {
		out32(prcm_base + CM_CAM_VIP2_CLKCTRL, CLKCTRL_MODULEMODE_AUTO);
		check_clkctrl_idlest_functional(prcm_base + CM_CAM_VIP2_CLKCTRL, "VIP2");
	}

	/* VIN3 */
	if (dev_num & VIP_DEVICE_3) {
		out32(prcm_base + CM_CAM_VIP3_CLKCTRL, CLKCTRL_MODULEMODE_AUTO);
		check_clkctrl_idlest_functional(prcm_base + CM_CAM_VIP3_CLKCTRL, "VIP3");
	}

	out32(prcm_base + CM_CAM_CLKSTCTRL, CLKSTCTRL_CLKTRCTRL_NO_SLEEP);
}

static int vip_clock_enabled(int dev_num)
{
	if ((dra74x_vip_clkctrl[dev_num] == -1) ||
		((in32(dra74x_vip_clkctrl[dev_num]) & CLKCTRL_MODULEMODE_MASK) == CLKCTRL_MODULEMODE_DISABLED)) {
		return 0;
	}
	return 1;
}

void init_vpe_clocks()
{
	out32(prcm_base + CM_VPE_CLKSTCTRL, CLKSTCTRL_CLKTRCTRL_SW_WKUP);

	out32(prcm_base + CM_VPE_VPE_CLKCTRL, CLKCTRL_MODULEMODE_AUTO);
	check_clkctrl_idlest_functional(prcm_base + CM_VPE_VPE_CLKCTRL, "VPE");

	out32(prcm_base + CM_VPE_CLKSTCTRL, CLKSTCTRL_CLKTRCTRL_NO_SLEEP);
}

static int vpe_clock_enabled()
{
	if ((in32(prcm_base + CM_VPE_VPE_CLKCTRL) & CLKCTRL_MODULEMODE_MASK) == CLKCTRL_MODULEMODE_DISABLED) {
		return 0;
	}
	return 1;
}

void init_hdq_clock()
{
	out32(prcm_base + CM_L4PER_HDQ1W_CLKCTRL, CLKCTRL_MODULEMODE_ENABLED);
	check_clkctrl_idlest_functional(prcm_base + CM_L4PER_HDQ1W_CLKCTRL, "HDQ1W");
}

void init_graphics_clocks(void)
{
	/* Enable DSS clocks */
	out32(prcm_base + CM_DSS_CLKSTCTRL, CLKSTCTRL_CLKTRCTRL_SW_WKUP);
	out32(prcm_base + CM_DSS_DSS_CLKCTRL, CM_DSS_DSS_CLKCTRL_ENABLED);
	out32(prcm_base + CM_DSS_BB2D_CLKCTRL, CLKCTRL_MODULEMODE_ENABLED);
	check_clkctrl_idlest_disabled(prcm_base + CM_DSS_DSS_CLKCTRL, "CM_DSS_DSS_CLKCTRL");
	check_clkctrl_idlest_disabled(prcm_base + CM_DSS_BB2D_CLKCTRL, "CM_DSS_BB2D_CLKCTRL");
	out32(prcm_base + CM_DSS_CLKSTCTRL, CLKSTCTRL_CLKTRCTRL_NO_SLEEP);
}

void init_gpu_clock(uint32_t source)
{
	/* Enable SGX clocks */
	out32(prcm_base + CM_GPU_CLKSTCTRL, CLKSTCTRL_CLKTRCTRL_SW_WKUP);
	out32(prcm_base + CM_GPU_GPU_CLKCTRL, CLKCTRL_MODULEMODE_ENABLED | source);
	check_clkctrl_idlest_disabled(prcm_base + CM_GPU_GPU_CLKCTRL, "CM_GPU_GPU_CLKCTRL");
}

void init_dcan_clock(int dev_num)
{
	if (dev_num == 1) {
		out32(cm_wkupaon_base + CM_WKUPAON_DCAN1_CLKCTRL, CLKCTRL_MODULEMODE_ENABLED);
		check_clkctrl_idlest_functional(cm_wkupaon_base + CM_WKUPAON_DCAN1_CLKCTRL, "DCAN1");
	}

	if (dev_num == 2) {
		out32(prcm_base + CM_L4PER2_DCAN2_CLKCTRL, CLKCTRL_MODULEMODE_ENABLED);
		check_clkctrl_idlest_functional(prcm_base + CM_L4PER2_DCAN2_CLKCTRL, "DCAN2");
	}
}

void init_atl_clock(int source1, int source2)
{
	/* Configure ATL clocks */
	out32(prcm_base + CM_ATL_CLKSTCTRL, CLKSTCTRL_CLKTRCTRL_SW_WKUP);

	if (source1 != -1)
		sr32(prcm_base + CM_ATL_ATL_CLKCTRL, CLKSEL_SOURCE1_OFF, CLKSEL_SOURCE_WIDTH, source1);

	if (source2 != -1)
		sr32(prcm_base + CM_ATL_ATL_CLKCTRL, CLKSEL_SOURCE2_OFF, CLKSEL_SOURCE_WIDTH, source2);

	sr32(prcm_base + CM_ATL_ATL_CLKCTRL, CLKCTRL_MODULEMODE_OFFSET, CLKCTRL_MODULEMODE_WIDTH, CLKCTRL_MODULEMODE_ENABLED);
	check_clkctrl_idlest_functional(prcm_base + CM_ATL_ATL_CLKCTRL, "ATL");
}

void init_usb_ss_clock(int dev_num, int mode)
{
	out32(dra74x_usb_ss_clkctrl[dev_num], CLKCTRL_MODULEMODE_AUTO | mode);
	check_clkctrl_idlest_functional(dra74x_usb_ss_clkctrl[dev_num], "USB SS");
}

void init_usb_phy_clock(int dev_num)
{
	out32(dra74x_usb_phy_clkctrl[dev_num], CLKCTRL_OPTCLKEN_ENABLED);
	check_clkctrl_idlest_functional(dra74x_usb_phy_clkctrl[dev_num], "USB PHY");
}

void init_basic_clocks(void)
{
	/*
	 * CORE_CLK: CORE_X2_CLK, 266MHz
	 * L3_ICLK: CORE_CLK/2 = 133MHz
	 * L4_ICLK: L3_ICLK/2 = 66.5MHz
	 */
	out32(prcm_base + CM_CLKSEL_CORE, (0 << CLKSEL_SHIFT) | (1 << CLKSEL_L3_SHIFT) | (1 << CLKSEL_L4_SHIFT));

	/* DPLL IVA bypass clock is CORE_X2_CLK divided by 2: 133MHz */
	out32(prcm_base + CM_BYPCLK_DPLL_IVA, (1 << CLKSEL_SHIFT));

	/* Enable ivahd and sl2 clocks */
	out32(prcm_base + CM_IVA_IVAHD_CLKCTRL, CLKCTRL_MODULEMODE_AUTO);
	out32(prcm_base + CM_IVA_SL2_CLKCTRL, CLKCTRL_MODULEMODE_AUTO);
	out32(prcm_base + CM_IVA_CLKSTCTRL, CLKSTCTRL_CLKTRCTRL_SW_WKUP);
	check_clkstctrl_activity(prcm_base + CM_IVA_CLKSTCTRL, "IVA");

	/* Enable DSP1 clocks */
	out32(prcm_base + CM_DSP1_DSP1_CLKCTRL, CLKCTRL_MODULEMODE_AUTO);
	out32(prcm_base + CM_DSP1_CLKSTCTRL, CLKSTCTRL_CLKTRCTRL_SW_WKUP);
	check_clkstctrl_activity(prcm_base + CM_DSP1_CLKSTCTRL, "DSP1");

	/* Enable DSP2 clocks */
	out32(prcm_base + CM_DSP2_DSP2_CLKCTRL, CLKCTRL_MODULEMODE_AUTO);
	out32(prcm_base + CM_DSP2_CLKSTCTRL, CLKSTCTRL_CLKTRCTRL_SW_WKUP);
	check_clkstctrl_activity(prcm_base + CM_DSP2_CLKSTCTRL, "DSP2");

	/* Enable EVE1 clocks */
	out32(prcm_base + CM_EVE1_EVE1_CLKCTRL, CLKCTRL_MODULEMODE_AUTO);
	out32(prcm_base + CM_EVE1_CLKSTCTRL, CLKSTCTRL_CLKTRCTRL_SW_WKUP);
	check_clkstctrl_activity(prcm_base + CM_EVE1_CLKSTCTRL, "EVE1");

	/* Enable EVE2 clocks */
	out32(prcm_base + CM_EVE2_EVE2_CLKCTRL, CLKCTRL_MODULEMODE_AUTO);
	out32(prcm_base + CM_EVE2_CLKSTCTRL, CLKSTCTRL_CLKTRCTRL_SW_WKUP);
	check_clkstctrl_activity(prcm_base + CM_EVE2_CLKSTCTRL, "EVE2");

	/* L4PER and L3 clocks */
	out32(prcm_base + CM_L4PER_CLKSTCTRL, CLKCTRL_MODULEMODE_ENABLED);
	out32(prcm_base + CM_L3INSTR_L3_MAIN_2_CLKCTRL, CLKCTRL_MODULEMODE_AUTO);
	out32(prcm_base + CM_L3INSTR_L3_INSTR_CLKCTRL, CLKCTRL_MODULEMODE_AUTO);

	/* WD Timer clocks */
	out32(cm_wkupaon_base + CM_WKUPAON_WD_TIMER2_CLKCTRL, CLKCTRL_MODULEMODE_ENABLED);

	/* 32K Counter */
	out32(cm_wkupaon_base + CM_WKUPAON_COUNTER_32K_CLKCTRL, CLKCTRL_MODULEMODE_AUTO);

	/* MLB */
	out32(prcm_base + CM_L3INIT_MLB_SS_CLKCTRL, CLKCTRL_MODULEMODE_ENABLED);

	/* DMA */
	out32(prcm_base + CM_DMA_CLKSTCTRL, CLKSTCTRL_CLKTRCTRL_NO_SLEEP);

	/* EMIF */
	out32(prcm_base + CM_EMIF_CLKSTCTRL, CLKSTCTRL_CLKTRCTRL_SW_WKUP);
	out32(prcm_base + CM_EMIF_EMIF1_CLKCTRL, CLKCTRL_MODULEMODE_AUTO);
	out32(prcm_base + CM_EMIF_EMIF2_CLKCTRL, CLKCTRL_MODULEMODE_AUTO);

	/* Shared PHY */
	out32(prcm_base + CM_L3INIT_OCP2SCP1_CLKCTRL, 1);
	out32(prcm_base + CM_L3INIT_OCP2SCP3_CLKCTRL, 1);
	check_clkctrl_idlest_functional(prcm_base + CM_L3INIT_OCP2SCP1_CLKCTRL, "OCP2SCP1");
	check_clkctrl_idlest_functional(prcm_base + CM_L3INIT_OCP2SCP3_CLKCTRL, "OCP2SCP3");
}

void init_edma_clock()
{
	uintptr_t cm_base = DRA74X_PRCM_BASE;

	out32(cm_base + CM_L3MAIN1_TPCC_CLKCTRL, CLKCTRL_MODULEMODE_AUTO);
	out32(cm_base + CM_L3MAIN1_TPTC1_CLKCTRL, CLKCTRL_MODULEMODE_AUTO);
	out32(cm_base + CM_L3MAIN1_TPTC2_CLKCTRL, CLKCTRL_MODULEMODE_AUTO);
	check_clkctrl_idlest_functional(cm_base + CM_L3MAIN1_TPCC_CLKCTRL, "TPCC");
	check_clkctrl_idlest_functional(cm_base + CM_L3MAIN1_TPTC1_CLKCTRL, "TPTC1");
	check_clkctrl_idlest_functional(cm_base + CM_L3MAIN1_TPTC1_CLKCTRL, "TPTC2");
}

void dump_clocks()
{
	unsigned freq;
	int i;

	if (debug_flag) {
		kprintf("\n");
		kprintf("SYS_CLK1                : %dHz\n", get_sys_clkspeed());
		kprintf("SYS_CLK2                : %dHz\n", get_sys_clk2_freq());
		kprintf("\n");

		kprintf("DPLL_MPU                : %dHz\n", calc_dpll_freq(DPLL_MPU));
		kprintf("    DPLL_MPU (M2)       : %dHz\n", calc_dpll_post_freq_m2(DPLL_MPU));
		kprintf("DPLL_DDR                : %dHz\n", calc_dpll_freq(DPLL_DDR));
		kprintf("    DPLL_DDR (M2)       : %dHz\n", calc_dpll_post_freq_m2(DPLL_DDR));
		kprintf("    DPLL_DDR (H11)      : %dHz\n", calc_dpll_post_freq_h11(DPLL_DDR));
		kprintf("DPLL_PER                : %dHz\n", calc_dpll_freq(DPLL_PER));
		kprintf("    DPLL_PER (M2)       : %dHz\n", calc_dpll_post_freq_m2(DPLL_PER));
		kprintf("    DPLL_PER (X2_M2)    : %dHz\n", calc_dpll_post_freq_m2(DPLL_PER) * 2);
		kprintf("    DPLL_PER (H11)      : %dHz\n", calc_dpll_post_freq_h11(DPLL_PER));
		kprintf("    DPLL_PER (H12)      : %dHz\n", calc_dpll_post_freq_h12(DPLL_PER));
		kprintf("    DPLL_PER (H13)      : %dHz\n", calc_dpll_post_freq_h13(DPLL_PER));
		kprintf("    DPLL_PER (H14)      : %dHz\n", calc_dpll_post_freq_h14(DPLL_PER));
		kprintf("DPLL_CORE               : %dHz\n", calc_dpll_freq(DPLL_CORE));
		kprintf("    DPLL_CORE (M2)      : %dHz\n", calc_dpll_post_freq_m2(DPLL_CORE));
		kprintf("    DPLL_CORE (H12)     : %dHz\n", calc_dpll_post_freq_h12(DPLL_CORE));
		kprintf("    DPLL_CORE (H13)     : %dHz\n", calc_dpll_post_freq_h13(DPLL_CORE));
		kprintf("    DPLL_CORE (H14)     : %dHz\n", calc_dpll_post_freq_h14(DPLL_CORE));
		kprintf("    DPLL_CORE (H21)     : %dHz\n", calc_dpll_post_freq_h21(DPLL_CORE));
		kprintf("    DPLL_CORE (H22)     : %dHz\n", calc_dpll_post_freq_h22(DPLL_CORE));
		kprintf("    DPLL_CORE (H23)     : %dHz\n", calc_dpll_post_freq_h23(DPLL_CORE));
		kprintf("    DPLL_CORE (H24)     : %dHz\n", calc_dpll_post_freq_h24(DPLL_CORE));
		kprintf("DPLL_ABE                : %dHz\n", calc_dpll_freq(DPLL_ABE));
		kprintf("    DPLL_ABE (M2)       : %dHz\n", calc_dpll_post_freq_m2(DPLL_ABE));
		kprintf("    DPLL_ABE (X2_M2)    : %dHz\n", calc_dpll_post_freq_m2(DPLL_ABE) * 2);
		kprintf("    DPLL_ABE (X2_M3)    : %dHz\n", calc_dpll_post_freq_m3(DPLL_ABE));
		kprintf("DPLL_IVA                : %dHz\n", calc_dpll_freq(DPLL_IVA));
		kprintf("    DPLL_IVA (M2)       : %dHz\n", calc_dpll_post_freq_m2(DPLL_IVA));
		kprintf("DPLL_EVE                : %dHz\n", calc_dpll_freq(DPLL_EVE));
		kprintf("    DPLL_EVE (M2)       : %dHz\n", calc_dpll_post_freq_m2(DPLL_EVE));
		kprintf("DPLL_DSP                : %dHz\n", calc_dpll_freq(DPLL_DSP));
		kprintf("    DPLL_DSP (M2)       : %dHz\n", calc_dpll_post_freq_m2(DPLL_DSP));
		kprintf("    DPLL_DSP (M3)       : %dHz\n", calc_dpll_post_freq_m3(DPLL_DSP));
		kprintf("DPLL_GMAC               : %dHz\n", calc_dpll_freq(DPLL_GMAC));
		kprintf("    DPLL_GMAC (M2)      : %dHz\n", calc_dpll_post_freq_m2(DPLL_GMAC));
		kprintf("    DPLL_GMAC (X2_M3)   : %dHz\n", calc_dpll_post_freq_m3(DPLL_GMAC));
		kprintf("    DPLL_GMAC (H11)     : %dHz\n", calc_dpll_post_freq_h11(DPLL_GMAC));
		kprintf("    DPLL_GMAC (H12)     : %dHz\n", calc_dpll_post_freq_h12(DPLL_GMAC));
		kprintf("    DPLL_GMAC (H13)     : %dHz\n", calc_dpll_post_freq_h13(DPLL_GMAC));
		kprintf("    DPLL_GMAC (H14)     : %dHz\n", calc_dpll_post_freq_h14(DPLL_GMAC));
		kprintf("DPLL_GPU                : %dHz\n", calc_dpll_freq(DPLL_GPU));
		kprintf("    DPLL_GPU (M2)       : %dHz\n", calc_dpll_post_freq_m2(DPLL_GPU));
		kprintf("DPLL_USB                : %dHz\n", calc_dpll_freq(DPLL_USB));
		kprintf("    DPLL_USB (CLKOUT)   : %dHz\n", calc_type_b_dpll_freq_clkout(DPLL_USB));
		kprintf("    DPLL_USB (CLKDCOLDO): %dHz\n", calc_dpll_freq(DPLL_USB) / 2);
		kprintf("DPLL_PCIE_REF           : %dHz\n", calc_dpll_freq(DPLL_PCIE));
		kprintf("    DPLL_PCIE (CLKOUT)   : %dHz\n", calc_type_b_dpll_freq_clkout(DPLL_PCIE));
		kprintf("    DPLL_PCIE (CLKDCOLDO): %dHz\n", calc_dpll_freq(DPLL_PCIE) / 2);

		kprintf("\n");

		if (debug_flag > 1) {
			for (i = 1; i <= 4; i++) {
				if ((freq = get_mmcsd_clock(i)) != 0) {
					kprintf("MMC%d clock          : %dHz\n", i, freq);
				} else {
					kprintf("MMC%d clock          : Disabled\n", i);
				}
			}

			for (i = 1; i <= 8; i++) {
				get_mcasp_clock(i);
			}

			for (i = 1; i <= 10; i++) {
				if ((freq = get_uart_clock(i)) != 0) {
					kprintf("UART%d clock         : %dHz\n", i, freq);
				} else {
					kprintf("UART%d clock         : Disabled\n", i);
				}
			}

			kprintf("QSPI clock          : %dHz\n", get_qspi_clock());
			kprintf("GMAC clock          : %s\n", gmac_clock_enabled() ? "Enabled" : "Disabled");
			kprintf("GPMC clock          : %s\n", gpmc_clock_enabled() ? "Enabled" : "Disabled");

			for (i = 1; i <= 16; i++) {
				kprintf("GP Timer%d clock     : %s\n", i, gptimer_clock_enabled(i) ? "Enabled" : "Disabled");
			}

			for (i = 1; i <= 5; i++) {
				kprintf("I2C%d clock          : %s\n", i, i2c_clock_enabled(i) ? "Enabled" : "Disabled");
			}

			for (i = 1; i <= 4; i++) {
				kprintf("SPI%d clock          : %s\n", i, mcspi_clock_enabled(i) ? "Enabled" : "Disabled");
			}

			for (i = 1; i <= 8; i++) {
				kprintf("GPIO%d clock         : %s\n", i, gpio_clock_enabled(i) ? "Enabled" : "Disabled");
			}

			for (i = 1; i <= 3; i++) {
				kprintf("VIP%d clock          : %s\n", i, vip_clock_enabled(i) ? "Enabled" : "Disabled");
			}

			kprintf("VPE clock           : %s\n", vpe_clock_enabled() ? "Enabled" : "Disabled");

			kprintf("\n");
		}
	}
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/dra74x/init_clocks.c $ $Rev: 768019 $")
#endif
