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


#include <pthread.h>
#include <sys/f3s_mtd.h>
#include "f3s_qspi.h"


/**********************************************
 * QSPI SSR (OTP) internal object
 *   - lock_region
 *   - lockall
 *   - stat
 *   - write
 *   - read
 **********************************************/

typedef struct qspi_ssr_object
{
	int maxsz;

	int (*lock_region)(qspi_dev_t *, int);
	int (*lockall)(qspi_dev_t *);
	int (*stat)(qspi_dev_t *, int, uint8_t *);
	int (*read)(qspi_dev_t *, int, int, uint8_t *);
	int (*write)(qspi_dev_t *, int, int, uint8_t *);
} qspi_ssr_t;

static qspi_ssr_t *qspi_ssr = NULL;

/* QSPI flash part specific functions 
 * Supported flash parts:
 *   - Spansion S25FL 
 *   - Micron N25Q
 */

/* Spansion S25FL OTP lock related functions 
 */

#define S25FL_LOCK_OFFSET        0x10
int s25fl_ssr_lock_region(qspi_dev_t *qspi, int region)
{
	int ret = EOK;
	uint32_t lock_bytes;

	/* Sanity check for region number */
	if (region > 31 || region == 0) {
		fprintf(stderr, "%s: invalid region %d \n", __func__, region);
		errno = EINVAL;
		return (-1);
	}

	/* Read current lock bytes */
	qspi_ssr->read(qspi, S25FL_LOCK_OFFSET, 4, (uint8_t *)&lock_bytes);

	/* Program lock bit for specified region */
	lock_bytes = lock_bytes & (~(1 << region));
	if (verbose) {
		fprintf(stderr, "%s [%d], lock bytes[%08X] \n", __func__, region, lock_bytes);
	}
	ret = qspi_ssr->write(qspi, S25FL_LOCK_OFFSET, 4, (uint8_t *)&lock_bytes);
	return (ret);
}

int s25fl_ssr_lockall(qspi_dev_t *qspi)
{
	int ret = EOK;
	uint32_t lock_bytes = 0x0;

	/* Program lock bit for all OTP regions except region 0 */
	if (verbose) {
		fprintf(stderr, "%s: lock bytes[%08X] \n", __func__, lock_bytes);
	}
	ret = qspi_ssr->write(qspi, S25FL_LOCK_OFFSET, 4, (uint8_t *)&lock_bytes);
	return (ret);
}

int s25fl_ssr_stat(qspi_dev_t *qspi, int32_t size, uint8_t *buf)
{
	uint32_t lock_bytes;
	uint32_t *buf32;

	/* Sanity check for region number */
	if (buf[0] > 31) {
		fprintf(stderr, "%s: invalid region %d \n", __func__, buf[0]);
		errno = EINVAL;
		return (-1);
	}

	/* Read current lock bytes */
	qspi_ssr->read(qspi, S25FL_LOCK_OFFSET, 4, (uint8_t *)&lock_bytes);
	if (verbose) {
		fprintf(stderr, "%s: lock bytes[%08X] \n", __func__, lock_bytes);
	}
	if(size == 1) {
		buf[0] = ((lock_bytes >> buf[0]) & 0x1) == 1 ? 0 : 1;
	} else if (size == 4) {
			buf32 = (uint32_t *)buf;
			*buf32 = ~(ENDIAN_LE32(lock_bytes));
	} else {
		fprintf(stderr, "%s: invalid buffer length \n", __func__);
		errno = EINVAL;
		return (-1);
	}
	return EOK;
}

/* Micron N25Q OTP lock related functions
 */
#define N25Q_LOCK_OFFSET        64
int n25q_ssr_lockall(qspi_dev_t *qspi)
{
	int ret = EOK;
	uint8_t lock_byte = 0xFE;

	/* Write bit 0 of byte 64 to lock SSR */
	ret = qspi_ssr->write(qspi, S25FL_LOCK_OFFSET, 1, (uint8_t *)&lock_byte);
	return (ret);
}

int n25q_ssr_stat(qspi_dev_t *qspi, int32_t size, uint8_t *buf)
{
	uint8_t lock_byte;

	/* Read OTP control byte 64 */
	qspi_ssr->read(qspi, N25Q_LOCK_OFFSET, 1, &lock_byte);
	buf[0] = (lock_byte & 0x1) == 1 ? 0 : 1;
	return EOK;
}

/* Common OTP Read/Write functons 
 */
int qspi_ssr_write(qspi_dev_t *qspi, int offset, int32_t size, uint8_t *buf)
{
	int loop;

	if (verbose) {
		fprintf(stderr, "%s: offset 0x%x, size %d \n", __func__, offset, size);
	}

	/* Select configuration port */
	qspi_mode_config(qspi, QSPI_MODE_CPORT);

	qspi_send_command(qspi, QSPI_CMD_WREN, 0, COMMAND_1_BYTE);

	if ((((uint32_t)buf) & 3) || (offset & 3) || (size & 3)) {

		/* Issue page program command plus 3 bytes address */
		qspi_send_command(qspi, QSPI_CMD_OTP_PRGM, size + 1, COMMAND_1_BYTE);
		qspi_send_command(qspi, offset, 0, COMMAND_3_BYTE);

		/* Write data with 1 byte command */
		while (size--) {
			qspi_send_command(qspi, *buf++, 0, COMMAND_1_BYTE);
		}
	} else {
		uint32_t *pbuf = (uint32_t *)buf;
		size = (size >> 2);

		/* Issue page program command plus 3 bytes address */
		qspi_send_command(qspi, QSPI_CMD_OTP_PRGM, size + 1, COMMAND_1_BYTE);
		qspi_send_command(qspi, offset, 0, COMMAND_3_BYTE);

		/* write data with 4 byte command  */
		while (size--) {
			uint32_t tmp = *pbuf++;
			qspi_send_command(qspi, ENDIAN_BE32(tmp), 0, COMMAND_4_BYTE);
		}
	}

	/* Wait for completion; max page program time 750us */
	for (loop = 100000; loop > 0; loop--) {
		if (!qspi_flash_chip_busy(qspi)) {
			return (EOK);
		}

		nanospin_ns(1000);
	}

	fprintf(stderr, "%s() failed to program QSPI Flash at 0x%x\n", __func__, offset);
	return (EIO);
}

int qspi_ssr_read(qspi_dev_t *qspi, int offset, int32_t size, uint8_t *buf)
{
	if (verbose) {
		fprintf(stderr, "%s: offset 0x%x, size %d \n", __func__, offset, size);
	}

	/* Select configuration port */
	qspi_mode_config(qspi, QSPI_MODE_CPORT);

	/* Send OTP read command */
	qspi_send_command(qspi, QSPI_CMD_OTP_READ, size + 2, COMMAND_1_BYTE);

	/* Send 24bit address */
	qspi_send_command(qspi, offset, 0, COMMAND_3_BYTE);

	/* Send dummy byte */
	qspi_send_command(qspi, 0x0, 0, COMMAND_1_BYTE);

	/* Read SSR data */
	while (size--) {
		*buf++ = qspi_read_data(qspi, 0, COMMAND_1_BYTE, qspi->read_mode);
	}
	return EOK;
}


/* local variables */
static qspi_ssr_t s25fl = {
	1024,
	s25fl_ssr_lock_region,
	s25fl_ssr_lockall,
	s25fl_ssr_stat,
	qspi_ssr_read,
	qspi_ssr_write
};

static qspi_ssr_t n25q = {
	65,
	NULL,
	n25q_ssr_lockall,
	n25q_ssr_stat,
	qspi_ssr_read,
	qspi_ssr_write
};

/**************************************
 * QSPI OTP access MTD API functon 
***************************************/

int f3s_qspi_v2ssrop(f3s_dbase_t *dbase,
			f3s_access_t *access,
			uint32_t op,
			uint32_t offset,
			int32_t size,
			uint8_t *buf)
{
	int ret = EOK;

	/* Check ssr offset */
	if (access->service->page(&access->socket, F3S_POWER_ALL, offset, NULL) == NULL) {
		fprintf(stderr, "(devf  t%d::%s:%d) page() returned NULL \n",
					pthread_self(), __func__, __LINE__);
		errno = EIO;
		return (-1);
	}

	/* Choose SSR object based on flash parts */
	if (qspi_ssr == NULL) {
		if (dbase->jedec_hi == MID_SPANSION) {
			qspi_ssr = &s25fl;
		} else if (dbase->jedec_hi == MID_MICRON) {
			qspi_ssr = &n25q;
		} else {
			fprintf(stderr, "%s: OTP access not supported for this part \n", __func__);
			errno = ENOTSUP;
			return (-1);
		}
	}

	/* Sanity check for offset and size */
	if (op == F3S_SSR_OP_READ || op == F3S_SSR_OP_WRITE) {
		if (offset + size > qspi_ssr->maxsz) {
			fprintf(stderr, "%s [off..0x%x, size..0x%x] out of scope [0x%x]\n",
				 __func__, offset, size, qspi_ssr->maxsz);
			errno = EINVAL;
			return (-1);
		}
	}

	switch (op) {
	case F3S_SSR_OP_LOCK:
		if (size == 0) {
			ret = qspi_ssr->lockall((qspi_dev_t *)(access->socket.socket_handle));
		} else {
			if (qspi_ssr->lock_region != NULL) {
				ret = qspi_ssr->lock_region((qspi_dev_t *)(access->socket.socket_handle), buf[0]);
			}
		}
		break;
	case F3S_SSR_OP_STAT:
		ret = qspi_ssr->stat((qspi_dev_t *)(access->socket.socket_handle), size, buf);
		break;
	case F3S_SSR_OP_WRITE:
		ret = qspi_ssr->write((qspi_dev_t *)(access->socket.socket_handle), offset, size, buf);
		break;
	case F3S_SSR_OP_READ:
		ret = qspi_ssr->read((qspi_dev_t *)(access->socket.socket_handle), offset, size, buf);
		break;
	default:
		errno = ENOTSUP;
		ret = -1;
		break;
	}
	return (ret);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION( "$URL: http://svn.ott.qnx.com/product/branches/6.6.0/trunk/hardware/flash/boards/ti_qspi/f3s_qspi_ssr.c $ $Rev: 761898 $" );
#endif
