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

extern char *bsargs;

/*
 * This is the main function for the SPI f3s flash file system.
 */
int main(int argc, char **argv)
{
	int error;
	static f3s_service_t service[] =
	{
		{
			sizeof(f3s_service_t),
			f3s_qspi_open,
			f3s_qspi_page,
			f3s_qspi_status,
			f3s_qspi_close
		},
		{
			0, 0, 0, 0, 0	/* mandatory last entry */
		}
	};

	static f3s_flash_v2_t flash[] =
	{
		{	/* Spansion S25FL256 family QSPI Flash*/
			sizeof(f3s_flash_v2_t),
			f3s_qspi_s25fl_ident,		/* Ident	*/
			f3s_qspi_s25fl_reset,		/* Reset	*/
			NULL,						/* Read		*/
			NULL,						/* Write	*/
			NULL,						/* Erase	*/
			NULL,						/* Suspend	*/
			NULL,						/* Resume	*/
			NULL,						/* Sync		*/
			f3s_qspi_read,				/* QSPI Read*/
			f3s_qspi_write,				/* Write	*/
			f3s_qspi_s25fl_erase,		/* Erase	*/
			f3s_qspi_suspend,			/* Suspend	*/
			f3s_qspi_resume,			/* Resume	*/
			f3s_qspi_sync,				/* Sync		*/
			NULL,						/* Islock	*/
			NULL,						/* Lock		*/
			NULL,						/* Unlock	*/
			NULL,						/* Unlockall*/
			f3s_qspi_v2ssrop,
		},
		{	/* Micron N25Q128 family QSPI Flash*/
			sizeof(f3s_flash_v2_t),
			f3s_qspi_n25q_ident,		/* Ident	*/
			f3s_qspi_n25q_reset,		/* Reset 	*/
			NULL,						/* Read		*/
			NULL,						/* Write	*/
			NULL,						/* Erase	*/
			NULL,						/* Suspend	*/
			NULL,						/* Resume	*/
			NULL,						/* Sync		*/
			f3s_qspi_read,				/* Read		*/
			f3s_qspi_write,				/* Write	*/
			f3s_qspi_n25q_erase,		/* Erase	*/
			f3s_qspi_suspend,			/* Suspend	*/
			f3s_qspi_resume,			/* Resume	*/
			f3s_qspi_sync,				/* Sync		*/
			NULL,						/* Islock	*/
			NULL,						/* Lock		*/
			NULL,						/* Unlock	*/
			NULL,						/* Unlockall */
			f3s_qspi_v2ssrop,
		},
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	/* mandatory last entry */
		}
	};

	if (strstr(argv[argc - 1], "clk") != NULL || strstr(argv[argc - 1], "edma") != NULL)
		bsargs = argv[--argc];

	/* init f3s */
	f3s_init(argc, argv, (f3s_flash_t *)flash);

	/* start f3s */
	error = f3s_start(service, (f3s_flash_t *)flash);

	return error;
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/6.6.0/trunk/hardware/flash/boards/ti_qspi/f3s_qspi_main.c $ $Rev: 759769 $")
#endif
