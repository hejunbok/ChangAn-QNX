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

/* Undocumented register for ES1.0 */
#define USB2_DUMMY_REG				0x4A0086C0

void init_usb3_phy()
{
	uint32_t scp_base = DRA74X_OCP2SCP1;
	uint32_t dpll_base = DRA74X_DPLLCTRL_USB_OTG_SS;
	uint32_t tx_base = DRA74X_USB3_PHY_TX;
	uint32_t val;

	/* USB3 need positive trimming offset to RTRIM, so increasing by 4 counts */
	val = in32(tx_base + USB3PHYTX_TRIM_OBSERVE_REG);
	val &= USB3PHYTX_TRIM_OBSERVE_REG_RTRIM_MASK << USB3PHYTX_TRIM_OBSERVE_REG_RTRIM_SHIFT;
	val = (val >> USB3PHYTX_TRIM_OBSERVE_REG_RTRIM_SHIFT) + 4;
	sr32(tx_base + USB3PHYTX_RTRIM_REG, USB3PHYTX_RTRIM_REG_MEM_RTRIM_SHIFT, 5, val);
	sr32(tx_base + USB3PHYTX_RTRIM_REG, USB3PHYTX_RTRIM_REG_MEM_OVRD_RTRIM_SHIFT, 1, 1);

	/* software reset OCP2SCP1 */
	sr32(scp_base + OCP2SCP_SYSCONFIG, OCP2SCP_SYSCONFIG_SOFTRESET_OFF, 1, 1);
	while ((in32(scp_base + OCP2SCP_SYSSTATUS) & OCP2SCP_SYSSTATUS_RESETDONE) == 0);

	/* set division ratio between OCP clock and SCP clock */
	/* Documentation says to leave it alone. */

	/* set sync1 and sync2 timing */
	/* Documentation says to set SYNC2 >= 6. */
	sr32(scp_base + OCP2SCP_TIMING, OCP2SCP_TIMING_SYN2_OFF, 4, 6);

	/* configure DPLL_USB_OTG_SS to generate 2.5GHz */

	/* DPLL rate = 20MHz * 1000	/ (7+1) = 2.5GHz */
	/* REFCLK rate = sysclk/(N+1) = 20MHz / 8 = 2.5MHz. */
	/* This must be between 0.62 and 2.5MHz */

	/* set N divisor for USB3 PHY DPLL */
	sr32(dpll_base + DPLL_CFG1, DPLL_CFG1_N_SHIFT, 8, 7);

	/* set SELFREQDCO field to indicate output range */
	sr32(dpll_base + DPLL_CFG2, DPLL_CFG2_SELFREQDCO_SHIFT, 3, 4);

	/* set M divisor for USB3 PHY DPLL */
	sr32(dpll_base + DPLL_CFG1, DPLL_CFG1_M_SHIFT, 12, 1000);
	sr32(dpll_base + DPLL_CFG4, DPLL_CFG4_REGM_F, 18, 0);

	/* configure SD divider */
	/* ceil(sysclk*M/(N+1)/256) = 10 */
	sr32(dpll_base + DPLL_CFG3, DPLL_CFG3_SD_SHIF, 8, 10);

	/* make it go */
	sr32(dpll_base + DPLL_GO, DPLL_GO_PLL_GO, 1, 1);

	/* wait for DPLL lock */
	while ((in32(dpll_base + DPLL_STATUS) & (DPLL_STATUS_LOCK << DPLL_STATUS_LOCK_SHIFT)) == 0);

	/* set phy ref clock and power up */
	out32(DRA74X_CTRL_CORE_BASE + CTRL_CORE_PHY_POWER_USB, (19 << PWRCTL_CLK_FREQ_SHIFT) | (3 << PWRCTL_CLK_CMD_SHIFT));
}

void init_usb1_otg(void)
{
	init_usb_ss_clock(1, CLKCTRL_OPTCLKEN_ENABLED);
	init_usb_phy_clock(1);

	/* select internal UTMI operation (default) */
	sr32(DRA74X_USB_DWC1 + USB_GUSB2PHYCFG, USB_GUSB2PHYCFG_ULPI_UTMI_SEL, 1, USB_GUSB2PHYCFG_ULPI_UTMI_SEL_UTML);

	/*
	 * Program USB2PHY register to adjust disconnect threshold to fix
	 * unintentional disconnect on USB2 link of USB3 port.
	 * Details are found in TI's Vayu Defect Report v 0.9 p.23 (AVATAR-00029)
	 */
	sr32(0x4a08404c, 31, 1, 1);
}

void init_usb2_otg(void)
{
	init_usb_ss_clock(2, CLKCTRL_OPTCLKEN_ENABLED);
	init_usb_phy_clock(2);

	/* USB2 needs this */
	sr32(USB2_DUMMY_REG, 8, 1, 1);

	/* select internal UTMI operation (default) */
	sr32(DRA74X_USB_DWC2 + USB_GUSB2PHYCFG, USB_GUSB2PHYCFG_ULPI_UTMI_SEL, 1, USB_GUSB2PHYCFG_ULPI_UTMI_SEL_UTML);

	/* power up USB2_PHY2 */
	sr32(DRA74X_CTRL_CORE_BASE + CTRL_CORE_SRCOMP_NORTH_SIDE, SRCOMP_NORTH_SIDE_USB2PHY_PD, 1, 0);

	/*
	 * Program USB2PHY register to adjust disconnect threshold to fix
	 * unintentional disconnect on USB2 link of USB3 port.
	 * Details are found in TI's Vayu Defect Report v 0.9 p.23 (AVATAR-00029)
	 */
	sr32(0x4a08504c, 31, 1, 1);
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/dra74x/init_usb_otg.c $ $Rev: 751557 $")
#endif
