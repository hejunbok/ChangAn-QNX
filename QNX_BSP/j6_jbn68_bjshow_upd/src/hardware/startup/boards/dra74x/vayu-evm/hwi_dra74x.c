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
#include <hwinfo_private.h>
#include <drvr/hwinfo.h>
#include "hwinfo_dra74x.h"
#include "board.h"

/*
 * Add DRA74X specific devices to the hardware info section of the syspage
*/

void hwi_dra74x()
{
	int i;

	/* add 4 UART's */
	{
		unsigned hwi_off;
		hwiattr_uart_t uart_attr = HWIATTR_UART_T_INITIALIZER;

		/* All the UARTs operate from a fixed functional clock of 48 MHz only */
		struct hwi_inputclk clksrc = {.clk = 48000000, .div = 16};

		/* each UART has an interrupt */
		HWIATTR_UART_SET_NUM_IRQ(&uart_attr, 1);
		HWIATTR_UART_SET_NUM_CLK(&uart_attr, 1);

		/* create uart1 and set the clock source */
		HWIATTR_UART_SET_LOCATION(&uart_attr, DRA74X_UART1_BASE, DRA74X_UART_REG_SIZE, 0, hwi_find_as(DRA74X_UART1_BASE, 1));
		hwi_off = hwidev_add_uart(DRA74x_HWI_UART, &uart_attr, HWI_NULL_OFF);
		ASSERT(hwi_find_unit(hwi_off) == 0);
		hwitag_set_inputclk(hwi_off, 0, &clksrc);

		/* create uart2 and set the clock source */
//		HWIATTR_UART_SET_LOCATION(&uart_attr, DRA74X_UART2_BASE, DRA74X_UART_REG_SIZE, 0, hwi_find_as(DRA74X_UART2_BASE, 1));
//		hwi_off = hwidev_add_uart(DRA74x_HWI_UART, &uart_attr, HWI_NULL_OFF);
//		ASSERT(hwi_find_unit(hwi_off) == 1);
//		hwitag_set_inputclk(hwi_off, 0, &clksrc);

		/* create uart3 and set the clock source */
		HWIATTR_UART_SET_LOCATION(&uart_attr, DRA74X_UART3_BASE, DRA74X_UART_REG_SIZE, 0, hwi_find_as(DRA74X_UART3_BASE, 1));
		hwi_off = hwidev_add_uart(DRA74x_HWI_UART, &uart_attr, HWI_NULL_OFF);
		ASSERT(hwi_find_unit(hwi_off) == 1);
		hwitag_set_inputclk(hwi_off, 0, &clksrc);

		/* create uart4 and set the clock source */
		HWIATTR_UART_SET_LOCATION(&uart_attr, DRA74X_UART4_BASE, DRA74X_UART_REG_SIZE, 0, hwi_find_as(DRA74X_UART4_BASE, 1));
		hwi_off = hwidev_add_uart(DRA74x_HWI_UART, &uart_attr, HWI_NULL_OFF);
		ASSERT(hwi_find_unit(hwi_off) == 2);
		hwitag_set_inputclk(hwi_off, 0, &clksrc);

		/* create uart8 and set the clock source */
		HWIATTR_UART_SET_LOCATION(&uart_attr, DRA74X_UART8_BASE, DRA74X_UART_REG_SIZE, 0, hwi_find_as(DRA74X_UART8_BASE, 1));
		hwi_off = hwidev_add_uart(DRA74x_HWI_UART, &uart_attr, HWI_NULL_OFF);
		ASSERT(hwi_find_unit(hwi_off) == 3);
		hwitag_set_inputclk(hwi_off, 0, &clksrc);

		/* Assign interrupt vectors to the 4 UART devices */
		hwitag_set_avail_ivec(hwi_find_device(DRA74x_HWI_UART, 0), -1, DRA74X_UART1_IRQ);
		//hwitag_set_avail_ivec(hwi_find_device(DRA74x_HWI_UART, 1), -1, DRA74X_UART2_IRQ);
		hwitag_set_avail_ivec(hwi_find_device(DRA74x_HWI_UART, 1), -1, DRA74X_UART3_IRQ);
		hwitag_set_avail_ivec(hwi_find_device(DRA74x_HWI_UART, 2), -1, DRA74X_UART4_IRQ);
		hwitag_set_avail_ivec(hwi_find_device(DRA74x_HWI_UART, 3), -1, DRA74X_UART8_IRQ);
	}

	/* add 4 McSPI's */
	{
		unsigned hwi_off;
		hwiattr_spi_t spi_attr = HWIATTR_SPI_T_INITIALIZER;

		/* All MCSPIs opearte on 48MHz only */
		struct hwi_inputclk clksrc = {.clk = 48000000, .div = 1};

		/* each SPI has an interrupt */
		HWIATTR_SPI_SET_NUM_IRQ(&spi_attr, 1);
		HWIATTR_SPI_SET_NUM_CLK(&spi_attr, 1);

		/* create spi0 and set the clock source */
		HWIATTR_SPI_SET_LOCATION(&spi_attr, DRA74X_MCSPI1_HL_BASE, DRA74X_MCSPI_REG_SIZE, 0, hwi_find_as(DRA74X_MCSPI1_HL_BASE, 1));
		hwi_off = hwibus_add_spi(HWI_NULL_OFF, &spi_attr);
		ASSERT(hwi_find_unit(hwi_off) == 0);
		hwitag_set_inputclk(hwi_off, 0, &clksrc);

		/* create spi1 and set the clock source */
		HWIATTR_SPI_SET_LOCATION(&spi_attr, DRA74X_MCSPI2_HL_BASE, DRA74X_MCSPI_REG_SIZE, 0, hwi_find_as(DRA74X_MCSPI2_HL_BASE, 1));
		hwi_off = hwibus_add_spi(HWI_NULL_OFF, &spi_attr);
		ASSERT(hwi_find_unit(hwi_off) == 1);
		hwitag_set_inputclk(hwi_off, 0, &clksrc);

		/* create spi2 and set the clock source */
		HWIATTR_SPI_SET_LOCATION(&spi_attr, DRA74X_MCSPI3_HL_BASE, DRA74X_MCSPI_REG_SIZE, 0, hwi_find_as(DRA74X_MCSPI3_HL_BASE, 1));
		hwi_off = hwibus_add_spi(HWI_NULL_OFF, &spi_attr);
		ASSERT(hwi_find_unit(hwi_off) == 2);
		hwitag_set_inputclk(hwi_off, 0, &clksrc);

		/* create spi3 and set the clock source */
		HWIATTR_SPI_SET_LOCATION(&spi_attr, DRA74X_MCSPI4_HL_BASE, DRA74X_MCSPI_REG_SIZE, 0, hwi_find_as(DRA74X_MCSPI4_HL_BASE, 1));
		hwi_off = hwibus_add_spi(HWI_NULL_OFF, &spi_attr);
		ASSERT(hwi_find_unit(hwi_off) == 3);
		hwitag_set_inputclk(hwi_off, 0, &clksrc);

		/* Assign interrupt vectors to the 4 SPI buses */
		hwitag_set_avail_ivec(hwi_find_bus(HWI_ITEM_BUS_SPI, 0), -1, DRA74X_MCSPI1_IRQ);
		hwitag_set_avail_ivec(hwi_find_bus(HWI_ITEM_BUS_SPI, 1), -1, DRA74X_MCSPI2_IRQ);
		hwitag_set_avail_ivec(hwi_find_bus(HWI_ITEM_BUS_SPI, 2), -1, DRA74X_MCSPI3_IRQ);
		hwitag_set_avail_ivec(hwi_find_bus(HWI_ITEM_BUS_SPI, 3), -1, DRA74X_MCSPI4_IRQ);
	}

	/* add 5 I2C's */
	{
		unsigned hwi_off;
		hwiattr_i2c_t i2c_attr = HWIATTR_I2C_T_INITIALIZER;

		/* All I2Cs operate on 96MHz only */
		struct hwi_inputclk clksrc = {.clk = 96000000, .div = 1};

		/* each I2C has an interrupt */
		HWIATTR_I2C_SET_NUM_IRQ(&i2c_attr, 1);
		HWIATTR_I2C_SET_NUM_CLK(&i2c_attr, 1);

		/* create i2c0 and set the clock source */
		HWIATTR_I2C_SET_LOCATION(&i2c_attr, DRA74X_I2C1_BASE, DRA74X_I2C_REG_SIZE, 0, hwi_find_as(DRA74X_I2C1_BASE, 1));
		hwi_off = hwibus_add_i2c(HWI_NULL_OFF, &i2c_attr);
		ASSERT(hwi_find_unit(hwi_off) == 0);
		hwitag_set_inputclk(hwi_off, 0, &clksrc);

		/* create i2c1 and set the clock source */
		HWIATTR_I2C_SET_LOCATION(&i2c_attr, DRA74X_I2C2_BASE, DRA74X_I2C_REG_SIZE, 0, hwi_find_as(DRA74X_I2C2_BASE, 1));
		hwi_off = hwibus_add_i2c(HWI_NULL_OFF, &i2c_attr);
		ASSERT(hwi_find_unit(hwi_off) == 1);
		hwitag_set_inputclk(hwi_off, 0, &clksrc);

		/* create i2c2 and set the clock source */
		HWIATTR_I2C_SET_LOCATION(&i2c_attr, DRA74X_I2C3_BASE, DRA74X_I2C_REG_SIZE, 0, hwi_find_as(DRA74X_I2C3_BASE, 1));
		hwi_off = hwibus_add_i2c(HWI_NULL_OFF, &i2c_attr);
		ASSERT(hwi_find_unit(hwi_off) == 2);
		hwitag_set_inputclk(hwi_off, 0, &clksrc);

		/* create i2c3 and set the clock source */
		HWIATTR_I2C_SET_LOCATION(&i2c_attr, DRA74X_I2C4_BASE, DRA74X_I2C_REG_SIZE, 0, hwi_find_as(DRA74X_I2C4_BASE, 1));
		hwi_off = hwibus_add_i2c(HWI_NULL_OFF, &i2c_attr);
		ASSERT(hwi_find_unit(hwi_off) == 3);
		hwitag_set_inputclk(hwi_off, 0, &clksrc);

		/* create i2c4 and set the clock source */
		HWIATTR_I2C_SET_LOCATION(&i2c_attr, DRA74X_I2C5_BASE, DRA74X_I2C_REG_SIZE, 0, hwi_find_as(DRA74X_I2C5_BASE, 1));
		hwi_off = hwibus_add_i2c(HWI_NULL_OFF, &i2c_attr);
		ASSERT(hwi_find_unit(hwi_off) == 4);
		hwitag_set_inputclk(hwi_off, 0, &clksrc);

		/* Assign interrupt vectors to the 4 I2C buses */
		hwitag_set_avail_ivec(hwi_find_bus(HWI_ITEM_BUS_I2C, 0), -1, DRA74X_I2C1_IRQ);
		hwitag_set_avail_ivec(hwi_find_bus(HWI_ITEM_BUS_I2C, 1), -1, DRA74X_I2C2_IRQ);
		hwitag_set_avail_ivec(hwi_find_bus(HWI_ITEM_BUS_I2C, 2), -1, DRA74X_I2C3_IRQ);
		hwitag_set_avail_ivec(hwi_find_bus(HWI_ITEM_BUS_I2C, 3), -1, DRA74X_I2C4_IRQ);
		hwitag_set_avail_ivec(hwi_find_bus(HWI_ITEM_BUS_I2C, 4), -1, DRA74X_I2C5_IRQ);
	}

	/* add the WDT device */
	{
		unsigned hwi_off;
		hwiattr_timer_t attr = HWIATTR_TIMER_T_INITIALIZER;
		const struct hwi_inputclk clksrc_kick = {.clk = 1000, .div = 1};

		HWIATTR_TIMER_SET_NUM_CLK(&attr, 1);

		HWIATTR_TIMER_SET_LOCATION(&attr, DRA74X_WDTIMER2_BASE, 0x1000, 0, hwi_find_as(DRA74X_WDTIMER2_BASE, 1));
		hwi_off = hwidev_add_timer(DRA74x_HWI_WDT, &attr, HWI_NULL_OFF);
		ASSERT(hwi_off != HWI_NULL_OFF);
		hwitag_set_inputclk(hwi_off, 0, (struct hwi_inputclk *)&clksrc_kick);
	}

	/* Add SDIO */
	{
		unsigned	hwi_off;
		hwi_tag	 *tag;
		unsigned	string_name, string_opt;

		hwi_off = hwibus_add(HWI_ITEM_BUS_SDIO, HWI_NULL_OFF);
		string_name = add_string("omap");

		/* Micro SD slot on the CPU board */
		hwitag_add_location(hwi_off, DRA74X_MMC1_BASE, DRA74X_MMC_SIZE, 0, hwi_find_as(DRA74X_MMC1_BASE, 0));
		hwitag_add_inputclk(hwi_off, get_mmcsd_clock(1), 1);
		hwitag_add_irq(hwi_off, DRA74X_MMC1_IRQ);
		tag = hwi_alloc_tag(HWI_TAG_INFO(dll));
		tag->dll.name = string_name;

		/*
		 * Card Detect through GPIO, no WP support
		 * GPIO 6_27 for CD
		 */
		string_opt = add_string("bs=cd_irq=1187:cd_base=0x4805d000:cd_pin=27");
		tag = hwi_alloc_tag(HWI_TAG_INFO(optstr));
		tag->optstr.string = string_opt;

		/* eMMC */
		hwi_off = hwibus_add(HWI_ITEM_BUS_SDIO, HWI_NULL_OFF);
		hwitag_add_location(hwi_off, DRA74X_MMC2_BASE, DRA74X_MMC_SIZE, 0, hwi_find_as(DRA74X_MMC2_BASE, 0));
		hwitag_add_inputclk(hwi_off, get_mmcsd_clock(2), 1);
		hwitag_add_irq(hwi_off, DRA74X_MMC2_IRQ);
		tag = hwi_alloc_tag(HWI_TAG_INFO(dll));
		tag->dll.name = string_name;

		/* "emmc" implies "nocd" option; support 8 data bits */
		string_opt = add_string("bs=emmc:bw=8");
		tag = hwi_alloc_tag(HWI_TAG_INFO(optstr));
		tag->optstr.string = string_opt;

		/* MMC3 on expander board */
		hwi_off = hwibus_add(HWI_ITEM_BUS_SDIO, HWI_NULL_OFF);
		hwitag_add_location(hwi_off, DRA74X_MMC3_BASE, DRA74X_MMC_SIZE, 0, hwi_find_as(DRA74X_MMC3_BASE, 0));
		hwitag_add_inputclk(hwi_off, get_mmcsd_clock(3), 1);
		hwitag_add_irq(hwi_off, DRA74X_MMC3_IRQ);
		tag = hwi_alloc_tag(HWI_TAG_INFO(dll));
		tag->dll.name = string_name;
		string_opt = add_string("bs=nocd");
		tag = hwi_alloc_tag(HWI_TAG_INFO(optstr));
		tag->optstr.string = string_opt;
		tag = hwi_alloc_tag(HWI_TAG_INFO(dma));
		tag->dma.chnl = 78;	/* MMC3 transmit request */
		tag = hwi_alloc_tag(HWI_TAG_INFO(dma));
		tag->dma.chnl = 77;	/* MMC3 receive request */
	}

	/* Add Phoenix RTC */
	{
		hwiattr_rtc_t rtcattr = HWIATTR_RTC_T_INITIALIZER;
		HWIATTR_SET_OPTSTR(&rtcattr.common, "i2c=/dev/i2c0,slave=0x58");
		hwidev_add_rtc("phoenix", &rtcattr, HWI_NULL_OFF);
	}

	/* Add VIP1/2/3 (video capture devices) */
	for (i = 0; i < 3; ++i) {
		unsigned hwi_off = hwidev_add("vip", hwi_devclass_NONE, HWI_NULL_OFF);
		ASSERT(hwi_off != HWI_NULL_OFF);

		{
			hwiattr_common_t vip_attr = HWIATTR_COMMON_INITIALIZER;
			uint32_t base;
			const char *optstr = NULL;

			switch(i) {
			case 0:
				base = DRA74X_VIP1_BASE;
				optstr = "name=vip1";
				break;
			case 1:
				base = DRA74X_VIP2_BASE;
				optstr = "name=vip2";
				break;
			case 2:
				base = DRA74X_VIP3_BASE;
				/* "PORTA of VIP3 module instance supports up to [16-bit]
				 * data, and PORTB is not used (this exception applies for
				 * both slice0 and slice1). */
				optstr = "name=vip3,ports_limited";
				break;
			default:
				ASSERT(0 && "bad VIP index");
			}

			if (optstr) {
				HWIATTR_SET_OPTSTR(&vip_attr, optstr);
			}
			HWIATTR_SET_NUM_IRQ(&vip_attr, 1);
			HWIATTR_SET_LOCATION(&vip_attr, base, 64*1024 /*len*/,
					0 /*reg_shift*/, hwi_find_as(base, 1) /*addrspace*/);

			hwitag_add_common(hwi_off, &vip_attr);
		}

		hwitag_set_ivec(hwi_off, 0, VIP1_IRQ + i + DRA74X_NUM_INTERNAL_IRQS);
	}

	/* Add VPE (Video Processing Engine--adaptive deinterlacer, etc.) */
	{
		unsigned hwi_off = hwidev_add("vpe", hwi_devclass_NONE, HWI_NULL_OFF);
		ASSERT(hwi_off != HWI_NULL_OFF);

		{
			hwiattr_common_t vpe_attr = HWIATTR_COMMON_INITIALIZER;
			const uint32_t base = 0x489D0000;
			HWIATTR_SET_NUM_IRQ(&vpe_attr, 1);
			HWIATTR_SET_LOCATION(&vpe_attr, base, 64*1024 /*len*/,
					0 /*reg_shift*/, hwi_find_as(base, 1) /*addrspace*/);

			hwitag_add_common(hwi_off, &vpe_attr);
		}

		/* IRQs 166 (also see mpu_irq_map in hw_init.c) */
		hwitag_set_ivec(hwi_off, 0, VPE_IRQ + DRA74X_NUM_INTERNAL_IRQS);
	}

	/* Add Crypto AES */
	{
		unsigned hwi_off = hwidev_add("dma_crypto", hwi_devclass_NONE, HWI_NULL_OFF);
		ASSERT(hwi_off != HWI_NULL_OFF);

		{
			hwiattr_common_t crypto_attr = HWIATTR_COMMON_INITIALIZER;
			HWIATTR_SET_NUM_IRQ(&crypto_attr, 1);
			hwitag_add_common(hwi_off, &crypto_attr);
		}

		hwitag_set_ivec(hwi_off, 0, CRYPTO_AES_IRQ + DRA74X_NUM_INTERNAL_IRQS);
	}

	/* Add Crypto MD5 */
	{
		unsigned hwi_off = hwidev_add("sha2md5_crypto", hwi_devclass_NONE, HWI_NULL_OFF);
		ASSERT(hwi_off != HWI_NULL_OFF);

		{
			hwiattr_common_t crypto_attr = HWIATTR_COMMON_INITIALIZER;
			HWIATTR_SET_NUM_IRQ(&crypto_attr, 1);
			hwitag_add_common(hwi_off, &crypto_attr);
		}

		hwitag_set_ivec(hwi_off, 0, CRYPTO_MD5_IRQ + DRA74X_NUM_INTERNAL_IRQS);
	}
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/dra74x/vayu-evm/hwi_dra74x.c $ $Rev: 766791 $")
#endif
