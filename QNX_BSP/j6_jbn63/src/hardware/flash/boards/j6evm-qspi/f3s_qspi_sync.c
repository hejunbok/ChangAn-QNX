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
 * This is the sync callout for SPI serial NOR flash.
 */

int32_t f3s_qspi_sync(f3s_dbase_t *dbase, f3s_access_t *access, uint32_t flags, uint32_t offset)
{
    qspi_dev_t  *qspi = (qspi_dev_t *)access->socket.memory;
    int         rc;

    // check if offset does not fit in array
    if (offset >= qspi->chip_size)
        return (ERANGE);

    rc = qspi_flash_chip_busy(qspi);

    if (-1 == rc)
        return (EIO);

    if (FLASH_WIP == rc)    // write in progress
        return (EAGAIN);

    return (EOK);
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/flash/boards/j6evm-qspi/f3s_qspi_sync.c $ $Rev: 738509 $")
#endif
