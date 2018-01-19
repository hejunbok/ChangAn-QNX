/*
 * $QNXLicenseC: 
 * Copyright 2010, QNX Software Systems.  
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

#ifndef _DWCOTG_H_INCLUDED
#define _DWCOTG_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <errno.h>
#include <gulliver.h>
#include <queue.h>
#include <sys/slog.h>
#include <hw/inout.h>
#include <drvr/common.h>

#include "sys/io-usb_dcd.h"

////////////////////////////////////////////////////////////////////////////////
//                              DEBUG OPTIONS                                 //
////////////////////////////////////////////////////////////////////////////////

#define DWCOTG_DEBUG

#ifdef DWCOTG_DEBUG
	#define DWCOTG_SLOGF_DBG( dc, level, fmt, ... ) dwcotg_slogf( dc, level, fmt, __VA_ARGS__ )
#else
	#define DWCOTG_SLOGF_DBG( dc, level, fmt, ... )
#endif


////////////////////////////////////////////////////////////////////////////////


#define MAX_N_ENDPOINTS 				32			
#define DWCOTG_SIZE						0x100000	// includes all core regs including xHCI and memory

#define DWCOTG_EVTQ_MEM_SIZE			4096		// divide by 4-bytes for max queued events 
#define DWCOTG_EVT_SZ					4			
#define DWCOTG_N_EVT					( DWCOTG_EVTQ_MEM_SIZE / DWCOTG_EVT_SZ )	


#define SETUP_PACKET_SIZE				8

// delayed execution events

#define PULSE_DELAY_SETUP_PKT			(_PULSE_CODE_MINAVAIL+1)


// xHCI are at Offset 0, but are unused in device mode


// General Registers


#define GSBUSCFG0			0xc100
	#define INCRBRSTENA			( 1 << 0 )
	#define INCR4BRSTENA		( 1 << 1 )
	#define INCR8BRSTENA		( 1 << 2 )
	#define INCR16BRSTENA		( 1 << 3 )
	#define INCR32BRSTENA		( 1 << 4 )
	#define INCR64BRSTENA		( 1 << 5 )
	#define INCR128BRSTENA		( 1 << 6 )
	#define INCR256BRSTENA		( 1 << 7 )
	#define DATBIGEND			( 1 << 11 )
	#define SBUSSTOREANDFORWARD	( 1 << 12 )

#define GSBUSCFG1			0xc104
	#define BREQLIMIT_POS		8
	#define BREQLIMIT_MSK		( 0xf << BREQLIMIT_POS )
	#define EN1KPAGE            ( 1 << 12 )

#define GTXTHRCFG			0xc108
	#define USBMAXTXBURSTSIZE_POS	16	
	#define USBMAXTXBURSTSIZE_MSK	( 0xff << USBMAXTXBURSTSIZE_POS )
	#define USBTXPKTCNT_POS			24	
	#define USBTXPKTCNT_MSK			( 0xf << USBTXPKTCNT_POS )
	#define USBTXPKTCNTSEL			( 1 << 29 )
	
#define GRXTHRCFG			0xc10c
	#define USBMAXRXBURSTSIZE_POS	19
	#define USBMAXRXBURSTSIZE_MSK	( 0xf << USBMAXRXBURSTSIZE_POS )
	#define USBRXPKTCNT_POS			24
	#define USBRXPKTCNT_MSK			( 0xf << USBRXPKTCNT_POS )
	#define USBRXPKTCNTSEL			( 1 << 29 )

#define GCTL				0xc110
	#define DSBLCLKGTNG				( 1 << 0 )
	#define HSFSLSPWRCLMP			( 1 << 1 )
	#define SSPWRCLMP				( 1 << 2 )
	#define DISSCRAMBLE				( 1 << 3 )
	#define SCALEDOWN_POS			4
	#define SCALEDOWN_MSK			( 3 << SCALEDOWN_POS )
	#define RAMCLKSEL_POS			6
	#define RAMCLKSEL_MSK			( 3 << RAMCLKSEL_POS )
	#define RAMCLKSEL_BUS			( 0 << RAMCLKSEL_POS )
	#define RAMCLKSEL_PIPE			( 1 << RAMCLKSEL_POS )
	#define RAMCLKSEL_PIPEBY2		( 2 << RAMCLKSEL_POS )
	#define DEBUGATTACH				( 1 << 8 )
	#define LPBKEN					( 1 << 9 )
	#define LOCALLPBKEN				( 1 << 10 )
	#define CORESOFTRESET			( 1 << 11 )
	#define PRTCAPDIR_POS			12
	#define PRTCAPDIR_MSK			( 3 << PRTCAPDIR_POS )
	#define PRTCAPDIR_HOST			( 1 << PRTCAPDIR_POS )
	#define PRTCAPDIR_DEVICE		( 2 << PRTCAPDIR_POS )
	#define PRTCAPDIR_OTG			( 3 << PRTCAPDIR_POS )
	
	
	#define FRMSCLDWN_POS			14
	#define FRMSCLDWN_MSK			( 3 << FRMSCLDWN_POS )
	#define U2RSTECN				( 1 << 16 )
	#define PWRDNSCALE_POS			19
	#define PWRDNSCALE_MSK			( 0xfff < PWRDNSCALE_POS )
	
	
#define GEVTEN				0xc114
	#define ULPICKEVTEN_I2CCKEVTEN		( 1 << 0 )
	#define I2CEVTEN					( 1 << 1 )
	
	
#define GSTS				0xc118
	#define CURMOD_POS			0
	#define CURMOD_MSK			( 3 << CURMOD_POS )
	#define CURMOD_DEVICE		( 0 << CURMOD_POS )
	#define CURMOD_HOST			( 1 << CURMOD_POS )
	#define CURMOD_DRD			( 2 << CURMOD_POS )
	#define BUSERRADDRVLD		( 1 << 4 )
	#define CSRTIMEOUT			( 1 << 5 )
	#define DEVICE_IP			( 1 << 6 )
	#define HOST_IP				( 1 << 7 )
	#define ADP_IP				( 1 << 8 )
	#define BC_IP				( 1 << 9 )
	#define OTG_IP				( 1 << 10 )
	#define CBELT_POS			20
	#define CBELT_MSK			( 0x7ff << CBELT_POS )
	
#define GSNPSID			0xc120
	#define SYNOPSYSID_REL_POS	0
	#define SYNOPSYSID_REL_MSK	( 0xffff << SYNOPSYSID_REL_POS )
	#define SYNOPSYSID_CORE_POS	16
	#define SYNOPSYSID_CORE_MSK	( 0xffff << SYNOPSYSID_CORE_POS )
	
	#define DWC3_REVISION_173A	0x5533173a
	#define DWC3_REVISION_175A	0x5533175a
	#define DWC3_REVISION_180A	0x5533180a
	#define DWC3_REVISION_183A	0x5533183a
	#define DWC3_REVISION_185A	0x5533185a
	#define DWC3_REVISION_188A	0x5533188a
	#define DWC3_REVISION_190A	0x5533190a

#define GGPIO			0xc124
#define GGUID			0xc128

#define GUCTL			0xc12c
	#define DTFT_POS				0	
	#define DTFT_MSK				( 0x1ff << DTFT_POS )
	#define DTCT_POS				9
	#define DTCT_MSK				( 3 << DTCT_POS )
	#define USBHSTINMAXBURST_POS	11
	#define USBHSTINMAXBURST_MSK	( 7 << USBHSTINMAXBURST_POS )
	#define USBHSTINAUTORETRYEN		( 1 << 14 )
	#define CMDEVADDR				( 1 << 15 )
	#define RESBWHSEPS				( 1 << 16 )
	#define SPRSCTRLTRANSEN			( 1 << 17 )

#define GBUSERRADDRLO	0xc130	
#define GBUSERRADDRHI	0xc134

#define GPRTBIMAPLO		0xc138
	#define BINUM1_POS		0
	#define BINUM1_MSK		( 0xf << BINUM1_POS )
	
	
// The hardware parameters contains some interesting core parameters that may
// be useful at runtime ( e.g. num endpoints supported )  They are not 
// fully listed here most of them will not be used at runtime... add macros as
// required
	
#define GHWPARAMS0 		0xc140
	#define MDWIDTH_POS					8
	#define MDWIDTH_MSK					( 0xff << MDWIDTH_POS )
	
#define	GHWPARAMS1		0xc144
#define GHWPARAMS2		0xc148
#define GHWPARAMS3		0xc14c
	#define DWC_USB3_NUM_EPS_POS		12 
	#define DWC_USB3_NUM_EPS_MSK		( 0x3f << DWC_USB3_NUM_EPS_POS )
	#define DWC_USB3_NUM_IN_EPS_POS		18
	#define DWC_USB3_NUM_IN_EPS_MSK		( 0x1f << DWC_USB3_NUM_IN_EPS_POS )	
#define GHWPARAMS4		0xc150
#define GHWPARAMS5		0xc154
#define GHWPARAMS6		0xc158
#define GHWPARAMS7		0xc15c
	#define RAM1_DEPTH_POS			0
	#define RAM1_DEPTH_MSK			( 0xffff << RAM1_DEPTH_POS )
	#define RAM2_DEPTH_POS			16
	#define RAM2_DEPTH_MSK			( 0xffff << RAM2_DEPTH_POS )
	
	
#define GHWPARAMS8		0xc600

#define GDBGFIFOSPACE	0xc160
	#define FIFOQUEUESELECT_PORTSEL_POS		0
	#define FIFOQUEUESELECT_PORTSEL_MSK		( 0xff << FIFOQUEUESELECT_PORTSEL_POS )
	#define SPACE_AVAILABLE_POS				16
	#define SPACE_AVAILABLE_MSK				( 0xffff << SPACE_AVAILABLE_POS )
	

#define GDBGLTSSM		0xc164
	#define TXONESZEROS				( 1 << 0 )
	#define RXTERMINATION			( 1 << 1 )
	#define TXSWING_					( 1 << 2 )
	#define TXMARGIN_POS_			3
	#define TXMARGIN_MSK_			( 7 << TXMARGIN_POS )
	#define TXDEEMPHASIS_POS_		6
	#define TXDEEMPHASIS_MSK_		( 3 << TXDEEMPHASIS_POS )
	#define RXEQTRAIN				( 1 << 8 )
	#define POWERDOWN_POS			9
	#define POWERDOWN_MSK			( 3 << POWERDOWN_POS )
	#define TXDETRXLOOPBACK			( 1 << 12 )
	#define RXPOLARITY				( 1 << 15 ) 
	#define TXELECIDLE				( 1 << 16 )
	#define ELASTICBUFFERMODE_		( 1 << 17 )
	#define LTDBSUBSTATE_POS		18
	#define LTDBSUBSTATE_MSK		(0xf << LTDBSUBSTATE_POS )
	#define LTDBLINKSTATE_POS		22
	#define LTDBLINKSTATE_MSK		( 0xff << LTDBLINKSTATE_POS )
	      #define LTDBLINKSTATE_ON                0
		  #define LTDBLINKSTATE_U1                1
		  #define LTDBLINKSTATE_SLEEP             2
		  #define LTDBLINKSTATE_SUSPEND   3
		  #define LTDBLINKSTATE_DISCONNECTED      4
		  #define LTDBLINKSTATE_EARLY_SUSP        5
		  #define LTDBLINKSTATE_INACT             6
		  #define LTDBLINKSTATE_POLL              7
		  #define LTDBLINKSTATE_RECOV             8
		  #define LTDBLINKSTATE_HRESET    9
		  #define LTDBLINKSTATE_CMPLY             0xa
		  #define LTDBLINKSTATE_LPBK              0xb
		  #define LTDBLINKSTATE_RESET             0xe
		  #define LTDBLINKSTATE_RESUME    0xf
	#define LTDBTIMEOUT				( 1 << 26 )
	
#define GDBGLSPMUX	0xc170
	#define LSPSEL0_POS				0
	#define LSPSEL0_MSK				( 0xf << LSPSEL0_MSK )
	#define LSPSEL1_POS				4
	#define LSPSEL1_MSK				( 0xf << LSPSEL1_MSK )
	#define LSPSEL2_POS				8
	#define LSPSEL2_MSK				( 0x3f << LSPSEL2_MSK )
	
#define GDBGLSP				0xc174
#define	GDBGEPINFO0			0xc178
#define GDBGEPINFO1			0xc17c

#define GPRTBIMAP_HSLO		0xc180
	#define BINUM1_POS				0
	#define BINUM1_MASK				( 0xf << BINUM1_POS )

#define GPRTBIMAP_HSHI		0xc184

#define GPRTBIMAP_FSLO		0xc180
	#define BINUM1F_POS				0
	#define BINUM1F_MASK				( 0xf << BINUM1_POS )

#define GPRTBIMAP_FSHI		0xc184

#define GUSB2PHYCFG			0xc200
	#define TOUTCAL_POS				0
	#define TOUTCAL_MSK				( 7 << TOUTCAL_POS )
	#define PHYIF					( 1 << 3 ) 
	#define ULPI_UTMI_SEL			( 1 << 4 )
	#define FSINTF					( 1 << 5 )
	#define SUSPHY					( 1 << 6 )
	#define PHYSEL					( 1 << 7 )
	#define ENBLSLPM				( 1 << 8 )
	#define USBTRDTIM_POS			10
	#define USBTRDTIM_MSK			( 0xf << USBTRDTIM_POS )
	#define PHYLPWRCLKSEL			( 1 << 14 )
	#define ULPIAUTORES				( 1 << 15 )
	#define ULPICLKSUSM				( 1 << 16 )
	#define ULPIEXTVBUSDRV			( 1 << 17 )
	#define ULPIEXTVBUSINDICATOR	( 1 << 18 )
	#define PHYINTRNUM_POS			19
	#define PHYINTRNUM_MSK			( 0x3f << PHYINTRNUM_POS )
	#define PHYSOFTRST				( 1 << 31 )
	

#define GUSB2PHYACC		0xc280
	#define REGDATA_POS			0
	#define REGDATA_MSK			( 0xff << REGDATA_POS )
	#define EXTREGADDR_POS		8
	#define EXTREGADDR_MSK		( 0x3f << EXTREGADDR_POS )
	#define REGADDR_POS			16
	#define REGADDR_MSK			( 0x3f << REGADDR_POS )
	#define REGWR				( 1 << 22 ) 
	#define VSTBSY				( 1 << 23 )
	#define VSTSDONE			( 1 << 24 )
	#define NEWREGREQ			( 1 << 25 )
	#define DISULPIDRVR			( 1 << 26 )

	
#define GUSB3PIPECTL		0xc2c0
	#define ELASTICBUFFERMODE		( 1 << 0 )
	#define TXDEEMPHASIS_POS		1
	#define TXDEEMPHASIS_MSK		( 0x3 << TXDEEMPHASIS_POS )
	#define TXMARGIN_POS			3
	#define TXMARGIN_MSK			( 0x3 << TXMARGIN_POS )
	#define TXSWING					( 1 << 6 )
	#define LFPSFILT				( 1 << 9 )
	#define P3P2TRANOK				( 1 << 11 )
	#define LFPSP0ALGN				( 1 << 12 )
	#define SKIPRXDET				( 1 << 13 )
	#define ABORTRXDETINU2			( 1 << 14 )
	#define DATWIDTH_POS			15
	#define DATWIDTH_MSK			( 3 << DATWIDTH_POS )
	#define SUSPENDENABLE			( 1 << 17 )
	#define DELAYP0TOP1P2P3			( 1 << 18 )
	#define P1P2P3DELAY_POS			19
	#define P1P2P3DELAY_MSK			( 7 << P1P2P3DELAY_POS )
	#define DISRXDETU3RXDET			( 1 << 22 )
	#define STARTRXDETU3RXDET		( 1 << 23 )
	#define REQUEST_P1P2P3			( 1 << 24 )
	#define PHYSOFTRST				( 1 << 31 )

#define FIFOMEM_BLK_SIZE			8

#define GTXFIFOSIZ(n)		(0xc300 + (4*n) )	// 0 <= n <= 15
	#define TXFDEP_POS				0
	#define TXFDEP_MSK				( 0xffff << TXFDE_POS )
	#define TXFSTADDR_POS			16
	#define TXFSTADDR_MSK			( 0xffff << TXFSTADDR_POS )
	
	
#define GRXFIFOSIZ(n)		(0xc380 + (4*n) )	// 0 <= n <= 2
	#define RXFDEP_POS				0
	#define RXFDEP_MSK				( 0xffff << RXFDE_POS )
	#define RXFSTADDR_POS			16
	#define RXFSTADDR_MSK			( 0xffff << RXFSTADDR_POS )
	
	
#define GEVNTADRLO			0xc400
#define GEVNTADRHI			0xc404
#define GEVNTSIZ			0xc408
	#define EVENTSIZ_POS			0
	#define EVENTSIZ_MSK			( 0xffff << EVENTSIZ_POS )
	#define EVNTINTRPTMASK			( 1 << 31 )
	
#define GEVNTCOUNT			0xc40c
	#define EVNTCOUNT_POS			0
	#define EVNTCOUNT_MSK			( 0xffff << EVNTCOUNT_POS )
	
	
// device controller registers

#define DCFG				0xc700
	#define DEVSPD_POS				0
	#define DEVSPD_MSK				( 7 << DEVSPD_POS )
	#define DEVSPD_HIGH				( 0 << DEVSPD_POS )
	#define DEVSPD_FULL				( 1 << DEVSPD_POS )
	#define DEVSPD_SUPER			( 4 << DEVSPD_POS )
	#define DEVADDR_POS				3
	#define DEVADDR_MSK				( 0x7f << DEVADDR_POS )
	#define PERFRINT_POS			10
	#define PERFRINT_MSK			( 3 << PERFRINT_POS )
	#define PERFRINT_80				( 0 << PERFRINT_POS )
	#define PERFRINT_85				( 1 << PERFRINT_POS )
	#define PERFRINT_90				( 2 << PERFRINT_POS )
	#define PERFRINT_95				( 3 << PERFRINT_POS )
	#define INTRNUM_POS				12
	#define INTRNUM_MSK				( 0x1f << INTRNUM_POS )
	#define NUMP_POS				17
	#define NUMP_MSK				( 0x1f << NUMP_POS )
	#define NUMP_DEFAULT			( 4 << NUMP_POS )
	#define LPMCAP					( 1 << 22 )
	#define IGNORESTREAMPP			( 1 << 23 )


#define DCTL				0xc704
	#define TSTCTL_POS				1
	#define TSTCTL_MSK				( 7 << TSTCTL_POS )
	#define TSTCTL_DISABLE			( 0 << TSTCTL_POS )
	#define TSTCTL_J				( 1 << TSTCTL_POS )
	#define TSTCTL_K				( 2 << TSTCTL_POS )
	#define TSTCTL_SE0_NAK			( 3 << TSTCTL_POS )
	#define TSTCTL_TESTPKT			( 4 << TSTCTL_POS )
	#define TSTCTL_FORCE_ENABLE		( 5 << TSTCTL_POS )
	#define ULSTCHNGREQ_POS			5
	#define ULSTCHNGREQ_MSK			( 0xf << ULSTCHNGREQ_POS ) 	
	#define ULSTCHNGREQ_NO_ACTION	( 0x0 << ULSTCHNGREQ_POS ) 	
	#define ULSTCHNGREQ_SS_DISABLE	( 0x4 << ULSTCHNGREQ_POS ) 	
	#define ULSTCHNGREQ_SSRX_DET	( 0x5 << ULSTCHNGREQ_POS ) 	
	#define ULSTCHNGREQ_SSINACTIVE	( 0x6 << ULSTCHNGREQ_POS ) 	
	#define ULSTCHNGREQ_SSREOV		( 0x8 << ULSTCHNGREQ_POS ) 	
	#define ULSTCHNGREQ_SS_COMPLIANCE			( 0xa << ULSTCHNGREQ_POS ) 	
	#define ULSTCHNGREQ_SS_LOOPBACK	( 0xb << ULSTCHNGREQ_POS ) 	
	#define LINK_PM_POS				( 9 )
	#define LINK_PM_MASK			( 0xf << LINK_PM_POS )
	#define ACCEPTU1ENA				( 1 << 9 )
	#define INITU1ENA				( 1 << 10 )
	#define ACCEPTU2ENA				( 1 << 11 )
	#define INITU2ENA				( 1 << 12 )
	#define TRGTULST_POS			17
	#define TRGTULST_MSK			( 0xf << TRGTULST_POS )
	#define TRGTULST_U2				( 0x2 << TRGTULST_POS )
	#define TRGTULST_U3				( 0x3 << TRGTULST_POS )
	#define TRGTULST_SS_DIS			( 0x4 << TRGTULST_POS )
	#define TRGTULST_RX_DET			( 0x5 << TRGTULST_POS )
	#define TRGTULST_SS_INACT		( 0x6 << TRGTULST_POS )
	#define APPL1RES				( 1 << 23 )
	#define HIRDTHRES_TIME_POS		24
	#define HIRDTHRES_TIME_MSK		( 0xf << HIRDTHRES_TIME_POS )
	#define HIRDTHRES				( 1 << 28 )
	#define LSFTRST					( 1 << 29 )
	#define CSFTRST					( 1 << 30 )
	#define RUNSTOP					( 1 << 31 )
	
	
#define DEVTEN			0xc708
	#define DISCONNEVTEN			( 1 << 0 )
	#define USBRSTEN				( 1 << 1 )
	#define CONNECTDONEEN			( 1 << 2 )
	#define ULSTCNGEN				( 1 << 3 )
	#define WKUPEVTEN				( 1 << 4 )
	#define EOPFEN					( 1 << 6 )
	#define SOFEN					( 1 << 7 )
	#define ERRTICERREN				( 1 << 9 )
	#define CMDCMPLTEN				( 1 << 10 )
	#define EVNTOVERFLOWEN			( 1 << 11 )
	#define VNDRDEVTSTRCVEDEN		( 1 << 12 )
	#define INACTTIMEOUTRCVEDEN		( 1 << 13 )
	
#define DSTS			0xc70c
	#define CONNECTSPD_POS			0
	#define CONNECTSPD_MSK			( 7 << CONNECTSPD_POS )
	#define CONNECTSPD_HIGH			( 0 << CONNECTSPD_POS )
	#define CONNECTSPD_FULL1		( 1 << CONNECTSPD_POS )
	#define CONNECTSPD_LOW			( 2 << CONNECTSPD_POS )
	#define CONNECTSPD_FULL2		( 3 << CONNECTSPD_POS )
	#define CONNECTSPD_SUPER		( 4 << CONNECTSPD_POS )
	#define SOFFN_POS				3
	#define SOFFN_MSK				( 0x3fff << SOFFN_POS )
	#define RXFIFOEMPTY				( 1 << 17 )
	#define USBLNKST_POS			18
	#define USBLNKST_MSK			( 0xf << USBLNKST_POS )
	#define DEVCTRLHLT				( 1 << 22 )
	#define COREIDLE				( 1 << 23 )
	#define PWRUPREQ				( 1 << 24 )
	
#define DGCMDPAR		0xc710
#define DGCMD			0xc714
	#define CMDTYP_POS				0
	#define CMDTYP_MSK				( 0xff <<  CMDTYP_POS )
	#define CMDTYP_EP_NRDY			( 0xc <<  CMDTYP_POS )
	#define CMDTYP_TRANS_WKUP		( 0x3 <<  CMDTYP_POS )
	#define CMDTYP_ALLFIFO_FLUSH	( 0xa <<  CMDTYP_POS )
	#define CMDTYP_SETPERI_PARAM	( 0x2 <<  CMDTYP_POS )
	#define CMDTYP_TX_LPM			( 0x1 <<  CMDTYP_POS )
	#define CMDTYP_SELECT_FLUSH		( 0x9 <<  CMDTYP_POS )
	#define CMDTYP_SOC_LOOPBACK		( 0x10 <<  CMDTYP_POS )
	#define CMDIOC					( 1 << 8 )	
	#define CMDACT					( 1 << 10 )
	#define CMDSTATUS				( 1 << 15 )

#define DALEPENA		0xc720
	#define EPIDX( n, d )			( 2*(n) + ((d)?1:0) )
	#define EPMSK( idx )            ( 1 << (idx) )	
	#define EPNUM( idx )			( (idx) >> 1 )
	#define EPDIR( idx )			( ( (idx) & 1) ? "IN" : "OUT" )
	
	
#define DEPCMDPAR2(n)				( 0xc800 + ( 0x10 * ( n ) ) )		// 0 <= n <= 31
#define DEPCMDPAR1(n)				( 0xc804 + ( 0x10 * ( n ) ) )		// 0 <= n <= 31
#define DEPCMDPAR0(n)				( 0xc808 + ( 0x10 * ( n ) ) )		// 0 <= n <= 31
#define DEPCMD(n)					( 0xc80C + ( 0x10 * ( n ) ) )		// 0 <= n <= 31
	#define EPCMDTYP_POS			0
	#define EPCMDTYP_MSK			( 0xf << EPCMDTYP_POS )
	#define EPCMDTYP_DEPCFG			( 0x1 << EPCMDTYP_POS )
	#define EPCMDTYP_DEPXFERCFG		( 0x2 << EPCMDTYP_POS )
	#define EPCMDTYP_DEPGETDSEQ		( 0x3 << EPCMDTYP_POS )
	#define EPCMDTYP_DEPSETSTALL	( 0x4 << EPCMDTYP_POS )
	#define EPCMDTYP_DEPCSTALL		( 0x5 << EPCMDTYP_POS )
	#define EPCMDTYP_DEPSTRTXFER	( 0x6 << EPCMDTYP_POS )
	#define EPCMDTYP_DEPUPDXFER		( 0x7 << EPCMDTYP_POS )	
	#define EPCMDTYP_DEPENDXFER		( 0x8 << EPCMDTYP_POS )
	#define EPCMDTYP_DEPSTARTCFG	( 0x9 << EPCMDTYP_POS )
	#define EPCMDIOC				( 1 << 8 )
	#define EPCMDACT				( 1 << 10 )
	#define EPHIPRI_FORCERM			( 1 << 11 )
	#define EPCMDSTATUS_POS			12
	#define EPCMDSTATUS_MSK			( 0xf << EPCMDSTATUS_POS )
	#define EPCMDPARAM_EVTPARAM_POS		16
	#define EPCMDPARAM_EVTPARAM_MSK		( 0xffff < EPCMDPARAM_EVTPARAM_POS )
	#define EPCMDPARAM_XFER_RSC_IDX_POS	16
	#define EPCMDPARAM_XFER_RSC_IDX_MSK	 ( 0x7f << EPCMDPARAM_XFER_RSC_IDX_POS )
	


// Command Parameters

// DEPCFG
//Parameter2: Reserved
//Parameter1:
#define EPCMD_PRM_INTNUM_POS			0
#define EPCMD_PRM_INTNUM_MSK			( 0x1f << EPCMD_PRM_INTNUM_POS )
#define EPCMD_PRM_XferCmplEn			( 1 << 8 )
#define EPCMD_PRM_XferInProgEn			( 1 << 9 )
#define EPCMD_PRM_XferNRdyEn			( 1 << 10 )
#define EPCMD_PRM_RxTxfifoEvtEn 		( 1 << 11 )
#define EPCMD_PRM_StreamEvtEn   		( 1 << 12 )
#define EPCMD_PRM_bInterval_m1_POS		16
#define EPCMD_PRM__bInterval_m1_MSK		( 0xff << EPCMD_PRM_bInterval_m1_POS )
#define EPCMD_PRM_StrmCap				( 1 << 24 )
#define EPCMD_PRM_EPNUM_POS				26
#define EPCMD_PRM_EPNUM_MSK				( 0xf << EPCMD_PRM_EPNUM_POS )
#define EPCMD_PRM_EPDIR_IN				( 1 << 25 )
#define EPCMD_PRM_EPDIR_OUT				( 0 << 25 )
#define EPCMD_PRM_BULK_BASED			( 1 << 30 )
#define EPCMD_PRM_FIFO_BASED			( 1 << 31 )
//Parameter0:
#define EPCMD_PRM_EPType_POS			1
#define EPCMD_PRM_EPType_MSK			( 3 << EPCMD_PRM_EPType_POS )
#define EPCMD_PRM_MPS_POS				3
#define EPCMD_PRM_MPS_MSK				( 0x7ff << EPCMD_PRM_MPS_POS )
#define EPCMD_PRM_FIFONUM_POS			17
#define EPCMD_PRM_FIFONUM_MSK			( 0x1f << EPCMD_PRM_FIFONUM_POS )
#define EPCMD_PRM_BURST_SZ_POS			22
#define EPCMD_PRM_BURST_SZ_MSK			( 0xf << EPCMD_PRM_BURST_SZ_POS )
#define EPCMD_PRM_SEQN_POS				26
#define EPCMD_PRM_SEQN_MSK				( 0x1f << EPCMD_PRM_SEQN_POS )
#define EPCMD_PRM_SEQN_IGNORE			( 1 << 31 )


// DEPXFERCFG
//Parameter2: Reserved
//Parameter1: Reserved
//Parameter0: 
#define EPCMD_PRM_NumXferRes_POS		0
#define EPCMD_PRM_NumXferRes_MSK		( 0xffff << EPCMD_PRM_NumXferRes_POS )






	

// otg regs
	
#define OCFG		0xcc00
	#define SRPCAP			( 1 << 0 )
	#define HNPCAP			( 1 << 1 )
	#define OTGVERSION		( 1 << 2 )
	
#define OCTL		0xcc04
	#define HSTSETHNPEN		( 1 << 0 )
	#define DEVSETHNPEN		( 1 << 1 )
	#define TERMSELDLPULSE	( 1 << 2 )
	#define SESREQ			( 1 << 3 )
	#define HNPREQ			( 1 << 4 )
	#define PRTPWRCTL		( 1 << 5 )
	#define PERIMODE		( 1 << 6 )

#define OEVT		0xcc08
	#define OEVTERROR				( 1 << 0 )
	#define SESREQSTS				( 1 << 1 )
	#define HSTNEGSTS				( 1 << 2 )
	#define BSESVLD					( 1 << 3 )
	#define OTGBDEVVBUSCHNGEVNT		( 1 << 8 )
	#define OTGBDEVSESSVLDDETEVNT	( 1 << 9 )
	#define OTGBDEVHNPCHNGEVNT		( 1 << 10 )
	#define OTGBDEVHOSTENDEVNT		( 1 << 11 )
	#define OTGADEVSESSENDDETEVNT	( 1 << 16 )
	#define OTGADEVSRPDETEVNT		( 1 << 17 )
	#define OTGADEVHNPCHNGDETEVNT	( 1 << 18 )
	#define OTGADEVHOSTEVNT			( 1 << 19 )
	#define OTGADEVBHOSTENDEVNT		( 1 << 20 )
	#define OTGCONIDSTSCHNGEVNT		( 1 << 24 )
	#define DEVICEMODE				( 1 << 31 )
	#define DEVICEMODE_A			( 0 << 31 )
	#define DEVICEMODE_B			( 1 << 31 )
	
#define OEVTEN		0xcc0c
	#define OTGBDEVVBUSCHNGEVNTEN		( 1 << 8 )
	#define OTGBDEVSESSVLDDETEVNTEN		( 1 << 9 )
	#define OTGBDEVHNPCHNGEVNTEN		( 1 << 10 )
	#define OTGBDEVHOSTENDEVNTEN		( 1 << 11 )
	
	#define OTGADEVSESSENDDETEVNTEN		( 1 << 16 )
	#define OTGADEVSRPDETEVNTEN			( 1 << 17 )
	#define OTGADEVHNPCHNGDETEVNTEN		( 1 << 18 )
	#define OTGADEVHOSTEVNTEN			( 1 << 19 )
	#define OTGADEVBHOSTENDEVNTEN		( 1 << 20 )
	#define OTGCONIDSTSCHNGEVNTEN		( 1 << 24 )
	
#define OSTS		0xcc10
	#define CONIDSTS_A					( 0 << 0 )
	#define CONIDSTS_B					( 1 << 0 )
	#define VBUSVLD						( 1 << 1 )	
	#define BSESVLD_					( 1 << 2 )
	#define XHCIPRTPOWER				( 1 << 3 )
	#define PERIPHERALSTATE				( 1 << 4 )
	#define OTGSTATE_POS				8
	#define OTGSTATE_MSK				( 0xf << OTGSTATE_POS )
	
	
	
	
// TRB Data structure

typedef struct _trb {
	uint32_t bufptr_low;
	uint32_t bufptr_high;
	uint32_t bufsz_sts;
	uint32_t control;
} trb_t;

typedef struct _trb_cluster {
	trb_t    *trb_arr;
	off64_t  trb_base_paddr64;
	int      n_trb_in_cluster;
} trb_cluster_t;


// TRB Control - Word 3
#define TRB_CTL_HWO						( 1 << 0 )
#define TRB_CTL_LST						( 1 << 1 )
#define TRB_CTL_CHN						( 1 << 2 )
#define TRB_CTL_CSP						( 1 << 3 )
#define TRB_CTL_TRBCTL_POS				4
#define TRB_CTL_TRBCTL_MSK				( 0x3f << TRB_CTL_TRBCTL_POS )
	#define TRBCTL_NORMAL					( 0x1 << TRB_CTL_TRBCTL_POS )
	#define TRBCTL_CONTROL_SETUP			( 0x2 << TRB_CTL_TRBCTL_POS )
	#define TRBCTL_CONTROL_STATUS2			( 0x3 << TRB_CTL_TRBCTL_POS )
	#define TRBCTL_CONTROL_STATUS3			( 0x4 << TRB_CTL_TRBCTL_POS )
	#define TRBCTL_CONTROL_DATA				( 0x5 << TRB_CTL_TRBCTL_POS )
	#define TRBCTL_ISOCH_FIRST				( 0x6 << TRB_CTL_TRBCTL_POS )
	#define TRBCTL_ISOCH					( 0x7 << TRB_CTL_TRBCTL_POS )
	#define TRBCTL_LINK_TRB					( 0x8 << TRB_CTL_TRBCTL_POS )
#define TRB_CTL_ISP_IMI					( 1 << 10 )		
#define TRB_CTL_IOC						( 1 << 11 )
#define TRB_CTL_SID_SOF_POS				14
#define TRB_CTL_DIS_SOF_MSK				( 0xffff << TRB_CTL_SID_SOF_POS )

// TRB bufsize and status - Word 2
#define TRB_BUFSZ_POS					0
#define TRB_BUFSZ_MSK					( 0xffffff << TRB_BUFSZ_POS )

#define TRB_PCM1_POS					24
#define TRB_PCM1_MSK					( 23 << TRB_PCM1_POS )

#define TRB_STS_POS						28
#define TRB_STS_MSK						( 0xf << TRB_STS_POS )
	#define TRB_STS_OK						( 0 << TRB_STS_POS )
	#define TRB_STS_MISSED_ISOCH			( 1 << TRB_STS_POS )
	#define TRB_STS_SETUP_PENDING			( 2 << TRB_STS_POS )



// Events


typedef enum {
	EVT_CLASS_DEVICE,
	EVT_CLASS_EP,
	EVT_ULPI_CARKIT,
	EVT_I2C_CARKIT,
	EVT_I2C,
	EVT_UNKNOWN,	
} evt_class_t;

typedef enum {
	/* Endpoint Events */
	XferComplete,
	XferInProgress,
	XferNotReady,
	RxTxfifoEvt,
	StreamEvt,
	EPCmdCmplt,

	/* Device Events */
	DisconnEvt,
	USBRst,
	ConnectDone,
	ULStChng,
	WkUpEvt,
	EOPF,
	Sof,
	ErrticErr,
	CmdCmplt,
	EvntOverflow,
	VndrDevTstRcved,
	
	/* Unknown */
	UnknownEvent
} evt_type_t;

static const char* const event_str_arr[] = {
	// Endpoint Events
	[XferComplete] = "EVT_CLASS_EP:XferComplete",
	[XferInProgress] = "EVT_CLASS_EP:XferInProgress",
	[XferNotReady] = "EVT_CLASS_EP:XferNotReady",
	[RxTxfifoEvt] = "EVT_CLASS_EP:RxTxfifoEvt",
	[StreamEvt] = "EVT_CLASS_EP:StreamEvt",
	[EPCmdCmplt] = "EVT_CLASS_EP:EPCmdCmplt",
	
	// Device Events
	[DisconnEvt] = "EVT_CLASS_DEVICE:DisconnEvt",
	[USBRst] = "EVT_CLASS_DEVICE:USBRst",
	[ConnectDone] = "EVT_CLASS_DEVICE:ConnectDone",
	[ULStChng] = "EVT_CLASS_DEVICE:ULStChng",
	[WkUpEvt] = "EVT_CLASS_DEVICE:WkUpEvt",
	[EOPF] = "EVT_CLASS_DEVICE:EOPF",
	[Sof] = "EVT_CLASS_DEVICE:Sof",
	[ErrticErr] = "EVT_CLASS_DEVICE:ErrticErr",
	[CmdCmplt] = "EVT_CLASS_DEVICE:CmdCmplt",
	[EvntOverflow] = "EVT_CLASS_DEVICE:EvntOverflow",
	[VndrDevTstRcved] = "EVT_CLASS_DEVICE:VndrDevTstRcved",
	
	// Other Events
	
	[UnknownEvent] = "UnknownEvent"
};


typedef struct _evt {
	evt_class_t		evt_class;
	evt_type_t		evt_type;
	uint32_t		epidx;		// hardware endpoint associated with EP event
	uint32_t		raw_event;
} evt_t;

// Event Macros used for Decode

#define EVT_TYPE_MSK			( 1 << 0 )
#define EVT_TYPE_EP				( 0 << 0 )
#define EVT_TYPE_NON_EP			( 1 << 0 )

// Endpoint Event Macros
#define EVT_EP_NUM_POS				1
#define EVT_EP_NUM_MSK				( 0x1f << EVT_EP_NUM_POS )

#define EVT_EP_TYPE_POS				6
#define EVT_EP_TYPE_MSK				( 0xf << EVT_EP_TYPE_POS )
#define EVT_EP_XferComplete			( 0x1 << EVT_EP_TYPE_POS )
#define EVT_EP_XferInProgress		( 0x2 << EVT_EP_TYPE_POS )
#define EVT_EP_XferNotReady			( 0x3 << EVT_EP_TYPE_POS )
#define EVT_EP_RxTxfifoEvt			( 0x4 << EVT_EP_TYPE_POS )
#define EVT_EP_StreamEvt			( 0x6 << EVT_EP_TYPE_POS )
#define EVT_EP_EPCmdCmplt			( 0x7 << EVT_EP_TYPE_POS )

// Endpoint Event Status:  XferNotReady 	
#define EVT_EPSTS_POS				12
#define EVT_EPSTS_MSK				( 0xf << EVT_EPSTS_STREAM_POS )

#define EVT_EPSTS_XferNotActive		( 0 << 15 )
#define EVT_EPSTS_XferActive		( 1 << 15 )
#define EVT_EPSTS_STAGE_POS			12
#define EVT_EPSTS_STAGE_MSK			( 3 << EVT_EPSTS_STAGE_POS )
#define EVT_EPSTS_STAGE_SETUP		( 0 << EVT_EPSTS_STAGE_POS )
#define EVT_EPSTS_STAGE_DATA		( 1 << EVT_EPSTS_STAGE_POS )
#define EVT_EPSTS_STAGE_STATUS		( 2 << EVT_EPSTS_STAGE_POS )

// Endpoint Event Status:  XferComplete or XferInProgress 	
#define EVT_EPSTS_LST				( 1 << 15 )
#define EVT_EPSTS_MISSED_ISOCH		( 1 << 15 )
#define EVT_EPSTS_IOC				( 1 << 14 )
#define EVT_EPSTS_SHORTPKT_RX		( 1 << 13 )
#define EVT_EPSTS_BUS_ERROR			( 1 << 12 )

// Withing Stream Event
#define EVT_EPSTS_STREAM_NOT_FOUND	( 0x2 << EVT_EPSTS_POS )
#define EVT_EPSTS_STREAM_FOUND		( 0x1 << EVT_EPSTS_POS )

// Endpoint Event Status:  In response to Start Transfer or Set Transfer Resource
#define EVT_EPSTS_STREAM_BUSTIME_EXP	( 0x2 << EVT_EPSTS_POS )
#define EVT_EPSTS_STREAM_NO_RESOURCE	( 0x1 << EVT_EPSTS_POS )

// Endpoint Event Parameters
#define EVT_EPPARAM_POS				 16
#define EVT_EPPARAM_MSK				 ( 0xffff << EVT_EPPARAM_POS )
#define EVT_EPPARAM_STREAM_POS		 EVT_EPPARAM_POS
#define EVT_EPPARAM_STREAM_MSK		 EVT_EPPARAM_MSK
#define EVT_EPPARAM_UFRAME_POS		 EVT_EPPARAM_POS
#define EVT_EPPARAM_XFER_RSC_IDX_POS 16
#define EVT_EPPARAM_XFER_RSC_IDX_MSK ( 0x3f << EVT_EPPARAM_XFER_RSC_IDX_POS )

#define EVT_EPPARAM_UFRAME_MSK		EVT_EPPARAM_MSK
#define EVT_EPPARAM_CMDTYPE_POS		24
#define EVT_EPPARAM_CMDTYPE_MSK		( 0xf << EVT_EPPARAM_CMDTYPE_POS )


// Non EP Events

#define EVT_TYPE2_POS			1
#define EVT_TYPE2_MASK			( 0x7f << EVT_TYPE2_POS )
#define EVT_TYPE2_DEVICE		( 0 << EVT_TYPE2_POS )
#define EVT_TYPE2_ULPI_CARKIT	( 3 << EVT_TYPE2_POS )
#define EVT_TYPE2_I2C			( 4 << EVT_TYPE2_POS )

// Device Events
#define EVT_DEV_POS					8
#define EVT_DEV_MSK					( 0xf << EVT_DEV_POS )
#define EVT_DEV_DisconnEvt			( 0x0 << EVT_DEV_POS )
#define EVT_DEV_USBRst				( 0x1 << EVT_DEV_POS )
#define EVT_DEV_ConnectDone			( 0x2 << EVT_DEV_POS )
#define EVT_DEV_ULStChng			( 0x3 << EVT_DEV_POS )
#define EVT_DEV_WkUpEvt				( 0x4 << EVT_DEV_POS )
#define EVT_DEV_EOPF				( 0x6 << EVT_DEV_POS )
#define EVT_DEV_Sof					( 0x7 << EVT_DEV_POS )
#define EVT_DEV_ErrticErr			( 0x9 << EVT_DEV_POS )
#define EVT_DEV_CmdCmplt			( 0xa << EVT_DEV_POS )
#define EVT_DEV_EvntOverflow		( 0xb << EVT_DEV_POS )
#define EVT_DEV_VndrDevTstRcved		( 0xc << EVT_DEV_POS )

// EventInfo for Device Events
#define EVT_DEVINFO_POS				16
#define EVT_DEVINFO_MSK				( 0xff << EVT_DEVINFO_POS )
#define EVT_DEVINFO_SSEVT			( 1 << ( EVT_DEVINFO_POS + 4 ) )
#define EVT_DEVINFO_NON_SSEVT		( 0 << ( EVT_DEVINFO_POS + 4 ) )
#define EVT_DEVINFO_LINKSTATE_POS	EVT_DEVINFO_POS
#define EVT_DEVINFO_LINKSTATE_MSK	( 0xf << EVT_DEVINFO_POS )


/* driver type definitions */

typedef struct _dctrl_t   dctrl_t; 
typedef struct _ep_ctx    ep_ctx_t;
typedef struct _cmd_t     cmd_t;

struct _cmd_t {
	uint32_t    cmd;
	uint32_t    p0;
	uint32_t    p1;
	uint32_t    p2;
	
	// in blocking mode, the result_event is delivered
	uint32_t result_event;
	
	// in non-blocking mode, the transfer resource index is returned
	uint32_t                 xfer_rsc_idx;	
};

/* driver endpoint context */

// The IDLE state is very broad ( p122 of dwcotg ref manual), so break it down into substates 

typedef enum { 	CONTROL_PHASE_SETUP,
				CONTROL_PHASE_DATA, 
				CONTROL_PHASE_STATUS } control_phase_t; 


#define EPFLAG_ENABLED					( 1 << 0 )
#define EPFLAG_XFER_ACTIVER				( 1 << 1 )
#define EPFLAG_STALLED                  ( 1 << 2 )
#define EPFLAG_XFER_NOT_READY            ( 1 << 3 )

struct _ep_ctx {
	uint32_t				flags;
	uint32_t				num;
	uint32_t				dir;
	uint32_t				idx;
	uint32_t				mps;
	uint32_t				type;	// control, bulk, isoch, or int
	uint32_t                fifonum;
	unsigned				blknum;
	uint32_t                burstsz;
	iousb_endpoint_t		*iousb_ep;
	
	// data transfer vars
	iousb_transfer_t		*urb;			
	uint32_t				xfer_flags;		
	uint32_t				req_xfer_len;
	uint32_t				bytes_xfered;	
	
	trb_cluster_t          *trb_cluster;

	// Control Endpoint specific... not used by other endpoint types
	uint8_t					*setup_packet;
	off64_t					setup_packet_paddr64;
	int						setup_packet_delay;
	
	control_phase_t         control_phase;
	
	// used to store the command complete event data for the endpoint
	uint32_t                cmd_complete_event;
	
	// resource index for the active transfer
	uint32_t                 xfer_rsc_idx;

};

/* SoC-specific MUSBMHDRC configurations */

#define DC_FLAG_CONNECTED         ( 1 << 0 )
#define DC_FLAG_SOFT_CONNECT      ( 1 << 1 )
#define DC_FLAG_NO_LINK_PM        ( 1 << 2 )

typedef enum {
    USBPHY_PARAMETER_OVERRIDE_A,
    USBPHY_PARAMETER_OVERRIDE_B,
    USBPHY_PARAMETER_OVERRIDE_C,
    USBPHY_PARAMETER_OVERRIDE_D,
    USBPHY_MAX_REG
} phy_tuning_t;

/* device controller context */
struct _dctrl_t {
	uint32_t				flags;
	usbdc_device_t			*udc;   
	char					*serial_string;    
	uint8_t					*IoBase;    
	int						verbosity;
	uint32_t				maxspeed;
	uint32_t				link_pm_mask;
	pthread_mutex_t			mutex;
	
	void                    *fifomem_hdl;
	
	ep_ctx_t				ep_arr[MAX_N_ENDPOINTS]; 
	char                    ep_resalloc_arr[MAX_N_ENDPOINTS];
	int                     n_ep;	
	
	uint32_t				*eventq_mem;
	uint32_t				eventq_cur_elem;
	
	void					*xdc;	// soc specific device controller context

	// delayed execution handler members
	int						chid;
	int						coid;
	int						tid;

	uint32_t				phy_tuning[USBPHY_MAX_REG];
};



/* Regsiters Access Macros */


static inline uint32_t HW_Read32( uint8_t * iobase, uint32_t offset ) {
	uint32_t data;
	
	data = *((volatile uint32_t*)(iobase + offset));
	
	return data;
}

static inline void HW_Write32( uint8_t * iobase, uint32_t offset, uint32_t data ) {
	*((volatile uint32_t*)(iobase + offset)) = data;
}

static inline void HW_Write32Or( uint8_t * iobase, uint32_t offset, uint32_t data ) {
	data = HW_Read32( iobase, offset ) | data;
	HW_Write32( iobase, offset, data);
}

static inline void HW_Write32And( uint8_t * iobase, uint32_t offset, uint32_t data ) {
	data = HW_Read32( iobase, offset ) & data;
	HW_Write32( iobase, offset, data);
}

static inline uint16_t HW_Read16( uint8_t * iobase, uint32_t offset ) {
	uint16_t data;
	data = *((volatile uint16_t*)(iobase + offset));
	return data;
}

static inline void HW_Write16( uint8_t * iobase, uint32_t offset, uint16_t data ) {
	*((volatile uint16_t*)(iobase + offset)) = data;
}

static inline void HW_Write16Or( uint8_t * iobase, uint32_t offset, uint16_t data ) {
	data = HW_Read16( iobase, offset ) | data;
	HW_Write16( iobase, offset, data);
}

static inline void HW_Write16And( uint8_t * iobase, uint32_t offset, uint16_t data ) {
	data = HW_Read16( iobase, offset ) & data;
	HW_Write16( iobase, offset, data);
}

static inline uint8_t HW_Read8( uint8_t * iobase, uint32_t offset ) {
	uint8_t data;
	data = *((volatile uint8_t*)(iobase + offset));
	return data;
}

static inline void HW_Write8( uint8_t * iobase, uint32_t offset, uint8_t data ) {
	*((volatile uint8_t*)(iobase + offset)) = data;
}

static inline void HW_Write8Or( uint8_t * iobase, uint32_t offset, uint8_t data ) {
	data = HW_Read8( iobase, offset ) | data;
	HW_Write8( iobase, offset, data);
}

static inline void HW_Write8And( uint8_t * iobase, uint32_t offset, uint8_t data ) {
	data = HW_Read8( iobase, offset ) & data;
	HW_Write8( iobase, offset, data);
}




/* function prototypes */

uint32_t dwcotg_init( usbdc_device_t *udc, io_usbdc_self_t *udc_self, char *args);
uint32_t dwcotg_start( usbdc_device_t *udc );
uint32_t dwcotg_stop( usbdc_device_t *udc );
uint32_t dwcotg_shutdown( usbdc_device_t *udc );
uint32_t dwcotg_set_bus_state( usbdc_device_t *udc, uint32_t device_state );
uint32_t dwcotg_set_device_feature( usbdc_device_t *udc, uint32_t feature, uint16_t wIndex );
uint32_t dwcotg_clear_device_feature( usbdc_device_t *udc, uint32_t feature );
uint32_t dwcotg_set_sel( usbdc_device_t *udc, uint8_t u1sel, uint8_t u1pel, uint16_t u2sel, uint16_t u2pel );

uint32_t dwcotg_set_device_address(  usbdc_device_t *udc, uint32_t device_address );
uint32_t dwcotg_select_configuration( usbdc_device_t *udc, uint8_t config );
uint32_t dwcotg_get_descriptor( usbdc_device_t *udc, uint8_t type, uint8_t index, uint16_t lang_id, uint8_t **ddesc, uint32_t speed );
uint32_t dwcotg_get_device_descriptor( usbdc_device_t *udc, uint8_t **ddesc, uint32_t speed );
uint32_t dwcotg_get_config_descriptor( usbdc_device_t *udc, uint8_t **cdesc, uint8_t config_num, uint32_t speed );
uint32_t dwcotg_get_string_descriptor( usbdc_device_t *udc, uint8_t **sdesc, uint8_t index , uint32_t speed);

uint32_t dwcotg_set_endpoint_state( usbdc_device_t *udc, iousb_endpoint_t *ep, uint32_t ep_state );
uint32_t dwcotg_clear_endpoint_state( usbdc_device_t *udc, iousb_endpoint_t *ep, uint32_t ep_state );
uint32_t dwcotg_interrupt( usbdc_device_t *udc );

uint32_t dwcotg_control_endpoint_enable( void *chdl, iousb_device_t *device, iousb_endpoint_t *ep );
uint32_t dwcotg_control_endpoint_disable( void *chdl, iousb_endpoint_t *ep );
uint32_t dwcotg_control_transfer_abort( void *chdl, iousb_transfer_t *urb, iousb_endpoint_t *ED );
uint32_t dwcotg_control_transfer( void *chdl, iousb_transfer_t *urb, iousb_endpoint_t *endp, uint8_t *buffer, _uint32 length, _uint32 flags );

uint32_t dwcotg_endpoint_enable( void *chdl, iousb_device_t *device, iousb_endpoint_t *ep );
uint32_t dwcotg_endpoint_disable( void *chdl, iousb_endpoint_t *ep );
uint32_t dwcotg_transfer_abort( void *chdl, iousb_transfer_t *urb, iousb_endpoint_t *ED );
uint32_t dwcotg_transfer( void *chdl, iousb_transfer_t *urb, iousb_endpoint_t *endp, uint8_t *buffer, _uint32 length, _uint32 flags );

uint32_t dwcotg_isoch_endpoint_enable( void *chdl, iousb_device_t *device, iousb_endpoint_t *ep );
uint32_t dwcotg_isoch_endpoint_disable( void *chdl, iousb_endpoint_t *ep );
uint32_t dwcotg_isoch_transfer_abort( void *chdl, iousb_transfer_t *urb, iousb_endpoint_t *ED );
uint32_t dwcotg_isoch_transfer( void *chdl, iousb_transfer_t *urb, iousb_endpoint_t *endp, uint8_t *buffer, _uint32 length, _uint32 flags );

int dwcotg_slogf( dctrl_t * dc, int level, const char *fmt, ...);
int dwcotg_xfer_start( dctrl_t * dc, trb_cluster_t * trb_cluster, ep_ctx_t * ep, uint32_t epidx );
int dwcotg_xfer_abort( dctrl_t * dc, ep_ctx_t *ep, int epidx );
int dwcotg_setup_packet_get( dctrl_t * dc );

/* TRB funcs */

trb_cluster_t *  trb_cluster_alloc( dctrl_t * dc, int n_trb_in_cluster );
void trb_cluster_free( dctrl_t * dc, trb_cluster_t * trb_cluster );
void trb_build_ctrl_setup( trb_cluster_t * trb_cluster, uint64_t setup_packet_paddr );
void trb_build_ctrl_data( trb_cluster_t * trb_cluster, uint64_t buf_paddr64, uint32_t size );
void trb_build_ctrl_status2( trb_cluster_t * trb_cluster );
void trb_build_ctrl_status3( trb_cluster_t * trb_cluster );
void trb_build_normal( trb_cluster_t * trb_cluster, uint64_t buf_paddr64, uint32_t size );

/* command funcs */
void epcmd_build_DEPSTARTCFG( cmd_t * cmd,  int xfer_rsc_idx );
void epcmd_build_DEPCFG( cmd_t * cmd, ep_ctx_t * ep, uint8_t dir, uint8_t ignore_seqn );
void epcmd_build_DEPSTRTXFER( cmd_t * cmd, trb_cluster_t * trb_cluster );
void epcmd_build_DEPENDXFER( cmd_t * cmd , uint32_t xfer_rsc_idx );
void epcmd_build_DEPXFERCFG( cmd_t * cmd, int n_xfer_resources );
void epcmd_build_DEPSETSTALL( cmd_t * cmd );
void epcmd_build_DEPCSTALL( cmd_t * cmd );
int  epcmd_send( dctrl_t * dc,ep_ctx_t *ep,  uint32_t epidx, cmd_t * cmd, int is_blocking );

void dgcmd_build_FIFO_ALL_FLUSH( cmd_t * cmd );
void dgcmd_build_SETPERI_PARAM( cmd_t * cmd, uint16_t latency );
int dgcmd_send( dctrl_t * dc, 	cmd_t * cmd  ) ;



/* event handling */

void event_process( dctrl_t * dc, evt_t * decoded_event );
void event_decode( dctrl_t * dc, uint32_t raw_event, evt_t * decoded_event );

/* platform specific callouts */

int 	dwcotg_custom_init1( dctrl_t * dc, char * args );
int 	dwcotg_custom_init2( dctrl_t * dc );
int 	dwcotg_custom_fini1( dctrl_t * dc );
int 	dwcotg_custom_fini2( dctrl_t * dc );
int 	dwcotg_custom_reset( dctrl_t * dc );
int		dwcotg_phy_in_reset( dctrl_t * dc );

/* fifo alloc */
void * fifomem_init( dctrl_t  *dc );
void fifomem_fini( void * hdl );
int fifomem_alloc( void * hdl, int n_pkts, int mps, int * n_blks_allocated );
void fifomem_free( void * hdl, int blknum );
int fifomem_alloc_ep0( void * hdl, int * n_blks_allocated );
void fifomem_free_ep0( void * hdl, int blknum );

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devu/dc/dwcotg3/dwcotg.h $ $Rev: 743948 $")
#endif
