/*
 * $QNXLicenseC:
 * Copyright 2007, 2008, QNX Software Systems.
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

#include "externs.h"

// Display bytes of data in hex and ascii
//
void
display_data_bytes (char *name, _uint8 * ptr, int len, const char *dir)
{

	int     i, j, k;
	int     lbytes = 8;					// 8 bytes to one line
	char    hex_buf[(lbytes * 5) + 1];

	i = j = k = 0;

	// Display data 'lbytes' max on one line in hex
	while (i < len)
	{
		memset (hex_buf, 0, lbytes * 5 + 1);
		k = 0;
		do
		{
			// Save hex value in hex output buffer
			sprintf (hex_buf + k, "0x%2.2x ", ptr[i]);
			k += 5;
			i++;
		}
		while (k < (lbytes * 5) && i < len);

		if ((j = strlen (hex_buf)) < (lbytes * 5))
		{
			// Pad to full line length with spaces
			memset (hex_buf + j, 0x20, (lbytes * 5) - j);
		}

		if (i <= lbytes)
			slogf (_SLOG_SETCODE (_SLOGC_CHAR, 0), _SLOG_DEBUG1, "(%s %s size = %3d) - %s", dir, name, len, hex_buf);
		else
		{
			char pad[_POSIX_PATH_MAX] = { NULL };
			memset (pad, ' ', strlen(dir) + strlen(name));
			slogf (_SLOG_SETCODE (_SLOGC_CHAR, 0), _SLOG_DEBUG1, "%s                 %s", pad, hex_buf);
		}
	}
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devc/seromap_hci/utils.c $ $Rev: 765635 $")
#endif
