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
 * This is the reset callout for QSPI serial NOR flash.
 */
void f3s_qspi_s25fl_reset(f3s_dbase_t *dbase,
					f3s_access_t *access,
					uint32_t flags,
					uint32_t offset)
{
	qspi_flash_reset((qspi_dev_t *)(access->socket.socket_handle));
	return;
}


void f3s_qspi_n25q_reset(f3s_dbase_t *dbase,
					f3s_access_t *access,
					uint32_t flags,
					uint32_t offset)
{
	return;
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/6.6.0/trunk/hardware/flash/boards/ti_qspi/f3s_qspi_reset.c $ $Rev: 744783 $")
#endif
