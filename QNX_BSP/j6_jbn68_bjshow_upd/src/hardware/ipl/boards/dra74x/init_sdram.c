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

//EMIF1
const struct emif_regs emif_1_regs_ddr3_532_mhz_1cs_dra_es1 = {
	.sdram_config_init			= 0x61851B32,
	.sdram_config				= 0x61851B32,
	.sdram_config2				= 0x08000000,
	.ref_ctrl					= 0x00001035,
	/* REFRESH_RATE @ init is (500us x 532MHz)/16 */
	.ref_ctrl_init				= 0x000040F1,
	.sdram_tim1					= 0xCCCF36B3,
	.sdram_tim2					= 0x328F7FDA,
	.sdram_tim3					= 0x027F88A8,
	.read_idle_ctrl				= 0x00050000,
	.zq_config					= 0x0007190B,
	.temp_alert_config			= 0x00000000,
	.emif_ddr_phy_ctlr_1_init	= 0x0E20400A,
	.emif_ddr_phy_ctlr_1		= 0x0E24400A,
	.emif_ddr_ext_phy_ctrl_1	= 0x04040100,
	.emif_ddr_ext_phy_ctrl_2	= 0x008B008B,
	.emif_ddr_ext_phy_ctrl_3	= 0x00870087,
	.emif_ddr_ext_phy_ctrl_4	= 0x00910091,
	.emif_ddr_ext_phy_ctrl_5	= 0x00930093,
	.emif_rd_wr_lvl_rmp_win		= 0x00000000,
	.emif_rd_wr_lvl_rmp_ctl		= 0x80000000, //Tony: enable leveling other than gel file.
	.emif_rd_wr_lvl_ctl			= 0x00000000,
	.emif_rd_wr_exec_thresh		= 0x00000305
};

//EMIF2
const struct emif_regs emif_2_regs_ddr3_532_mhz_1cs_dra_es1 = {
	.sdram_config_init			= 0x61851B32,
	.sdram_config				= 0x61851B32,
	.sdram_config2				= 0x08000000,
	.ref_ctrl					= 0x00001035,
	/* REFRESH_RATE @ init is (500us x 532MHz)/16 */
	.ref_ctrl_init				= 0x000040F1,
	.sdram_tim1					= 0xCCCF36B3,
	.sdram_tim2				= 0x328F7FDA,
	.sdram_tim3					= 0x027F88A8,
	.read_idle_ctrl				= 0x00050000,
	.zq_config					= 0x0007190B,
	.temp_alert_config			= 0x00000000,
	.emif_ddr_phy_ctlr_1_init	= 0x0E20400A,
	.emif_ddr_phy_ctlr_1		= 0x0E24400A,
	.emif_ddr_ext_phy_ctrl_1	= 0x04040100,
	.emif_ddr_ext_phy_ctrl_2	= 0x00990099,
	.emif_ddr_ext_phy_ctrl_3	= 0x009D009D,
	.emif_ddr_ext_phy_ctrl_4	= 0x008B008B,
	.emif_ddr_ext_phy_ctrl_5	= 0x00860086,
	.emif_rd_wr_lvl_rmp_win		= 0x00000000,
	.emif_rd_wr_lvl_rmp_ctl		= 0x80000000, //Tony: enable leveling other than gel file.
	.emif_rd_wr_lvl_ctl			= 0x00000000,
	.emif_rd_wr_exec_thresh		= 0x00000305
};

#define EMIF_EXT_PHY_CTRL_TIMING_REG	0x5
#define EMIF_EXT_PHY_CTRL_CONST_REG		0x14

//EMIF2
const unsigned dra_ddr3_ext_phy_ctrl_const_base_es1_emif2[EMIF_EXT_PHY_CTRL_CONST_REG] = {
	0xBB00BB, //CTRL_6
	0x340034, //CTRL_7
	0x300030, //CTRL_8
	0x300030, //CTRL_9
	0x310031, //CTRL_10
	0x440044, //CTRL_11
	0x1FF01FF, //CTRL_12
	0x1FF01FF, //CTRL_13
	0x1FF01FF, //CTRL_14
	0x1FF01FF, //CTRL_15
	0x400040, //CTRL_16
	0x520052, //CTRL_17
	0x530053, //CTRL_18
	0x480048, //CTRL_19
	0x490049, //CTRL_20
	0x600060, //CTRL_21
	0x800080, //CTRL_22
	0x800080, //CTRL_23
	0x40010080, //CTRL_24
	0x8102040 //CTRL_25
};

//EMIF1
const unsigned dra_ddr3_ext_phy_ctrl_const_base_es1_emif1[EMIF_EXT_PHY_CTRL_CONST_REG] = {
	0xBB00BB, //CTRL_6
	0x370037, //CTRL_7
	0x320032, //CTRL_8
	0x350035, //CTRL_9
	0x340034, //CTRL_10
	0x440044, //CTRL_11
	0x1FF01FF, //CTRL_12
	0x1FF01FF, //CTRL_13
	0x1FF01FF, //CTRL_14
	0x1FF01FF, //CTRL_15
	0x400040, //CTRL_16
	0x440044, //CTRL_17
	0x420042, //CTRL_18
	0x4E004E, //CTRL_19
	0x510051, //CTRL_20
	0x600060, //CTRL_21
	0x800080, //CTRL_22
	0x800080, //CTRL_23
	0x40010080, //CTRL_24
	0x8102040 //CTRL_25
};

static void emif_update_timings(unsigned base, const struct emif_regs *regs)
{
	out32(base + EMIF_SDRAM_REF_CTRL_SHDW, regs->ref_ctrl);
	out32(base + EMIF_SDRAM_TIM_1_SHDW, regs->sdram_tim1);
	out32(base + EMIF_SDRAM_TIM_2_SHDW, regs->sdram_tim2);
	out32(base + EMIF_SDRAM_TIM_3_SHDW, regs->sdram_tim3);

	out32(base + EMIF_PWR_MGMT_CTRL, 0x0);
	out32(base + EMIF_PWR_MGMT_CTRL_SHDW, 0x0);

	out32(base + EMIF_DLL_CALIB_CTRL_SHDW, regs->read_idle_ctrl);
	out32(base + EMIF_ZQ_CONFIG, regs->zq_config);
	out32(base + EMIF_TEMP_ALERT_CONFIG, regs->temp_alert_config);
	out32(base + EMIF_DDR_PHY_CTRL_1_SHDW, regs->emif_ddr_phy_ctlr_1);
#define EMIF_L3_CONFIG_VAL_SYS_10_MPU_5_LL_0	0x0A500000
	out32(base + EMIF_OCP_CONFIG, EMIF_L3_CONFIG_VAL_SYS_10_MPU_5_LL_0);
}

static void config_data_eye_leveling_samples(unsigned emif_base)
{
	/* EMIF_SDRAM_CONFIG_EXT-Read data eye leveling no of samples =4 */
	if (emif_base == DRA74X_EMIF1_BASE)
		out32(DRA74X_CTRL_WKUP_BASE + CTRL_WKUP_EMIF1_SDRAM_CONFIG_EXT, 0x0000127); // CONTROL_EMIF1_SDRAM_CONFIG_EXT
	else if (emif_base == DRA74X_EMIF2_BASE)
		out32(DRA74X_CTRL_WKUP_BASE + CTRL_WKUP_EMIF2_SDRAM_CONFIG_EXT, 0x0000127); // CONTROL_EMIF2_SDRAM_CONFIG_EXT
}

static void ddr3_sw_leveling(unsigned base, const struct emif_regs *regs)
{
	out32(base + EMIF_DDR_PHY_CTRL_1, regs->emif_ddr_phy_ctlr_1);
	out32(base + EMIF_DDR_PHY_CTRL_1_SHDW, regs->emif_ddr_phy_ctlr_1);

	config_data_eye_leveling_samples(base);

	out32(base + EMIF_RD_WR_LVL_CTL, regs->emif_rd_wr_lvl_ctl);
	out32(base + EMIF_SDRAM_CONFIG, regs->sdram_config);
}

/* Configure the EMIF. Table 15-112. EMIF Global Initialization */
void init_emif(unsigned base)
{
	int	i;
	unsigned	*ext_phy_ctrl_base;
	const struct emif_regs *regs;
	unsigned reg;

	if (base == DRA74X_EMIF1_BASE)
		regs = &emif_1_regs_ddr3_532_mhz_1cs_dra_es1;
	else
		regs = &emif_2_regs_ddr3_532_mhz_1cs_dra_es1;

	if (!(in32(DRA74X_PRM_BASE + PRM_RSTST) & PRM_RSTST_WARM_RESET_MASK)) {
		/*
		 * On Cold reset
		 * Set SDRAM_CONFIG and PHY control registers to locked frequency
		 * and RL =7. As the default values of the Mode Registers are not
		 * defined, contents of mode Registers must be fully initialized.
		 * H/W takes care of this initialization
		 */
		out32(base + EMIF_SDRAM_CONFIG_2, regs->sdram_config2);
		out32(base + EMIF_SDRAM_CONFIG, regs->sdram_config_init);
		out32(base + EMIF_DDR_PHY_CTRL_1, regs->emif_ddr_phy_ctlr_1_init);

		/* Update timing registers */
		out32(base + EMIF_SDRAM_TIM_1, regs->sdram_tim1);
		out32(base + EMIF_SDRAM_TIM_2, regs->sdram_tim2);
		out32(base + EMIF_SDRAM_TIM_3, regs->sdram_tim3);

		out32(base + EMIF_SDRAM_REF_CTRL, regs->ref_ctrl_init);
		out32(base + EMIF_DLL_CALIB_CTRL, regs->read_idle_ctrl);

		ext_phy_ctrl_base = (unsigned *) &(regs->emif_ddr_ext_phy_ctrl_1);

		/* Configure external phy control timing registers */
		for (i = 0; i < EMIF_EXT_PHY_CTRL_TIMING_REG; i++) {
			out32(base + EMIF_DDR_EXT_PHY_CTRL_X(i), *ext_phy_ctrl_base);

			/* Write the shadow register here as well */
			out32(base + EMIF_DDR_EXT_PHY_CTRL_X_SHDW(i), *ext_phy_ctrl_base);
			ext_phy_ctrl_base ++;
		}

		/*
		 * external phy 6-24 registers do not change with
		 * ddr frequency
		 */
		for (i = 0; i < EMIF_EXT_PHY_CTRL_CONST_REG; i++) {
			if (base == DRA74X_EMIF1_BASE) {
				out32(base + EMIF_DDR_EXT_PHY_CTRL_X(i + EMIF_EXT_PHY_CTRL_TIMING_REG), dra_ddr3_ext_phy_ctrl_const_base_es1_emif1[i]);
				/* Update shadow registers */
				out32(base + EMIF_DDR_EXT_PHY_CTRL_X_SHDW(i + EMIF_EXT_PHY_CTRL_TIMING_REG), dra_ddr3_ext_phy_ctrl_const_base_es1_emif1[i]);
			} else {
				out32(base + EMIF_DDR_EXT_PHY_CTRL_X(i + EMIF_EXT_PHY_CTRL_TIMING_REG), dra_ddr3_ext_phy_ctrl_const_base_es1_emif2[i]);
				/* Update shadow registers */
				out32(base + EMIF_DDR_EXT_PHY_CTRL_X_SHDW(i + EMIF_EXT_PHY_CTRL_TIMING_REG), dra_ddr3_ext_phy_ctrl_const_base_es1_emif2[i]);
			}
		}

		/* enable leveling */
		out32(base + EMIF_RD_WR_LVL_RMP_CTL, regs->emif_rd_wr_lvl_rmp_ctl);
		ddr3_sw_leveling(base, regs);
		out32(base + EMIF_SDRAM_REF_CTRL, regs->ref_ctrl);
	} else { /* On warm reset */

		/*
		 * Set to lpmode self-refresh
		 */
		reg = in32(base + EMIF_PWR_MGMT_CTRL);
		reg &= ~(0x7 << 8);
		reg |= 2 << 8;
		reg &= ~(0xf << 4);
		out32 (base + EMIF_PWR_MGMT_CTRL, reg);

		/* dummy read for the new SR_TIM to be loaded */
		in32(base + EMIF_PWR_MGMT_CTRL);

		/* Reset phy */
		reg = in32(base + EMIF_IODFT_TLGC);
		reg |= (1 << 10);
		out32 (base + EMIF_IODFT_TLGC, reg);

		ddr3_sw_leveling(base, regs);
	}

	/* Write to the shadow registers */
	emif_update_timings(base, regs);
}

/* Initialize the DDR SDRAM */
void init_sdram(void)
{
	out32(DRA74X_PRCM_BASE + CM_DLL_CTRL, 0);

	/* ddr init */
	init_dmm();
	init_emif(DRA74X_EMIF1_BASE);
	init_emif(DRA74X_EMIF2_BASE);
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/ipl/boards/dra74x/init_sdram.c $ $Rev: 755833 $")
#endif
