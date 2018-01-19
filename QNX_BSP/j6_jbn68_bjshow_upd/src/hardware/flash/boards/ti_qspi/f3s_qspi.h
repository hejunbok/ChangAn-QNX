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


/*
 * This file contains definitions for QSPI serial NOR flash driver callouts.
 */
#ifndef __F3S_QSPI_H_INCLUDED
#define __F3S_QSPI_H_INCLUDED

#include <sys/mman.h>
#include <sys/f3s_mtd.h>
#include <sys/neutrino.h>
#include <hw/inout.h>
#include <pthread.h>
#include "ti_qspi.h"

// Flash Device Operation Instruction
#define QSPI_CMD_WRREG			0x01	// write cr register
#define QSPI_CMD_PAGE_PRGM	0x02	// page program
#define QSPI_CMD_READ			0x03	// read memory
#define QSPI_CMD_WRDI			0x04	// write disable
#define QSPI_CMD_RDSR			0x05	// read status register 1
#define FLASH_WIP				(1 << 0)	// write in progress
#define FLASH_ES				(1 << 1)	// erase suspended
#define QSPI_CMD_WREN			0x06	// write enable
#define QSPI_CMD_RDSR2			0x07	// read status register 2
#define QSPI_CMD_HS_READ		0x0B	// high speed read
#define QSPI_CMD_READ_BURST	0x0C	// nB burst with wrap
#define QSPI_CMD_READ_PI		0x08	// read page index
#define QSPI_CMD_READ_I		0x09	//
#define QSPI_CMD_READ_BI		0x10	//
#define QSPI_CMD_4PP			0x12	// page program with 4 bytes address
#define QSPI_CMD_3PP_032p		0x02	// page program with 3 bytes address for 032p
#define QSPI_CMD_SEC_ERASE		0x20	// sector erase
#define QSPI_CMD_4P4E			0x21	// 4KB sector erase, 4B address
#define QSPI_CMD_WRITE_RSUM 	0x30	// write resume
#define QSPI_CMD_4QPP			0x34	// Quad page program with 4 bytes address
#define QSPI_CMD_3QPP_032p		0x32	// Quad page program with 3 bytes address for 032p
#define QSPI_CMD_RDCR			0x35	// read configuration register 1
#define CR_QUAD					(1 << 1)	// QUAD mode
#define QSPI_CMD_EQIO			0x38	// enable Quad I/O
#define QSPI_CMD_WBPR			0x42	// write block protection register
#define QSPI_CMD_OTP_PRGM		0x42	// OTP program
#define QSPI_CMD_OTP_READ		0x4B	// OTP read
#define QSPI_CMD_RSTEN			0x66	// reset enable
#define QSPI_CMD_QOFR			0x6B	// Quad Output Fast Read
#define QSPI_CMD_4QOR			0x6C	// Quad read with 4 bytes address
#define QSPI_CMD_3QOR_032p	0x6B	// Quad read with 3 bytes address for 032p
#define QSPI_CMD_ERSP			0x75	// erase suspend
#define QSPI_CMD_ERRS			0x7A	// erase resume
#define QSPI_CMD_LOCK_SID		0x85	// lock out security ID proframming
#define QSPI_CMD_READ_SID		0x88	// read security ID
#define QSPI_CMD_LBPR			0x8D	// lock down block protection register
#define QSPI_CMD_REMS			0x90	// Read Identification
#define QSPI_CMD_RDID			0x9F	// read JEDEC-ID
#define QSPI_CMD_PRGM_SID		0xA5	// program user security ID area
#define QSPI_CMD_READ_QJID		0xAF	// read Quad JEDEC-ID
#define QSPI_CMD_WRITE_SUSP 	0xB0	// write suspend
#define QSPI_CMD_SET_BURST		0xC0	// set burst
#define QSPI_CMD_CHIP_ERASE 	0xC7	// chip erase
#define QSPI_CMD_BLK_ERASE		0xD8	// block erase
#define QSPI_CMD_4SE			0xDC	// 64KB sector erase, 4B address
#define QSPI_CMD_BLK_ERASE_032p		0x60	// block erase for 032p
#define QSPI_CMD_3SE_032p		0xD8    // 64KB sector erase, 4B address for 032p
#define QSPI_CMD_4QIOR			0xEC	// Quad I/O read with 4 bytes address
#define QSPI_CMD_RST			0xF0	// software reset
#define QSPI_CMD_RSTQIO		0xFF	// reset Quad I/O

/* Micron specific command */
#define QSPI_CMD_WRVECR			0x61
#define QSPI_CMD_RDVECR			0x65
	#define VECR_QUAD_IN		(1 << 7)
	#define VECR_DUAl_IN		(1 << 6)
	#define VECR_XIP		(1 << 3)
#define QSPI_CMD_RFSR			0x70
	#define FSR_PE			(1 << 7)
#define QSPI_CMD_RBPR			0x72	// read block protection register
#define QSPI_CMD_RDVCR			0x85
#define QSPI_CMD_WRVCR			0x81
#define QSPI_CMD_RDNVCR			0xB5
#define QSPI_CMD_WNVCR			0xB1
#define QSPI_CMD_ENTER_4BYTE_MODE	0xB7
#define QSPI_CMD_EXIT_4BYTE_MODE	0xE9

int32_t f3s_qspi_open(f3s_socket_t *socket,
					uint32_t flags);

uint8_t *f3s_qspi_page(f3s_socket_t *socket,
						uint32_t page,
						uint32_t offset,
						int32_t *size);

int32_t f3s_qspi_read(f3s_dbase_t *dbase,
					f3s_access_t *access,
					uint32_t flags,
					uint32_t offset,
					int32_t buffer_size,
					uint8_t *buffer);
int32_t f3s_qspi_status(f3s_socket_t *socket,
						uint32_t flags);

void f3s_qspi_close(f3s_socket_t *socket,
					uint32_t flags);

int32_t f3s_qspi_s25fl_ident(f3s_dbase_t * dbase, f3s_access_t * access,
			_uint32 flags, _uint32 offset);

int32_t f3s_qspi_n25q_ident(f3s_dbase_t * dbase, f3s_access_t * access,
			_uint32 flags, _uint32 offset);

int32_t f3s_qspi_write(f3s_dbase_t * dbase, f3s_access_t * access,
			_uint32 flags, _uint32 offset,
			_int32 size, _uint8 * buffer);

int f3s_qspi_s25fl_erase(f3s_dbase_t * dbase, f3s_access_t * access,
			_uint32 flags, _uint32 offset);

int f3s_qspi_n25q_erase(f3s_dbase_t * dbase, f3s_access_t * access,
			_uint32 flags, _uint32 offset);

int f3s_qspi_suspend(f3s_dbase_t *dbase, f3s_access_t *access,
					uint32_t flags, uint32_t offset);

int f3s_qspi_resume(f3s_dbase_t *dbase, f3s_access_t *access,
					uint32_t flags, uint32_t offset);

int32_t f3s_qspi_sync(f3s_dbase_t *dbase, f3s_access_t *access,
					uint32_t flags, uint32_t offset);

void f3s_qspi_s25fl_reset(f3s_dbase_t *dbase, f3s_access_t *access,
					uint32_t flags, uint32_t offset);

void f3s_qspi_n25q_reset(f3s_dbase_t *dbase, f3s_access_t *access,
					uint32_t flags, uint32_t offset);

int f3s_qspi_v2ssrop(f3s_dbase_t *dbase, f3s_access_t *access, uint32_t op, 
				uint32_t offset, int32_t size, uint8_t *buf);

#endif /* __F3S_QSPI_H_INCLUDED */


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/6.6.0/trunk/hardware/flash/boards/ti_qspi/f3s_qspi.h $ $Rev: 761898 $")
#endif
