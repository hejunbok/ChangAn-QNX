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


/*
 * This file contains definitions for QSPI serial NOR flash driver callouts.
 */

#ifndef __F3S_QSPI_H_INCLUDED
#define __F3S_QSPI_H_INCLUDED

#include <sys/mman.h>
#include <sys/f3s_mtd.h>
#include <sys/neutrino.h>
#include <hw/inout.h>
#include <pthread.h>

#include "j6qspi.h"


int32_t f3s_qspi_open(f3s_socket_t *socket,
                     uint32_t flags);

uint8_t *f3s_qspi_page(f3s_socket_t *socket,
                      uint32_t page,
                      uint32_t offset,
                      int32_t *size);

int32_t f3s_qspi_read(f3s_dbase_t *dbase,
                    f3s_access_t *access,
                    uint32_t flags,
                    uint32_t offset,
                    int32_t buffer_size,
                    uint8_t *buffer);
int32_t f3s_qspi_status(f3s_socket_t *socket,
                       uint32_t flags);

void f3s_qspi_close(f3s_socket_t *socket,
                   uint32_t flags);

int32_t f3s_qspi_ident(f3s_dbase_t * dbase, f3s_access_t * access,
			_uint32 flags, _uint32 offset);

int32_t f3s_qspi_write(f3s_dbase_t * dbase, f3s_access_t * access,
			_uint32 flags, _uint32 offset,
			_int32 size, _uint8 * buffer);

int f3s_qspi_erase(f3s_dbase_t * dbase, f3s_access_t * access,
			_uint32 flags, _uint32 offset);

int f3s_qspi_suspend(f3s_dbase_t *dbase, f3s_access_t *access,
                     uint32_t flags, uint32_t offset);

int f3s_qspi_resume(f3s_dbase_t *dbase, f3s_access_t *access,
                     uint32_t flags, uint32_t offset);

int32_t f3s_qspi_sync(f3s_dbase_t *dbase, f3s_access_t *access,
                    uint32_t flags, uint32_t offset);

void f3s_qspi_reset(f3s_dbase_t *dbase, f3s_access_t *access,
                    uint32_t flags, uint32_t offset);

#endif /* __F3S_QSPI_H_INCLUDED */


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/flash/boards/j6evm-qspi/f3s_qspi.h $ $Rev: 738509 $")
#endif
