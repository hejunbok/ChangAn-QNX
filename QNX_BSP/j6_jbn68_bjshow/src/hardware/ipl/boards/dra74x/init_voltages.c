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

char avs_enabled        = 0xff;
char abb_result[]       = {ABB_NOT_ENABLED, ABB_NOT_ENABLED, ABB_NOT_ENABLED, ABB_NOT_ENABLED, ABB_NOT_ENABLED};

#define SMPS_BASE_VOLT_UV               500000  // Standard offset is 0.5v expressed in uv
#define SMPS_BASE_VOLT_UV_STEP          10000   // 10 mV represented in uV
#define SMPS_START_CODE                 6

/**
 * abb_setup_timings: ABB Timing setup
 * - Description: This function sets up ABB LDO settling time after
 * transition to a new OPP using the following formula:
 * SR2_WTCNT_VALUE = settling time / (# SYS_CLK cycles / SYS_CLK rate))
 */
static void abb_setup_timings(int domain)
{
    unsigned sys_rate, sr2_cnt, clk_cycles, regval;
    unsigned setup_reg;

    switch(domain) {
        case DOMAIN_MPU:
            setup_reg       = DRA74X_DEVICE_PRM_BASE + PRM_ABBLDO_MPU_SETUP;
            break;
        case DOMAIN_GPU:
            setup_reg       = DRA74X_DEVICE_PRM_BASE + PRM_ABBLDO_GPU_SETUP;
            break;
        case DOMAIN_IVA:
            setup_reg       = DRA74X_DEVICE_PRM_BASE + PRM_ABBLDO_IVA_SETUP;
            break;
        case DOMAIN_DSPEVE:
            setup_reg       = DRA74X_DEVICE_PRM_BASE + PRM_ABBLDO_DSPEVE_SETUP;
            break;
        case DOMAIN_CORE:
        default:
            return;
    }

    /* calculate SR2_WTCNT_VALUE */
    sys_rate = DIV_ROUND( get_sys_clkspeed(), 1000000);
    clk_cycles = DIV_ROUND(OMAP_ABB_CLOCK_CYCLES * 10, sys_rate);
    sr2_cnt = DIV_ROUND(OMAP_ABB_SETTLING_TIME * 10, clk_cycles);

    regval = in32(setup_reg);
    regval &= ~(OMAP_ABB_SETUP_SR2_WTCNT_VALUE_MASK);
    regval |= (sr2_cnt << OMAP_ABB_SETUP_SR2_WTCNT_VALUE_SHIFT);
    out32(setup_reg, regval);

    return;

}
/**
 * abb_setup_ldovbb: Setup LDO VBB from EFUSE registers
 * - Description: This function reads out the efuse values for
 * the corresponding OPP and setup LDOVBB accordingly.
 * - Returns:
 *  - -1 for OPP with ABB disabled, 0 otherwise
 */
static int abb_setup_ldovbb(int domain)
{
    unsigned vset, regval;
    unsigned efuse_reg, voltctrl_reg;

    switch(domain) {
        case DOMAIN_MPU:
#ifdef MPU_OPP_HIGH
            efuse_reg       = DRA74X_CTRL_CORE_BASE + CTRL_CORE_STD_FUSE_OPP_VMIN_MPU_4;
#elif MPU_OPP_OD
            efuse_reg       = DRA74X_CTRL_CORE_BASE + CTRL_CORE_STD_FUSE_OPP_VMIN_MPU_3;
#else
            efuse_reg       = DRA74X_CTRL_CORE_BASE + CTRL_CORE_STD_FUSE_OPP_VMIN_MPU_2;
#endif
            voltctrl_reg    = DRA74X_CTRL_WKUP_BASE + CTRL_WKUP_LDOVBB_MPU_VOLTAGE_CTRL;
            break;
        case DOMAIN_GPU:
            efuse_reg       = DRA74X_CTRL_CORE_BASE + CTRL_CORE_STD_FUSE_OPP_VMIN_GPU_2;
            voltctrl_reg    = DRA74X_CTRL_WKUP_BASE + CTRL_WKUP_LDOVBB_GPU_VOLTAGE_CTRL;
            break;
        case DOMAIN_IVA:
#ifdef IVA_OPP_HIGH
            efuse_reg       = DRA74X_CTRL_CORE_BASE + CTRL_CORE_STD_FUSE_OPP_VMIN_IVA_4;
#else
            efuse_reg       = DRA74X_CTRL_CORE_BASE + CTRL_CORE_STD_FUSE_OPP_VMIN_IVA_2;
#endif
            voltctrl_reg    = DRA74X_CTRL_CORE_BASE + CTRL_CORE_LDOVBB_IVA_VOLTAGE_CTRL;
            break;
        case DOMAIN_DSPEVE:
#ifdef DSP_OPP_OD
			efuse_reg       = DRA74X_CTRL_CORE_BASE + CTRL_CORE_STD_FUSE_OPP_VMIN_DSPEVE_3;
#else
            efuse_reg       = DRA74X_CTRL_CORE_BASE + CTRL_CORE_STD_FUSE_OPP_VMIN_DSPEVE_2;
#endif
            voltctrl_reg    = DRA74X_CTRL_CORE_BASE + CTRL_CORE_LDOVBB_DSPEVE_VOLTAGE_CTRL;
            break;
        case DOMAIN_CORE:
        default:
            return -1;
    }

    /*
    * Ensure ABB is enabled
    */
    vset = in32(efuse_reg);
    if (!(vset & FUSE_ABBEN)) {
        // ABB not needed/enabled
        return -1;
    }

    /* prepare VSET value for LDOVBB mux register */
    vset &= FUSE_VSET_MASK;
    vset >>= FUSE_VSET_SHIFT;
    vset <<= VSET_OUT_SHIFT;
    vset |= FBB_MUX_CTRL;

    /* setup LDOVBB using fused value */
    regval = in32(voltctrl_reg);
    regval &= ~VSET_OUT_MASK;
    regval |= vset << VSET_OUT_SHIFT;
    out32(voltctrl_reg, regval);
    return 0;
}

void abb_setup(int domain)
{
    unsigned regval;
    volatile int timeout;
    unsigned setup_reg, ctrl_reg, irqstatus_reg, txdone_mask;

    switch(domain) {
        case DOMAIN_MPU:
            setup_reg       = DRA74X_DEVICE_PRM_BASE + PRM_ABBLDO_MPU_SETUP;
            ctrl_reg        = DRA74X_DEVICE_PRM_BASE + PRM_ABBLDO_MPU_CTRL;
            irqstatus_reg   = DRA74X_OCP_SOCKET_PRM_BASE + PRM_IRQSTATUS_MPU_2;
            txdone_mask     = ABB_MPU_DONE_ST;
            break;
        case DOMAIN_GPU:
            setup_reg       = DRA74X_DEVICE_PRM_BASE + PRM_ABBLDO_GPU_SETUP;
            ctrl_reg        = DRA74X_DEVICE_PRM_BASE + PRM_ABBLDO_GPU_CTRL;
            irqstatus_reg   = DRA74X_OCP_SOCKET_PRM_BASE + PRM_IRQSTATUS_MPU;
            txdone_mask     = ABB_GPU_DONE_ST;
            break;
        case DOMAIN_IVA:
            setup_reg       = DRA74X_DEVICE_PRM_BASE + PRM_ABBLDO_IVA_SETUP;
            ctrl_reg        = DRA74X_DEVICE_PRM_BASE + PRM_ABBLDO_IVA_CTRL;
            irqstatus_reg   = DRA74X_OCP_SOCKET_PRM_BASE + PRM_IRQSTATUS_MPU;
            txdone_mask     = ABB_IVA_DONE_ST;
            break;
        case DOMAIN_DSPEVE:
            setup_reg       = DRA74X_DEVICE_PRM_BASE + PRM_ABBLDO_DSPEVE_SETUP;
            ctrl_reg        = DRA74X_DEVICE_PRM_BASE + PRM_ABBLDO_DSPEVE_CTRL;
            irqstatus_reg   = DRA74X_OCP_SOCKET_PRM_BASE + PRM_IRQSTATUS_MPU;
            txdone_mask     = ABB_DSPEVE_DONE_ST;
            break;
        case DOMAIN_CORE:
        default:
            abb_result[domain] = ABB_NOT_ENABLED;
            return;
    }

    if (abb_setup_ldovbb(domain)) {
        abb_result[domain] = ABB_NOT_ENABLED;
        return;
    }

    /* clear ABB registers */
    out32(setup_reg, 0);
    out32(ctrl_reg, 0);

    /* configure timings, based on oscillator value */
    abb_setup_timings(domain);

    /* clear pending interrupts before setup */
    regval = in32(irqstatus_reg);
    regval |= (txdone_mask);
    out32(irqstatus_reg, regval);

    /* select ABB type */
    regval = in32(setup_reg);
    regval |= OMAP_ABB_SETUP_ACTIVE_FBB_SEL_MASK | OMAP_ABB_SETUP_SR2EN_MASK;
    out32(setup_reg, regval);

    /* initiate ABB ldo change */
    regval = in32(ctrl_reg);
    regval = OMAP_ABB_CONTROL_FAST_OPP_SEL_MASK | OMAP_ABB_CONTROL_OPP_CHANGE_MASK;
    out32(ctrl_reg, regval);

    /* wait until transition complete */
    timeout = 12000000;
    while(!(in32(irqstatus_reg) & txdone_mask) && timeout--);
    if(timeout <= 0) {
        abb_result[domain] = ABB_TRANS_ERROR;
        return;
    }

    /* clear ABB tranxdone */
    regval = in32(irqstatus_reg);
    regval |= (txdone_mask);
    out32(irqstatus_reg, regval);

    abb_result[domain] = ABB_ENABLED;
    return;
}

/*
 * The voltage value is calcualted as follows:
 * vdd = 1,150,000 uV step = 10,000uV PALMAS_SMPS_BASE_VOLT = 500,000uV
 * offset = (vdd - PALMAS_SMPS_BASE_VOLT + step - 1) / step
 * */
static unsigned get_offset_code(unsigned volt_offset)
{
    unsigned    offset_code;

    volt_offset -= SMPS_BASE_VOLT_UV;
    offset_code = (volt_offset + SMPS_BASE_VOLT_UV_STEP - 1) / SMPS_BASE_VOLT_UV_STEP;
    return (offset_code + SMPS_START_CODE);
}

unsigned optimize_vcore_voltage(unsigned efuse_addr, unsigned bits)
{
    if (bits == 16) {
        return in16(efuse_addr);
    } else if (bits == 32) {
        return in32(efuse_addr);
    }

    return 0;
}

void do_scale_vcore(int domain, omap_i2c_dev_t *pmic_dev, unsigned vcore_reg)
{
    unsigned val;
    unsigned char buf[2];
    uint32_t efuse_base = DRA74X_CTRL_CORE_BASE;

    switch(domain) {
        case DOMAIN_MPU:
#ifdef MPU_OPP_HIGH
            val = optimize_vcore_voltage(efuse_base + CTRL_CORE_STD_FUSE_OPP_VMIN_MPU_4, EFUSE_REGBITS);
            if(0 == val) {
                avs_enabled &= ~(1 << DOMAIN_MPU);
                val = MPU_OPP_VOLT_HIGH;
            }
#elif MPU_OPP_OD
            val = optimize_vcore_voltage(efuse_base + CTRL_CORE_STD_FUSE_OPP_VMIN_MPU_3, EFUSE_REGBITS);
            if(0 == val) {
                avs_enabled &= ~(1 << DOMAIN_MPU);
                val = MPU_OPP_VOLT_OD;
            }
#else
            val = optimize_vcore_voltage(efuse_base + CTRL_CORE_STD_FUSE_OPP_VMIN_MPU_2, EFUSE_REGBITS);
            if(0 == val) {
                avs_enabled &= ~(1 << DOMAIN_MPU);
                val = MPU_OPP_VOLT_NOM;
            }
#endif
            break;
        case DOMAIN_CORE:
            val = optimize_vcore_voltage(efuse_base + CTRL_CORE_STD_FUSE_OPP_VMIN_CORE_2, EFUSE_REGBITS);
            if(0 == val) {
                avs_enabled &= ~(1 << DOMAIN_CORE);
                val = CORE_OPP_VOLT_NOM;
            }
            break;
        case DOMAIN_IVA:
#ifdef IVA_OPP_HIGH
            val = optimize_vcore_voltage(efuse_base + CTRL_CORE_STD_FUSE_OPP_VMIN_IVA_4, EFUSE_REGBITS);
            if(0 == val) {
                avs_enabled &= ~(1 << DOMAIN_IVA);
                val = IVA_OPP_VOLT_HIGH;
            }
#else
            val = optimize_vcore_voltage(efuse_base + CTRL_CORE_STD_FUSE_OPP_VMIN_IVA_2, EFUSE_REGBITS);
            if(0 == val) {
                avs_enabled &= ~(1 << DOMAIN_IVA);
                val = IVA_OPP_VOLT_NOM;
            }
#endif
            break;
        case DOMAIN_DSPEVE:
#ifdef DSP_OPP_OD
			val = optimize_vcore_voltage(efuse_base + CTRL_CORE_STD_FUSE_OPP_VMIN_DSPEVE_3, EFUSE_REGBITS);
			if(0 == val) {
				avs_enabled &= ~(1 << DOMAIN_DSPEVE);
				val = DSPEVE_OPP_VOLT_OD;
			}
#else
            val = optimize_vcore_voltage(efuse_base + CTRL_CORE_STD_FUSE_OPP_VMIN_DSPEVE_2, EFUSE_REGBITS);
            if(0 == val) {
                avs_enabled &= ~(1 << DOMAIN_DSPEVE);
                val = DSPEVE_OPP_VOLT_NOM;
            }
#endif
            break;
        case DOMAIN_GPU:
            val = optimize_vcore_voltage(efuse_base + CTRL_CORE_STD_FUSE_OPP_VMIN_GPU_2, EFUSE_REGBITS);
            if(0 == val) {
                avs_enabled &= ~(1 << DOMAIN_GPU);
                val = GPU_OPP_VOLT_NOM;
            }
            break;
        default:
            val = 0;
            break;
    }

    if(0 == val) {
        return;
    }

    /* convert to uV for better accuracy in the calculations */
    buf[1] = (unsigned char)get_offset_code(val * 1000);
    buf[0] = (unsigned char)vcore_reg;

    if (omap_i2c_write(pmic_dev, buf, 2)) {
    } else {
        omap_i2c_read(pmic_dev, vcore_reg, 1, buf, 1);
    }
}

void print_abb_result(char abb_result)
{
    switch(abb_result) {
        case ABB_ENABLED:
            log_putstr("enabled.\n");
            break;
        case ABB_TRANS_ERROR:
            log_putstr("ERROR in enabling ABB!!!!\n");
            break;
        case  ABB_NOT_ENABLED:
        default:
            log_putstr("disabled.\n");
            break;
    }
}

void print_avs_abb_info(void)
{
    /* print AVS/ABB related info */
    log_putstr("\n\n*****************\n");
    log_putstr("MPU AVS 0: ");
    log_putstr((avs_enabled & (1 << DOMAIN_MPU))? "enabled.\n" : "disabled.\n");
    log_putstr("IVA AVS 0: ");
    log_putstr((avs_enabled & (1 << DOMAIN_IVA))? "enabled.\n" : "disabled.\n");
    log_putstr("CORE AVS 0: ");
    log_putstr((avs_enabled & (1 << DOMAIN_CORE))? "enabled.\n" : "disabled.\n");
    log_putstr("DSPEVE AVS 0: ");
    log_putstr((avs_enabled & (1 << DOMAIN_DSPEVE))? "enabled.\n" : "disabled.\n");
    log_putstr("GPU AVS 0: ");
    log_putstr((avs_enabled & (1 << DOMAIN_GPU))? "enabled.\n" : "disabled.\n");

    log_putstr("\nMPU ABB: ");
    print_abb_result(abb_result[DOMAIN_MPU]);
    log_putstr("IVA ABB: ");
    print_abb_result(abb_result[DOMAIN_IVA]);
    log_putstr("DSPEVE ABB: ");
    print_abb_result(abb_result[DOMAIN_DSPEVE]);
    log_putstr("GPU ABB: ");
    print_abb_result(abb_result[DOMAIN_GPU]);
    log_putstr("CORE ABB: disabled.\n");

#ifdef MPU_OPP_HIGH
    log_putstr("\nWARNING: RUNNING AT MPU OPP HIGH!! DVFS Manager is required\n");
#endif

    log_putstr("*****************\n\n");
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/ipl/boards/dra74x/init_voltages.c $ $Rev: 755833 $")
#endif
