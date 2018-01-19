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

#ifndef __TI_QSPI_H__
#define __TI_QSPI_H__

#include <sys/cache.h>
#include <arm/dra74x.h>
#include <arm/dm6446.h>

#define COMMAND_1_BYTE						1
#define COMMAND_2_BYTE						2
#define COMMAND_3_BYTE						3
#define COMMAND_4_BYTE						4

/* JEDEC IDs */
#define MID_SPANSION    0x01
#define DID_S25FL256    0x02
#define DID_S25FL128    0x20
#define DID_S25FL032    0x02
#define DID_DENS_S25FL128    0x18
#define DID_DENS_S25FL256    0x19
#define DID_DENS_S25FL032    0x15

#define MID_MICRON      0x20
#define DID_N25Q512     0x20BA
#define DID_N25Q128     0x18BA
#define DID_N25Q64      0x17BA

/* QSPI controller registers */
#define QSPI_PID							0x00
#define QSPI_SYSCONFIG						0x10
#define QSPI_INTR_STATUS_SET				0x20
#define QSPI_INTR_STATUS_CLEAR				0x24
#define QSPI_INTR_ENABLE_SET				0x28
#define QSPI_INTR_ENABLE_CLEAR				0x2C
	#define QSPI_INTR_WIRQ					(1 << 1)
	#define QSPI_INTR_FIRQ					(1 << 0)
#define QSPI_SPI_CLOCK_CNTRL				0x40
	#define QSPI_CLKEN						(1 << 31)
#define QSPI_SPI_DC							0x44
#define QSPI_SPI_CMD						0x48
	#define QSPI_CMD_FLEN_SHIFT				0
	#define QSPI_CMD_WLEN_SHIFT				19
	#define QSPI_CMD_CSNUM_SHIFT			28
	#define QSPI_CMD_WIRQ					(1 << 14)
	#define QSPI_CMD_FIRQ					(1 << 15)
	#define QSPI_CMD_4PIN_READ_SINGLE		(1 << 16)
	#define QSPI_CMD_4PIN_WRITE_SINGLE		(2 << 16)
	#define QSPI_CMD_4PIN_READ_DUAL			(3 << 16)
	#define QSPI_CMD_4PIN_RW_INVALID		(4 << 16)
	#define QSPI_CMD_3PIN_READ_SINGLE		(5 << 16)
	#define QSPI_CMD_3PIN_WRITE_SINGLE		(6 << 16)
	#define QSPI_CMD_6PIN_READ_QUAD			(7 << 16)
#define QSPI_SPI_STATUS						0x4C
	#define QSPI_STATUS_BUSY				(1 << 0)
	#define QSPI_STATUS_WC					(1 << 1)
	#define QSPI_STATUS_FC					(1 << 2)
	#define QSPI_STATUS_WDCNT_SHIFT			16
	#define QSPI_STATUS_WDCNT_MASK			0xFFF
#define QSPI_SPI_DATA						0x50
#define QSPI_SPI_SETUP0						0x54
#define QSPI_SPI_SETUP1						0x58
#define QSPI_SPI_SETUP2						0x5C
#define QSPI_SPI_SETUP3						0x60
	#define QSPI_SETUP_READ_NORM			(0 << 12)
	#define QSPI_SETUP_READ_DUAL			(1 << 12)
	#define QSPI_SETUP_READ_QUAD			(3 << 12)
	#define QSPI_SETUP_NUM_D_BYTES_SHIFT	10
	#define QSPI_SETUP_DUMMY_16BITS			(2 << 10)
	#define QSPI_SETUP_DUMMY_24BITS			(3 << 10)
	#define QSPI_SETUP_ADDR_1BYTE			(0 << 8)
	#define QSPI_SETUP_ADDR_2BYTE			(1 << 8)
	#define QSPI_SETUP_ADDR_3BYTE			(2 << 8)
	#define QSPI_SETUP_ADDR_4BYTE			(3 << 8)
	#define QSPI_SETUP_RCMD_SHIFT			0
	#define QSPI_SETUP_WCMD_SHIFT			16
	#define	QSPI_SETUP_NUM_D_BITS_SHIFT		24
#define QSPI_SPI_SWITCH						0x64
	#define QSPI_SWITCH_MM_INT_EN			(1 << 1)	// memory mapped mode interrupt enable
	#define QSPI_SWITCH_MMPT_S				(1 << 0)	// MPT select
#define QSPI_SPI_DATA1						0x68
#define QSPI_SPI_DATA2						0x6C
#define QSPI_SPI_DATA3						0x70

#define QSPI_MEMMAPPED_CS_MASK				0x700
#define QSPI_MEMMAPPED_CS_SHIFT 			8
#define QSPI_MMR							0
#define QSPI_CS0							1
#define QSPI_CS1							2
#define QSPI_CS2							3
#define QSPI_CS3							4

/* Threashold of whether using DMA for read */
#define EDMA_THRESHOLD						64
#define EDMA_BUFLEN							(16 * 1024)

typedef struct _qspi_dev_t
{
	uintptr_t	core_base;
	unsigned	core_pbase;
	unsigned	core_size;

	uintptr_t	ctrlcore_control_io2;
	uint8_t		*memory;
	int32_t		suspended;
	uintptr_t	edma_base;

	int			cs;
	int			read_mode;	// R/W mode: SPI, Dual SPI, QUAD SPI
	int			write_mode;

	int			edma_chnl;
	int			edma_event;
	int			edma_irq;
	int			input_clk;	// QSPI fucntional clock
	int			clk;		// clock to QSPI NOR part

	void		*v_buf;		// virtual address of DMA buffer, or write compare buffer
	uint32_t	p_dmabuf;	// pysical address of DMA buffer
	int			iid;		// EDMA interrupt ID
	int			chid;
	int			coid;
	struct sigevent	event;
#define QSPI_EDMA_EVENT		0x05
	int			irq_expected;
	int			nommap;
	int			addr_mode;
#define QSPI_ADDR_4BYTE		1
#define QSPI_ADDR_3BYTE		0
	int32_t		mode;
#define QSPI_MODE_MMAP		1	// memory mapped mode
#define QSPI_MODE_CPORT		0	// configuration port
#define QSPI_MODE_UNKNOWN	-1
	struct cache_ctrl	cachectl;
} qspi_dev_t;

int qspi_flash_read_ident(qspi_dev_t *, uint8_t* buf, int bytes);
int qspi_flash_s25fl_sector_erase(qspi_dev_t *, int offset, int sa_bits);
int qspi_flash_n25q_sector_erase(qspi_dev_t *, int offset);
int qspi_flash_erase_suspend(qspi_dev_t *);
int qspi_flash_erase_resume(qspi_dev_t *);
int qspi_flash_s25fl_page_program(qspi_dev_t *, uint32_t offset, int len, uint8_t* data);
int qspi_flash_n25q_page_program(qspi_dev_t *, uint32_t offset, int len, uint8_t* data);
int qspi_flash_read(qspi_dev_t *, int offset, uint8_t* buffer, int size);
int qspi_flash_chip_busy(qspi_dev_t *);
int qspi_flash_reset(qspi_dev_t *);
int qspi_attach_irq(qspi_dev_t *);

void qspi_mode_config(qspi_dev_t *qspi, int mode);
int qspi_read_data(qspi_dev_t *qspi, int flen, int bw, int mode);
int qspi_read_register(qspi_dev_t *qspi, uint32_t cmd, int len);
int qspi_send_command(qspi_dev_t *qspi, uint32_t cmd, int flen, int bw);

typedef struct {
	volatile uint32_t	opt;
	volatile uint32_t	src;
	volatile uint32_t	abcnt;
	volatile uint32_t	dst;
	volatile uint32_t	srcdstbidx;
	volatile uint32_t	linkbcntrld;
	volatile uint32_t	srcdstcidx;
	volatile uint32_t	ccnt;
} edma_param;
#endif


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/6.6.0/trunk/hardware/flash/boards/ti_qspi/ti_qspi.h $ $Rev: 761898 $")
#endif
