/*
 * $QNXLicenseC:
 * Copyright 2011~2014, QNX Software Systems.
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

 
#ifndef OMAP543x_H
#define OMAP543x_H

// OTG Wrapper Registers
#define USBOTGSS_WRAPPER_SIZE				0x110

/* USBOTG in OMAP5 */
#define OMAP5_DWC_USB3_BASE					0x4a030000
#define OMAP5_USBOTGSS_WRAPPER_BASE			0x4a020000

/* USB_DWCx in Dra7xx */
#define DRRA7XX_USB_DWC1					0x48890000
#define DRRA7XX_USB_DWC2					0x488D0000
#define DRRA7XX_USB_DWC3					0x48910000
#define DRRA7XX_USB_DWC4					0x48950000

#define DRA7XX_USB_WRAPPER1					0x48880000
#define DRA7XX_USB_WRAPPER2					0x488C0000
#define DRA7XX_USB_WRAPPER3					0x48900000
#define DRA7XX_USB_WRAPPER4					0x48940000

	#define USBOTGSS_IRQENABLE_SET_0		0x2c
		#define COREIRQ_EN					( 1 << 0 )
	
	#define USBOTGSS_SYSCONFIG				0x10
		#define IDLEMODE_POS				2
		#define IDLEMODE_MSK				( 0x3 << IDLEMODE_POS )
		#define IDLEMODE_FORCEIDLE			( 0x0 << IDLEMODE_POS )
		#define IDLEMODE_NOIDLE				( 0x1 << IDLEMODE_POS )
		#define IDLEMODE_SMARTIDLE			( 0x2 << IDLEMODE_POS )
		
		#define STANDBYMODE_POS				4
		#define STANDBYMODE_MSK				( 0x3 << STANDBYMODE_POS )
		#define STANDBYMODE_FORCESTANDBY	( 0x0 << STANDBYMODE_POS )
		#define STANDBYMODE_NOSTANDBY		( 0x1 << STANDBYMODE_POS )
		#define STANDBYMODE_SMARTSTANDBY2	( 0x2 << STANDBYMODE_POS )
		#define STANDBYMODE_SMARTSTANDBY3	( 0x3 << STANDBYMODE_POS )
		
	#define USBOTGSS_UTMI_OTG_STATUS		0x84
		#define OTGSTATUS_VBUSVALID			( 1 << 1 )
		#define OTGSTATUS_SESSVALID			( 1 << 2 )
		#define OTGSTATUS_SESSEND			( 1 << 3 )
		#define OTGSTATUS_IDDIG_FLOAT		( 1 << 4 )
		#define OTGSTATUS_POWERPRESENT		( 1 << 9 )
		#define OTGSTATUS_SWMODE			( 1 << 31 )
		
#define CONTROL_PHY_POWER_USB_				0x4a002370
#define CONTROL_PHY_POWER_USB_SIZE			4

	#define USB_PWRCTL_CLK_FREQ_POS			22
	#define USB_PWRCTL_CLK_FREQ_MSK			( 0x3ff << USB_PWRCTL_CLK_FREQ_POS )
	
	#define USB_PWRCTL_CLK_CMD_POS			14
	#define USB_PWRCTL_CLK_CMD_MSK			( 0xff << USB_PWRCTL_CLK_CMD_POS )
	#define USB_PWRCTL_CLK_CMD_TXRXON		( 0x3 << USB_PWRCTL_CLK_CMD_POS )

#define USB3_PHY_BASE						0x4a084400
#define USB3_PHY_SIZE						0x1000				
	#define PHYRX_ANA						0xc
	#define PHYRX_TRIM						0x1c
	#define PHYRX_DLL						0x24
	#define PHYRX_DIGITAL_MODES				0x28
	#define PHYRX_EQUALIZER					0x38

	#define PHYTX_CONFIG					0x40c
	#define PHYTX_TEST						0x42c
	#define PHYTX_PATGEN					0x430

//USBOTG in AM437x
#define AM437X_USB_DWC1                     0x48390000
#define AM437X_USB_DWC2                     0x483D0000
#define AM437X_USB_WRAPPER1                 0x48380000
#define AM437X_USB_WRAPPER2                 0x483C0000
#define AM437X_USBOTGSS_WRAPPER_SIZE        0x10000

#define AM437X_USBOTGSS_IRQENABLE_SET_0		0x28
#define AM437X_USBOTGSS_UTMI_OTG_STATUS		0x504

typedef enum {
    OMAP5 = 0,
    DRA7X = 1,
    AM437X =2,
}Platform_Type;

extern Platform_Type platform_t;


// omap4430 context
typedef struct _xdctrl_t {
	uint8_t			* wrapper_base;
	uint8_t			* control_phy_power_usb;
	uint8_t			* phybase;
} xdctrl_t;
#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devu/dc/dwcotg3/omap543x/omap543x.h $ $Rev: 762382 $")
#endif
