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

static void init_core_voltages()
{
	omap_i2c_dev_t pmic_dev = {
		.base = DRA74X_I2C1_BASE,
		.clock = I2C_SPEED,
		.slave = TPS659038_I2C_SLAVE_ADDR
	};

	omap_i2c_init(&pmic_dev);

	/* Votage for CORE */
	do_scale_vcore(DOMAIN_CORE, &pmic_dev, TPS659038_REG_ADDR_SMPS7);

	/* Voltage for MPU */
	do_scale_vcore(DOMAIN_MPU, &pmic_dev, TPS659038_REG_ADDR_SMPS12);

	/* Votage for IVA */
	do_scale_vcore(DOMAIN_IVA, &pmic_dev, TPS659038_REG_ADDR_SMPS8);

	/* Votage for GPU */
	do_scale_vcore(DOMAIN_GPU, &pmic_dev, TPS659038_REG_ADDR_SMPS6);

	/* Votage for EVE */
	do_scale_vcore(DOMAIN_DSPEVE, &pmic_dev, TPS659038_REG_ADDR_SMPS45);

	omap_i2c_deinit(&pmic_dev);

	/* setup ABB */
	abb_setup(DOMAIN_MPU);
	abb_setup(DOMAIN_IVA);
	abb_setup(DOMAIN_GPU);
	abb_setup(DOMAIN_DSPEVE);
}

static void init_prcm(void)
{
	recalibrate_io_delay();
	init_basic_clocks();

	/* General Purpose Timer2 is used for delay() and benchmarking */
	init_gptimer_clock(2, 0);
	omap_timer_enable(DRA74X_TIMER2_BASE, get_sys_clkspeed());

	init_wdt_clock();

	setup_ddr_dpll();

	/* CORE dpll */
	setup_core_dpll();

	/* PER dpll */
	setup_per_dpll();

	/* MPU dpll */

	/*
	 * We can't boot J6 in 1.1/1.5GHz by default if DVFS is not guaranteed to be running
	 * Also we don't see the boot time has significant improve by increasing CPU frequency
	 * We leave the OPP_HIGH/OPP_OD code here just for future use
	 */
#ifdef MPU_OPP_HIGH
	setup_mpu_dpll_1500mhz();
#elif MPU_OPP_OD
	setup_mpu_dpll_1176mhz();
#else
	setup_mpu_dpll_1ghz();
#endif

	/* USB(EHCI) DPLL */
	setup_usb_dpll();

	/* IVA dpll*/
#ifdef IVA_OPP_HIGH
	setup_iva_dpll(1);
#else
	setup_iva_dpll(0);
#endif

	/* EVE dpll */
	setup_eve_dpll();

	/* DSP dpll */
#ifdef DSP_OPP_OD
	setup_dsp_dpll_700mhz();
#else
	setup_dsp_dpll_600mhz();
#endif

	/* GPU dpll */
	setup_gpu_dpll_2128mhz();

	/* GMAC dpll */
	setup_gmac_dpll();

	/* PCIE dpll */
	setup_pcie_dpll();

	/* ABE dpll */
	setup_abe_dpll(SYS_CLK2_INPUT);
}

void init_dmm()
{
	out32(DRA74X_DMM_BASE + DMM_LISA_MAP_3, 0);
	out32(DRA74X_DMM_BASE + DMM_LISA_MAP_2, 0);
	out32(DRA74X_DMM_BASE + DMM_LISA_MAP_1, 0);
	out32(DRA74X_DMM_BASE + DMM_LISA_MAP_0, 0);

	// hinge board has 2.0 GB of memory
	// EMIF1 --> 4Gb * 2 = 1GB
	// EMIF2 --> 4Gb * 2 = 1GB
	// so mapping 2GB interleaved
	out32(DRA74X_DMM_BASE + DMM_LISA_MAP_3, 0xFF020100);
	out32(DRA74X_DMM_BASE + DMM_LISA_MAP_2, 0x80740300);
	out32(DRA74X_DMM_BASE + DMM_LISA_MAP_1, 0);
	out32(DRA74X_DMM_BASE + DMM_LISA_MAP_0, 0);
	out32(DRA74X_MA_BASE + DMM_LISA_MAP_3, 0xFF020100);
	out32(DRA74X_MA_BASE + DMM_LISA_MAP_2, 0x80740300);
	out32(DRA74X_MA_BASE + DMM_LISA_MAP_1, 0);
	out32(DRA74X_MA_BASE + DMM_LISA_MAP_0, 0);
}

/* Enable TPS51200 DDR Termination Regulator on Rev G */
#if 0
static void	init_tps51200(void)
{
#define TPS51200_EN		11 /* GPIO7_11 */

	/* Drive GPIO7_11 to high to enable it */
	sr32(DRA74X_GPIO7_BASE + GPIO_OE, TPS51200_EN, 1, 0);
	sr32(DRA74X_GPIO7_BASE + GPIO_DATAOUT, TPS51200_EN, 1, 1);
}
#endif

static int init_qspi()
{
	qspi_dev_t qspi = {
		.qspi_part = PART_S25FL,
		.core_base = DRA74X_QSPI_CORE_BASE,
		.input_frq = 192000000,
		.edma_base = DRA74X_EDMA_BASE,
		.edma_chnl = 8,
		.clock = 64000000,
		.sector_size = 64 * 1024,
		.buff_size = 256,
		.wdt_toggle = 0,
		.verbose = 0
	};

	if (qspi_initialize(&qspi)) {
		ser_putstr("QSPI interface init failed\n");
		return -1;
	}

	return 0;
}

void init_hw_platform(void)
{
	unlock_ctrl_module();
	init_i2c_clock(1);
	init_core_voltages();
	init_prcm();
	watchdog_init();
	init_pinmux();
	io_settings();

	/* UART1 is the IPL console */
	init_uart_clock(1);
	init_seromap(DRA74X_UART1_BASE, 115200, 48000000, 16);

//	init_tps51200();
	init_sdram();

	/* Enable 192MHX for MMC1 and MMC2 */
	init_mmc_clock(1, (1 << CLKCTRL_CLKSEL_OFFSET));
	init_mmc_clock(2, (1 << CLKCTRL_CLKSEL_OFFSET));
	init_qspi_clock();
	init_edma_clocks();

	init_edma();
	init_qspi();

	print_avs_abb_info();
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/ipl/boards/dra74x/arm/vayu-evm.le.v7/board.c $ $Rev: 768019 $")
#endif
