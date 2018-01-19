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
 * This is the ident callout for SPI serial NOR flash.
 */

int32_t f3s_qspi_ident(f3s_dbase_t *dbase,
                       f3s_access_t *access,
                       uint32_t flags,
                       uint32_t offset)
{
    int32_t     unit_size;
    int32_t     geo_index;
    int32_t     geo_pos;
    uint8_t     buf[88];
    int32_t     chip_size = 0;
    qspi_dev_t  *qspi = (qspi_dev_t *)access->socket.memory;

    qspi_flash_read_ident(qspi, buf, 81);

    if (buf[0x10] != 'Q' || buf[0x11] != 'R' || buf[0x12] != 'Y' ||
            buf[0x40] != 'P' || buf[0x41] != 'R' || buf[0x42] != 'I') {
        fprintf(stderr, "\n(devf  t%d::%s:%d) Identifed not supported \n", pthread_self(), __func__, __LINE__);
        fprintf(stderr, "%02x %02x %02x\n", buf[0x10], buf[0x11], buf[0x12]);
        return (ENOTSUP);
    }

    /* Fill dbase entry */
    dbase->struct_size = sizeof(*dbase);
    dbase->jedec_hi    = buf[0];
    dbase->jedec_lo    = ((uint16_t)buf[1] << 8) | buf[2];
    dbase->name        = "CFI_MirrorBit_SIO";

    /* Read buffer size information */
    dbase->buffer_size = buf[0x2b];
    dbase->buffer_size <<= 8;
    dbase->buffer_size += buf[0x2a];

    /* Value is 2^N bytes per chip */
    dbase->buffer_size = 1 << dbase->buffer_size;

    /* Read number of geometries */
    dbase->geo_num = buf[0x2c];
    if (verbose > 3)
        fprintf(stderr, "(devf  t%d::%s:%d) dbase->geo_num = %d\n", pthread_self(), __func__, __LINE__, dbase->geo_num);

    /* Read geometry information */
    for (geo_index = 0, geo_pos = 0x2d; geo_index < dbase->geo_num; geo_index++, geo_pos += 4) {
        /* Read number of units */
        dbase->geo_vect[geo_index].unit_num   = buf[geo_pos + 1];
        dbase->geo_vect[geo_index].unit_num <<= 8;
        dbase->geo_vect[geo_index].unit_num  += buf[geo_pos + 0];
        dbase->geo_vect[geo_index].unit_num  += 1;

        /* Read size of unit */
        unit_size   = buf[geo_pos + 3];
        unit_size <<= 8;
        unit_size  += buf[geo_pos + 2];

        /* Interpret according to the CFI specs */
        if (unit_size == 0) unit_size  = 128;
        else                unit_size *= 256;

        chip_size += unit_size * dbase->geo_vect[geo_index].unit_num;

        /* Convert size to power of 2 */
        dbase->geo_vect[geo_index].unit_pow2 = 0;
        while (unit_size > 1) {
            unit_size >>= 1;
            dbase->geo_vect[geo_index].unit_pow2++;
        }

        if (verbose > 3) {
            fprintf(stderr, "(devf  t%d::%s:%d) dbase->geo_vect[%d].unit_pow2 = %d\n",
                pthread_self(), __func__, __LINE__, geo_index,dbase->geo_vect[geo_index].unit_pow2);
            fprintf(stderr, "(devf  t%d::%s:%d) dbase->geo_vect[%d].unit_num  = %d\n",
                pthread_self(), __func__, __LINE__, geo_index,dbase->geo_vect[geo_index].unit_num);
        }
    }

    qspi->chip_size = chip_size;

    /* Detect read / write suspend */
    if      (buf[0x46] == 1) dbase->flags = F3S_ERASE_FOR_READ;
    else if (buf[0x46] == 2) dbase->flags = F3S_ERASE_FOR_READ | F3S_ERASE_FOR_WRITE;
    else                     dbase->flags = 0;

    /* Set bus and chip parameters */
    // not important
    dbase->chip_inter = 1;
    dbase->chip_width = 8;

    return (EOK);
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/flash/boards/j6evm-qspi/f3s_qspi_ident.c $ $Rev: 738509 $")
#endif
