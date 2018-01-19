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

/* Pin Mux Registers and the Corresponding Values */
static unsigned pin_data_dp[] = {
	MMC1_CLK, (IEN | PTU | PDIS | M0),
	MMC1_CMD, (IEN | PTU | PDIS | M0),
	MMC1_DAT0, (IEN | PTU | PDIS | M0),
	MMC1_DAT1, (IEN | PTU | PDIS | M0),
	MMC1_DAT2, (IEN | PTU | PDIS | M0),
	MMC1_DAT3, (IEN | PTU | PDIS | M0),
	MMC1_SDCD, (IEN | PDIS | M14),
	MMC1_SDWP, (FSC | IEN | PTD | PEN | M14),
	UART1_RXD, (FSC | IEN | PTU | PDIS | M0),
	UART1_TXD, (FSC | IEN | PTU | PDIS | M0),
	UART1_CTSN, (IEN | PTU | PDIS | M3),
	UART1_RTSN, (IEN | PTU | PDIS | M3),
	I2C1_SDA, (IEN | PTU | PDIS | M0),
	I2C1_SCL, (IEN | PTU | PDIS | M0),
	GPMC_AD0, (IEN | PTD | PEN | M3),
	GPMC_AD1, (IEN | PTD | PEN | M3),
	GPMC_AD2, (IEN | PTD | PEN | M3),
	GPMC_AD3, (IEN | PTD | PEN | M3),
	GPMC_AD4, (IEN | PTD | PEN | M3),
	GPMC_AD5, (IEN | PTD | PEN | M3),
	GPMC_AD6, (IEN | PTD | PEN | M3),
	GPMC_AD7, (IEN | PTD | PEN | M3),
	GPMC_AD8, (IEN | PTD | PEN | M3),
	GPMC_AD9, (IEN | PTD | PEN | M3),
	GPMC_AD10, (IEN | PTD | PEN | M3),
	GPMC_AD11, (IEN | PTD | PEN | M3),
	GPMC_AD12, (IEN | PTD | PEN | M3),
	GPMC_AD13, (IEN | PTD | PEN | M3),
	GPMC_AD14, (IEN | PTD | PEN | M3),
	GPMC_AD15, (IEN | PTD | PEN | M3),
	GPMC_A0, (IEN | PDIS | M3),
	GPMC_A1, (IEN | PDIS | M3),
	GPMC_A2, (IEN | PDIS | M3),
	GPMC_A3, (IEN | PDIS | M3),
	GPMC_A4, (IEN | PDIS | M3),
	GPMC_A5, (IEN | PDIS | M3),
	GPMC_A6, (IEN | PDIS | M3),
	GPMC_A7, (IEN | PDIS | M3),
	GPMC_A8, (IEN | PDIS | M3),
	GPMC_A9, (IEN | PDIS | M3),
	GPMC_A10, (IEN | PDIS | M3),
	GPMC_A11, (IEN | PDIS | M3),
	GPMC_A12, (IEN | PDIS | M15),
	GPMC_A13, (IEN | PDIS | M1),	/* qspi1_rtclk */
	GPMC_A14, (IEN | PDIS | M1),	/* qspi1_d3 */
	GPMC_A15, (IEN | PDIS | M1),	/* qspi1_d2 */
	GPMC_A16, (IEN | PDIS | M1),	/* qspi1_d0 */
	GPMC_A17, (IEN | PDIS | M1),	/* qspi1_d1 */
	GPMC_A18, (IEN | PDIS | M1),	/* qspi1_sclk */
	GPMC_A19, (IEN | PTU | M1),		/* mmc2_dat4 */
	GPMC_A20, (IEN | PTU | M1),		/* mmc2_dat5 */
	GPMC_A21, (IEN | PTU | M1),		/* mmc2_dat6 */
	GPMC_A22, (IEN | PTU | M1),		/* mmc2_dat7 */
	GPMC_A23, (IEN | PTU | M1),		/* mmc2_clk */
	GPMC_A24, (IEN | PTU | M1),		/* mmc2_dat0 */
	GPMC_A25, (IEN | PTU | M1),		/* mmc2_dat1 */
	GPMC_A26, (IEN | PTU | M1),		/* mmc2_dat2 */
	GPMC_A27, (IEN | PTU | M1),		/* mmc2_dat3 */
	GPMC_CS1, (IEN | PTU | M1),		/* mmm2_cmd */
	GPMC_CS0, (IEN | PTU | PDIS | M15),
	GPMC_CS2, (IEN | PTU | PDIS | M1),
	GPMC_CS3, (IEN | PTU | PDIS | M3),
	GPMC_CLK, (IEN | PTU | PDIS | M15),
	GPMC_ADVN_ALE, (IEN | PTU | PDIS | M15),
	GPMC_OEN_REN, (IEN | PTU | PDIS | M15),
	GPMC_WEN, (IEN | PTU | PDIS | M15),
	GPMC_BEN0, (IEN | PTU | PDIS | M15),
	GPMC_BEN1, (IEN | PTU | PDIS | M15),
	GPMC_WAIT0, (FSC | IEN | PTU | PDIS | M15),
	VIN1A_CLK0, (IEN | PDIS | M0),
	VIN1B_CLK1, (FSC | IEN | PDIS | M15),
	VIN1A_DE0, (IEN | PDIS | M0),
	VIN1A_FLD0, (IEN | PDIS | M0),
	VIN1A_HSYNC0, (IEN | PDIS | M0),
	VIN1A_VSYNC0, (IEN | PDIS | M0),
	VIN1A_D0, (IEN | PDIS | M0),
	VIN1A_D1, (IEN | PDIS | M0),
	VIN1A_D2, (IEN | PDIS | M0),
	VIN1A_D3, (IEN | PDIS | M0),
	VIN1A_D4, (IEN | PDIS | M0),
	VIN1A_D5, (IEN | PDIS | M0),
	VIN1A_D6, (IEN | PDIS | M0),
	VIN1A_D7, (IEN | PDIS | M0),
	VIN1A_D8, (IEN | PDIS | M0),
	VIN1A_D9, (IEN | PDIS | M0),
	VIN1A_D10, (IEN | PDIS | M0),
	VIN1A_D11, (IEN | PDIS | M0),
	VIN1A_D12, (IEN | PDIS | M0),
	VIN1A_D13, (IEN | PDIS | M0),
	VIN1A_D14, (IEN | PDIS | M0),
	VIN1A_D15, (IEN | PDIS | M0),
	VIN1A_D16, (IEN | PDIS | M0),
	VIN1A_D17, (IEN | PDIS | M0),
	VIN1A_D18, (IEN | PDIS | M0),
	VIN1A_D19, (IEN | PDIS | M0),
	VIN1A_D20, (IEN | PDIS | M0),
	VIN1A_D21, (IEN | PDIS | M0),
	VIN1A_D22, (IEN | PDIS | M0),
	VIN1A_D23, (IEN | PDIS | M0),
	VIN2A_CLK0, (IEN | PDIS | M15),
	VIN2A_DE0, (IEN | PDIS | M15),
	VIN2A_FLD0, (IEN | PDIS | M15),
	VIN2A_HSYNC0, (IEN | PDIS | M15),
	VIN2A_VSYNC0, (IEN | PDIS | M15),
	VIN2A_D0, (IEN | PDIS | M15),
	VIN2A_D1, (IEN | PDIS | M15),
	VIN2A_D2, (IEN | PDIS | M15),
	VIN2A_D3, (IEN | PDIS | M15),
	VIN2A_D4, (IEN | PDIS | M15),
	VIN2A_D5, (IEN | PDIS | M15),
	VIN2A_D6, (IEN | PDIS | M15),
	VIN2A_D7, (IEN | PDIS | M15),
	VIN2A_D8, (IEN | PDIS | M15),
	VIN2A_D9, (IEN | PDIS | M15),
	VIN2A_D10, (IEN | PDIS | M15),
	VIN2A_D11, (IEN | PDIS | M15),
	VIN2A_D12, (M3),
	VIN2A_D13, (M3),
	VIN2A_D14, (M3),
	VIN2A_D15, (M3),
	VIN2A_D16, (M3),
	VIN2A_D17, (M3),
	VIN2A_D18, (IEN | PDIS | M3),
	VIN2A_D19, (IEN | PDIS | M3),
	VIN2A_D20, (IEN | PDIS | M3),
	VIN2A_D21, (IEN | PDIS | M3),
	VIN2A_D22, (IEN | PDIS | M3),
	VIN2A_D23, (IEN | PDIS | M3),
	VOUT1_CLK, (IEN | PDIS | M0),
	VOUT1_DE, (IEN | PDIS | M0),
	VOUT1_FLD, (IEN | PDIS | M15),
	VOUT1_HSYNC, (IEN | PDIS | M0),
	VOUT1_VSYNC, (IEN | PDIS | M0),
	VOUT1_D0, (IEN | PDIS | M0),
	VOUT1_D1, (IEN | PDIS | M0),
	VOUT1_D2, (IEN | PDIS | M0),
	VOUT1_D3, (IEN | PDIS | M0),
	VOUT1_D4, (IEN | PDIS | M0),
	VOUT1_D5, (IEN | PDIS | M0),
	VOUT1_D6, (IEN | PDIS | M0),
	VOUT1_D7, (IEN | PDIS | M0),
	VOUT1_D8, (IEN | PDIS | M0),
	VOUT1_D9, (IEN | PDIS | M0),
	VOUT1_D10, (IEN | PDIS | M0),
	VOUT1_D11, (IEN | PDIS | M0),
	VOUT1_D12, (IEN | PDIS | M0),
	VOUT1_D13, (IEN | PDIS | M0),
	VOUT1_D14, (IEN | PDIS | M0),
	VOUT1_D15, (IEN | PDIS | M0),
	VOUT1_D16, (IEN | PDIS | M0),
	VOUT1_D17, (IEN | PDIS | M0),
	VOUT1_D18, (IEN | PDIS | M0),
	VOUT1_D19, (IEN | PDIS | M0),
	VOUT1_D20, (IEN | PDIS | M0),
	VOUT1_D21, (IEN | PDIS | M0),
	VOUT1_D22, (IEN | PDIS | M0),
	VOUT1_D23, (IEN | PDIS | M0),
	MDIO_MCLK, (PTU | PEN | M0),
	MDIO_D, (IEN | PTU | PEN | M0),
	RMII_MHZ_50_CLK, (IEN | PDIS | M15),
	UART3_RXD, (FSC | IEN | PDIS | M15),
	UART3_TXD, (FSC | IEN | PDIS | M15),
	RGMII0_TXC, (M0),
	RGMII0_TXCTL, (M0),
	RGMII0_TXD3, (M0),
	RGMII0_TXD2, (M0),
	RGMII0_TXD1, (M0),
	RGMII0_TXD0, (M0),
	RGMII0_RXC, (IEN | M0),
	RGMII0_RXCTL, (IEN | M0),
	RGMII0_RXD3, (IEN | M0),
	RGMII0_RXD2, (IEN | M0),
	RGMII0_RXD1, (IEN | M0),
	RGMII0_RXD0, (IEN | M0),
	USB1_DRVVBUS, (IEN | PTU | PDIS | M14),	/* GPIO6_12 so as USB host driver can control vbus */
	USB2_DRVVBUS, (IEN | PTU | PDIS | M14),	/* GPIO6_13 so as USB host driver can control vbus */
	GPIO6_14, (IEN | PTU | PEN | M9),
	GPIO6_15, (IEN | PTU | PEN | M9),
	GPIO6_16, (IEN | PTU | PDIS | M14),
	XREF_CLK0, (IEN | PDIS | M4),
	XREF_CLK1, (IEN | PDIS | M4),
	XREF_CLK2, (IEN | PDIS | M3),
	XREF_CLK3, (IEN | PTD | PEN | M14),
	MCASP1_ACLKX, (IEN | PDIS | M0),
	MCASP1_FSX, (FSC | IEN | PDIS | M0),
	MCASP1_ACLKR, (IEN | PTD | PEN | M14),
	MCASP1_FSR, (IEN | PDIS | M15),
	MCASP1_AXR0, (FSC | IEN | PDIS | M0),
	MCASP1_AXR1, (FSC | IEN | PDIS | M0),
	MCASP1_AXR2, (IEN | PTD | PEN | M14),	/* GPIO5_4 as BT_EN */
	MCASP1_AXR3, (IEN | PTD | PEN | M14),	/* GPIO5_5 as GPS_PPS_OUT */
	MCASP1_AXR4, (IEN | PTD | PEN | M14),	/* GPIO5_6 as GPS_TIME_STAMP */
	MCASP1_AXR5, (IEN | PTD | PEN | M14),	/* GPIO5_7 as WLAN_IRQ */
	MCASP1_AXR6, (IEN | PTD | PEN | M14),	/* GPIO5_8 as WL_EN */
	MCASP1_AXR7, (IEN | PTD | PEN | M14),
	MCASP1_AXR8, (FSC | IEN | PDIS | M1),
	MCASP1_AXR9, (FSC | IEN | PDIS | M1),
	MCASP1_AXR10, (FSC | IEN | PDIS | M1),
	MCASP1_AXR11, (FSC | IEN | PDIS | M1),
	MCASP1_AXR12, (FSC | IEN | PTU | PEN | M1),	/* BT(PCM_DOUT, input): McASP7_AXR0 */
	MCASP1_AXR13, (FSC | IEN | PTD | PEN | M1),	/* BT(PCM_PIN, output): McASP7_AXR1 */
	MCASP1_AXR14, (FSC | IEN | PTU | PEN | M1),	/* BT: McASP7_CLKX, input in slave mode */
	MCASP1_AXR15, (FSC | IEN | PTU | PEN | M1),	/* BT: McASP7_FSX, input in slave mode */
	MCASP2_ACLKX, (IEN | PDIS | M0),
	MCASP2_FSX, (FSC | IEN | PDIS | M0),
	MCASP2_ACLKR, (IEN | PDIS | M15),
	MCASP2_FSR, (IEN | PDIS | M15),
	MCASP2_AXR0, (IEN | PDIS | M0),
	MCASP2_AXR1, (IEN | PDIS | M0),
	MCASP2_AXR2, (FSC | IEN | PDIS | M0),
	MCASP2_AXR3, (FSC | IEN | PDIS | M0),
	MCASP2_AXR4, (IEN | PDIS | M0),
	MCASP2_AXR5, (IEN | PDIS | M0),
	MCASP2_AXR6, (IEN | PDIS | M0),
	MCASP2_AXR7, (IEN | PDIS | M0),
	MCASP3_ACLKX, (IEN | PDIS | M0),
	MCASP3_FSX, (FSC | IEN | PDIS | M0),
	MCASP3_AXR0, (FSC | IEN | PDIS | M0),
	MCASP3_AXR1, (FSC | IEN | PDIS | M0),
	MCASP4_ACLKX, (IEN | PTU | PEN | M4),
	MCASP4_FSX, (IEN | PTU | PEN | M4),
	MCASP4_AXR0, (IEN | PDIS | M15),
	MCASP4_AXR1, (IEN | PDIS | M15),
	MCASP5_ACLKX, (IEN | PDIS | M0),
	MCASP5_FSX, (IEN | PDIS | M0),
	MCASP5_AXR0, (IEN | PDIS | M0),
	MCASP5_AXR1, (IEN | PDIS | M15),
	GPIO6_10, (IEN | PTU | PDIS | M15),
	GPIO6_11, (IEN | PTU | PDIS | M0),
	MMC3_CLK, (IEN | PTU | PDIS | M0),
	MMC3_CMD, (IEN | PTU | PDIS | M0),
	MMC3_DAT0, (IEN | PTU | PDIS | M0),
	MMC3_DAT1, (IEN | PTU | PDIS | M0),
	MMC3_DAT2, (IEN | PTU | PDIS | M0),
	MMC3_DAT3, (IEN | PTU | PDIS | M0),
	MMC3_DAT4, (IEN | PTU | PDIS | M15),
	MMC3_DAT5, (IEN | PTU | PDIS | M15),
	MMC3_DAT6, (IEN | PTU | PDIS | M15),
	MMC3_DAT7, (IEN | PTU | PDIS | M15),
	SPI1_SCLK, (IEN | PDIS | M0),				/* spi1_sclk */
	SPI1_D1, (IEN | PDIS | M0),					/* spi1_d1 */
	SPI1_D0, (IEN | PDIS | M0),					/* spi1_d0 */
	SPI1_CS0, (IEN | PTU | PDIS | M0),			/* spi1_cs0 */
	SPI1_CS1, (IEN | PTU | PDIS | M14),			/* GPIO7_11 */
	SPI1_CS2, (FSC | IEN | PTU | PDIS | M6),	/* hdmi_hpd */
	SPI1_CS3, (FSC | IEN | PTU | PEN | M6),		/* hdmi_cec */
	SPI2_SCLK, (IEN | PDIS | M1),				/* uart3_rxd */
	SPI2_D1, (FSC | IEN | PDIS | M1),			/* uart3_txd */
	SPI2_D0, (FSC | IEN | PDIS | M1),			/* uart3_cts */
	SPI2_CS0, (FSC | IEN | PTU | PDIS | M1),	/* uart3_rts */
	DCAN1_TX, (FSC | IEN | PTU | PDIS | M0),
	DCAN1_RX, (FSC | IEN | PTU | PDIS | M0),
	UART2_RXD, (IEN | PTU | PDIS | M3),
	UART2_TXD, (IEN | PTU | PDIS | M3),
	UART2_CTSN, (IEN | PTU | PDIS | M3),
	UART2_RTSN, (IEN | PTU | PDIS | M3),
	I2C2_SDA, (IEN | PTU | PDIS | M0),
	I2C2_SCL, (IEN | PTU | PDIS | M0),
	WAKEUP0, (PEN | M0),
	WAKEUP1, (PEN | M0),
	WAKEUP2, (IEN | PDIS | M14),				/* For touch screen IRQ */
	WAKEUP3, (PEN | M15),
	ON_OFF, (PTU | PDIS | M0),
	RTC_PORZ, (PEN | M0),
	TMS, (IEN | PTU | PDIS | M0),
	TDI, (FSC | IEN | PTU | PDIS | M0),
	TDO, (IEN | PTU | PDIS | M0),
	TCLK, (IEN | PTU | PDIS | M0),
	TRSTN, (IEN | PDIS | M0),
	RTCK, (IEN | PTD | PEN | M0),
	EMU0, (IEN | PTU | PDIS | M0),
	EMU1, (IEN | PTU | PDIS | M0),
	RESETN, (PTU | PDIS | M0),
	NMIN, (PDIS | M0),
	RSTOUTN, (PDIS | M0),
};

/* Initialize the Pin Muxing */
void init_pinmux(void)
{
	int i, size;

	size = sizeof(pin_data_dp) / sizeof(unsigned);
	for (i = 0; i < size; i += 2)
		out32(DRA74X_CONTROL_PADCONF_CORE_BASE + pin_data_dp[i], pin_data_dp[i + 1]);
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/ipl/boards/dra74x/arm/vayu-evm.le.v7/init_pinmux.c $ $Rev: 751557 $")
#endif
