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

#include "f3s_qspi.h"

#define ARRAY_SIZE(array) (sizeof(array)/sizeof((array)[0]))

static const f3s_dbase_t supported_devices[] =
{
		// Micron N25Q512
		{
			sizeof(f3s_dbase_t),	/* size of complete structure with geometries */
			0,						/* status of structure */
			MID_MICRON,				/* jedec high byte - manufacturer ID */
			DID_N25Q512,			/* jedec low byte - device ID */
			"N25Q512A QSPI NOR Flash",
			F3S_ERASE_FOR_READ | F3S_ERASE_FOR_WRITE,/* flags for capabilities */
			1,						/* interleave for chips on bus */
			8,						/* width of chip */
			640000U,				/* typical write time for cell (ns) */
			600000000U,				/* typical erase time for unit (ns) */
			0,						/* read mode voltage needed */
			0,						/* program mode voltage needed */
			0,						/* number of erase cycles */
			0,						/* poll count timeout */
			0,						/* depth of erase queue per chip */
			0,						/* number of write buffers per chip */
			256,					/* size of write buffers */
			1,						/* number of geometries in vector */
			{{1024, 16}}			/* number of erase units for geometry; power 2 size of a unit */
		},

		// Micron N25Q128A
		{
			sizeof(f3s_dbase_t),	/* size of complete structure with geometries */
			0,						/* status of structure */
			MID_MICRON,				/* jedec high byte - manufacturer ID */
			DID_N25Q128,			/* jedec low byte - device ID */
			"N25Q128A QSPI NOR Flash",
			F3S_ERASE_FOR_READ | F3S_ERASE_FOR_WRITE,/* flags for capabilities */
			1,						/* interleave for chips on bus */
			8,						/* width of chip */
			640000U,				/* typical write time for cell (ns) */
			600000000U,				/* typical erase time for unit (ns) */
			0,						/* read mode voltage needed */
			0,						/* program mode voltage needed */
			0,						/* number of erase cycles */
			0,						/* poll count timeout */
			0,						/* depth of erase queue per chip */
			0,						/* number of write buffers per chip */
			256,					/* size of write buffers */
			1,						/* number of geometries in vector */
			{{256, 16}}			/* number of erase units for geometry; power 2 size of a unit */
		},
		// Micron N25Q64A
		{
			sizeof(f3s_dbase_t),	/* size of complete structure with geometries */
			0,						/* status of structure */
			MID_MICRON,				/* jedec high byte - manufacturer ID */
			DID_N25Q64,			/* jedec low byte - device ID */
			"N25Q64A QSPI NOR Flash",
			F3S_ERASE_FOR_READ | F3S_ERASE_FOR_WRITE,/* flags for capabilities */
			1,						/* interleave for chips on bus */
			8,						/* width of chip */
			640000U,				/* typical write time for cell (ns) */
			600000000U,				/* typical erase time for unit (ns) */
			0,						/* read mode voltage needed */
			0,						/* program mode voltage needed */
			0,						/* number of erase cycles */
			0,						/* poll count timeout */
			0,						/* depth of erase queue per chip */
			0,						/* number of write buffers per chip */
			256,					/* size of write buffers */
			1,						/* number of geometries in vector */
			{{128, 16}}			/* number of erase units for geometry; power 2 size of a unit */
		},

		{0, 0xffff, 0, 0, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

static void n25q_qspi_init(qspi_dev_t *qspi)
{
	int dummy_bits;

	// Write VCR/VECR register if necessary
	qspi_mode_config(qspi, QSPI_MODE_CPORT);
	qspi_send_command(qspi, QSPI_CMD_WREN, 0, COMMAND_1_BYTE);
	if(qspi->nommap) {
		qspi_send_command(qspi, (QSPI_CMD_WRVCR << 8) | 0xFB, 0, COMMAND_2_BYTE);
	} else {
		qspi_send_command(qspi, (QSPI_CMD_WRVCR << 8) | 0x4B, 0, COMMAND_2_BYTE);
	}
	qspi_send_command(qspi, QSPI_CMD_WREN, 0, COMMAND_1_BYTE);
	if(qspi->nommap) {
		qspi_send_command(qspi, (QSPI_CMD_WRVECR << 8) | 0xDF, 0, COMMAND_2_BYTE);
	} else {
		qspi_send_command(qspi, (QSPI_CMD_WRVECR << 8) | 0xCF, 0, COMMAND_2_BYTE);
	}

	if (qspi->addr_mode == QSPI_ADDR_4BYTE) { // Switch to 4 byte address mode
		qspi_mode_config(qspi, QSPI_MODE_CPORT);
		qspi_send_command(qspi, QSPI_CMD_WREN, 0, COMMAND_1_BYTE);
		qspi_send_command(qspi, QSPI_CMD_ENTER_4BYTE_MODE, 0, COMMAND_1_BYTE);
	}

	if (verbose) {
		unsigned vcr, vecr, nvcr;
		vcr = qspi_read_register(qspi, QSPI_CMD_RDVCR, COMMAND_1_BYTE);
		vecr = qspi_read_register(qspi, QSPI_CMD_RDVECR, COMMAND_1_BYTE);
		nvcr = qspi_read_register(qspi, QSPI_CMD_RDNVCR, COMMAND_1_BYTE);
		fprintf(stderr, "%s: VCR 0x%X, VECR 0x%X, NVCR 0x%X \n", __func__, vcr, vecr, nvcr);
	}

	dummy_bits = (qspi->nommap) ? 8 : 4;
	out32(qspi->core_base + QSPI_SPI_SETUP0, QSPI_CMD_QOFR << QSPI_SETUP_RCMD_SHIFT
			| (qspi->addr_mode == QSPI_ADDR_4BYTE ? QSPI_SETUP_ADDR_4BYTE : QSPI_SETUP_ADDR_3BYTE)
			| (0 << QSPI_SETUP_NUM_D_BYTES_SHIFT)
			| QSPI_SETUP_READ_QUAD
			| 0 << QSPI_SETUP_WCMD_SHIFT
			| (dummy_bits << QSPI_SETUP_NUM_D_BITS_SHIFT));
}

int32_t f3s_qspi_n25q_ident(f3s_dbase_t *dbase,
						f3s_access_t *access,
						uint32_t flags,
						uint32_t offset)
{
	qspi_dev_t *qspi = (qspi_dev_t *)access->socket.socket_handle;
	uint8_t	buf[21] = {0};
	int i;

	qspi_flash_read_ident(access->socket.socket_handle, buf, 20);

	for (i = 0; i < ARRAY_SIZE(supported_devices); i++) {
		const f3s_dbase_t *sd = &supported_devices[i];

		if (sd->struct_size == 0) return (ENOENT);
		if ((buf[0] == sd->jedec_hi) && (buf[1] == (sd->jedec_lo & 0xFF)) && (buf[2] == (sd->jedec_lo >> 8))) {

			*dbase = *sd;	// update database entry
			access->socket.window_size = dbase->geo_vect[0].unit_num * (1 << dbase->geo_vect[0].unit_pow2);
			qspi->addr_mode = (sd->jedec_lo == DID_N25Q512) ? QSPI_ADDR_4BYTE : QSPI_ADDR_3BYTE;
			n25q_qspi_init(qspi);
			if (verbose > 2) {
				fprintf(stderr, "Detected QSPI Flash: %s, JEDEC 0x%x - 0x%X, Address mode - %d \n",
						dbase->name, dbase->jedec_hi, dbase->jedec_lo, qspi->addr_mode);
			}

			return (EOK);
		}
	}

	return (ENOENT);
}

int qspi_flash_n25q_sector_erase(qspi_dev_t *qspi, int offset)
{
	int loop;
	int fsr;

	// configuration port is selected
	qspi_mode_config(qspi, QSPI_MODE_CPORT);

	// enable write
	qspi_send_command(qspi, QSPI_CMD_WREN, 0, COMMAND_1_BYTE);

	// issue erase cmmand and sector address
	if (qspi->addr_mode == QSPI_ADDR_4BYTE) {
		//qspi_send_command(qspi, QSPI_CMD_4SE, 1, COMMAND_1_BYTE);
		qspi_send_command(qspi, QSPI_CMD_BLK_ERASE, 1, COMMAND_1_BYTE);
		qspi_send_command(qspi, offset, 0, COMMAND_4_BYTE);
	} else {
		qspi_send_command(qspi, ((QSPI_CMD_BLK_ERASE << 24) | (offset & 0xffffff)), 0, COMMAND_4_BYTE);
	}

	for (loop = 900000; loop > 0; loop--) {
		if (!qspi_flash_chip_busy(qspi)) {
			fsr = qspi_read_register(qspi, QSPI_CMD_RFSR, COMMAND_1_BYTE);
			if (fsr & FSR_PE) {
				return (EOK);
			} else {
				// possibly error bit is set
				fprintf(stderr, "%s() at offset 0x%x, QSPI FSR 0x%x\n", __func__, offset, fsr);
				return (EIO);
			}
		}
	}

	fprintf(stderr, "%s() failed to erase QSPI NOR at offset 0x%x\n", __func__, offset);
	return EIO;
}

int qspi_flash_n25q_page_program(qspi_dev_t *qspi, uint32_t offset, int size, uint8_t *buf)
{
	int	loop;
	int	fsr;

	// configuration port is selected
	qspi_mode_config(qspi, QSPI_MODE_CPORT);

	qspi_send_command(qspi, QSPI_CMD_WREN, 0, COMMAND_1_BYTE);

	if ((((uint32_t)buf) & 3) || (offset & 3) || (size & 3)) {

		if (qspi->addr_mode == QSPI_ADDR_4BYTE) {
			qspi_send_command(qspi, QSPI_CMD_PAGE_PRGM, size + 1, COMMAND_1_BYTE);
			qspi_send_command(qspi, offset, 0, COMMAND_4_BYTE);
		} else {
			// issue page program command plus 3 bytes address
			qspi_send_command(qspi, (QSPI_CMD_PAGE_PRGM << 24) | (offset & 0xffffff), size, COMMAND_4_BYTE);
		}

		// write data
		while (size--) {
			qspi_send_command(qspi, *buf++, size, COMMAND_1_BYTE);
		}
	} else {
		uint32_t *pbuf = (uint32_t *)buf;
		size = (size >> 2);

		if (qspi->addr_mode == QSPI_ADDR_4BYTE) {
			qspi_send_command(qspi, QSPI_CMD_PAGE_PRGM, size + 1, COMMAND_1_BYTE);
			qspi_send_command(qspi, offset, 0, COMMAND_4_BYTE);
		} else {
			// issue page program command plus 3 bytes address
			qspi_send_command(qspi, (QSPI_CMD_PAGE_PRGM << 24) | (offset & 0xffffff), size, COMMAND_4_BYTE);
		}

		// write data
		while (size--) {
			uint32_t tmp = *pbuf++;
			qspi_send_command(qspi, ENDIAN_BE32(tmp), size, COMMAND_4_BYTE);
		}
	}

	// max page program time 750us
	for (loop = 100000; loop > 0; loop--) {
		if (!qspi_flash_chip_busy(qspi)) {
			fsr = qspi_read_register(qspi, QSPI_CMD_RFSR, COMMAND_1_BYTE);
			if (fsr & FSR_PE) {
				return (EOK);
			} else {
				// possibly error bit is set
				fprintf(stderr, "%s() at offset 0x%x, QSPI FSR 0x%x\n", __func__, offset, fsr);
				return (EIO);
			}
		}

		nanospin_ns(1000);
	}

	fprintf(stderr, "%s() failed to program QSPI Flash at 0x%x size 0x%x\n", __func__, offset, size);
	return (EIO);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/6.6.0/trunk/hardware/flash/boards/ti_qspi/f3s_qspi_n25q.c $ $Rev: 765822 $")
#endif
