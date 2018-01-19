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
#include <private/sdmmc.h>

unsigned int chip_revision;

IPL_BootOpt_t ipl_boot_menu()
{
	char	opt;

	while (1) {
		ser_putstr("Command: \n");
		ser_putstr("Press 'S' for SERIAL download.\n");
		ser_putstr("Press 'M' for SDMMC download, file QNX-IFS assumed.\n");
		ser_putstr("Press 'E' for EMMC download, file QNX-IFS assumed.\n");
		ser_putstr("Press 'Q' for QSPI NOR flash download, IFS assumed to be present at 64KB offset.\n");
		ser_putstr("Press 'T' for Memory test.\n");

		opt = ser_getchar();

		switch (opt) {
		case 'M':
		case 'm':
			return (IPL_BOOT_SD);
		case 'E':
		case 'e':
			return (IPL_BOOT_EMMC);
		case 'Q':
		case 'q':
			return (IPL_BOOT_QSPI);
		case 's':
		case 'S':
			return (IPL_BOOT_SERIAL);
		case 't':
		case 'T':
			return (IPL_MEM_TEST);
		}

		ser_putstr("Unrecognized option\n");
	}
}

int main(void)
{
	unsigned		image, start;
	IPL_BootOpt_t	bootOpt;

	chip_revision = get_omap_rev();

#if defined (CACHE_ENABLED)
	enable_cache_mmu();
#endif

	/* Initialize Pin Muxing, SDRAM, Clocks etc. */
	init_hw_platform();

	/*
	 * The eMMC part takes 400-600ms between the first CMD1 and fully powered up
	 * Thus issuing CMD0 and CMD1 in IPL will significantly reduce the startup time of MMCSD driver on eMMC
	 */
	powerup_emmc(2);

	log_putstr("\nQNX Neutrino Initial Program Loader for DRA74X Vayu EVM\n");

	print_omap_revision();

	bootOpt = ipl_boot_detect();

#if MANBOOT
	bootOpt = ipl_boot_menu();
#endif

	while (1) {
		image = QNX_LOAD_ADDR;
		switch (bootOpt) {
			case IPL_BOOT_SD:
				if (sdmmc_load_file(SD_INTERFACE, 1, image, "QNX-IFS", 0) != 0) {
					ser_putstr("load image from SD failed\n");
					goto print_boot_menu;
				}
				break;
			case IPL_BOOT_EMMC:
				if (sdmmc_load_file(EMMC_INTERFACE, 2, image, "QNX-IFS", 0) != 0) {
					ser_putstr("load image from eMMC failed\n");
					goto print_boot_menu;
				}
				break;
			case IPL_BOOT_QSPI:
				if (qspi_load_image(QSPI_IFS_OFFSET, image) != 0) {
					ser_putstr("load image from QSPI flash failed\n");
					goto print_boot_menu;
				}
				break;
			case IPL_BOOT_SERIAL:
				ser_putstr("Send IFS image through serial now...\n");
				if (image_download_ser(image) != 0) {
					ser_putstr("Download image failed\n");
					goto print_boot_menu;
				}

				ser_putstr("Download ok...\n");

				/* get remaining bytes */
				while (ser_poll())
					ser_getchar();

				break;
			case IPL_MEM_TEST:
				sdram_test(DRA74x_EMIF_CS0_BASE, MEG(1536));
			default:
				goto print_boot_menu;
		}

		if (bootOpt == IPL_BOOT_SERIAL) {
			start = image_scan_2(image, image + MAX_SCAN, 0);
		} else {
			start = image_scan_2(image, image + MAX_SCAN, 1);
		}

		if (start != 0xffffffff) {
#ifdef HS_AUTH_IFS
			/* Authenticate loaded image before jumping execution */
			authenticate_image_signature(image, startup_hdr.stored_size + start - image + IFS_CERT_SIZE);
#endif
			log_putstr("Found image		@ 0x");
			log_puthex(start);
			log_putstr("\n");
			image_setup_2(start);

			log_putstr("Jumping to startup @ 0x");
			log_puthex(startup_hdr.startup_vaddr);
			log_putstr("\n\n");

#if defined (CACHE_ENABLED)
			/*
			 * Flush D-cache and invalidate I-cache before jumping
			 */
			arm_v7_dcache_flush();
			arm_v7_icache_invalidate();
#endif

#if defined(DEBUG_BOOT_TIMING)
			omap_timer_curr("IPL total", TIMING_MILLI_SECOND);
#endif
			image_start_2(start);

			/* Never reach here */
			return 0;
		}

		ser_putstr("Image_scan failed...\n");

print_boot_menu:
		bootOpt = ipl_boot_menu();
	}

	return 0;
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/ipl/boards/dra74x/arm/vayu-evm.le.v7/main.c $ $Rev: 758539 $")
#endif
