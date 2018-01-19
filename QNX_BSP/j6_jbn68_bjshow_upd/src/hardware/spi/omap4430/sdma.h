/*
 * $QNXLicenseC:
 * Copyright 2006-2008 QNX Software Systems.
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
	
#ifndef OMAP_DMA__H_
#define OMAP_DMA__H_
	
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <atomic.h>
#include <inttypes.h>
#include <sys/mman.h>
#include <sys/rsrcdbmgr.h>
#include <sys/rsrcdbmsg.h>
#include <sys/neutrino.h>
#include <hw/inout.h>
#include <sys/slog.h>
	
#define DMA4_BASE_ADDR	0x4a056000
#define SDMA_BASE_IRQ	256

/* SDMA Mapping Requests */ 
#define S_DMA_NONE	-1

#define S_DMA_34	34	/* McSPI module 1—transmit request channel 0*/
#define S_DMA_35	35	/* McSPI module 1—receive request channel 0*/
#define S_DMA_36	36	/* McSPI module 1—transmit request channel 1*/
#define S_DMA_37	37	/* McSPI module 1—receive request channel 1*/
#define S_DMA_38	38	/* McSPI module 1—transmit request channel 2*/
#define S_DMA_39	39	/* McSPI module 1—receive request channel 2*/
#define S_DMA_40	40	/* McSPI module 1—transmit request channel 3*/
#define S_DMA_41	41	/* McSPI module 1—receive request channel 3*/
#define S_DMA_42	42	/* McSPI module 2—transmit request channel 0*/
#define S_DMA_43	43	/* McSPI module 2—receive request channel 0*/
#define S_DMA_44	44	/* McSPI module 2—transmit request channel 1*/
#define S_DMA_45	45	/* McSPI module 2—receive request channel 1*/
#define S_DMA_14	14	/* McSPI module 3—transmit request channel 0*/
#define S_DMA_15	15	/* McSPI module 3—receive request channel 0*/
#define S_DMA_22	22	/* McSPI module 3—transmit request channel 1*/
#define S_DMA_23	23	/* McSPI module 3—receive request channel 1 */
#define S_DMA_69	69	/* McSPI module 4—transmit request channel 0*/
#define S_DMA_70	70	/* McSPI module 4—receive request channel 0*/
	
#define GCR_ARBITRATION_RATE			0xff0000
#define DMA_DEFAULT_FIFO_DEPTH	 		0x10

#define OMAP_DMA4_CHANNEL_COUNT			32	
	
/*Bit definitions for DMA4_CSDP*/ 
#define DMA4_CSDP_SRC_ENDIAN 			(1<<21)
#define DMA4_CSDP_SRC_ENDIAN_LOCK		(1<<20)
#define DMA4_CSDP_DST_ENDIAN			(1<<19)
#define DMA4_CSDP_DST_ENDIAN_LOCK		(1<<18)
#define DMA4_CSDP_WRITE_MODE_1			(1<<17)
#define DMA4_CSDP_WRITE_MODE_0			(1<<16)
#define DMA4_CSDP_DST_BURST_EN_1		(1<<15)
#define DMA4_CSDP_DST_BURST_EN_0		(1<<14)
#define DMA4_CSDP_DST_PACKED			(1<<13)
#define DMA4_CSDP_SRC_BURST_EN_1		(1<<8)
#define DMA4_CSDP_SRC_BURST_EN_0		(1<<7)
#define DMA4_CSDP_SRC_PACKED			(1<<6)
#define DMA4_CSDP_DATA_TYPE_32BIT			2
#define DMA4_CSDP_DATA_TYPE_16BIT			1
#define DMA4_CSDP_DATA_TYPE_8BIT		 	0
	
/*Bit definitions for DMA4_CCR*/ 
#define DMA4_CCR_WRITE_PRIORITY			(1<<26)
#define DMA4_CCR_BUFFERING_DISABLE		(1<<25)
#define DMA4_CCR_SEL_SRC_DST_SYNC		(1<<24)
#define DMA4_CCR_PREFETCH				(1<<23)
#define DMA4_CCR_SUPERVISOR 			(1<<22)
#define DMA4_CCR_SECURE					(1<<21)
#define DMA4_SYNCHRO_CONTROL_UPPER_OFF	(19)
#define DMA4_CCR_BS						(1<<18)
#define DMA4_CCR_TRANSPARENT_COPY_ENABLE	(1<<17)
#define DMA4_CCR_CONST_FILL_ENABLE		(1<<16)
#define DMA4_CCR_DST_CONSTANT			(0 <14)
#define DMA4_CCR_DST_POST_INCR			(1<<14)
#define DMA4_CCR_DST_AMODE_1			(1<<15)
#define DMA4_CCR_DST_AMODE_0			(1<<14)
#define DMA4_CCR_SRC_CONSTANT			(0)
#define DMA4_CCR_SRC_POST_INCR			(1<<12)
#define DMA4_CCR_SRC_AMODE_1			(1<<13)
#define DMA4_CCR_SRC_AMODE_0			(1<<12)
#define DMA4_CCR_WR_ACTIVE				(1<<10)
#define DMA4_CCR_RD_ACTIVE				(1<<9)
#define DMA4_CCR_SUSPEND_SENSITIVE		(1<<8)
#define DMA4_CCR_ENABLE					(1<<7)
#define DMA4_CCR_READ_PRIORITY			(1<<6)
#define DMA4_CCR_FS						(1<<5)
	
/*Bit definitions for DMA4_CLNK_CTRL*/ 
#define DMA4_CLNK_CTRL_ENABLE_LNK		(1<<15)
#define DMA4_CICR_FRAME_IE				(1<<3)
#define DMA4_CSR_FRAME					(1<<3)
#define DMA4_CSR_MSK					(0x1ffe)
	
#define CONSTANT_ADDRESS_MODE			0	
#define POST_INCREMENTED_ADDRESS_MODE	1
#define SINGLE_INDEX_ADDRESS_MODE		2
#define DOUBLE_INDEX_ADDRESS_MODE		3	

typedef volatile uint32_t vuint32_t;

typedef struct dma4
{
	vuint32_t resv0[2];
	vuint32_t irqstatus[4];
	vuint32_t irqenable[4];
	vuint32_t sysstatus;
	vuint32_t ocp_sysconfig;
	vuint32_t resv1[13];
	vuint32_t caps_0;
	vuint32_t resv2;
	vuint32_t caps_2;
	vuint32_t caps_3;
	vuint32_t caps_4;
	vuint32_t gcr;
	vuint32_t resv3;
	struct dma4_channel
	{
		vuint32_t ccr;
		vuint32_t clnk_ctrl;
		vuint32_t cicr;
		vuint32_t csr;
		vuint32_t csdp;
		vuint32_t cen;
		vuint32_t cfn;
		vuint32_t cssa;
		vuint32_t cdsa;
		vuint32_t cse;
		vuint32_t csf;
		vuint32_t cde;
		vuint32_t cdf;
		vuint32_t csac;
		vuint32_t cdac;
		vuint32_t ccen;
		vuint32_t ccfn;
		vuint32_t color;
		vuint32_t resv4[6];
	} channel[32];
}dma4_t;

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/spi/omap4430/sdma.h $ $Rev: 716138 $")
#endif
	
