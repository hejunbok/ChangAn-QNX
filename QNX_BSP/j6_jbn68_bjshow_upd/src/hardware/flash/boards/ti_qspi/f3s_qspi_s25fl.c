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

	// Spansion S25FL256S
	{
		sizeof(f3s_dbase_t),/* size of complete structure with geometries */
		0,					/* status of structure */
		MID_SPANSION,		/* jedec high byte - manufacturer ID */
		DID_DENS_S25FL256,		/* jedec low byte - device ID - Density*/
		"S25FL256S QSPI NOR Flash",
		0,					/* flags for capabilities */
		1,					/* interleave for chips on bus */
		8,					/* width of chip */
		640000U,			/* typical write time for cell (ns) */
		600000000U,			/* typical erase time for unit (ns) */
		0,					/* read mode voltage needed */
		0,					/* program mode voltage needed */
		0,					/* number of erase cycles */
		0,					/* poll count timeout */
		0,					/* depth of erase queue per chip */
		0,					/* number of write buffers per chip */
		0,					/* size of write buffers */
		0,					/* number of geometries in vector */
		{{0,0}}				/* number of erase units for geometry; power 2 size of a unit */
	},

	// Spansion S25FL032S
	{
		sizeof(f3s_dbase_t),/* size of complete structure with geometries */
		0,					/* status of structure */
		MID_SPANSION,		/* jedec high byte - manufacturer ID */
		DID_DENS_S25FL032,		/* jedec low byte - device ID - Density*/
		"S25FL032P QSPI NOR Flash",
		0,					/* flags for capabilities */
		1,					/* interleave for chips on bus */
		8,					/* width of chip */
		640000U,			/* typical write time for cell (ns) */
		600000000U,			/* typical erase time for unit (ns) */
		0,					/* read mode voltage needed */
		0,					/* program mode voltage needed */
		0,					/* number of erase cycles */
		0,					/* poll count timeout */
		0,					/* depth of erase queue per chip */
		0,					/* number of write buffers per chip */
		0,					/* size of write buffers */
		0,					/* number of geometries in vector */
		{{0,0}}				/* number of erase units for geometry; power 2 size of a unit */
	},
	{0, 0xffff, 0, 0, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

static void	s25fl_fill_db(f3s_access_t *access, f3s_dbase_t *dbase, uint8_t *buf)
{
	int32_t	geo_index;
	int32_t	geo_pos;
	int32_t	chip_size = 0;
	int32_t	unit_size;

	/* Read buffer size information */
	dbase->buffer_size = buf[0x2b];
	dbase->buffer_size <<= 8;
	dbase->buffer_size += buf[0x2a];

	/* Value is 2^N bytes per chip */
	dbase->buffer_size = 1 << dbase->buffer_size;

	/* Read number of geometries */
	dbase->geo_num = buf[0x2c];

	/* Read geometry information */
	for (geo_index = 0, geo_pos = 0x2d; geo_index < dbase->geo_num; geo_index++, geo_pos += 4) {
		/* Read number of units */
		dbase->geo_vect[geo_index].unit_num	= buf[geo_pos + 1];
		dbase->geo_vect[geo_index].unit_num <<= 8;
		dbase->geo_vect[geo_index].unit_num	+= buf[geo_pos + 0];
		dbase->geo_vect[geo_index].unit_num	+= 1;

		/* Read size of unit */
		unit_size	= buf[geo_pos + 3];
		unit_size <<= 8;
		unit_size	+= buf[geo_pos + 2];

		/* Interpret according to the CFI specs */
		if (unit_size == 0) unit_size	= 128;
		else				unit_size *= 256;

		chip_size += unit_size * dbase->geo_vect[geo_index].unit_num;

		/* Convert size to power of 2 */
		dbase->geo_vect[geo_index].unit_pow2 = 0;
		while (unit_size > 1) {
			unit_size >>= 1;
			dbase->geo_vect[geo_index].unit_pow2++;
		}
	}

	access->socket.window_size = chip_size;

	/* Detect read / write suspend */
	if		(buf[0x46] == 1) dbase->flags = F3S_ERASE_FOR_READ;
	else if (buf[0x46] == 2) dbase->flags = F3S_ERASE_FOR_READ | F3S_ERASE_FOR_WRITE;
}

static int s25fl_qspi_init(qspi_dev_t *qspi)
{
	unsigned sr1, cr;

	// Select configuration port
	qspi_mode_config(qspi, QSPI_MODE_CPORT);

	// enable write
	qspi_send_command(qspi, QSPI_CMD_WREN, 0, COMMAND_1_BYTE);

	// Read Status register 1
	sr1 = qspi_read_register(qspi, QSPI_CMD_RDSR, COMMAND_1_BYTE);

	// Read Configuration Register
	cr	= qspi_read_register(qspi, QSPI_CMD_RDCR, COMMAND_1_BYTE);

	if (verbose > 3) {
		fprintf(stderr, "sr1 = 0x%x, cr = 0x%x\n",  sr1, cr);
	}

	// Quad already enabled?
	if ((cr & CR_QUAD))
		return (EOK);

	// Set Configuration register 2nd bit to 1 for Quad enable
	cr |= CR_QUAD;

	if (verbose > 3) {
		fprintf(stderr, "cr = 0x%x\n", cr);
	}

	// Write CR register
	qspi_send_command(qspi, (QSPI_CMD_WRREG << 16) | (sr1 << 8 ) | cr, 0, COMMAND_3_BYTE);

	// Verify Configuration Register
	cr = qspi_read_register(qspi, QSPI_CMD_RDCR, COMMAND_1_BYTE);

	if (verbose > 3) {
		fprintf(stderr, "cr = 0x%x\n", cr);
	}

	// Quad already enabled?
	if ((cr & CR_QUAD))
		return (EOK);

	fprintf(stderr, "(devf t%d::%s::%d) failed to set QSPI NOR to Quad mode\n",
			pthread_self(), __func__, __LINE__);
	return (ENODEV);
}

int32_t f3s_qspi_s25fl_ident(f3s_dbase_t *dbase,
						f3s_access_t *access,
						uint32_t flags,
						uint32_t offset)
{
	uint8_t	buf[88] = {0};
	int i;

	qspi_dev_t *qspi = (qspi_dev_t *)access->socket.socket_handle;

	qspi_flash_read_ident(qspi, buf, 81);

	if (buf[0x10] != 'Q' || buf[0x11] != 'R' || buf[0x12] != 'Y' ||
		buf[0x40] != 'P' || buf[0x41] != 'R' || buf[0x42] != 'I') {
		return (ENOTSUP);
	}

	for (i = 0; i < ARRAY_SIZE(supported_devices); i++) {
		const f3s_dbase_t *sd = &supported_devices[i];


		if (sd->struct_size == 0) return (ENOENT);
		if (buf[0] == sd->jedec_hi && buf[2] == sd->jedec_lo) {

			*dbase = *sd;	// update database entry
			s25fl_fill_db(access, dbase, buf);
			if (s25fl_qspi_init(qspi))
				return (ENODEV);

			if (verbose > 3) {
				fprintf(stderr, "Detected QSPI Falsh: %s, JEDEC 0x%x - 0x%x - 0x%x\n", dbase->name, dbase->jedec_hi, buf[1], dbase->jedec_lo);
			}

			return (EOK);
		}
	}

	return (ENOENT);
}

int qspi_flash_s25fl_sector_erase(qspi_dev_t *qspi, int offset, int sa_bits)
{
	uint32_t erase_cmd;
	unsigned sr1, cr;

#if 0
	if (sa_bits > 12)
		erase_cmd = QSPI_CMD_4SE;
	else
		erase_cmd = QSPI_CMD_4P4E;
#endif

	// configuration port is selected
	qspi_mode_config(qspi, QSPI_MODE_CPORT);

	// enable write
	qspi_send_command(qspi, QSPI_CMD_WREN, 0, COMMAND_1_BYTE);

	if (verbose > 5) {
		// Read Status register 1
		sr1 = qspi_read_register(qspi, QSPI_CMD_RDSR, COMMAND_1_BYTE);
		// Read Configuration Register
		cr	= qspi_read_register(qspi, QSPI_CMD_RDCR, COMMAND_1_BYTE);
		fprintf(stderr, "sr1 = 0x%x, cr = 0x%x, offset = %d\n",  sr1, cr, offset);
	}

#if 1	//64KB Sectore Erase
	// send erase command
	qspi_send_command(qspi, QSPI_CMD_3SE_032p, 1, COMMAND_1_BYTE);

	// issue 3 bytes sector address
	return qspi_send_command(qspi, offset, 2, COMMAND_3_BYTE);
#else	//Bulk Erase
	qspi_send_command(qspi, QSPI_CMD_BLK_ERASE_032p, 1, COMMAND_1_BYTE);;
	return qspi_send_command(qspi, offset, 0, COMMAND_3_BYTE);
#endif
}

/*
 * QSPI page program
 */
int qspi_flash_s25fl_page_program(qspi_dev_t *qspi, uint32_t offset, int size, uint8_t *buf)
{
	int	loop;

	// configuration port is selected
	qspi_mode_config(qspi, QSPI_MODE_CPORT);

	qspi_send_command(qspi, QSPI_CMD_WREN, 0, COMMAND_1_BYTE);	

	// issue page program command plus 3 bytes address
	qspi_send_command(qspi, QSPI_CMD_3PP_032p, size + 1, COMMAND_1_BYTE);
	qspi_send_command(qspi, offset, size + 1, COMMAND_3_BYTE);

	// write data
	while (size--) {
		qspi_send_command(qspi, *buf++, size, COMMAND_1_BYTE);
	}

	// max page program time 750us
	for (loop = 100000; loop > 0; loop--) {
		if (!qspi_flash_chip_busy(qspi)) {
			return (EOK);
		}

		nanospin_ns(1000);
	}

	fprintf(stderr, "%s() failed to program QSPI Flash at 0x%x size 0x%x\n", __func__, offset, size);
	return (EIO);
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/6.6.0/trunk/hardware/flash/boards/ti_qspi/f3s_qspi_s25fl.c $ $Rev: 765822 $")
#endif
