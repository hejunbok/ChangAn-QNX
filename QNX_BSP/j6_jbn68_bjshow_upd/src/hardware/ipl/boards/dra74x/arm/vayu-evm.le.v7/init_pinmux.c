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
	RMII_MHZ_50_CLK, (IEN | PDIS | M0),    //RMII_MHZ_50_CLK

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
	MCASP1_AXR2, (IEN | PDIS | M6),    //VOUT2_D2
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
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/ipl/boards/dra74x/arm/vayu-evm.le.v7/init_pinmux.c $ $Rev: 770720 $")
#endif
