/*
 * $QNXLicenseC:
 * Copyright 2013, QNX Software Systems.
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

// Module Description: OMAP generic board specific interface for sdio/hc/omap.c driver

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <hw/inout.h>
#include "bs.h"
#include "omap.h"

#ifdef USE_LDO
#include <ldo.h>
#endif

#define		GPIO_SIZE		0x2000
#define		GPIO_DATAIN		0x138

/*
 * Handle CD/WP if no card detection, or through MMCHS interrupt
 */
static int generic_cd( sdio_hc_t *hc )
{
#if 1
	/*
	 * When the MMCHS controller is powered off, the MMCHS_PSTATE register is not accessible,
	 * So, if the host driver doesn't change the it's power manager, we can't use MMCHS_PSTATE for CD/WP
	 */
	return CD_INS;
#else
	omap_hc_mmchs_t	*mmchs;
	omap_ext_t		*ext;
	uint32_t		status;
	int				cstate = CD_RMV;
	uintptr_t		base;

	mmchs = (omap_hc_mmchs_t *)hc->cs_hdl;
	ext = (omap_ext_t *)hc->bs_hdl;
	base = mmchs->mmc_base;

	/* Assuming card is always inserted */
	if (ext->nocd) {
		return CD_INS;
	}

	/* MMCx_CD and MMCx_WP pins are connected so that MMCHS_PSTATE register can tell CD/WP status */
	status = in32(mmchs->mmc_base + MMCHS_PSTATE);

	if (status & PSTATE_CDPL) {
		cstate |= CD_INS;

		/* Write protected */
		if (!(status & PSTATE_WP))
			cstate |= CD_WP;
	}

	return (cstate);
#endif
}

/* CD/WP through GPIO pins, interrupt may or may not be used */
static int generic_gpio_cd(sdio_hc_t *hc)
{
	omap_ext_t	*ext = (omap_ext_t *)hc->bs_hdl;
	int cstate = CD_RMV;

	/* CD pin low to indicate card inserted */
	if (!(in32(ext->cd_base + GPIO_DATAIN) & (1 << ext->cd_pin))) {
		cstate = CD_INS;

		/* Write protected */
		if ((ext->wp_base != MAP_DEVICE_FAILED) && ((ext->wp_polarity << ext->wp_pin) == (in32(ext->wp_base + GPIO_DATAIN) & (1 << ext->wp_pin)))) {
			cstate |= CD_WP;
		}
	}

	if (ext->cd_irq)
		InterruptUnmask( ext->cd_irq, ext->cd_iid );

	return cstate;
}

/* Use ":" as separator, other than the regular ","	*/
static int my_getsubopt(char **optionp, char * const *tokens, char **valuep) {
	char		*p, *opt;
	int			len, index;
	const char	*token;

	*valuep = 0;

	for(opt = p = *optionp, len = 0; *p && *p != ':'; p++, len++) {
		if(*p == '=') {
			for(*valuep = ++p; *p && *p != ':'; p++) {
				/* nothing to do */
			}
			break;
		}
	}
	if(*p) {
		*p++ = '\0';
	}
	*optionp = p;

	for(index = 0; (token = *tokens++); index++) {
		if(*token == *opt && !strncmp(token, opt, len) && token[len] == '\0') {
			return index;
		}
	}
	*valuep = opt;
	return -1;
}

/*
 * Board Specific options can be passed through command line arguments or syspage optstr attribute,
 * but the syspage way is recommended since it can pass different options to different MMCHS contollers
 * Example of the BS options: bs=cd_irq=165:cd_base=0x0209C000:cd_pin=5:vdd1_8
 *		-- CD pin is GPIO1[5] (GPIO1 Base: 0x0209C000)
 *		-- IRQ for GPIO1[5] is 165
 *		-- 1.8v is supported
 * Also please note that the optstr passed from syspage can be overwritten by the SDMMC command line arguments
 *
 * Notes:
 *		CD_BASE=base, CD_PIN=pin, CD_IRQ=irq can be replaced by one single option: CD=base^pin^irq
 *		WP_BASE=base, WP_PIN=pin can be replaced by one single option: WP=base^pin
 *		The polarity of the WP pin can be negated with the optional '~' preceding the pin number;
 * For example:
 *		cd=0x4805d000^27^1187 is equavalent to cd_base=0x4805d000:cd_pin=27:cd_irq=1187
 *		wp=0x4805d000^27 is equavalent to wp_base=0x4805d000:wp_pin=27
 */
static int generic_bs_args(sdio_hc_t *hc, char *options)
{
	char		*value;
	int			opt;
	omap_ext_t	*ext = (omap_ext_t *)hc->bs_hdl;
	static char	*opts[] = {
#define	EMMC		0
			"emmc",		/* implies "nocd" option is set as well */
#define	NOCD		1
			"nocd",		/* no card detection capability */
#define	CD_IRQ		2
			"cd_irq",	/* Interrupt vector for GPIO CD pin */
#define	CD_BASE		3
			"cd_base",	/* GPIO base address for the CD pin */
#define	CD_PIN		4
			"cd_pin",
#define CD			5	/* This option covers CD_IRQ, CD_BASE, CD_PIN */
			"cd",
#define	WP_BASE		6
			"wp_base",	/* GPIO base address for the WP pin */
#define	WP_PIN		7
			"wp_pin",
#define WP			8	/* This option covers WP_BASE, WP_PIN */
			"wp",
#define	BW			9
			"bw",		/* data bus width */
#define	VDD1_8		10
			"vdd1_8",	/* 1.8v support capability */
			NULL };

	while (options && *options != '\0') {
		if ((opt = my_getsubopt( &options, opts, &value)) == -1) {
			sdio_slogf( _SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0, "%s: invalid BS options %s", __func__, options);
			return EINVAL;
		}

		switch (opt) {
			case EMMC:
				hc->caps |= HC_CAP_SLOT_TYPE_EMBEDDED;
				hc->flags |= HC_FLAG_DEV_MMC;
				ext->vdd1_8 = 1;
			case NOCD:		/* EMMC implies NOCD option */
				ext->nocd = 1;
				break;
			case CD_IRQ:	/* If irq is not set, it works in non-interrupt mode */
				ext->cd_irq = strtol(value, 0, 0);
				break;
			case CD_BASE:
				ext->cd_pbase = strtol(value, 0, 0);
				break;
			case CD_PIN:
				ext->cd_pin = strtol(value, 0, 0);
				break;
			case CD:
				ext->cd_pbase = strtol(value, &value, 0);
				if (*value == '^') {
					ext->cd_pin = strtol(value + 1, &value, 0);
				}
				if (*value == '^') {
					ext->cd_irq = strtol(value + 1, &value, 0);
				}
				break;
			case WP_BASE:
				ext->wp_pbase = strtol(value, 0, 0);
				break;
			case WP_PIN:
				if (*value == '~') {
					ext->wp_polarity = 1;
					value++;
				}
				ext->wp_pin = strtol(value, 0, 0);
				break;
			case WP:
				ext->wp_pbase = strtol(value, &value, 0);
				if (*value == '^') {
					if (*(value + 1) == '~') {
						ext->wp_polarity = 1;
						value++;
					}       
					ext->wp_pin = strtol(value + 1, &value, 0);
				}
				break;
			case BW:
				ext->bw = strtol(value, 0, 0);
				break;
			case VDD1_8:
				ext->vdd1_8 = 1;
				break;
			default:
				break;
		}
	}

	return EOK;
}

static int generic_dinit( sdio_hc_t *hc )
{
	omap_ext_t *ext = (omap_ext_t *)hc->bs_hdl;

	if (ext) {
		if (ext->cd_iid != -1) {
			InterruptDetach (ext->cd_iid);
		}

		if (ext->cd_base != MAP_DEVICE_FAILED)
			munmap_device_io (ext->cd_base, GPIO_SIZE);

		if (ext->wp_base != MAP_DEVICE_FAILED)
			munmap_device_io (ext->wp_base, GPIO_SIZE);

		free( ext );
	}

	/* dinit() in host driver */
	return (omap_dinit(hc));
}

int bs_set_ldo( sdio_hc_t *hc, int ldo, int voltage )
{
	int		 status = EOK;

#ifdef USE_LDO
	omap_ext_t	*ext;

	ldo_vout_t	vout;
	ldo_state_t vstate;

	ext = hc->bs_hdl;

	if( ext->pmic_fd == -1 )
		return( EOK );

	ldo = ( ldo == SDIO_LDO_VCC ) ? LDO_SDMMC_VCC : LDO_SDMMC_VCC_IO;
	vstate.id = ldo;
	vstate.state = voltage ? LDO_POWER_ON : LDO_POWER_OFF;
	if( ( status = devctl( ext->pmic_fd, DCMD_LDO_SET_STATE, &vstate, sizeof( vstate ), NULL ) ) == EOK ) {
		if( voltage ) {
			vout.id	 = ldo;
			vout.vout = voltage;
			status = devctl( ext->pmic_fd, DCMD_LDO_SET_VOUT, &vout, sizeof( vout ), NULL );
		}
	}
#else

// Fix this

#endif
	return( status );
}

static int bs_ldo_init( sdio_hc_t *hc )
{
#ifdef USE_LDO
	omap_ext_t *ext;
	ext = hc->bs_hdl;

	if( ( ext->pmic_fd = open( LDO_PATH, O_RDWR ) ) == -1 ) {
		sdio_slogf( _SLOGC_SDIODI, _SLOG_ERROR, 1, 1, "%s: open %s - %s", __FUNCTION__, LDO_PATH, strerror( errno ) );
		return( errno );
	}
#else
// Fix this
	return( ENODEV );
#endif

	return( EOK );
}

static int generic_init(sdio_hc_t *hc) {
	omap_ext_t	*ext;
	sdio_hc_cfg_t	*cfg = &hc->cfg;
	int	status;

	if (!(ext = calloc(1, sizeof(omap_ext_t))))
		return ENOMEM;

	/* By default 1.8V is not enabled */
	hc->bs_hdl	= ext;
	memset(ext, 0, sizeof(omap_ext_t));
	ext->cd_base = MAP_DEVICE_FAILED;
	ext->wp_base = MAP_DEVICE_FAILED;
	ext->cd_iid = -1;

	if (generic_bs_args(hc, cfg->options))
		return EINVAL;

	if (ext->cd_pbase) {
		if (MAP_DEVICE_FAILED == (ext->cd_base = mmap_device_io(GPIO_SIZE, ext->cd_pbase))) {
			sdio_slogf( _SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0, "%s: GPIO mmap_device_io failed.", __func__);
			return ENOMEM;
		}
	}

	if (ext->wp_pbase) {
		if (MAP_DEVICE_FAILED == (ext->wp_base = mmap_device_io(GPIO_SIZE, ext->wp_pbase))) {
			sdio_slogf( _SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0, "%s: GPIO mmap_device_io failed.", __func__);
			generic_dinit(hc);
			return ENOMEM;
		}
	}

	if (ext->cd_irq) {
		struct sigevent	event;

		SIGEV_PULSE_INIT(&event, hc->hc_coid, SDIO_PRIORITY, HC_EV_CD, NULL);

		if (-1 == (ext->cd_iid = InterruptAttachEvent(ext->cd_irq, &event, _NTO_INTR_FLAGS_TRK_MSK))) {
			sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, 1, 0, "%s: InterruptAttachEvent(%d) failure - %s", __FUNCTION__, ext->cd_irq, strerror(errno));
			generic_dinit(hc);
			return errno;
		}

		hc->caps |= HC_CAP_CD_INTR;
	}

	if (EOK != (status = omap_init(hc))) {
		generic_dinit(hc);
		return status;
	}

	/*
	 * overwrite cd/dinit functions
	 * by default, CD is detected through MMCHS_PSTATE register and interrupt (HC_CAP_CD_INTR is set)
	 */
	hc->entry.dinit = generic_dinit;
	if (ext->cd_pbase)
		hc->entry.cd = generic_gpio_cd;
	else
		hc->entry.cd = generic_cd;

	/* Overwrite some of the capabilities that are set by omap_init() */
	if (ext->nocd)
		hc->caps &= ~HC_CAP_CD_INTR;

	/*
	 * "bs=vdd1_8" must be set in order to enable 1.8v operations
	 */
	if (!ext->vdd1_8) {
		hc->ocr	 &= ~OCR_VDD_17_195;
		hc->caps &= ~HC_CAP_SV_1_8V;
		hc->caps &= ~( HC_CAP_SDR12 | HC_CAP_SDR25 | HC_CAP_SDR50 | HC_CAP_SDR104 | HC_CAP_DDR50 | HC_CAP_HS200 );
	} else {
		hc->ocr	 |= OCR_VDD_17_195;
	}


	if (ext->bw == 8) {
		hc->caps |= (HC_CAP_BW4 | HC_CAP_BW8);
	} else if (ext->bw == 4) {
		hc->caps |= HC_CAP_BW4;
		hc->caps &= ~HC_CAP_BW8;
	} else if (ext->bw == 1) {
		hc->caps &= ~(HC_CAP_BW4 | HC_CAP_BW8);
	}

	/* Non-EMMC and supporting 1.8V */
	if (!(hc->caps & HC_CAP_SLOT_TYPE_EMBEDDED) && (ext->vdd1_8))
		bs_ldo_init( hc);

	return EOK;
}

static int generic_cd_event(sdio_hc_t *hc) {
	sdio_hc_event(hc, HC_EV_CD);
	return EOK;
}

int bs_event(sdio_hc_t *hc, sdio_event_t *ev) {
	int	status;

	switch(ev->code) {
		case HC_EV_CD:
			status = generic_cd_event(hc);
			break;

		default:
			status = ENOTSUP;
			break;
	}

	return status;
}

sdio_product_t	sdio_ti_products[] = {
		{ SDIO_DEVICE_ID_WILDCARD, 0, 0, "omap", generic_init },
};

sdio_vendor_t	sdio_vendors[] = {
		{ SDIO_VENDOR_ID_WILDCARD, "TI", sdio_ti_products },
			{ 0, NULL, NULL }
};


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devb/sdmmc/arm/omap_generic.le.v7/bs.c $ $Rev: 757124 $")
#endif
