/*
 * $QNXLicenseC:
 * Copyright 2014, QNX Software Systems.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0

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
#include <ipl.h>
#include <private/omap_timer.h>
#include <private/omap_sdma.h>
#include <private/omap3_edma.h>
#include <stdint.h>
#include <hw/inout.h>
#include <hw/omap_i2c.h>
#include <arm/dra74x.h>

#ifndef __DRA7XX_IPL_H_INCLUDED
#define __DRA7XX_IPL_H_INCLUDED

/* IFS image scan range */
#define MAX_SCAN                            0x200

/* IFS image offset at QSPI flash */
#define QSPI_IFS_OFFSET                     (64 * 1024)

#ifdef HS_AUTH_IFS
#define IFS_CERT_SIZE                       280
#endif

#define KILO(x)                             ((unsigned)((x) * 1024))
#define MEG(x)                              ((unsigned)((x) * KILO(1024)))

#define DIV_ROUND(n,d)                      (((n) + ((d)/2)) / (d))

#define SYS_CLK1_INPUT                      1
#define SYS_CLK2_INPUT                      2
#define FUNC_32K_CLK_INPUT                  3

/* Register definitions for PMIC TPS659038 */
#define TPS659038_I2C_SLAVE_ADDR            0x58
#define TPS659038_REG_ADDR_SMPS12           0x23
#define TPS659038_REG_ADDR_SMPS45           0x2B
#define TPS659038_REG_ADDR_SMPS6            0x2F
#define TPS659038_REG_ADDR_SMPS7            0x33
#define TPS659038_REG_ADDR_SMPS8            0x37

#define I2C_SPEED                           100000

/*
* If the efuse values are not popualated, we will read '0' and need to set the default voltages,
* otherwise it won't boot
*/
#define MPU_OPP_VOLT_HIGH                   1250    /* milli volts */
#define MPU_OPP_VOLT_OD                     1160
#define MPU_OPP_VOLT_NOM                    1060

#define DSPEVE_OPP_VOLT_NOM                 1060
#define DSPEVE_OPP_VOLT_OD                  1160

#define IVA_OPP_VOLT_NOM                    1060
#define IVA_OPP_VOLT_HIGH                   1250

#define GPU_OPP_VOLT_NOM                    1090

#define CORE_OPP_VOLT_NOM                   1030

/* Domain defitions */
#define DOMAIN_MPU                          0
#define DOMAIN_GPU                          1
#define DOMAIN_CORE                         2
#define DOMAIN_IVA                          3
#define DOMAIN_DSPEVE                       4

#define DOMAINS_TOTAL                       5

#define ABB_ENABLED                         0
#define ABB_NOT_ENABLED                     1
#define ABB_TRANS_ERROR                     2

/*
 * The uncompressed IFS will be at 0x80100000 -- QNX_LOAD_ADDR
 * This implies that tha maximum decompressed IFS is 31MB
 * For larger IFS, need to overwrite this in the BS code
 */
#define QNX_LOAD_ADDR                       0x82000000

/* Maximum counter for hardware loop if any */
#define LDELAY                              10000

#define SD_INTERFACE                        1
#define EMMC_INTERFACE                      2

/*comment out all debug info */
#if (!MANBOOT)
#define NO_DISPLAY
#endif

#ifdef NO_DISPLAY
#define log_putstr(a)
#define log_puthex(a)
#define log_putdec(a)
#else
#define log_putstr(a)   ser_putstr(a)
#define log_puthex(a)   ser_puthex(a)
#define log_putdec(a)   ser_putdec(a)
#endif /* NO_DISPLAY*/

struct emif_regs {
    unsigned freq;
    unsigned sdram_config_init;
    unsigned sdram_config;
    unsigned sdram_config2;
    unsigned ref_ctrl;
    unsigned ref_ctrl_init;
    unsigned sdram_tim1;
    unsigned sdram_tim2;
    unsigned sdram_tim3;
    unsigned read_idle_ctrl;
    unsigned zq_config;
    unsigned temp_alert_config;
    unsigned emif_ddr_phy_ctlr_1_init;
    unsigned emif_ddr_phy_ctlr_1;
    unsigned emif_ddr_ext_phy_ctrl_1;
    unsigned emif_ddr_ext_phy_ctrl_2;
    unsigned emif_ddr_ext_phy_ctrl_3;
    unsigned emif_ddr_ext_phy_ctrl_4;
    unsigned emif_ddr_ext_phy_ctrl_5;
    unsigned emif_rd_wr_lvl_rmp_win;
    unsigned emif_rd_wr_lvl_rmp_ctl;
    unsigned emif_rd_wr_lvl_ctl;
    unsigned emif_rd_wr_exec_thresh;
};

static inline void sr32(unsigned addr, unsigned start_bit, unsigned num_bits, unsigned value)
{
    unsigned tmp, msk = 0;
    msk = 1 << num_bits;
    --msk;
    tmp = in32(addr) & ~(msk << start_bit);
    tmp |= value << start_bit;
    out32(addr, tmp);
}

/*
 * Boot devices as defined by the boot ROM
 */
typedef enum {
    BOOT_XIP            = 0x1,
    BOOT_XIP_WAIT       = 0x2,
    BOOT_NAND           = 0x3,
    BOOT_ONENAND        = 0x4,
    BOOT_SD_CARD        = 0x5,
    BOOT_EMMC_PARTITION = 0x6,
    BOOT_EMMC           = 0x7,
    BOOT_SATA           = 0x9,
    BOOT_QSPI_1         = 0xA,
    BOOT_QSPI_4         = 0xB,
    BOOT_UART3          = 0x43,
    BOOT_USB            = 0x45,
} bootDevices_t;

typedef enum {
    IPL_BOOT_SD,
    IPL_BOOT_EMMC,
    IPL_BOOT_QSPI,
    IPL_BOOT_NOR,
    IPL_BOOT_SERIAL,
    IPL_UPDATE_IPL,
    IPL_UPDATE_IFS,
    IPL_UPDATE_IFS_ALL,
    IPL_UPDATE_IMAGE,
    IPL_MEM_TEST,
    IPL_RESET,

    IPL_PRINT_BOOT_STRUCT,
    IPL_CLEAR_IMAGE_FLAGS,
    IPL_CLEAR_UPDATE_FLAG,

    IPL_BOOT_UNKNOWN
} IPL_BootOpt_t;

/* These functions are provided by the common code */
extern uint32_t get_sys_clkspeed();
extern void watchdog_init(void);
extern void io_settings(void);
extern void unlock_ctrl_module(void);
extern void enable_cache_mmu();
extern unsigned get_mmcsd_clock(int device);
extern void init_sdram(void);
extern void init_emif(unsigned base);
extern void setup_dplls(void);
extern void init_hw_platform(void);
extern void init_basic_clocks(void);
extern void init_gptimer_clock(int dev_num, int mode);
extern void init_i2c_clock(int dev_num);
extern void init_uart_clock(int dev_num);
extern void init_mmc_clock(int dev_num, int mode);
extern void init_wdt_clock(void);
extern void init_edma_clocks(void);
extern void init_qspi_clock(void);
extern void recalibrate_io_delay(void);
extern void init_sdram(void);
extern void init_edma(void);
extern void watchdog_toggle(void);

extern int powerup_emmc(int);
extern IPL_BootOpt_t ipl_boot_detect(void);
extern int sdmmc_load_file(int, unsigned, unsigned, const char *, unsigned);
extern int qspi_load_image(unsigned, unsigned);
extern unsigned int get_omap_rev(void);
extern int sdram_test(unsigned int, unsigned int);
extern void print_omap_revision(void);

//extern unsigned optimize_vcore_voltage(unsigned efuse_addr, unsigned bits);
extern void do_scale_vcore(int domain, omap_i2c_dev_t *pmic_dev, unsigned vcore_reg);
extern void abb_setup(int domain);
extern void print_avs_abb_info();

extern void setup_core_dpll(void);
extern void setup_per_dpll(void);
extern void setup_mpu_dpll_1ghz(void);
extern void setup_mpu_dpll_1176mhz(void);
extern void setup_mpu_dpll_1500mhz(void);
extern void setup_iva_dpll(int iva_opp);
extern void setup_eve_dpll(void);
extern void setup_dsp_dpll_600mhz(void);
extern void setup_dsp_dpll_700mhz(void);
extern void setup_gpu_dpll_1700mhz(void);
extern void setup_gpu_dpll_2128mhz(void);
extern void setup_gmac_dpll(void);
extern void setup_pcie_dpll(void);
extern void setup_abe_dpll(int sys_clk);
extern void setup_usb_dpll(void);
extern void setup_ddr_dpll(void);

extern unsigned authenticate_image_signature(unsigned start_addr, unsigned size);
extern unsigned hal_pub_to_sec_dispatcher(unsigned func_id, unsigned p0_in, unsigned p1_in, unsigned p2_in);

/* These functions are provided by BS code */
extern void init_dmm(void);
IPL_BootOpt_t ipl_boot_menu();

#endif /* __DRA7XX_IPL_H_INCLUDED */


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/ipl/boards/dra74x/dra74x_ipl.h $ $Rev: 768019 $")
#endif
