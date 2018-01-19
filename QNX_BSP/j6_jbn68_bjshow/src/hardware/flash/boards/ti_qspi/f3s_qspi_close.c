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

/*
 * This is the close callout for QSPI serial NOR flash.
 */
void f3s_qspi_close(f3s_socket_t *socket, uint32_t flags)
{
	qspi_dev_t	*qspi = (qspi_dev_t *)socket->memory;

	out32(qspi->core_base + QSPI_SYSCONFIG, 0);			// force idle
	out32(qspi->core_base + QSPI_SPI_CLOCK_CNTRL, 0);	// disable clock

	if (qspi->edma_chnl != -1) {
		InterruptDetach(qspi->iid);
		ConnectDetach(qspi->coid);
		ChannelDestroy(qspi->chid);

		munmap(qspi->v_buf, EDMA_BUFLEN);
		munmap_device_io(qspi->edma_base, DRA74X_EDMA_SIZE);

		cache_fini(&qspi->cachectl);
	} else {
		free(qspi->v_buf);
	}

	munmap_device_memory(qspi->memory, socket->window_size);
	munmap_device_io(qspi->core_base, qspi->core_size);
	munmap_device_io(qspi->ctrlcore_control_io2, 4);

	free(qspi);
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/6.6.0/trunk/hardware/flash/boards/ti_qspi/f3s_qspi_close.c $ $Rev: 744783 $")
#endif
