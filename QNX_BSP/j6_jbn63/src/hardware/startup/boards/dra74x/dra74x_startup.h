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

#include "startup.h"
#include <arm/dra74x.h>
#include <hw/omap_i2c.h>

#ifndef __DRA74X_STARTUP_H
#define __DRA74X_STARTUP_H

#define LDELAY 12000000

#define SYS_CLK1_INPUT					1
#define SYS_CLK2_INPUT					2
#define FUNC_32K_CLK_INPUT				3

#define I2C_SPEED						100000

#define CLOCK_48MHz						48000000
#define CLOCK_128MHz					128000000
#define CLOCK_192MHz					192000000

typedef enum {
	DPLL_PER,
	DPLL_CORE,
	DPLL_ABE,
	DPLL_MPU,
	DPLL_IVA,
	DPLL_USB,
	DPLL_DSP,
	DPLL_GMAC,
	DPLL_GPU,
	DPLL_DDR,
	DPLL_PCIE,
	DPLL_EVE
} dpll_type_t;

/* Register definitions for PMIC TPS659038 */
#define TPS659038_I2C_SLAVE_ADDR		0x58	/* page 1 */
#define TPS659038_I2C_SLAVE_ADDR_PG2	0x59	/* Page 2 */
#define TPS659038_I2C_SLAVE_ADDR_PG3	0x5A	/* page 3 */
#define TPS659038_REG_ADDR_SMPS12		0x23
#define TPS659038_REG_ADDR_SMPS45		0x2B
#define TPS659038_REG_ADDR_SMPS6		0x2F
#define TPS659038_REG_ADDR_SMPS7		0x33
#define TPS659038_REG_ADDR_SMPS8		0x37

#define TPS659038_REG_ADDR_DESIGNREV	0x57
	#define TPS659038_ES1_0				0x0
	#define TPS659038_ES2_0				0x1
	#define TPS659038_ES2_1				0x2

/* Greatest Common Dividor */
static inline unsigned
get_gcd(unsigned a, unsigned b)
{
	unsigned c;

	while (1) {
		c = a % b;
		if (c == 0)
			return b;
		a = b;
		b = c;
	}
	return 0;
}

static inline void sr32(unsigned addr, unsigned start_bit, unsigned num_bits, unsigned value)
{
	unsigned tmp, msk = 0;
	msk = 1 << num_bits;
	--msk;
	tmp = in32(addr) & ~(msk << start_bit);
	tmp |= value << start_bit;
	out32(addr, tmp);
}

static inline unsigned long long get_timer_value()
{
	unsigned	hi, lo;

	asm volatile("mrrc p15, 0, %0, %1, c14" : "=r" (lo), "=r" (hi));

	return ((unsigned long long) hi << 32) | lo;
}

/* Map IRQ_CROSSBAR inputline crossbar_irq to module_irq line for the corresponding INTC/DREQ. */
struct crossbar_entry {
	uint16_t	module_irq;
	uint16_t	crossbar_irq;
};

typedef struct io_delay {
	uint32_t	addr;
	uint32_t	dly;
} dra7xx_io_delay_t;

/*
 * The following code is from TI and is required for the J6 EVM board
 * Undocumented registers
 */
#define CFG_IO_DELAY_BASE				0x4844A000
#define CFG_IO_DELAY_LOCK				(CFG_IO_DELAY_BASE + 0x02C)
#define CFG_RGMII0_TXCTL				(CFG_IO_DELAY_BASE + 0x74C)
#define CFG_RGMII0_TXD0					(CFG_IO_DELAY_BASE + 0x758)
#define CFG_RGMII0_TXD1					(CFG_IO_DELAY_BASE + 0x764)
#define CFG_RGMII0_TXD2					(CFG_IO_DELAY_BASE + 0x770)
#define CFG_RGMII0_TXD3					(CFG_IO_DELAY_BASE + 0x77C)
#define CFG_VIN2A_D13					(CFG_IO_DELAY_BASE + 0xA7C)
#define CFG_VIN2A_D17					(CFG_IO_DELAY_BASE + 0xAAC)
#define CFG_VIN2A_D16					(CFG_IO_DELAY_BASE + 0xAA0)
#define CFG_VIN2A_D15					(CFG_IO_DELAY_BASE + 0xA94)
#define CFG_VIN2A_D14					(CFG_IO_DELAY_BASE + 0xA88)

#define CFG_IO_DELAY_UNLOCK_KEY			0x0000AAAA
#define CFG_IO_DELAY_LOCK_KEY			0x0000AAAB
#define CFG_IO_DELAY_ACCESS_PATTERN		0x00029000
#define CFG_IO_DELAY_LOCK_MASK			0x400

/* Delay value to add to calibrated value */
#define RGMII0_TXCTL_DLY_VAL			((0x3 << 5) + 0x8)
#define RGMII0_TXD0_DLY_VAL				((0x3 << 5) + 0x8)
#define RGMII0_TXD1_DLY_VAL				((0x3 << 5) + 0x2)
#define RGMII0_TXD2_DLY_VAL				((0x4 << 5) + 0x0)
#define RGMII0_TXD3_DLY_VAL				((0x4 << 5) + 0x0)
#define VIN2A_D13_DLY_VAL				((0x3 << 5) + 0x8)
#define VIN2A_D17_DLY_VAL				((0x3 << 5) + 0x8)
#define VIN2A_D16_DLY_VAL				((0x3 << 5) + 0x2)
#define VIN2A_D15_DLY_VAL				((0x4 << 5) + 0x0)
#define VIN2A_D14_DLY_VAL				((0x4 << 5) + 0x0)

#define CM_DSS_DSS_CLKCTRL_ENABLED		0xf02

/* Video Input Port */
#define VIP_DEVICE_1					(1 << 0)
#define VIP_DEVICE_2					(1 << 1)
#define VIP_DEVICE_3					(1 << 2)

/**************************************************************************
 * Variables defined in ./dra74x folder
 * To be shared with board specific code
 *************************************************************************/
extern unsigned hundred_loop_time;
extern unsigned overhead;

uint32_t get_sys_clkspeed();

/**************************************************************************
 * Routines defined in ./dra74x folder
 * To be shared with board specific code
 *************************************************************************/
void init_ipu(void);
void init_ipu1(void);
void init_ipu2(void);
void init_basic_clocks(void);
void init_gptimer_clock(int dev_num, int mode);
void init_i2c_clock(int dev_num);
void init_uart_clock(int dev_num);
void init_mcspi_clock(int dev_num);
void init_qspi_clock(void);
void init_mcasp_clock(int dev_num);
void init_mmc_clock(int dev_num, int mode);
unsigned get_mmcsd_clock(int dev_num);
void init_gpio_clock(int dev_num, int mode);
void init_kdb_clock(void);
void init_gpmc_clock(void);
void init_hdq_clock(void);
void init_vin_clocks(int dev_num);
void init_vpe_clocks(void);
void init_graphics_clocks();
void init_gpu_clock(uint32_t source);
void init_dcan_clock(int dev_num);
void init_atl_clock(int source1, int source2);
void init_usb_ss_clock(int dev_num, int mode);
void init_usb_phy_clock(int dev_num);
void init_gmac_clock(void);
void init_edma_clock(void);

void init_usb1_otg(void);
void init_usb2_otg(void);
void init_usb3_phy(void);

void dra7xx_adj_io_delay(const dra7xx_io_delay_t *io_dly);
void init_qtime_dra74x(void);
void init_clocks(void);
void hw_init(void);
void hwi_dra74x();
void wdt_enable(void);
void wdt_disable(void);
void init_edma(void);
void init_ocp2scp3(void);
void init_sata(void);
void mmc1_select_1_8v(void);
unsigned calc_omap_dpll_and_set_freq(unsigned *processor_speed_ptr, unsigned *max_speed_ptr);

void dra74x_set_mpu_crossbar(unsigned base, struct crossbar_entry const *array, int size);
void dra74x_set_edma_crossbar(unsigned base, struct crossbar_entry const *array, int size);
void read_pmic_revision();
unsigned calc_dpll_freq(dpll_type_t type);
void dump_clocks();
void setup_abe_dpll(int sys_clk);
void setup_gpu_dpll_1700mhz();
void setup_gpu_dpll_2128mhz();

void secure_monitor_call(uint16_t func_id, int p0_in, int p1_in, int * p0_out, int * p1_out);

void usec_delay(unsigned usec);

/**************************************************************************
 * Routines supposed to be defined in board specific code
 * To be called in ./dra74x code
 *************************************************************************/
void board_disable_mmu(void);
uint32_t get_sys_clk2_freq();
#endif


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/dra74x/dra74x_startup.h $ $Rev: 768019 $")
#endif
