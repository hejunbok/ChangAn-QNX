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
#include <private/fat-fs.h>
#include <private/omap_sdhc.h>
#include <private/omap_bps.h>
#include "qspi.h"

extern unsigned long _TI_ROM_r0;

/*
 * The buffer used by fat-fs.c as the common buffer, which should be cache-disabled,
 * Also ADMA requires that the buffers should be in the external memory
 */
static unsigned char fat_buf2[FAT_COMMON_BUF_SIZE] __attribute__ ((section(".scratch")));
static unsigned char fat_buf1[FAT_FS_INFO_BUF_SIZE] __attribute__ ((section(".scratch")));

/* ADMA demands that both the buffer and DMA descriptor should be in DRAM, not internal SRAM */
static omap_adma32_t adma_des[MAX_BLKCNT * SDMMC_BLOCKSIZE / ADMA_CHUNK_SIZE] __attribute__ ((section(".scratch")));

static uint32_t dra74x_mmcsd_base[] = {
	-1,
	DRA74X_MMC1_BASE,
	DRA74X_MMC2_BASE,
	DRA74X_MMC3_BASE,
	DRA74X_MMC4_BASE
};

int sdmmc_load_file(int interface, unsigned dev, unsigned address, const char *fn, unsigned caps)
{
	sdmmc_t			sdmmc;
	int				status;
	uint32_t		base = dra74x_mmcsd_base[dev];
	omap_adma_ext_t dma_ext = {
		.adma_des = adma_des
	};

	omap_sdmmc_init_hc(&sdmmc, base, get_mmcsd_clock(dev) / 1000, SDMMC_VERBOSE_LVL_0, OMAP_SDMMC_ADMA, &dma_ext);
	sdmmc.caps = caps;

	if (interface == SD_INTERFACE) {
		if (SDMMC_OK != sdmmc_init_sd(&sdmmc)) {
			ser_putstr("SDMMC interface init failed\n");
			return -1;
		}
	} else {
		if (SDMMC_OK != sdmmc_init_mmc(&sdmmc)) {
			ser_putstr("SDMMC interface init failed\n");
			return -1;
		}
	}

	log_putstr("Load QNX image from SDMMC...\n");

	fat_sdmmc_t fat = {
		.ext = &sdmmc,
		.buf1 = fat_buf1,
		.buf1_len = FAT_FS_INFO_BUF_SIZE,
		.buf2 = fat_buf2,
		.buf2_len = FAT_COMMON_BUF_SIZE,
		.verbose = 0
	};

	if (fat_init(&fat)) {
		ser_putstr("Failed to init fat-fs\n");
		status = SDMMC_ERROR;
		goto done;
	}

#if defined (CACHE_ENABLED)
	arm_v7_dcache_flush();
#endif

	status = fat_copy_named_file((unsigned char *)address, (char *)fn);

#if defined(DEBUG_BOOT_TIMING)
	omap_timer_curr("loading IFS from SD/eMMC ", TIMING_MILLI_SECOND);
#endif

done:
	sdmmc_fini(&sdmmc);
	return status;
}

int powerup_emmc(int dev)
{
	sdmmc_t	sdmmc;
	uint32_t base = dra74x_mmcsd_base[dev];

	omap_sdmmc_init_hc(&sdmmc, base, get_mmcsd_clock(dev) / 1000, SDMMC_VERBOSE_LVL_0, OMAP_SDMMC_DMA_NONE, (void *)0);
	sdmmc_powerup_card(&sdmmc);
	sdmmc_fini(&sdmmc);

	return SDMMC_OK;
}

int qspi_load_image(unsigned start, unsigned address)
{
	uint32_t offset;
	struct startup_header	*hdr;

#if defined (CACHE_ENABLED)
	arm_v7_dcache_flush();
#endif

	log_putstr("Load QNX image from QSPI NOR flash...\n");

	/* make it ready for reading */
	qspi_mode_config(QSPI_MODE_MMAP);

	for (offset = start; offset < start + MAX_SCAN; offset += 4) {
		hdr = (struct startup_header *)(offset + DRA74X_QSPI_SFI_MM_BASE);
		if (hdr->signature == STARTUP_HDR_SIGNATURE) {
#ifdef HS_AUTH_IFS
			qspi_read(start, address, hdr->stored_size + offset - start + IFS_CERT_SIZE);
#else
			qspi_read(offset, address, hdr->stored_size);
#endif

#if defined(DEBUG_BOOT_TIMING)
			omap_timer_curr("loading IFS from QSPI", TIMING_MILLI_SECOND);
#endif
			return 0;
		}
	}

	return (-1);
}

IPL_BootOpt_t ipl_boot_detect(void)
{
	j5_bps_t *bps = (j5_bps_t *)_TI_ROM_r0;
	IPL_BootOpt_t bootOpt = IPL_BOOT_UNKNOWN;

	/* Where did IPL boot from? */
	switch (bps->boot_dev) {
		case BOOT_XIP:
		case BOOT_XIP_WAIT:
			log_putstr("Booting from NOR Flash\n");
			bootOpt = IPL_BOOT_NOR;
			break;
		case BOOT_SD_CARD:
			log_putstr("Booting from SD card\n");
			bootOpt = IPL_BOOT_SD;
			break;
		case BOOT_EMMC:
		case BOOT_EMMC_PARTITION:
			log_putstr("Booting from eMMC\n");
			bootOpt = IPL_BOOT_EMMC;
			break;
		case BOOT_QSPI_1:
		case BOOT_QSPI_4:
			log_putstr("Booting from QSPI\n");
			bootOpt = IPL_BOOT_QSPI;
			break;
		case BOOT_UART3:
			log_putstr("Booting from UART3\n");
			bootOpt = IPL_BOOT_SERIAL;
			break;
		default:
			log_putstr("Unknown boot device\n");
			break;
	}
	return bootOpt;
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/ipl/boards/dra74x/load_ifs.c $ $Rev: 758539 $")
#endif
