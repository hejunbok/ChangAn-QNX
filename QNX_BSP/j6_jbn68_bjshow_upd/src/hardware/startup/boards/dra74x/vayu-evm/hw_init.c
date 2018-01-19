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
#include "board.h"

extern int pmic_revision;

/* TLC59108 registers */
#define SUBADR1_ADDR		0x0E
#define SUBADR2_ADDR		0x0F
#define SUBADR3_ADDR		0x10
#define SUBADR1_DEFAULT		0x092
#define SUBADR2_DEFAULT		0x094
#define SUBADR3_DEFAULT		0x098

/* Amount of time to leave WiFi chip in reset */
#define WIFI_RESET_DELAY	200000

/***************************************************************************************************
 * Crossbar interrupts
 ***************************************************************************************************/
const struct crossbar_entry mpu_irq_map[] = {
	/* MPU_IRQ, CROSSBAR_IRQ */
	{49, 361},		/* EDMA_TPPC_IRQ_REGION0 */
	{145, 232},		/* PCIe_SS1_IRQ_INT0 mapped to MPU_IRQ_145 */
	{146, 233},		/* PCIe_SS1_IRQ_INT1 mapped to MPU_IRQ_146 */
	{147, 355},		/* PCIe_SS2_IRQ_INT0 mapped to MPU_IRQ_147 */
	{148, 356},		/* PCIe_SS2_IRQ_INT1 mapped to MPU_IRQ_148 */
	
	{84, 219},		/* UART8_IRQ(IRQ_CROSSBAR_219) mapped to MPU_IRQ_84*/

	/*
	 * McASP1 interrupts are mapped to MPU_IRQ directly by default
	 *		McASP1_IRQ_AREVT -> MPU_IRQ_108
	 *		McASP1_IRQ_AXEVT -> MPU_IRQ_109
	 */
	 
//	{155, 148},		/* MCASP2_IRQ_AREVT mapped to MPU_IRQ_155 */
//	{156, 149},		/* MCASP2_IRQ_AXEVT mapped to MPU_IRQ_156 */
	{158, 150},		/* MCASP3_IRQ_AREVT mapped to MPU_IRQ_158 */
	{157, 151},		/* MCASP3_IRQ_AXEVT mapped to MPU_IRQ_157 */
	{151, 156},		/* MCASP6_IRQ_AREVT mapped to MPU_IRQ_151 */
	{152, 157},		/* MCASP6_IRQ_AXEVT mapped to MPU_IRQ_152 */
	{153, 158},		/* MCASP7_IRQ_AREVT mapped to MPU_IRQ_153 */
	{154, 159},		/* MCASP7_IRQ_AXEVT mapped to MPU_IRQ_154 */

	{115, 335},		/* GMAC_SW_IRQ_RX_PULSE */
	{116, 336},		/* GMAC_SW_IRQ_TX_PULSE */
	{117, 337},		/* GMAC_SW_IRQ_MISC_PULSE */

	{VIP1_IRQ, 351},	/* VIP1_IRQ_1 */
	{VIP2_IRQ, 352},	/* VIP2_IRQ_1 */
	{VIP3_IRQ, 353},	/* VIP3_IRQ_1 */
	{VPE_IRQ, 354},	/* VPE_IRQ */

	{CRYPTO_AES_IRQ, 110},	/* Crypto: AES */
	{CRYPTO_MD5_IRQ, 44},	/* Crypto: MD5 */
};

const struct crossbar_entry edma_dreq_map[] = {
	/* DMA_EDMA_DREQ, CROSSBAR_DMA */
	{9, 128},			/* MCASP1_DREQ_RX */
	{8, 129},			/* MCASP1_DREQ_TX */
//	{11, 130},		/* MCASP2_DREQ_RX */
//	{10, 131},		/* MCASP2_DREQ_TX */
	{59, 132},		/* MCASP3_DREQ_RX */
	{58, 133},		/* MCASP3_DREQ_TX */
	{17, 138},		/* MCASP6_DREQ_RX */
	{16, 139},		/* MCASP6_DREQ_TX */
	{19, 140},		/* MCASP7_DREQ_RX */
	{18, 141},		/* MCASP7_DREQ_TX */
	{30, 147},		/* UART8_DREQ_RX(DMA_CROSSB AR_147) mapped to DMA_EDMA_DREQ_30*/	
	{31, 146},		/* UART8_DREQ_TX(DMA_CROSSB AR_146) mapped to DMA_EDMA_DREQ_31*/	
};

static void set_crossbar_reg(void)
{
	dra74x_set_mpu_crossbar(DRA74X_CTRL_CORE_BASE + CTRL_CORE_MPU_IRQ_4_5, mpu_irq_map,
		sizeof(mpu_irq_map)/sizeof(struct crossbar_entry));

	dra74x_set_edma_crossbar(DRA74X_CTRL_CORE_BASE + CTRL_CORE_DMA_EDMA_DREQ_0_1, edma_dreq_map,
		sizeof(edma_dreq_map)/sizeof(struct crossbar_entry));
}

#if 0
/***************************************************************************************************
 * COM8 module
 ***************************************************************************************************/
static void init_com8()
{
	uint8_t buf[2];
	omap_i2c_dev_t i2c_dev = {
		.base = DRA74X_I2C1_BASE,
		.clock = I2C_SPEED,
		.slave = IO_EXPANDER_2_I2C_ADDR
	};

	/* IO Expander PCF8575, U57 */
	omap_i2c_init(&i2c_dev);

	memset(buf, NULL, sizeof (buf));
	if (0 != omap_i2c_read(&i2c_dev, 0, 1, buf, 2)) {
		kprintf("failed to read from PCF8575\n");
	}

	/* Clear P15 (McASP1_ENn) to enable COM8
	 * Clear P16 (SEL_UART3_SPI2) to select UART3 over SPI2
	 * Note: Switches 6 (McASP1_ENn) on SW5 (USERCONFIG) must
	 *		also be in the OFF position.
	 */
	buf[1] &= ~(PCF8575_P15 | PCF8575_P16);

	if (gpmc_vout3) {
		/* Clear P0 (SEL_GPMC_AD_VID_S0) to enable FPD-III */
		buf[0] &= ~(PCF8575_P0);
	} else {
		/* Set P0 (SEL_GPMC_AD_VID_S0) to enable GPMC interface for either NAND for NOR Flash */
		buf[0] |= PCF8575_P0;
	}

	if (0 != omap_i2c_write(&i2c_dev, buf, 2)) {
		kprintf("failed to write to PCF8575\n");
	}

	/* IO Expander PCF8575, U119 */
	i2c_dev.base = DRA74X_I2C2_BASE,
	i2c_dev.slave = IO_EXPANDER_3_I2C_ADDR;
	omap_i2c_init(&i2c_dev);

	memset(buf, NULL, sizeof (buf));
	if (0 != omap_i2c_read(&i2c_dev, 0, 1, buf, 2)) {
		kprintf("failed to read from PCF8575\n");
	}

	/* Clear P1 (VIN6_SEL_S0) to select McASP1,2,3, & 7 */
	buf[0] &= ~(PCF8575_P1);
	if (0 != omap_i2c_write(&i2c_dev, buf, 2)) {
		kprintf("failed to write to PCF8575\n");
	}

	/* PMIC configuration */
	i2c_dev.base = DRA74X_I2C1_BASE;
	i2c_dev.slave = TPS659038_I2C_SLAVE_ADDR;
	omap_i2c_init(&i2c_dev);

	/* select CLK32KGA as GPIO_5 output */
	buf[0] = PMIC_PAD2A;
	buf[1] = 0x2b;
	if (omap_i2c_write (&i2c_dev, buf, 2) != 0) {
		kprintf ("PMIC write PAD2 failed\n");
	}

	/* enable CLK32K */
	buf[0] = PMIC_CLK32AUDIO_CTRLA;
	buf[1] = 0x01;
	if (omap_i2c_write (&i2c_dev, buf, 2) != 0) {
		kprintf ("PMIC write CTRLA failed\n");
	}

	omap_i2c_deinit(&i2c_dev);
}

/***************************************************************************************************
 * JAMR3 board init
 ***************************************************************************************************/
static void init_jamr3()
{
	uint8_t buf[2];

	/*
	 * This is for IO Expander PCF8575, U0005 on JAMR3 application board
	 * Please note that on JAMR3, SW2 should be set to OFF-OFF,
	 * so that I2C4 on CPU board can connect to JAMR3
	 */
	omap_i2c_dev_t i2c_dev = {
		.base = DRA74X_I2C4_BASE,
		.clock = I2C_SPEED,
		.slave = IO_EXPANDER_2_I2C_ADDR
	};

	omap_i2c_init(&i2c_dev);

	memset(buf, NULL, sizeof (buf));
	if (0 != omap_i2c_read(&i2c_dev, 0, 1, buf, 2)) {
		kprintf("failed to read from PCF8575 on JAMR3 board\n");
	}

	/* Clear P10 (SEL_TVP_FPD) to enable the rearview camera */
	buf[1] &= ~PCF8575_P10;
	if (0 != omap_i2c_write(&i2c_dev, buf, 2)) {
		kprintf("failed to write to PCF8575 on JAMR3 board\n");
	}

	omap_i2c_deinit(&i2c_dev);
}
#endif

/***************************************************************************************************
 * ALL
 ***************************************************************************************************/
/* Pin Mux Registers and the Corresponding Values */
static const uint32_t pin_data_all[] = {

	/* U52-15*/
	MMC1_CLK, (IEN | PTU | PDIS | M0),    //MMC1_CLK
	MMC1_CMD, (IEN | PTU | PDIS | M0),    //MMC1_CMD
	MMC1_DAT0, (IEN | PTU | PDIS | M0),    //MMC1_D0
	MMC1_DAT1, (IEN | PTU | PDIS | M0),    //MMC1_D1
	MMC1_DAT2, (IEN | PTU | PDIS | M0),    //MMC1_D2
	MMC1_DAT3, (IEN | PTU | PDIS | M0),    //MMC1_D3
	MMC1_SDCD, (IEN | PTU | PDIS | M0),    //MMC1_CD
	MMC1_SDWP, (IEN | PTU | PDIS | M0),    //MMC1_WP
	MMC3_CLK, (IEN | PDIS | M4),    //VIN2B_D7
	MMC3_CMD, (IEN | PDIS | M4),    //VIN2B_D6
	MMC3_DAT0, (IEN | PDIS | M4),    //VIN2B_D5
	MMC3_DAT1, (IEN | PDIS | M4),    //VIN2B_D4
	MMC3_DAT2, (IEN | PDIS | M4),    //VIN2B_D3
	MMC3_DAT3, (IEN | PDIS | M4),    //VIN2B_D2
	MMC3_DAT4, (IEN | PDIS | M4),    //VIN2B_D1
	MMC3_DAT5, (IEN | PDIS | M4),    //VIN2B_D0
	MMC3_DAT6, (IEN | PDIS | M4),    //VIN2B_DE1
	MMC3_DAT7, (IEN | PDIS | M4),    //VIN2B_CLK1

	/* U52-14 */
	DCAN1_TX, (FSC | IEN | PTU | PEN | M6),    //HDMI_CEC
	DCAN1_RX, (FSC | IEN | PTU | PDIS | M6),    //HDMI_HPD
	GPIO6_10, (IEN | PDIS | M4),    //VIN2B_HSYNC1
	GPIO6_11, (IEN | PDIS | M4),    //VIN2B_VSYNC1
	GPIO6_14, (IEN | PDIS | M6),    //VOUT2_HSYNC
	GPIO6_15, (IEN | PDIS | M6),    //VOUT2_VSYNC
	GPIO6_16, (IEN | PDIS | M6),    //VOUT2_FLD
	SPI1_SCLK, (IEN | PDIS | M0),    //SPI1_SCLK
	SPI1_CS0, (IEN | PTU | PDIS | M0),    //SPI1_CS0
	SPI1_CS1, (IEN | PTU | PDIS | M0),    //SPI1_CS1
	SPI1_CS2, (FSC | IEN | PTU | PDIS | M1),	    //UART4_RXD
	SPI1_CS3, (FSC | IEN | PTU | PDIS | M1),	    //UART4_TXD
	SPI1_D1, (IEN | PDIS | M0),	    //SPI1_D1
	SPI1_D0, (IEN | PDIS | M0),    //SPI1_D0
	SPI2_SCLK, (IEN | PDIS | M1),    //UART3_RXD
	SPI2_D1, (FSC | IEN | PDIS | M1),    //UART3_TXD
	SPI2_D0, (FSC | IEN | PDIS | M1),    //UART3_CTSN
	SPI2_CS0, (FSC | IEN | PTU | PDIS | M1),    //UART3_RTSN
	UART1_RXD, (FSC | IEN | PTU | PDIS | M0),    //UART1_RXD
	UART1_TXD, (FSC | IEN | PTU | PDIS | M0),    //UART1_TXD
	UART1_CTSN, (IEN | PTU | PDIS | M3),    //MMC4_CLK
	UART1_RTSN, (IEN | PTU | PDIS | M3),    //MMC4_CMD
	UART2_RXD, (IEN | PTU | PDIS | M3),    //MMC4_D0
	UART2_TXD, (IEN | PTU | PDIS | M3),    //MMC4_D1
	UART2_CTSN, (IEN | PTU | PDIS | M3),    //MMC4_D2
	UART2_RTSN, (IEN | PTU | PDIS | M3),    //MMC4_D3
	UART3_RXD, (FSC | IEN | PDIS | M15),    //NC
	UART3_TXD, (FSC | IEN | PDIS | M15),    //NC
	I2C2_SDA, (IEN | PTU | PDIS | M0),    //I2C2_SDA
	I2C2_SCL, (IEN | PTU | PDIS | M0),    //I2C2_SCL
	I2C1_SDA, (IEN | PTU | PDIS | M0),    //I2C1_SDA
	I2C1_SCL, (IEN | PTU | PDIS | M0),    //I2C1_SCL

	/* U52-4 */
	GPMC_WAIT0, (FSC | IEN | PTU | PDIS | M15),    //NC
	GPMC_AD0, (IEN | PTD | PEN | M3),   //VOUT3_D0
	GPMC_AD1, (IEN | PTD | PEN | M3),   //VOUT3_D1
	GPMC_AD2, (IEN | PTD | PEN | M3),   //VOUT3_D2
	GPMC_AD3, (IEN | PTD | PEN | M3),   //VOUT3_D3
	GPMC_AD4, (IEN | PTD | PEN | M3),   //VOUT3_D4
	GPMC_AD5, (IEN | PTD | PEN | M3),   //VOUT3_D5
	GPMC_AD6, (IEN | PTD | PEN | M3),   //VOUT3_D6
	GPMC_AD7, (IEN | PTD | PEN | M3),   //VOUT3_D7
	GPMC_AD8, (IEN | PTD | PEN | M3),   //VOUT3_D8
	GPMC_AD9, (IEN | PTD | PEN | M3),   //VOUT3_D9
	GPMC_AD10, (IEN | PTD | PEN | M3),   //VOUT3_D10
	GPMC_AD11, (IEN | PTD | PEN | M3),   //VOUT3_D11
	GPMC_AD12, (IEN | PTD | PEN | M3),   //VOUT3_D12
	GPMC_AD13, (IEN | PTD | PEN | M3),   //VOUT3_D13
	GPMC_AD14, (IEN | PTD | PEN | M3),   //VOUT3_D14
	GPMC_AD15, (IEN | PTD | PEN | M3),   //VOUT3_D15
	GPMC_A0, (IEN | PDIS | M3),   //VOUT3_D16
	GPMC_A1, (IEN | PDIS | M3),   //VOUT3_D17
	GPMC_A2, (IEN | PDIS | M3),   //VOUT3_D18
	GPMC_A3, (IEN | PDIS | M3),   //VOUT3_D19
	GPMC_A4, (IEN | PDIS | M3),   //VOUT3_D20
	GPMC_A5, (IEN | PDIS | M3),   //VOUT3_D21
	GPMC_A6, (IEN | PDIS | M3),   //VOUT3_D22
	GPMC_A7, (IEN | PDIS | M3),   //VOUT3_D23
	GPMC_A8, (IEN | PDIS | M3),   //VOUT3_HSYNC
	GPMC_A9, (IEN | PDIS | M3),   //VOUT3_VSYNC
	GPMC_A10, (IEN | PDIS | M3),   //VOUT3_DE
	GPMC_A11, (IEN | PDIS | M3),   //VOUT3_FLD
	GPMC_A12, (IEN | PDIS | M15),    //NC
	GPMC_A13, (IEN | PDIS | M1),    //QSPI_RTCLK
	GPMC_A14, (IEN | PDIS | M1),    //QSPI_D3
	GPMC_A15, (IEN | PDIS | M1),    //QSPI_D2
	GPMC_A16, (IEN | PDIS | M1),    //QSPI_D1
	GPMC_A17, (IEN | PDIS | M1),    //QSPI_D0
	GPMC_A18, (IEN | PDIS | M1),    //QSPI_SCLK
	/*
	 * J6 errata i863: MMC2 Has PU/PD Contention Immediately After Release From Reset
	 *
	 * SW should minimize the time eMMC DAT pins spend in the PU/PD contention state to a maximum of 
	 * 200 hours in a device life cycle. This is done by configuring MMC pinmux configuration as early as possible,
	 * and enable PU on eMMC DAT lines simultaneously.
	 */
	GPMC_A19, (IEN | PTU | M1),    //MMC2_D4
	GPMC_A20, (IEN | PTU | M1),    //MMC2_D5
	GPMC_A21, (IEN | PTU | M1),    //MMC2_D6
	GPMC_A22, (IEN | PTU | M1),    //MMC2_D7
	GPMC_A23, (IEN | PTU | M1),    //MMC2_CLK
	GPMC_A24, (IEN | PTU | M1),    //MMC2_D0
	GPMC_A25, (IEN | PTU | M1),    //MMC2_D1
	GPMC_A26, (IEN | PTU | M1),    //MMC2_D2
	GPMC_A27, (IEN | PTU | M1),    //MMC2_D3
	GPMC_CS0, (IEN | PTU | PDIS | M14),    //GPIO2_19, PMIC_INT_3V3
	GPMC_CS1, (IEN | PTU | M1),    //MMC2_CMD
	GPMC_CS2, (IEN | PDIS | M1),    //QSPI1_CS0
	GPMC_CS3, (IEN | PDIS | M3),    //VOUT3_CLK
	GPMC_CLK, (IEN | PTU | PDIS | M8),    //I2C3_SCL
	GPMC_BEN0, (IEN | PTU | PDIS | M15),    //NC
	GPMC_BEN1, (IEN | PTU | PDIS | M15),    //NC
	GPMC_ADVN_ALE, (IEN | PTU | PDIS | M8),    //I2C3_SDA
	GPMC_OEN_REN, (IEN | PTU | PDIS | M15),    //NC
	GPMC_WEN, (IEN | PTU | PDIS | M15),    //NC

	/* U52-8 */
	VIN1A_CLK0, (IEN | PTD | PEN | M14),    //GPIO2_30
	VIN1B_CLK1, (IEN | PTD | PEN | M14),    //GPIO2_31
	VIN1A_DE0, (FSC | IEN | PTU | PDIS | M5),    //UART7_RXD
	VIN1A_FLD0, (FSC | IEN | PTU | PDIS | M5),    //UART7_TXD
	VIN1A_HSYNC0, (FSC | IEN | PDIS | M5),    //UART7_CTSN
	VIN1A_VSYNC0, (FSC | IEN | PTU | PDIS | M5),    //UART7_RTSN
	VIN1A_D0, (FSC | IEN | PTU | PDIS | M5),    //UART8_RXD
	VIN1A_D1, (FSC | IEN | PTU | PDIS | M5),    //UART8_TXD
	VIN1A_D2, (FSC | IEN | PDIS | M5),    //UART8_CTSN
	VIN1A_D3, (FSC | IEN | PTU | PDIS | M5),    //UART8_RTSN
	VIN1A_D4, (IEN | PTD | PEN | M14),    //GPIO3_8
	VIN1A_D5, (IEN | PTD | PEN | M14),    //GPIO3_9
	VIN1A_D6, (IEN | PTD | PEN | M14),    //GPIO3_10
	VIN1A_D7, (IEN | PTD | PEN | M14),    //GPIO3_11
	VIN1A_D8, (IEN | PTD | PEN | M14),    //GPIO3_12
	VIN1A_D9, (IEN | PTD | PEN | M14),    //GPIO3_13
	VIN1A_D10, (IEN | PTD | PEN | M14),    //GPIO3_14
	VIN1A_D11, (IEN | PTD | PEN | M14),    //GPIO3_15
	VIN1A_D12, (IEN | PTD | PEN | M14),    //GPIO3_16
	VIN1A_D13, (IEN | PTD | PEN | M14),    //GPIO3_17
	VIN1A_D14, (IEN | PTD | PEN | M14),    //GPIO3_18
	VIN1A_D15, (IEN | PTD | PEN | M14),    //GPIO3_19
	VIN1A_D16, (IEN | PTD | PEN | M14),    //GPIO3_20
	VIN1A_D17, (IEN | PTD | PEN | M14),    //GPIO3_21
	VIN1A_D18, (IEN | PTD | PEN | M14),    //GPIO3_22
	VIN1A_D19, (IEN | PTD | PEN | M14),    //GPIO3_23
	VIN1A_D20, (IEN | PTD | PEN | M14),    //GPIO3_24
	VIN1A_D21, (IEN | PTD | PEN | M14),    //GPIO3_25
	VIN1A_D22, (IEN | PTD | PEN | M14),    //GPIO3_26
	VIN1A_D23, (IEN | PTD | PEN | M14),    //GPIO3_27
	VIN2A_CLK0, (IEN | PTD | PEN | M14),    //GPIO3_28
	VIN2A_DE0, (IEN | PTD | PEN | M14),    //GPIO3_29
	VIN2A_FLD0, (IEN | PDIS | M15),    //NC
	VIN2A_HSYNC0, (IEN | PDIS | M8),    //SPI4_SCLK
	VIN2A_VSYNC0, (IEN | PDIS | M8),    //SPI4_D1
	VIN2A_D0, (IEN | PDIS | M8),    //SPI4_D0
	VIN2A_D1, (IEN | PDIS | M8),    //SPI4_CS0
	VIN2A_D2, (IEN | PTD | PEN | M14),    //GPIO4_3
	VIN2A_D3, (IEN | PTD | PEN | M14),    //GPIO4_4
	VIN2A_D4, (IEN | PTD | PEN | M14),    //GPIO4_5
	VIN2A_D5, (IEN | PTD | PEN | M14),    //GPIO4_6
	VIN2A_D6, (IEN | PTD | PEN | M14),    //GPIO4_7
	VIN2A_D7, (IEN | PTD | PEN | M14),    //GPIO4_8
	VIN2A_D8, (IEN | PTD | PEN | M14),    //GPIO4_9
	VIN2A_D9, (IEN | PTD | PEN | M14),    //GPIO4_10
	VIN2A_D10, (IEN | PDIS | M15),    //MDIO1_MCLK
	VIN2A_D11, (IEN | PDIS | M15),    //MDIO1_D
	VIN2A_D12, (M3),    //RGMII1_TXC
	VIN2A_D13, (M3),    //RGMII1_TXCTL
	VIN2A_D14, (M3),    //RGMII1_TXD3
	VIN2A_D15, (M3),    //RGMII1_TXD2
	VIN2A_D16, (M3),    //RGMII1_TXD1
	VIN2A_D17, (M3),    //RGMII1_TXD0
	VIN2A_D18, (IEN | M3),    //RGMII1_RXC
	VIN2A_D19, (IEN | M3),    //RGMII1_RXCTL
	VIN2A_D20, (IEN | M3),    //RGMII1_RXD3
	VIN2A_D21, (IEN | M3),    //RGMII1_RXD2
	VIN2A_D22, (IEN | M3),    //RGMII1_RXD1
	VIN2A_D23, (IEN | M3),    //RGMII1_RXD0

	/* U52-2 */
	VOUT1_CLK, (IEN | PDIS | M0),    //VOUT1_CLK
	VOUT1_DE, (IEN | PDIS | M0),    //VOUT1_DE
	VOUT1_FLD, (IEN | PDIS | M15),    //VOUT1_FLD
	VOUT1_HSYNC, (IEN | PDIS | M0),    //VOUT1_HSYNC
	VOUT1_VSYNC, (IEN | PDIS | M0),    //VOUT1_VSYNC
	VOUT1_D0, (IEN | PDIS | M0),    //VOUT1_D0
	VOUT1_D1, (IEN | PDIS | M0),    //VOUT1_D1
	VOUT1_D2, (IEN | PDIS | M0),    //VOUT1_D2
	VOUT1_D3, (IEN | PDIS | M0),    //VOUT1_D3
	VOUT1_D4, (IEN | PDIS | M0),    //VOUT1_D4
	VOUT1_D5, (IEN | PDIS | M0),    //VOUT1_D5
	VOUT1_D6, (IEN | PDIS | M0),    //VOUT1_D6
	VOUT1_D7, (IEN | PDIS | M0),    //VOUT1_D7
	VOUT1_D8, (IEN | PDIS | M0),    //VOUT1_D8
	VOUT1_D9, (IEN | PDIS | M0),    //VOUT1_D9
	VOUT1_D10, (IEN | PDIS | M0),    //VOUT1_D10
	VOUT1_D11, (IEN | PDIS | M0),    //VOUT1_D11
	VOUT1_D12, (IEN | PDIS | M0),    //VOUT1_D12
	VOUT1_D13, (IEN | PDIS | M0),    //VOUT1_D13
	VOUT1_D14, (IEN | PDIS | M0),    //VOUT1_D14
	VOUT1_D15, (IEN | PDIS | M0),    //VOUT1_D15
	VOUT1_D16, (IEN | PDIS | M0),    //VOUT1_D16
	VOUT1_D17, (IEN | PDIS | M0),    //VOUT1_D17
	VOUT1_D18, (IEN | PDIS | M0),    //VOUT1_D18
	VOUT1_D19, (IEN | PDIS | M0),    //VOUT1_D19
	VOUT1_D20, (IEN | PDIS | M0),    //VOUT1_D20
	VOUT1_D21, (IEN | PDIS | M0),    //VOUT1_D21
	VOUT1_D22, (IEN | PDIS | M0),    //VOUT1_D22
	VOUT1_D23, (IEN | PDIS | M0),    //VOUT1_D23

	/* U52-3 */
	RGMII0_RXC, (IEN | M0),    //RGMII0_RXC
	RGMII0_RXCTL, (IEN | M0),    //RGMII0_RXCTL
	RGMII0_RXD3, (IEN | M0),    //RGMII0_RXD0
	RGMII0_RXD2, (IEN | M0),    //RGMII0_RXD1
	RGMII0_RXD1, (IEN | M0),    //RGMII0_RXD2
	RGMII0_RXD0, (IEN | M0),    //RGMII0_RXD3
	RGMII0_TXC, (M0),    //RGMII0_TXC
	RGMII0_TXCTL, (M0),    //RGMII0_TXCTL
	RGMII0_TXD3, (M0),    //RGMII0_TXC3
	RGMII0_TXD2, (M0),    //RGMII0_TXC2
	RGMII0_TXD1, (M0),    //RGMII0_TXC1
	RGMII0_TXD0, (M0),    //RGMII0_TXC0
	MDIO_MCLK, (PTU | PEN | M0),    //MDIO0_MCLK
	MDIO_D, (IEN | PTU | PEN | M0),    //MDIO0_D
	RMII_MHZ_50_CLK, (IEN | PDIS | M15),    //RMII_MHZ_50_CLK

	/* U52-12 */
	USB1_DRVVBUS, (FSC | IEN | PDIS | M0),    //USB1_DRVVBUS
	USB2_DRVVBUS, (FSC | IEN | PDIS | M0),    //USB2_DRVVBUS
	//USB_RXP0,    //USB3.0
	//USB_RXN0,    //USB3.0
	//USB_TXP0,    //USB3.0
	//USB_TXN0,    //USB3.0
	//USB1_DP,    //USB3.0
	//USB1_DM,    //USB3.0
	//USB2_DP,    //USB3.0
	//USB2_DM,    //USB3.0

	/* U52-7 */
	XREF_CLK0, (IEN | PDIS | M15),    //NC
	XREF_CLK1, (IEN | PDIS | M15),    //NC
	XREF_CLK2, (IEN | PDIS | M6),    //VOUT2_CLK
	XREF_CLK3, (IEN | PDIS | M6),    //VOUT2_DE
	MCASP1_ACLKX, (IEN | PDIS | M0),    //I2S1_SCLK
	MCASP1_FSX, (FSC | IEN | PDIS | M0),   //I2S1_FSYNC
	MCASP1_ACLKR, (IEN | PDIS | M6),    //VOUT2_D0
	MCASP1_FSR, (IEN | PDIS | M6),    //VOUT2_D1
	MCASP1_AXR0, (FSC | IEN | PDIS | M0),    //I2S1_D0
	MCASP1_AXR1, (FSC | IEN | PDIS | M0),    //I2S1_D1
	MCASP1_AXR2, (IEN | PTD | PEN | M14),    //GPIO5_4 -> ipod_reset
	MCASP1_AXR3, (IEN | PDIS | M6),    //VOUT2_D3
	MCASP1_AXR4, (IEN | PDIS | M6),    //VOUT2_D4
	MCASP1_AXR5, (IEN | PDIS | M6),    //VOUT2_D5
	MCASP1_AXR6, (IEN | PDIS | M6),    //VOUT2_D6
	MCASP1_AXR7, (IEN | PDIS | M6),    //VOUT2_D7
	MCASP1_AXR8, (FSC | IEN | PDIS | M1),    //I2S6_D0
	MCASP1_AXR9, (FSC | IEN | PDIS | M1),    //I2S6_D1
	MCASP1_AXR10, (FSC | IEN | PDIS | M1),    //I2S6_SCLK
	MCASP1_AXR11, (FSC | IEN | PDIS | M1),    //I2S6_FSYNC
	MCASP1_AXR12, (FSC | IEN | PTU | PEN | M1),    //BT(PCM_DOUT, input): McASP7_AXR0
	MCASP1_AXR13, (FSC | IEN | PTD | PEN | M1),    //BT(PCM_PIN, output): McASP7_AXR1 */
	MCASP1_AXR14, (FSC | IEN | PTU | PEN | M1),    //BT: McASP7_CLKX, input in slave mode
	MCASP1_AXR15, (FSC | IEN | PTU | PEN | M1),    //BT: McASP7_FSX, input in slave mode
	MCASP2_ACLKX, (IEN | PDIS | M15),    //NC
	MCASP2_FSX, (FSC | IEN | PDIS | M15),    //NC
	MCASP2_ACLKR, (IEN | PDIS | M6),    //VOUT2_D8
	MCASP2_FSR, (IEN | PDIS | M6),    //VOUT2_D9
	MCASP2_AXR0, (IEN | PDIS | M6),    //VOUT2_D10
	MCASP2_AXR1, (IEN | PDIS | M6),    //VOUT2_D11
	MCASP2_AXR2, (IEN | PTD | PEN | M14),    //GPIO6_8
	MCASP2_AXR3, (IEN | PTD | PEN | M14),    //GPIO6_9
	MCASP2_AXR4, (IEN | PDIS | M6),    //VOUT2_D12
	MCASP2_AXR5, (IEN | PDIS | M6),    //VOUT2_D13
	MCASP2_AXR6, (IEN | PDIS | M6),    //VOUT2_D14
	MCASP2_AXR7, (IEN | PDIS | M6),    //VOUT2_D15
	MCASP3_ACLKX, (IEN | PDIS | M0),    //I2S3_SCLK
	MCASP3_FSX, (FSC | IEN | PDIS | M0),    //I2S3_FSYNC
	MCASP3_AXR0, (FSC | IEN | PDIS | M0),    //I2S3_D0
	MCASP3_AXR1, (FSC | IEN | PDIS | M0),    //I2S3_D1
	MCASP4_ACLKX, (IEN | PTU | PDIS | M4),    //I2C4_SDA
	MCASP4_FSX, (IEN | PTU | PDIS | M4),    //I2C4_SCL
	MCASP4_AXR0, (IEN | PDIS | M6),    //VOUT2_D18
	MCASP4_AXR1, (IEN | PDIS | M6),    //VOUT2_D19
	MCASP5_ACLKX, (IEN | PTU | PDIS | M4),    //I2C5_SDA
	MCASP5_FSX, (IEN | PTU | PDIS | M4),    //I2C5_SCL
	MCASP5_AXR0, (IEN | PDIS | M6),    //VOUT2_D22
	MCASP5_AXR1, (IEN | PDIS | M6),    //VOUT2_D23

	/* U52-13 */
	WAKEUP0, (PEN | M0),    //WAKEUP0
	WAKEUP1, (PEN | M0),    //WAKEUP1
	WAKEUP2, (PEN | M0),    //WAKEUP2
	WAKEUP3, (PEN | M0),    //WAKEUP3
	ON_OFF, (PTU | PDIS | M0),    //ON_OFF
	RTC_PORZ, (PEN | M0),	    //PORZ
	RESETN, (PTU | PDIS | M0),    //RESETN
	NMIN, (PDIS | M0),    //NMIN
	RSTOUTN, (PDIS | M0),    //RSTOUTN

	/* U52-16 */
	TMS, (IEN | PTU | PDIS | M0),    //JTAG_TMS
	TDI, (FSC | IEN | PTU | PDIS | M0),    //JTAG_TDI
	TDO, (IEN | PTU | PDIS | M0),    //JTAG_TDO
	TCLK, (IEN | PTU | PDIS | M0),    //JTAG_TCLK
	TRSTN, (IEN | PDIS | M0),    //JTAG_TRSTN
	RTCK, (IEN | PTD | PEN | M0),    //JTAG_RTCK
	EMU0, (IEN | PTU | PDIS | M0),    //JTAG_EMU0
	EMU1, (IEN | PTU | PDIS | M0),    //JTAG_EMU1
};

static void init_mux_all(void)
{
	int i, size;

	size = sizeof(pin_data_all) / sizeof(unsigned);
	for (i = 0; i < size; i += 2)
		out32(DRA74X_CONTROL_PADCONF_CORE_BASE + pin_data_all[i], pin_data_all[i + 1]);
}

#if 0
/***************************************************************************************************
 * Ethernet
 ***************************************************************************************************/
/* Pin Mux Registers and the Corresponding Values */
static const uint32_t pin_data_ether[] = {
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
};

/* This code is required to setup the second ethernet interface */
static void init_ethernet()
{
	int		i, size;

	// configure PINMUX
	size = sizeof(pin_data_ether) / sizeof(uint32_t);
	for (i = 0; i < size; i += 2)
		out32(DRA74X_CONTROL_PADCONF_CORE_BASE + pin_data_ether[i], pin_data_ether[i + 1]);
}

/***************************************************************************************************
 * LCD configuration
 * 1 Old code enable "all call", SD does not	isend -n /dev/i2c0 -a 0x40 0x00 0x00
 * 2 STBYB=1, LCD_AVDD_EN=1, LCD_ENBKL=0		isend -n /dev/i2c0 -a 0x40 0x0c 0x21
 * 3 SHLR=1, MODE3=1, DITH=0, UPDN=0			isend -n /dev/i2c0 -a 0x40 0x0d 0x05
 * 4 Brightness = 75% (192/256)					isend -n /dev/i2c0 -a 0x40 0x04 0xC0
 * 5 Config second LED expander					isend -n /dev/i2c0 -a 0x41 0x00 0x00
 * 6 Default test points to "high"				isend -n /dev/i2c0 -a 0x41 0x0c 0x00
 * 7 Default test points to "high", drive captouch reset "low"	isend -n /dev/i2c0 -a 0x41 0x0d 0x01
 * 8 Default test points to "high", drive captouch reset "high" isend -n /dev/i2c0 -a 0x41 0x0d 0x00
 ***************************************************************************************************/
 static int configure_tlc59108(omap_i2c_dev_t *i2c_dev, int slave)
 {
	int idx;
	uint8_t buf = 0, count = 0;
	uint8_t i2c_data[][2] = {{0x00, 0x00},
							{0x0c, 0x21},
							{0x0d, 0x05},
							{0x04, 0xc0}};

	i2c_dev->slave = slave;

	/*
	 * Polling TLC59108 until it's ready
	 * Disable kprintf() from i2c driver while polling
	 */
	omap_i2c_set_debug(0);
	for (;;) {
		if (!omap_i2c_read(i2c_dev, SUBADR1_ADDR, 1, &buf, 1) && (buf == SUBADR1_DEFAULT)) {
			break;
		}

		if ( count++ > 10) {
			kprintf("%s: TLC59108 is not accessible!\n", __func__);
			omap_i2c_set_debug(1);
			return -1;
		}
	}
	omap_i2c_set_debug(1);

	/* Now TLC59108 is accessible, configure it */
	for (idx = 0; idx < sizeof(i2c_data) / 2; idx++) {
		if (0 != omap_i2c_write(i2c_dev, &i2c_data[idx][0], 2)) {
			kprintf("%s: Failed to init TLC59108. I2C slave_addr = 0%x, data[0] = %x, data[1] = %x\n",
					__func__, slave, i2c_data[idx][0], i2c_data[idx][1]);
			return -1;
		}
	}

	return 0;
}

static void init_lcd()
{
	uint8_t buf[2];

	omap_i2c_dev_t i2c_dev = {
		.base = DRA74X_I2C1_BASE,
		.clock = I2C_SPEED,
		.slave = IO_EXPANDER_1_I2C_ADDR
	};

	omap_i2c_init(&i2c_dev);

	memset(buf, NULL, sizeof (buf));
	if (0 != omap_i2c_read(&i2c_dev, 0, 1, buf, 2)) {
		kprintf("failed to read from PCF8575\n");
		omap_i2c_deinit(&i2c_dev);
		return;
	}

	/* Clear P15 (CON_LCD_PWR_DN) to power up LCD */
	buf[1] &= ~PCF8575_P15;
	if (0 != omap_i2c_write(&i2c_dev, buf, 2)) {
		kprintf("failed to write to PCF8575\n");
		omap_i2c_deinit(&i2c_dev);
		return;
	}

	configure_tlc59108(&i2c_dev, TLC59108IPW_1_I2C_ADDR);

	/*
	 * There are 2 TLC59108 devices on the Spectrum 7" display which is used for Rev E or earlier boards,
	 * while there is only 1 TLC59108 on the 10" display.
	 * Technically Rev F (or newer hardware) can mount the 7" display, but this is the case in reality
	 */
	if (pmic_revision == TPS659038_ES1_0) {
		configure_tlc59108(&i2c_dev, TLC59108IPW_2_I2C_ADDR);
	}

	omap_i2c_deinit(&i2c_dev);
}

/***************************************************************************************************
 * Touch screen
 ***************************************************************************************************/
static void init_ts()
{
#define MXT224_IRQ	(1 << 2)	/* Touch screen interrupt GPIO1[2]*/
	uintptr_t base = DRA74X_GPIO1_BASE;

	/* Falling edge trigger */
	out32(base + GPIO_OE, in32(base + GPIO_OE) | MXT224_IRQ);
	out32(base + GPIO_LEVELDETECT0, in32(base + GPIO_LEVELDETECT0) & ~MXT224_IRQ);
	out32(base + GPIO_LEVELDETECT1, in32(base + GPIO_LEVELDETECT1) & ~MXT224_IRQ);
	out32(base + GPIO_RISINGDETECT, in32(base + GPIO_RISINGDETECT) & ~MXT224_IRQ);
	out32(base + GPIO_FALLINGDETECT, in32(base + GPIO_FALLINGDETECT) | MXT224_IRQ);
}

/***************************************************************************************************
 * Wifi
 ***************************************************************************************************/
/* Pin Mux Registers and the Corresponding Values */
static unsigned pin_data_dp[] = {
	UART1_CTSN, (IEN | PTU | PDIS | M3),
	UART1_RTSN, (IEN | PTU | PDIS | M3),
	UART2_RXD, (IEN | PTU | PDIS | M3),
	UART2_TXD, (IEN | PTU | PDIS | M3),
	UART2_CTSN, (IEN | PTU | PDIS | M3),
	UART2_RTSN, (IEN | PTU | PDIS | M3),
	MCASP1_AXR5, (IEN | PTU | WKEN | M14),
	MCASP1_AXR6, (PTU | M14),
};

/* Initialize the Pin Muxing */
static void init_mux(void)
{
	int i, size;

	size = sizeof(pin_data_dp) / sizeof(unsigned);
	for (i = 0; i < size; i += 2)
		out32(DRA74X_CONTROL_PADCONF_CORE_BASE + pin_data_dp[i], pin_data_dp[i + 1]);
}

/* Init WiFi interface (COMQ) */
static void init_wifi (void)
{
	uintptr_t prcm_base = DRA74X_PRCM_BASE;

	//init_mux();

	/* Set 48Mhz clock */
	out32(prcm_base + CM_L4PER_MMC4_CLKCTRL, (in32(prcm_base + CM_L4PER_MMC4_CLKCTRL) & ~0x01000000));

	/* Configure WiFi chip reset signal as output GPIO */
	sr32(DRA74X_GPIO5_BASE + GPIO_OE, 8, 1, 0);

	/* Reset WiFi chip */
	sr32(DRA74X_GPIO5_BASE + GPIO_DATAOUT, 8, 1, 0);
	usec_delay(WIFI_RESET_DELAY);
	sr32(DRA74X_GPIO5_BASE + GPIO_DATAOUT, 8, 1, 1);
}
#endif

/*
This function is used to initialise FPDLink-III serialiser chip
*/
static void init_ub925q()
{
	uint8_t buf[2];

	kprintf ("init_ub925q start\n");

	omap_i2c_dev_t i2c_dev = {
		//.base = DRA74X_I2C2_BASE,
		.base = DRA74X_I2C1_BASE,
		.clock = I2C_SPEED,
		.slave = 0x1B
	};

	omap_i2c_init(&i2c_dev);

	/* Set ub925q to 18 bit mode */
	buf[0] = 18;
	buf[1] = 0x04;
	if (0 != omap_i2c_write(&i2c_dev, buf, 2)) {
		kprintf("failed to write to ub925q\n");
	}

	buf[0] = 4;
	buf[1] = 0x8A;
	if (0 != omap_i2c_write(&i2c_dev, buf, 2)) {
		kprintf("failed to write to ub925q\n");
	}

	omap_i2c_deinit(&i2c_dev);

	kprintf ("init_ub925q end\n");
}

/***************************************************************************************************
 * MMCSD
 ***************************************************************************************************/
#define J6EVM_GPIO_SDCD (1 << 27)

void init_mmcsd(void)
{
	uintptr_t	base = DRA74X_PRCM_BASE;

	/* indicate stable voltages */
	out32(DRA74X_CTRL_CORE_CONTROL_PBIAS, in32(DRA74X_CTRL_CORE_CONTROL_PBIAS) |
					CTRL_CORE_CONTROL_PBIAS_SDCARD_BIAS_3V |
					CTRL_CORE_CONTROL_PBIAS_SDCARD_IO_PWRDNZ |
					CTRL_CORE_CONTROL_PBIAS_SDCARD_BIAS_PWRDNZ);

	/* PAD for eMMC, SD card, already configured by boot loader */
	base = DRA74X_GPIO6_BASE;
	out32(base + GPIO_OE, in32(base + GPIO_OE) | J6EVM_GPIO_SDCD);
	out32(base + GPIO_LEVELDETECT0, in32(base + GPIO_LEVELDETECT0) & ~J6EVM_GPIO_SDCD);
	out32(base + GPIO_LEVELDETECT1, in32(base + GPIO_LEVELDETECT1) & ~J6EVM_GPIO_SDCD);
	out32(base + GPIO_RISINGDETECT, in32(base + GPIO_RISINGDETECT) | J6EVM_GPIO_SDCD);
	out32(base + GPIO_FALLINGDETECT, in32(base + GPIO_FALLINGDETECT) | J6EVM_GPIO_SDCD);
	out32(base + GPIO_DEBOUNCEENABLE, in32(base + GPIO_DEBOUNCEENABLE) | J6EVM_GPIO_SDCD);
	out32(base + GPIO_DEBOUNCINGTIME, 31);
}
/***************************************************************************************************
 * Audio
 ***************************************************************************************************/
#if 0
static void init_audio()
{
	if (abe_dpll_196m) {
		/*
		 * In this path, McASP3 can work at 48Khz sample rate, also McASP7 can work in clock master mode
		 * No proper clock to satisfy the 44.1Khz sample rate required by radio
		 * AUX_CLK of all McASP instances comes from PER_ABE_X1_GFCLK (DPLL_ABE_M2) 196.608MHz / 16 = 12.288Mhz
		 * AHCLKX is derived from AUX_CLK, so that ahxclk should be set to output direction
		 */
		 return;
	}

	/*
	 * IPL configures ABE DPLL at 361Mhz, which ultimately feeds all McASP devices (exclude McASP7) and radio
	 * In order to have McASP7 and radio co-existing without stomping on each other's clock, McASP3 gets its AHCLKX clock from ATL2
	 * Please note that in this configuration, the sample rate of both radio and McASP3 should be 44.1Khz
	 */

	/* ABE M2 CLK (PER_ABE_X1_CLK) is MCLK: 180.6336MHz */
	sr32(DRA74X_PRCM_BASE + CM_DIV_M2_DPLL_ABE, 0, 5, 0x1);

	/* Configure ATL clocks, select PER_ABE_X1_CLK as CLKSEL_SOURCE2 */
	init_atl_clock(-1, 1);

	/*
	 * There is a single ATLPCLK mux for all ATL instances and
	 * is controlled by PCLKMUX0. The rest of PCLKMUXs don't have
	 * any effect of clock selection
	 */
	out32(ATL_BASE + ATL_PCLKMUX(ATL_INST(0)), ATL_PCLKMUX_ATLPCLK);

	/* Disable ATL while reparenting and changing ATLPCLK input */
	out32(ATL_BASE + ATL_SWEN(ATL_INST(2)), ATL_SWEN_DISABLE);

	/* ATL divider (ATL_INTERNAL_DIVIDER): ATLPCLK-to-ATCLK ratio - 1 */
	/* 180.6336MHz / 16 = 11.2896MHz */
	out32(ATL_BASE + ATL_ATLCR(ATL_INST(2)), 0xF);

	/* Baseband word select - McASP2 FSX - don't care currently */
	out32(ATL_BASE + ATL_BWSMUX(ATL_INST(2)), 0x3);

	/* Audio word select - McASP3 FSX */
	out32(ATL_BASE + ATL_AWSMUX(ATL_INST(2)), 0x4);

	/* PPMR to 0 */
	out32(ATL_BASE + ATL_PPMR(ATL_INST(2)), 0x0);

	/* Enable ATL */
	out32(ATL_BASE + ATL_SWEN(ATL_INST(2)), ATL_SWEN_ENABLE);

	/* McASP3 high clock from ATL_CLK2*/
	sr32(DRA74X_PRCM_BASE + CM_L4PER2_MCASP3_CLKCTRL, CM_MCASP_CLKSEL_AHCLKX_OFF, 4, CM_MCASP_CLKSEL_AHCLKX_ATL2);
}
#endif

static void init_audio_mcasp1()
{
	if (abe_dpll_196m) {
		/*
		 * In this path, McASP3 can work at 48Khz sample rate, also McASP7 can work in clock master mode
		 * No proper clock to satisfy the 44.1Khz sample rate required by radio
		 * AUX_CLK of all McASP instances comes from PER_ABE_X1_GFCLK (DPLL_ABE_M2) 196.608MHz / 16 = 12.288Mhz
		 * AHCLKX is derived from AUX_CLK, so that ahxclk should be set to output direction
		 */
		 return;
	}

	/*
	 * IPL configures ABE DPLL at 361Mhz, which ultimately feeds all McASP devices (exclude McASP7) and radio
	 * In order to have McASP7 and radio co-existing without stomping on each other’s clock, McASP3 gets its AHCLKX clock from ATL2
	 * Please note that in this configuration, the sample rate of both radio and McASP3 should be 44.1Khz
	 */

	/* ABE M2 CLK (PER_ABE_X1_CLK) is MCLK: 180.6336MHz */
	sr32(DRA74X_PRCM_BASE + CM_DIV_M2_DPLL_ABE, 0, 5, 0x1);

	/* Configure ATL clocks, select PER_ABE_X1_CLK as CLKSEL_SOURCE2 */
	init_atl_clock(-1, 1);

	/*
	 * There is a single ATLPCLK mux for all ATL instances and
	 * is controlled by PCLKMUX0. The rest of PCLKMUXs don't have
	 * any effect of clock selection
	 */
	out32(ATL_BASE + ATL_PCLKMUX(ATL_INST(0)), ATL_PCLKMUX_ATLPCLK);

	/* Disable ATL while reparenting and changing ATLPCLK input */
	out32(ATL_BASE + ATL_SWEN(ATL_INST(0)), ATL_SWEN_DISABLE);

	/* ATL divider (ATL_INTERNAL_DIVIDER): ATLPCLK-to-ATCLK ratio - 1 */
	/* 180.6336MHz / 16 = 11.2896MHz */
	out32(ATL_BASE + ATL_ATLCR(ATL_INST(0)), 0xF);

	/* Baseband word select - McASP1 FSX - don't care currently */
	out32(ATL_BASE + ATL_BWSMUX(ATL_INST(0)), 0x1); //McASP1_FSX
	
	/* Audio word select - McASP1 FSX */
	out32(ATL_BASE + ATL_AWSMUX(ATL_INST(0)), 0x1);  //McASP1_FSX
	
	/* PPMR to 0 */
	out32(ATL_BASE + ATL_PPMR(ATL_INST(0)), 0x0);

	/* Enable ATL */
	out32(ATL_BASE + ATL_SWEN(ATL_INST(0)), ATL_SWEN_ENABLE);

	/* McASP1 high clock from ATL_CLK0*/
	sr32(DRA74X_PRCM_BASE + CM_IPU_MCASP1_CLKCTRL, CM_MCASP_CLKSEL_AHCLKX_OFF, 4, CM_MCASP_CLKSEL_AHCLKX_ATL0);
}


static void init_usb_extra()
{
	/*
	 * Drive USBx_DRVVBUS high to keep vbus high
	 * this is needed by iap2 driver
	 * Another alternative would be change the USB host driver to dynamically drive vbus level
	 */

	/* USB1_DRVVBUS */
	sr32(DRA74X_GPIO6_BASE + GPIO_OE, 12, 1, 0);
	sr32(DRA74X_GPIO6_BASE + GPIO_DATAOUT, 12, 1, 1);

	/* USB2_DRVVBUS */
	sr32(DRA74X_GPIO6_BASE + GPIO_OE, 13, 1, 0);
	sr32(DRA74X_GPIO6_BASE + GPIO_DATAOUT, 13, 1, 1);
}

void set_io_delay()
{
	const dra7xx_io_delay_t io_dly[] = {
		{CFG_RGMII0_TXCTL, RGMII0_TXCTL_DLY_VAL},
		{CFG_RGMII0_TXD0, RGMII0_TXD0_DLY_VAL},
		{CFG_RGMII0_TXD1, RGMII0_TXD1_DLY_VAL},
		{CFG_RGMII0_TXD2, RGMII0_TXD2_DLY_VAL},
		{CFG_RGMII0_TXD3, RGMII0_TXD3_DLY_VAL},
		{CFG_VIN2A_D13, VIN2A_D13_DLY_VAL},
		{CFG_VIN2A_D17, VIN2A_D17_DLY_VAL},
		{CFG_VIN2A_D16, VIN2A_D16_DLY_VAL},
		{CFG_VIN2A_D15, VIN2A_D15_DLY_VAL},
		{CFG_VIN2A_D14, VIN2A_D14_DLY_VAL},
		{0}
	};

	/* Adjust IO delay for RGMII tx path */
	dra7xx_adj_io_delay (io_dly);
}

uint32_t get_sys_clk2_freq()
{
	return SYS_CLK2_FREQ;
}

static void init_89501()
{
	kprintf ("init_89501 start\n");

	//VCC_3V3_89501
	sr32(DRA74X_GPIO4_BASE + GPIO_OE, 4, 1, 0);
	sr32(DRA74X_GPIO4_BASE + GPIO_DATAOUT, 4, 1, 1);

	usec_delay(10000);

	//RESET_89501
	sr32(DRA74X_GPIO4_BASE + GPIO_OE, 3, 1, 0);
	sr32(DRA74X_GPIO4_BASE + GPIO_DATAOUT, 3, 1, 0);
	usec_delay(1000);
	sr32(DRA74X_GPIO4_BASE + GPIO_DATAOUT, 3, 1, 1);

	usec_delay(10000);

	//SOC_EN_89501
	sr32(DRA74X_GPIO6_BASE + GPIO_OE, 8, 1, 0);
	sr32(DRA74X_GPIO6_BASE + GPIO_DATAOUT, 8, 1, 1);
	
	kprintf ("init_89501 end\n");
}

static void init_wilink8 (void)
{
#define WLAN_IRQ	(1 << 16)	/* Wlan interrupt GPIO3[16]*/
	uintptr_t gpio_base = DRA74X_GPIO3_BASE;
	uintptr_t prcm_base = DRA74X_PRCM_BASE;

	uint8_t buf[2];

	kprintf ("init_wilink8 start\n");

	//##### PMIC_32K_CLKOUT #####
	/* PMIC configuration */
	omap_i2c_dev_t i2c_dev = {
		.base = DRA74X_I2C1_BASE,
		.clock = I2C_SPEED,
		.slave = TPS659038_I2C_SLAVE_ADDR
	};
	omap_i2c_init(&i2c_dev);

	/* select CLK32KGO as GPIO_5 output */
	buf[0] = PMIC_PAD2A;
	buf[1] = 0x2b;
	if (omap_i2c_write (&i2c_dev, buf, 2) != 0) {
		kprintf ("PMIC write PAD2 failed\n");
	}

	/* enable CLK32K */
	buf[0] = PMIC_CLK32AUDIO_CTRLA;
	buf[1] = 0x01;
	if (omap_i2c_write (&i2c_dev, buf, 2) != 0) {
		kprintf ("PMIC write CTRLA failed\n");
	}
	omap_i2c_deinit(&i2c_dev);

	//##### Wifi 48Mhz clock #####
	out32(prcm_base + CM_L4PER_MMC4_CLKCTRL, (in32(prcm_base + CM_L4PER_MMC4_CLKCTRL) & ~0x01000000));

	//##### WL_EN_3V3 #####
	sr32(gpio_base + GPIO_OE, 18, 1, 0);
	sr32(gpio_base + GPIO_DATAOUT, 18, 1, 0);
	usec_delay(1000);
	sr32(gpio_base + GPIO_DATAOUT, 18, 1, 1);

	//##### WL_IRQ_3V3 #####
	out32(gpio_base + GPIO_OE, in32(gpio_base + GPIO_OE) | WLAN_IRQ);
	out32(gpio_base + GPIO_LEVELDETECT0, in32(gpio_base + GPIO_LEVELDETECT0) & ~WLAN_IRQ);
	out32(gpio_base + GPIO_LEVELDETECT1, in32(gpio_base + GPIO_LEVELDETECT1) & ~WLAN_IRQ);
	out32(gpio_base + GPIO_RISINGDETECT, in32(gpio_base + GPIO_RISINGDETECT) & ~WLAN_IRQ);
	out32(gpio_base + GPIO_FALLINGDETECT, in32(gpio_base + GPIO_FALLINGDETECT) | WLAN_IRQ);

	//##### BT_EN_3V3 #####
	sr32(gpio_base + GPIO_OE, 13, 1, 0);
	sr32(gpio_base + GPIO_DATAOUT, 13, 1, 0);
	usec_delay(1000);
	sr32(gpio_base + GPIO_DATAOUT, 13, 1, 1);

	kprintf ("init_wilink8 end\n");
}

static void init_6638()
{
	kprintf ("init_6638 start\n");

	//AMP_STB_CPU
	sr32(DRA74X_GPIO3_BASE + GPIO_OE, 19, 1, 0);
	sr32(DRA74X_GPIO3_BASE + GPIO_DATAOUT, 19, 1, 1);

	usec_delay(10000);

	//RESET_TUNER
	sr32(DRA74X_GPIO3_BASE + GPIO_OE, 25, 1, 0);
	sr32(DRA74X_GPIO3_BASE + GPIO_DATAOUT, 25, 1, 0);
	usec_delay(1000);
	sr32(DRA74X_GPIO3_BASE + GPIO_DATAOUT, 25, 1, 1);
	
	kprintf ("init_6638 end\n");
}

static void init_xf6000()
{
	kprintf ("init_xf6000 start\n");

	//XF_3V3_EN
	sr32(DRA74X_GPIO3_BASE + GPIO_OE, 24, 1, 0);
	sr32(DRA74X_GPIO3_BASE + GPIO_DATAOUT, 24, 1, 1);

	usec_delay(10000);

	//XF_RESET
	sr32(DRA74X_GPIO3_BASE + GPIO_OE, 23, 1, 0);
	sr32(DRA74X_GPIO3_BASE + GPIO_DATAOUT, 23, 1, 0);
	usec_delay(1000);
	sr32(DRA74X_GPIO3_BASE + GPIO_DATAOUT, 23, 1, 1);
	
	kprintf ("init_xf6000 end\n");
}

static void init_tp_10g07()
{
	kprintf ("init_tp_10g07 start\n");
	
	//RESET_TP
	sr32(DRA74X_GPIO2_BASE + GPIO_OE, 30, 1, 0);
	sr32(DRA74X_GPIO2_BASE + GPIO_DATAOUT, 30, 1, 0);
	usec_delay(1000);
	sr32(DRA74X_GPIO2_BASE + GPIO_DATAOUT, 30, 1, 1);

	//TP_IRP
#define TP_IRQ	(1 << 8)	/* Touch screen interrupt GPIO3[8]*/
	uintptr_t base = DRA74X_GPIO3_BASE;

	/* Falling edge trigger */
	out32(base + GPIO_OE, in32(base + GPIO_OE) | TP_IRQ);
	out32(base + GPIO_LEVELDETECT0, in32(base + GPIO_LEVELDETECT0) & ~TP_IRQ);
	out32(base + GPIO_LEVELDETECT1, in32(base + GPIO_LEVELDETECT1) & ~TP_IRQ);
	out32(base + GPIO_RISINGDETECT, in32(base + GPIO_RISINGDETECT) & ~TP_IRQ);
	out32(base + GPIO_FALLINGDETECT, in32(base + GPIO_FALLINGDETECT) |TP_IRQ);
	
	kprintf ("init_tp_10g07 end\n");
}

static void init_adv7180()
{
	kprintf ("init_adv7180 start\n");

	//7180_PWR_EN
	sr32(DRA74X_GPIO3_BASE + GPIO_OE, 27, 1, 0);
	sr32(DRA74X_GPIO3_BASE + GPIO_DATAOUT, 27, 1, 1);

	usec_delay(10000);

	//ADV7180_RESET
	sr32(DRA74X_GPIO3_BASE + GPIO_OE, 22, 1, 0);
	sr32(DRA74X_GPIO3_BASE + GPIO_DATAOUT, 22, 1, 0);
	usec_delay(1000);
	sr32(DRA74X_GPIO3_BASE + GPIO_DATAOUT, 22, 1, 1);
	
	kprintf ("init_adv7180 end\n");
}

static void init_ipod_auth()
{
	kprintf ("init_ipod_auth start\n");

	//IPOD_RESET
	sr32(DRA74X_GPIO5_BASE + GPIO_OE, 4, 1, 0);
	sr32(DRA74X_GPIO5_BASE + GPIO_DATAOUT, 4, 1, 0);
	usec_delay(1000);
	sr32(DRA74X_GPIO5_BASE + GPIO_DATAOUT, 4, 1, 1);
	
	kprintf ("init_ipod_auth end\n");
}


/*
 This function is used to initialized any HW components that are required to
 * bring the kernel up and running. In normal mode or when using
 * power-management only the strict minimum should be initialize and other
 * components should be hold in their lowest power mode.
 */
void hw_init()
{

	init_mux_all();//all mux table for HG kernal board

	if (abe_dpll_196m) {
		setup_abe_dpll(FUNC_32K_CLK_INPUT);
	}

	/* SYS_CLK_32K_CLK as source */
	init_gptimer_clock(1, 1 << CLKCTRL_CLKSEL_OFFSET);

	/*
	 * We don't enable all timers to save power
	 * IPU clock is disabled, so TIMER5 - TIMER8 can not be enabled
	 */
	init_gptimer_clock(2, 0);
	init_gptimer_clock(3, 0);
	init_gptimer_clock(4, 0);

	init_mmcsd();
	init_edma_clock();
	init_edma();
	init_usb1_otg();
	init_usb2_otg();
	init_usb3_phy();
	init_usb_extra();

	/* Need to put init_ocp2scp3() before init_sata() and init_pci() */
	//init_ocp2scp3();
	//init_sata();
	set_crossbar_reg();
	read_pmic_revision();
	//init_lcd();
	//init_gpmc();
	//init_com8();
	//set_io_delay();
	//init_ethernet();
	//init_ts();
	
	init_audio_mcasp1();
	init_6638();
	init_xf6000();
	init_89501();
	init_wilink8();
	init_tp_10g07();

	init_ub925q();
	init_adv7180();
	init_ipod_auth();

#if 0
	if (gpmc_vout3) {
		init_ub925q();
	}

	if (jamr3) {
		init_jamr3();
	}

	if (wifi) {
		init_wifi ();
	}
#endif

}

/*
 * This function is used to initialize and pieces of HW startup could have a
 * dependency on. If startup do not needs a piece of HW to be initialized in
 * order to perform its normal task then the initialization code should be in
 * the hw_init function.
 */
void init_clocks(void)
{
	init_basic_clocks();

	/* Init IPU and timers needed by IPC */
	init_ipu();
	init_ipu1();
	init_ipu2();
	init_gptimer_clock(5, 0);
	init_gptimer_clock(6, 0);
	init_gptimer_clock(7, 0);
	init_gptimer_clock(8, 0);
	init_gptimer_clock(11, 0);

	/* Init timers needed by Radio */
	init_gptimer_clock(9, 0);
	init_gptimer_clock(10, 0);

	init_i2c_clock(1);
	init_i2c_clock(2);
	init_i2c_clock(3);
	init_i2c_clock(4);
	init_i2c_clock(5);

	/* UART1 is supposed to be enabled in first boot loader */
	init_uart_clock(3);	/* UART3 for Bluetooth */
	init_uart_clock(4);	/* UART4 for MCU */
	init_uart_clock(8);	/* UART8 for GPS */

	init_mcspi_clock(1);

	init_qspi_clock();

	/*
	 * MCASP clocks
	 * Only enable devices that are needed
	 * By default on all devices:
	 *		select ABE_24M_GFCLK as the CLKSEL_AHCLKX source
	 *		select PER_ABE_X1_GFCLK as the CLKSEL_AUX_CLK source
	 */
	init_mcasp_clock(1);	 
	init_mcasp_clock(3);
	init_mcasp_clock(6);
	init_mcasp_clock(7);

	/*
	 * MMC1: micso SD
	 * MMC2: eMMC
	 * MMC3: SD on the expander board
	 * MMC4: MMC interface for WiFi
	 * 192MHz clock source, divided by 1
	 */
	init_mmc_clock(1, (1 << CLKCTRL_CLKSEL_OFFSET));
	init_mmc_clock(2, (1 << CLKCTRL_CLKSEL_OFFSET));
	init_mmc_clock(3, (1 << CLKCTRL_CLKSEL_OFFSET));
	init_mmc_clock(4, (1 << CLKCTRL_CLKSEL_OFFSET));

	/* GPIO clocks with debouncing clock enabled (except GPIO1) */
	init_gpio_clock(1, 0);
	init_gpio_clock(2, CLKCTRL_OPTCLKEN_ENABLED);
	init_gpio_clock(5, CLKCTRL_OPTCLKEN_ENABLED);
	init_gpio_clock(6, CLKCTRL_OPTCLKEN_ENABLED);
	init_gpio_clock(3, CLKCTRL_OPTCLKEN_ENABLED);
	init_gpio_clock(4, CLKCTRL_OPTCLKEN_ENABLED);

	//init_gpmc_clock();

	/* Camera clocks */
	init_vin_clocks(VIP_DEVICE_1 | VIP_DEVICE_2 | VIP_DEVICE_3);

	/* Video Processing Engine (deinterlacing etc.) */
	init_vpe_clocks();

	init_graphics_clocks();

	/* We are expecting 532Mhz on M2 GPU_DPLL */
	if (calc_dpll_freq(DPLL_GPU) != 2128000000)
		setup_gpu_dpll_2128mhz();
	init_gpu_clock(CM_GPU_GPU_CLKCTRL_FCLK_GPU);

	init_dcan_clock(1);
	init_dcan_clock(2);

	init_gmac_clock();
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/dra74x/vayu-evm/hw_init.c $ $Rev: 773669 $")
#endif
