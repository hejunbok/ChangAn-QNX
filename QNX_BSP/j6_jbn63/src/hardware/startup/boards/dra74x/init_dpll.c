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

#define NUM_SYS_CLKS				7

struct dpll_params {
	unsigned	m;
	unsigned	n;
	char		m2;
	char		m3;
	char		h11;
	char		h12;
	char		h13;
	char		h14;
	char		h21;
	char		h22;
	char		h23;
	char		h24;
};

static const struct dpll_params ddr_dpll_params_2128mhz[NUM_SYS_CLKS] = {
	{266, 2, 2, 1, 8, -1, -1, -1, -1, -1, -1, -1},		/* 12 MHz	*/
	{266, 4, 2, 1, 8, -1, -1, -1, -1, -1, -1, -1},		/* 20 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{665, 11, 2, 1, 8, -1, -1, -1, -1, -1, -1, -1},		/* 19.2 MHz */
	{532, 12, 2, 1, 8, -1, -1, -1, -1, -1, -1, -1},		/* 26 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{665, 23, 2, 1, 8, -1, -1, -1, -1, -1, -1, -1}		/* 38.4 MHz */
};

static const struct dpll_params mpu_dpll_params_1ghz[NUM_SYS_CLKS] = {
	{250, 2, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1},		/* 12 MHz	*/
	{500, 9, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1},		/* 20 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{625, 11, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 19.2 MHz */
	{500, 12, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 26 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{625, 23, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1}		/* 38.4 MHz */
};

static const struct dpll_params mpu_dpll_params_1176mhz[NUM_SYS_CLKS] = {
	{294, 2, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1},		/* 12 MHz	*/
	{294, 4, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1},		/* 20 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{735, 11, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 19.2 MHz */
	{588, 12, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 26 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{735, 23, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1}		/* 38.4 MHz */
};

static const struct dpll_params mpu_dpll_params_1500mhz[NUM_SYS_CLKS] = {
	{375, 2, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1},		/* 12 MHz	*/
	{750, 9, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1},		/* 20 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{625, 7, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1},		/* 19.2 MHz */
	{750, 12, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 26 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{625, 15, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1}		/* 38.4 MHz */
};

static const struct dpll_params core_dpll_params_2128mhz_j6[NUM_SYS_CLKS] = {
	{266, 2, 2, 1, -1, 4, 62, 5, -1, 5, 4, 6},			/* 12 MHz	*/
	{266, 4, 2, 1, -1, 4, 62, 5, -1, 5, 4, 6},			/* 20 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{277, 4, 2, 1, -1, 4, 62, 5, -1, 5, 4, 6},			/* 19.2 MHz */
	{368, 8, 2, 1, -1, 4, 62, 5, -1, 5, 4, 6},			/* 26 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{277, 9, 2, 1, -1, 4, 62, 5, -1, 5, 4, 6}			/* 38.4 MHz */
};

static const struct dpll_params iva_dpll_params_2330mhz_j6_high[NUM_SYS_CLKS] = {
	{266, 2, 2, 1, -1, -1, -1, -1, -1, -1, -1, -1},		/* 12 MHz	*/
	{266, 4, 2, 1, -1, -1, -1, -1, -1, -1, -1, -1},		/* 20 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{665, 11, 2, 1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 19.2 MHz */
	{532, 12, 2, 1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 26 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{665, 23, 2, 1, -1, -1, -1, -1, -1, -1, -1, -1}		/* 38.4 MHz */
};

static const struct dpll_params per_dpll_params_768mhz_j6[NUM_SYS_CLKS] = {
	{32, 0, 4, 1, 3, 4, 4, 2, -1, -1, -1, -1},			/* 12 MHz	*/
	{96, 4, 4, 1, 3, 4, 4, 2, -1, -1, -1, -1},			/* 20 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{20, 0, 4, 1, 3, 4, 4, 2, -1, -1, -1, -1},			/* 19.2 MHz */
	{192, 12, 4, 1, 3, 4, 4, 2, -1, -1, -1, -1},		/* 26 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{10, 0, 4, 1, 3, 4, 4, 2, -1, -1, -1, -1}			/* 38.4 MHz */
};

static const struct dpll_params iva_dpll_params_2330mhz_j6[NUM_SYS_CLKS] = {
	{1165, 11, 3, 1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 12 MHz	*/
	{233, 3, 3, 1, -1, -1, -1, -1, -1, -1, -1, -1},		/* 20 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{182, 2, 3, 1, -1, -1, -1, -1, -1, -1, -1, -1},		/* 19.2 MHz */
	{224, 4, 3, 1, -1, -1, -1, -1, -1, -1, -1, -1},		/* 26 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{91, 2, 3, 1, -1, -1, -1, -1, -1, -1, -1, -1}		/* 38.4 MHz */
};

static const struct dpll_params usb_dpll_params_1920mhz[NUM_SYS_CLKS] = {
	{400, 4, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 12 MHz	*/
	{480, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 20 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{400, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 19.2 MHz */
	{480, 12, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 26 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{400, 15, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1}	/* 38.4 MHz */

};

static const struct dpll_params eve_dpll_params_2140mhz[NUM_SYS_CLKS] = {
	{535, 5, 2, 1, -1, -1, -1, -1, -1, -1, -1, -1},		/* 12 MHz	*/
	{214, 3, 2, 1, -1, -1, -1, -1, -1, -1, -1, -1},		/* 20 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{613, 10, 2, 1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 19.2 MHz */
	{535, 12, 2, 1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 26 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{613, 21, 2, 1, -1, -1, -1, -1, -1, -1, -1, -1}		/* 38.4 MHz */
};

static const struct dpll_params dsp_dpll_params_1200mhz[NUM_SYS_CLKS] = {
	{50, 0, 1, 3, -1, -1, -1, -1, -1, -1, -1, -1},		/* 12 MHz	*/
	{150, 4, 1, 3, -1, -1, -1, -1, -1, -1, -1, -1},		/* 20 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{125, 3, 1, 3, -1, -1, -1, -1, -1, -1, -1, -1},		/* 19.2 MHz */
	{300, 12, 1, 3, -1, -1, -1, -1, -1, -1, -1, -1},	/* 26 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{125, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1, -1}		/* 38.4 MHz */
};

static const struct dpll_params dsp_dpll_params_1400mhz[NUM_SYS_CLKS] = {
	{175, 2, 1, 3, -1, -1, -1, -1, -1, -1, -1, -1},		/* 12 MHz	*/
	{175, 4, 1, 3, -1, -1, -1, -1, -1, -1, -1, -1},		/* 20 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{875, 23, 1, 3, -1, -1, -1, -1, -1, -1, -1, -1},	/* 19.2 MHz */
	{350, 12, 1, 3, -1, -1, -1, -1, -1, -1, -1, -1},	/* 26 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{875, 47, 1, 3, -1, -1, -1, -1, -1, -1, -1, -1}		/* 38.4 MHz */
};

static const struct dpll_params gpu_dpll_params_1700mhz[NUM_SYS_CLKS] = {
	{425, 5, 2, 1, -1, -1, -1, -1, -1, -1, -1, -1},		/* 12 MHz	*/
	{170, 3, 2, 1, -1, -1, -1, -1, -1, -1, -1, -1},		/* 20 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{664, 14, 2, 1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 19.2 MHz */
	{425, 12, 2, 1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 26 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{819, 36, 2, 1, -1, -1, -1, -1, -1, -1, -1, -1}		/* 38.4 MHz */
};

static const struct dpll_params gpu_dpll_params_2128mhz[NUM_SYS_CLKS] = {
	{266, 2, 2, 1, -1, -1, -1, -1, -1, -1, -1, -1},		/* 12 MHz	*/
	{266, 4, 2, 1, -1, -1, -1, -1, -1, -1, -1, -1},		/* 20 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{277, 4, 2, 1, -1, -1, -1, -1, -1, -1, -1, -1},		/* 19.2 MHz */
	{368, 8, 2, 1, -1, -1, -1, -1, -1, -1, -1, -1},		/* 26 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{277, 9, 2, 1, -1, -1, -1, -1, -1, -1, -1, -1}		/* 38.4 MHz */
};

static const struct dpll_params gmac_dpll_params_2000mhz[NUM_SYS_CLKS] = {
	{250, 2, 4, 10, 40, 8, 10, -1, -1, -1, -1, -1},		/* 12 MHz	*/
	{250, 4, 4, 10, 40, 8, 10, -1, -1, -1, -1, -1},		/* 20 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{625, 11, 4, 10, 40, 8, 10, -1, -1, -1, -1, -1},	/* 19.2 MHz */
	{500, 12, 4, 10, 40, 8, 10, -1, -1, -1, -1, -1},	/* 26 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{625, 23, 4, 10, 40, 8, 10, -1, -1, -1, -1, -1}		/* 38.4 MHz */
};

static const struct dpll_params pcie_ref_dpllj_params_1500mhz[NUM_SYS_CLKS] = {
	{125, 0, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 12 MHz	*/
	{750, 9, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 20 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{625, 7, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 19.2 MHz */
	{750, 12, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 26 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{625, 15, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1}	/* 38.4 MHz */
};

static const struct dpll_params abe_dpll_params_sysclk1_361267khz[NUM_SYS_CLKS] = {
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 12 MHz	*/
	{16, 1, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1},		/* 20 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{16, 1, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1},		/* 19.2MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* 26 MHz	*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},	/* N/A		*/
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}	/* 38.4 MHz */
};

/* ABE M & N values with sysclk2 as input */
static const struct dpll_params abe_dpll_params_sysclk2_361267khz =
	{16, 1, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1};

/*
 * ABE M & N values with 32K clock as source
 * PER_ABE_X1_GFCLK = ABE_CLKOUT_M2 = 196608khz / 2 / M2 = 12288000
 */
static const struct dpll_params abe_dpll_params_32k_196608khz =
	{750, 0, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1};

static unsigned get_sysclk_index()
{
	return((in32(DRA74X_CM_CLKSEL_BASE + CM_CLKSEL_SYS) & SYS_CLKSEL_MASK) - 1);
}

/*****************************************************************
 * wait_for_match - wait for the bit mask to match the given value
 *****************************************************************/
static unsigned wait_for_match(unsigned bit_mask, unsigned match, unsigned addr, unsigned delay)
{
	unsigned count = 0, value;

	do {
		++count;
		value = in32(addr) & bit_mask;
		if (value == match) {
			return (1);
		}
		if (count == delay) {
			return (0);
		}
	} while (1);
}

#define OFF_DPLL_REG(r) (CM_##r##_DPLL_CORE - CM_CLKMODE_DPLL_CORE)
static unsigned check_for_lock(unsigned base)
{
	return (in32(base + OFF_DPLL_REG(IDLEST)) & ST_DPLL_CLK_MASK);
}

static void bypass_dpll(unsigned base)
{
	sr32(base + OFF_DPLL_REG(CLKMODE), 0, 3, DPLL_EN_FAST_RELOCK_BYPASS);
	wait_for_match(1 << 0, 0, base + OFF_DPLL_REG(IDLEST), LDELAY);
}

static void lock_dpll(unsigned base)
{
	sr32(base + OFF_DPLL_REG(CLKMODE), 0, 3, DPLL_EN_LOCK);
	if (0 == wait_for_match(1 << 0, 1, base + OFF_DPLL_REG(IDLEST), LDELAY)) {
		kprintf("DPLL lock failed for 0x%x\n", base);
	}
}

static void setup_post_dividers(unsigned base, const struct dpll_params *params)
{
	/* Setup post-dividers */
	if (params->m2 >= 0) {
		out32(base + OFF_DPLL_REG(DIV_M2), params->m2);
	}
	if (params->m3 >= 0) {
		out32(base + OFF_DPLL_REG(DIV_M3), params->m3);
	}
	if (params->h11 >= 0) {
		out32(base + OFF_DPLL_REG(DIV_H11), params->h11);
	}
	if (params->h12 >= 0) {
		out32(base + OFF_DPLL_REG(DIV_H12), params->h12);
	}
	if (params->h13 >= 0) {
		out32(base + OFF_DPLL_REG(DIV_H13), params->h13);
	}
	if (params->h14 >= 0) {
		out32(base + OFF_DPLL_REG(DIV_H14), params->h14);
	}
	if (params->h21 >= 0) {
		out32(base + OFF_DPLL_REG(DIV_H21), params->h21);
	}
	if (params->h22 >= 0) {
		out32(base + OFF_DPLL_REG(DIV_H22), params->h22);
	}
	if (params->h23 >= 0) {
		out32(base + OFF_DPLL_REG(DIV_H23), params->h23);
	}
	if (params->h24 >= 0) {
		out32(base + OFF_DPLL_REG(DIV_H24), params->h24);
	}
}

static void do_setup_dpll(unsigned base, const struct dpll_params *params, int lock, int dcc_en)
{
	unsigned	temp, M, N;

	temp = in32(base + OFF_DPLL_REG(CLKSEL));

	/* clear automation control over DPLL activity */
	sr32(base + OFF_DPLL_REG(AUTOIDLE), 0, 3, 0);

	if (check_for_lock(base)) {
		/*
		* The Dpll has already been locked by rom code using CH.
		* Check if M,N are matching with Ideal nominal opp values.
		* If matches, skip the rest otherwise relock.
		*/
		M = (temp & CM_CLKSEL_MULT_MASK) >> CM_CLKSEL_MULT_SHIFT;
		N = (temp & CM_CLKSEL_DPLL_DIV_MASK) >> CM_CLKSEL_DPLL_DIV_SHIFT;
		if ((M != (params->m)) || (N != (params->n))) {
			/* Dpll locked, but not for ideal M, N value */
		} else {
			/* Dpll locked with ideal values for nominal opps. */
			goto setup_post_dividers;
		}
	}

	bypass_dpll(base);

	/* Set M & N */
	temp &= ~CM_CLKSEL_MULT_MASK;
	temp |= (params->m << CM_CLKSEL_MULT_SHIFT) & CM_CLKSEL_MULT_MASK;

	temp &= ~CM_CLKSEL_DPLL_DIV_MASK;
	temp |= (params->n << CM_CLKSEL_DPLL_DIV_SHIFT) & CM_CLKSEL_DPLL_DIV_MASK;

	/* DCC_EN needs to be set for frequency higher than 1.4GHz */
	if (dcc_en) {
		temp |= (1 << CM_CLKSEL_DCC_EN_SHIFT);
	}

	out32(base + OFF_DPLL_REG(CLKSEL), temp);

	/* Lock */
	if (lock) {
		lock_dpll(base);
	}

setup_post_dividers:
	setup_post_dividers(base, params);
}

void setup_core_dpll()
{
	unsigned sysclk_ind = get_sysclk_index();
	const struct dpll_params *params = &core_dpll_params_2128mhz_j6[sysclk_ind];
	do_setup_dpll(DRA74X_PRCM_BASE + CM_CLKMODE_DPLL_CORE, params, 1, 0);
}

void setup_per_dpll()
{
	unsigned sysclk_ind = get_sysclk_index();
	const struct dpll_params *params = &per_dpll_params_768mhz_j6[sysclk_ind];
	do_setup_dpll(DRA74X_PRCM_BASE + CM_CLKMODE_DPLL_PER, params, 1, 0);
}

void setup_mpu_dpll_1ghz()
{
	unsigned sysclk_ind = get_sysclk_index();
	const struct dpll_params *params = &mpu_dpll_params_1ghz[sysclk_ind];
	do_setup_dpll(DRA74X_PRCM_BASE + CM_CLKMODE_DPLL_MPU, params, 1, 0);
}

void setup_mpu_dpll_1176mhz()
{
	unsigned sysclk_ind = get_sysclk_index();
	const struct dpll_params *params = &mpu_dpll_params_1176mhz[sysclk_ind];
	do_setup_dpll(DRA74X_PRCM_BASE + CM_CLKMODE_DPLL_MPU, params, 1, 0);
}

void setup_mpu_dpll_1500mhz()
{
	unsigned sysclk_ind = get_sysclk_index();
	const struct dpll_params *params = &mpu_dpll_params_1500mhz[sysclk_ind];
	do_setup_dpll(DRA74X_PRCM_BASE + CM_CLKMODE_DPLL_MPU, params, 1, 1);
}

void setup_iva_dpll(int iva_opp)
{
	unsigned sysclk_ind = get_sysclk_index();
	const struct dpll_params *params;

	/*
	 * We need to set IVA DPLL at OPP_HIGH (532Mhz) for video playback
	 * usecase to achieve 1080p@60fps
	 */
	if (iva_opp) {
		params = &iva_dpll_params_2330mhz_j6_high[sysclk_ind];
	} else {
		params = &iva_dpll_params_2330mhz_j6[sysclk_ind];
	}

	do_setup_dpll(DRA74X_PRCM_BASE + CM_CLKMODE_DPLL_IVA, params, 1, 0);
}

void setup_eve_dpll()
{
	unsigned sysclk_ind = get_sysclk_index();
	const struct dpll_params *params = &eve_dpll_params_2140mhz[sysclk_ind];
	do_setup_dpll(DRA74X_PRCM_BASE + CM_CLKMODE_DPLL_EVE, params, 1, 0);
}

void setup_dsp_dpll_600mhz()
{
	unsigned sysclk_ind = get_sysclk_index();
	const struct dpll_params *params = &dsp_dpll_params_1200mhz[sysclk_ind];
	do_setup_dpll(DRA74X_PRCM_BASE + CM_CLKMODE_DPLL_DSP, params, 1, 0);
}

void setup_dsp_dpll_700mhz()
{
	unsigned sysclk_ind = get_sysclk_index();
	const struct dpll_params *params = &dsp_dpll_params_1400mhz[sysclk_ind];
	do_setup_dpll(DRA74X_PRCM_BASE + CM_CLKMODE_DPLL_DSP, params, 1, 0);
}

void setup_gpu_dpll_1700mhz()
{
	unsigned sysclk_ind = get_sysclk_index();
	const struct dpll_params *params = &gpu_dpll_params_1700mhz[sysclk_ind];
	do_setup_dpll(DRA74X_PRCM_BASE + CM_CLKMODE_DPLL_GPU, params, 1, 0);
}

void setup_gpu_dpll_2128mhz()
{
	unsigned sysclk_ind = get_sysclk_index();
	const struct dpll_params *params = &gpu_dpll_params_2128mhz[sysclk_ind];
	do_setup_dpll(DRA74X_PRCM_BASE + CM_CLKMODE_DPLL_GPU, params, 1, 0);
}

void setup_gmac_dpll()
{
	unsigned sysclk_ind = get_sysclk_index();
	const struct dpll_params *params = &gmac_dpll_params_2000mhz[sysclk_ind];
	do_setup_dpll(DRA74X_PRCM_BASE + CM_CLKMODE_DPLL_GMAC, params, 1, 0);
}

void setup_pcie_dpll()
{
	unsigned sysclk_ind = get_sysclk_index();
	const struct dpll_params *params = &pcie_ref_dpllj_params_1500mhz[sysclk_ind];
	do_setup_dpll(DRA74X_PRCM_BASE + CM_CLKMODE_DPLL_PCIE_REF, params, 1, 0);
}

void setup_abe_dpll(int sys_clk)
{
	const struct dpll_params *params;
	unsigned sysclk_ind = get_sysclk_index();

	if (sys_clk == SYS_CLK1_INPUT) {
		/* sys_clk1 as input to ABE_DPLL_SYS_CLK */
		out32(DRA74X_CM_CLKSEL_BASE + CM_CLKSEL_ABE_PLL_SYS, 0);

		/* Selects ABE_DPLL_SYS_CLK for ABE_DPLL_CLK */
		out32(DRA74X_CM_CLKSEL_BASE + CM_CLKSEL_ABE_PLL_REF, 0);

		params = &abe_dpll_params_sysclk1_361267khz[sysclk_ind];
	} else if (sys_clk == SYS_CLK2_INPUT) {
		/* sys_clk2 as input to ABE_DPLL_SYS_CLK */
		out32(DRA74X_CM_CLKSEL_BASE + CM_CLKSEL_ABE_PLL_SYS, 1);

		/* Selects ABE_DPLL_SYS_CLK for ABE_DPLL_CLK */
		out32(DRA74X_CM_CLKSEL_BASE + CM_CLKSEL_ABE_PLL_REF, 0);
		params = &abe_dpll_params_sysclk2_361267khz;
	} else {
		/* Selects FUNC_32K_CLK for ABE_DPLL_CLK */
		out32(DRA74X_CM_CLKSEL_BASE + CM_CLKSEL_ABE_PLL_REF, 1);

		/* enable bits to acheive 196.608MHz from 32.768 KHz */
		sr32(DRA74X_PRCM_BASE + CM_CLKMODE_DPLL_ABE, 8, 4, 0xf);

		/* spend 4 REFCLK cycles at each stage */
		sr32(DRA74X_PRCM_BASE + CM_CLKMODE_DPLL_ABE, 5, 3, 1);

		params = &abe_dpll_params_32k_196608khz;
	}

	do_setup_dpll(DRA74X_PRCM_BASE + CM_CLKMODE_DPLL_ABE, params, 1, 0);
}

void setup_usb_dpll(void)
{
	const struct dpll_params *params;
	unsigned sys_clk_khz, sd_div, num, den;
	unsigned sysclk_ind = get_sysclk_index();

	sys_clk_khz = get_sys_clkspeed() / 1000;

	/*
	 * USB:
	 * USB dpll is J-type. Need to set DPLL_SD_DIV for jitter correction
	 * DPLL_SD_DIV = CEILING ([DPLL_MULT/(DPLL_DIV+1)]* CLKINP / 250)
	 *	 - where CLKINP is sys_clk in MHz
	 * Use CLKINP in KHz and adjust the denominator accordingly so
	 * that we have enough accuracy and at the same time no overflow
	 */
	params = &usb_dpll_params_1920mhz[sysclk_ind];
	num = params->m * sys_clk_khz;
	den = (params->n + 1) * 250 * 1000;
	num += den - 1;
	sd_div = num / den;
	sr32(DRA74X_PRCM_BASE + CM_CLKSEL_DPLL_USB, 24, 8, sd_div);

	/* Now setup the dpll with the regular function */
	do_setup_dpll(DRA74X_PRCM_BASE + CM_CLKMODE_DPLL_USB, params, 1, 0);
}

void setup_ddr_dpll(void)
{
	unsigned sysclk_ind = get_sysclk_index();
	const struct dpll_params *params = &ddr_dpll_params_2128mhz[sysclk_ind];
	do_setup_dpll(DRA74X_PRCM_BASE + CM_CLKMODE_DPLL_DDRPHY, params, 1, 0);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/dra74x/init_dpll.c $ $Rev: 768019 $")
#endif
