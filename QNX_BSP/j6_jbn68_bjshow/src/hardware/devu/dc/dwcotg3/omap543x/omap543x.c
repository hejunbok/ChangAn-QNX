/*
 * $QNXLicenseC:
 * Copyright 2010~2014, QNX Software Systems.
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


/* 
 *	Custom OMAP 4430 Callouts
 */

#include <stdlib.h>
#include <malloc.h>
#include <errno.h> 
#include <sys/mman.h>
#include <sys/slog.h>

#include "dwcotg.h"
#include "omap543x.h"


////////////////////////////////////////////////////////////////////////////////
//                        PRIVATE FUNCTIONS                                   //
////////////////////////////////////////////////////////////////////////////////

Platform_Type platform_t = 0;

int map_regs(dctrl_t * dc) {

	uint32_t	dwcotg_base = PCI_MEM_ADDR(dc->udc->hw_ctrl.pci_inf->CpuBaseAddress[0]);
	uint32_t	wrapper_base;
	uint32_t	wrapper_size;
	xdctrl_t	*xdc = dc->xdc;
	int		err;

	/* map in omap543x USB OTG wrapper registers... */
	switch ( dwcotg_base ) {
		case OMAP5_DWC_USB3_BASE:
			wrapper_base = OMAP5_USBOTGSS_WRAPPER_BASE;
			break;
		case DRRA7XX_USB_DWC1:
			wrapper_base = DRA7XX_USB_WRAPPER1;
			break;
		case DRRA7XX_USB_DWC2:
			wrapper_base = DRA7XX_USB_WRAPPER2;
			break;
		case DRRA7XX_USB_DWC3:
			wrapper_base = DRA7XX_USB_WRAPPER3;
			break;
		case DRRA7XX_USB_DWC4:
			wrapper_base = DRA7XX_USB_WRAPPER4;
			break;
		case AM437X_USB_DWC1:
			wrapper_base = AM437X_USB_WRAPPER1;
			platform_t = AM437X;
			break;
		case AM437X_USB_DWC2:
			wrapper_base = AM437X_USB_WRAPPER2;
			platform_t = AM437X;
			break;
		default:
			err = ENODEV;
			goto error;
	}

	if(platform_t == AM437X){
		wrapper_size = AM437X_USBOTGSS_WRAPPER_SIZE;
	} else {
		wrapper_size = USBOTGSS_WRAPPER_SIZE;
	}

	xdc->wrapper_base =
		mmap_device_memory( NULL,
							wrapper_size,
							PROT_READ | PROT_WRITE | PROT_NOCACHE,
							MAP_SHARED | MAP_PHYS,
							wrapper_base);

	if ( xdc->wrapper_base == MAP_FAILED ) {
		dwcotg_slogf( dc, _SLOG_ERROR, "%s mmap ( wrapper_base ) failed",__func__);
		err = ENOMEM;
		goto error;
	}

	if(platform_t != AM437X){
		/* map in omap543x USB OTG wrapper registers... */
		xdc->control_phy_power_usb =
			mmap_device_memory( NULL,
								CONTROL_PHY_POWER_USB_SIZE,
								PROT_READ | PROT_WRITE | PROT_NOCACHE,
								MAP_SHARED | MAP_PHYS,
								CONTROL_PHY_POWER_USB_);
		if ( xdc->control_phy_power_usb == MAP_FAILED ) {
			dwcotg_slogf( dc, _SLOG_ERROR, "%s mmap ( control_phy_power_usb ) failed",__func__);
			err = ENOMEM;
			goto error2;
		}

		/* map in omap543x USB OTG wrapper registers... */
		xdc->phybase =
			mmap_device_memory( NULL,
								USB3_PHY_SIZE,
								PROT_READ | PROT_WRITE | PROT_NOCACHE,
								MAP_SHARED | MAP_PHYS,
								USB3_PHY_BASE );
		if ( xdc->phybase == MAP_FAILED ) {
			dwcotg_slogf( dc, _SLOG_ERROR, "%s mmap ( phybase ) failed",__func__);
			err = ENOMEM;
			goto error3;
		}
	}

	return EOK;

error3:
	munmap( xdc->control_phy_power_usb, CONTROL_PHY_POWER_USB_SIZE );
error2:
	munmap( xdc->wrapper_base, USBOTGSS_WRAPPER_SIZE );
error:
	return err;
}


int unmap_regs(dctrl_t * dc) {
	xdctrl_t	* xdc = dc->xdc;

	if(xdc->phybase){
	    munmap( xdc->phybase, USB3_PHY_SIZE );
	}
	if(xdc->control_phy_power_usb){
	    munmap( xdc->control_phy_power_usb, CONTROL_PHY_POWER_USB_SIZE );
	}
	if(xdc->wrapper_base){
		if(platform_t == AM437X){
			munmap( xdc->wrapper_base, AM437X_USBOTGSS_WRAPPER_SIZE );
		} else {
			munmap( xdc->wrapper_base, USBOTGSS_WRAPPER_SIZE );
		}
	}
	return EOK;
}


int phy_config(dctrl_t * dc) {
	xdctrl_t	* xdc = dc->xdc;

	/* Magic Numbers are prefered values taken from omap5430 TRM in a table (28-17) called
	 * Preferred USB3_PHY_RX_SCP Register Settings in Section 28
	 */
	
	HW_Write32( xdc->phybase, PHYRX_ANA,			0x80028000 );
	HW_Write32( xdc->phybase, PHYRX_TRIM,			0x80000000);
	HW_Write32( xdc->phybase, PHYRX_DLL,			0xc0000000);
	HW_Write32( xdc->phybase, PHYRX_DIGITAL_MODES, 	0x04e33000 );
	HW_Write32( xdc->phybase, PHYRX_EQUALIZER,		0x480);
	
	return EOK;	
}

////////////////////////////////////////////////////////////////////////////////
//                         PUBLIC FUNCTIONS                                   //
////////////////////////////////////////////////////////////////////////////////

int dwcotg_custom_init1( dctrl_t * dc, char * args ) {
	int			err;
	xdctrl_t	* xdc;
	
	// allocate device specific controller context
	dc->xdc = xdc = calloc( 1, sizeof( xdctrl_t ) );
	if ( dc->xdc == NULL ) {
		dwcotg_slogf( dc, _SLOG_ERROR, "%s calloc() failed ",__func__);
		err = ENOMEM;
		goto error;
	}

	err = map_regs( dc );
	if ( err != EOK ) {
		dwcotg_slogf( dc, _SLOG_ERROR, "%s map_regs() failed... err = %d ",__func__, err);
		goto error2;
	}

	if(platform_t != AM437X){
	    // make sure the phy power sequencer if off
	    HW_Write32And( xdc->control_phy_power_usb, 0, ~USB_PWRCTL_CLK_CMD_MSK );

		// Use power-on / uboot initialized values for now because they seem to be working...
		// Need to determine optimal values for the registers.
		//phy_config( dc );
	}
	return EOK;

error2:
	free( dc->xdc );
error:
	return err;
}

int dwcotg_custom_init2( dctrl_t * dc ) {
	xdctrl_t * xdc = dc->xdc;

	// enable the core interrupts
	if(platform_t == AM437X){
		HW_Write32Or( xdc->wrapper_base, AM437X_USBOTGSS_IRQENABLE_SET_0, COREIRQ_EN );
	} else {
		HW_Write32Or( xdc->wrapper_base, USBOTGSS_IRQENABLE_SET_0, COREIRQ_EN );
	}
	// same as android for now.... SMART-IDLE doesn't seem to work correctly... needs further investigation
	HW_Write32( xdc->wrapper_base, USBOTGSS_SYSCONFIG, STANDBYMODE_NOSTANDBY | IDLEMODE_NOIDLE );

	// set fake OTG PHY signals
	if(platform_t == AM437X){
	    HW_Write32( xdc->wrapper_base, AM437X_USBOTGSS_UTMI_OTG_STATUS,
		OTGSTATUS_VBUSVALID | OTGSTATUS_SESSVALID | OTGSTATUS_IDDIG_FLOAT | OTGSTATUS_POWERPRESENT | OTGSTATUS_SWMODE );
	} else {
	    HW_Write32( xdc->wrapper_base, USBOTGSS_UTMI_OTG_STATUS,
		OTGSTATUS_VBUSVALID | OTGSTATUS_SESSVALID | OTGSTATUS_IDDIG_FLOAT | OTGSTATUS_POWERPRESENT | OTGSTATUS_SWMODE );
	}

	return EOK;
}

int dwcotg_custom_fini1( dctrl_t * dc ) {
	unmap_regs( dc );
	free( dc->xdc );
	return EOK;
}

int dwcotg_custom_fini2( dctrl_t * dc ) {
	xdctrl_t * xdc = dc->xdc;

	// disable the core interrupts
	if(platform_t == AM437X){
		HW_Write32And( xdc->wrapper_base, AM437X_USBOTGSS_IRQENABLE_SET_0, ~COREIRQ_EN );
	} else {
		HW_Write32And( xdc->wrapper_base, USBOTGSS_IRQENABLE_SET_0, ~COREIRQ_EN );
	}
	return EOK;
}

int dwcotg_custom_reset( dctrl_t * dc ) {
	return EOK;
}

int dwcotg_phy_in_reset( dctrl_t * dc ) {
	xdctrl_t * xdc = dc->xdc;
	if(platform_t != AM437X){
		HW_Write32Or( xdc->control_phy_power_usb, 0, USB_PWRCTL_CLK_CMD_TXRXON );
	}
	return EOK;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devu/dc/dwcotg3/omap543x/omap543x.c $ $Rev: 762414 $")
#endif
