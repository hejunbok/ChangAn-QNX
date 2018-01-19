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
#include "f3s_qspi.h"

/*
 * This is the write callout for SPI serial NOR flash.
 */
int32_t f3s_qspi_write(f3s_dbase_t *dbase,
						f3s_access_t *access,
						uint32_t flags,
						uint32_t offset,
						int32_t size,
						uint8_t *buffer)
{
	int	rc;
	void *cmpbuf;
	void *pbuf = buffer;
	int	pagesz = dbase->buffer_size;
	int	left;
	int	cursz;
	int	off = offset;
	qspi_dev_t *qspi = (qspi_dev_t *)access->socket.socket_handle;

	if (access->service->page(&access->socket, 0, offset, &size) == NULL)
		return -1;

	left = size;

	while (left) {
		cursz = pagesz - (off & (pagesz - 1));
		if (cursz > left)
			cursz = left;

		// write data to flash
		if (dbase->jedec_hi == MID_SPANSION) {
			rc = qspi_flash_s25fl_page_program(qspi, off, cursz, buffer);
		} else if (dbase->jedec_hi == MID_MICRON) {
			rc = qspi_flash_n25q_page_program(qspi, off, cursz, buffer);
		} else {
			errno = EIO;
			return -1;
		}
		if (rc != EOK) {
			errno = EIO;
			return -1;
		}

		off += cursz, buffer += cursz, left -= cursz;
	}

	if (!(flags & F3S_VERIFY_WRITE))
		return size;

	cmpbuf = qspi->v_buf;

	qspi_flash_read(qspi, offset, cmpbuf, size);

	// verify
	if (memcmp(cmpbuf, pbuf, size)) {
		fprintf(stderr, "(devf	t%d::%s:%d) program verify error\n"
						"between offset 0x%x and 0x%x, size = %d\n",
						pthread_self(), __func__, __LINE__,
						offset, offset + size, size);
		errno = EIO;
		size = -1;
	}

	// verify successful
	return size;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/6.6.0/trunk/hardware/flash/boards/ti_qspi/f3s_qspi_write.c $ $Rev: 761898 $")
#endif
