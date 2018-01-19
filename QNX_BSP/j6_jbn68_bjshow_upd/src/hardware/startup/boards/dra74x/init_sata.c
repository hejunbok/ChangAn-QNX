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
 * DRA74X specific SATA support.
 */

#include "dra74x_startup.h"

#define SATA_PLL_CFG1_M					600
#define SATA_PLL_CFG1_N					7
#define SATA_PLL_CFG3_SD				6

void init_ocp2scp3()
{
	uint32_t base = DRA74X_OCP2SCP3;

	/*
	 * set the ocp2scp3 values;
	 * as per DRA74X TRM, division ratio should not modified
	 * SYNC2 should use the value 0x6 (TRM update on OCP2SCP Timing Register
	 * to accomodate AVATAR-BUG00055)
	 * SYNC1 is undefined; hence default value is used
	 */
	out32(base + OCP2SCP_TIMING, in32(base + OCP2SCP_TIMING) | 0x06);

	/* Reset, assuming ICLK has been enabled in init_clocks() */
	out32(base + OCP2SCP_SYSCONFIG, (1 << OCP2SCP_SYSCONFIG_SOFTRESET_OFF));
	while (!(in32(base + OCP2SCP_SYSSTATUS) & OCP2SCP_SYSSTATUS_RESETDONE));
}

static void init_sata_phy()
{
	uint32_t base = DRA74X_OCP2SCP3_SATA_PHY_RX;
	uint32_t reg;

	/*
	 * According to Preferred SATA_PHY_RX SCP Register Settings in J6's TRM
	 * 1.5GHz, spread-spectrum is ON
	 */
	out32(base + SATAPHYRX_ANA_PROGRAMMABILITY_REG1, 0x40014000);
	out32(base + SATAPHYRX_TRIM_REG4, 0x40000000);
	out32(base + SATAPHYRX_DLL_REG1, 0x80a41915);
	out32(base + SATAPHYRX_DIGITAL_MODES_REG1, 0x1ae33000);
	out32(base + SATAPHYRX_EQUALIZER_REG1, 0xf806);
	out32(base + SATAPHYRX_IO_AND_A2D_OVERRIDES_REG1, 0);

	/* ALDO programming for TX compliance & improve overall performance */
	base = DRA74X_OCP2SCP3_SATA_PHY_TX;
	reg = in32(base + SATAPHYTX_LDO_CONTROL_REG);
	reg &= ~(SATAPHYTX_LDO_CONTROL_REG_ALDO_MASK << SATAPHYTX_LDO_CONTROL_REG_ALDO_SHIFT);
	reg |= (0x8 << SATAPHYTX_LDO_CONTROL_REG_ALDO_SHIFT);
	out32(base + SATAPHYTX_LDO_CONTROL_REG, reg);

	// SATA phy power
	base = DRA74X_CTRL_CORE_BASE;
	out32(base + CTRL_CORE_PHY_POWER_SATA, (19 << PWRCTL_CLK_FREQ_SHIFT) | (3 << PWRCTL_CLK_CMD_SHIFT));
	out32(base + CTRL_CORE_SATA_EXT_MODE, SATA_EXTENDED_MODE);
}

void init_sata(void)
{
	uint32_t	reg;
	uint32_t	base;

	// SATA reference clock
	out32(DRA74X_PRCM_BASE + CM_L3INIT_SATA_CLKCTRL, CLKCTRL_MODULEMODE_ENABLED | CLKCTRL_OPTCLKEN_ENABLED);

	/*
	 * set the configuration 1; The phy clocks
	 * set M and N values for Phy clocks
	 */
	base = DRA74X_DPLLCTRL_SATA_BASE;
	reg = in32(base + DPLL_CFG1);
	reg &= ~((DPLL_CFG1_M_MASK << DPLL_CFG1_M_SHIFT) |
						(DPLL_CFG1_N_MASK << DPLL_CFG1_N_SHIFT));
	reg |= ((SATA_PLL_CFG1_M << DPLL_CFG1_M_SHIFT) |
						(SATA_PLL_CFG1_N << DPLL_CFG1_N_SHIFT));
	out32(base + DPLL_CFG1, reg);

	/* set the dco frequence range */
	reg = in32(base + DPLL_CFG2);
	reg &= ~(DPLL_CFG2_SELFREQDCO_MASK << DPLL_CFG2_SELFREQDCO_SHIFT);
	reg |= (DPLL_CFG2_SELFREQDCO << DPLL_CFG2_SELFREQDCO_SHIFT);
	out32(base + DPLL_CFG2, reg);

	reg = in32(base + DPLL_CFG3);
	reg &= ~(DPLL_CFG3_SD_MASK << DPLL_CFG3_SD_SHIF);
	reg |= (SATA_PLL_CFG3_SD << DPLL_CFG3_SD_SHIF);
	out32(base + DPLL_CFG3, reg);

	reg = in32(base + DPLL_CFG4);
	reg &= ~DPLL_CFG4_REGM_F_MASK;
	reg |= DPLL_CFG4_REGM_F;
	out32(base + DPLL_CFG4, reg);

	out32(base + DPLL_GO, 1);

	/* Poll for the PLL lock */
	while (!(in32(base + DPLL_STATUS) &
			(DPLL_STATUS_LOCK << DPLL_STATUS_LOCK_SHIFT)));

	init_sata_phy();

	// SATA, no standby, no idle
	out32(DRA74X_SATA_WRAPPER, 0x14);
	out32(DRA74X_SATA_BASE, in32(DRA74X_SATA_BASE));

	// perform all firmware capability writes
	base = DRA74X_SATA_BASE;

	sr32(base + SATA_CAP, 27, 1, 0);
	sr32(base + SATA_PI, 0, 1, 1);
	sr32(base + SATA_CAP, 28, 1, 0);
	sr32(base + SATA_PxCMD, 21, 2, 0);
	sr32(base + SATA_PxCMD, 20, 1, 0);
	sr32(base + SATA_PxCMD, 19, 1, 0);
	sr32(base + SATA_PxCMD, 18, 1, 0);
	sr32(base + SATA_PxCMD, 17, 1, 1);
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/dra74x/init_sata.c $ $Rev: 751557 $")
#endif
