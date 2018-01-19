/*
 * $QNXLicenseC:
 * Copyright 2009, QNX Software Systems.
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


#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <gulliver.h>
#include <sys/slog.h>
#include <sys/neutrino.h>
#include <fs/etfs.h>
#include <hw/inout.h>
#include "devio.h"


/**
 * This function loads the generated syndromes used for ecc error location detection
 */
void elm_load_syndromes(struct etfs_devio *dev, unsigned ecc_mode, uint8_t *syndrome_ecc, uint32_t nibbles, uint8_t poly)
{
    /* reg 0 */
    out32(base_elm + ELM_SYNDROME_FRAGMENT_0_(0), syndrome_ecc[0]       |
                                                 (syndrome_ecc[1] << 8) |
                                                 (syndrome_ecc[2] << 16)|
                                                 (syndrome_ecc[3] << 24));

    /* reg 1 */
    out32(base_elm + ELM_SYNDROME_FRAGMENT_1_(0), syndrome_ecc[4]       |
                                                 (syndrome_ecc[5] << 8) |
                                                 (syndrome_ecc[6] << 16)|
                                                 (syndrome_ecc[7] << 24));
    if(ECC_BCH8 == ecc_mode) {
        /* reg 2 */
        out32(base_elm + ELM_SYNDROME_FRAGMENT_2_(0), syndrome_ecc[8]         |
                                                     (syndrome_ecc[9] << 8)   |
                                                     (syndrome_ecc[10] << 16) |
                                                     (syndrome_ecc[11] << 24));
        /* reg 3 */
        out32(base_elm + ELM_SYNDROME_FRAGMENT_3_(0), syndrome_ecc[12]        |
                                                     (syndrome_ecc[13] << 8)  |
                                                     (syndrome_ecc[14] << 16) |
                                                     (syndrome_ecc[15] << 24));
    }
}

/**
 * This function uses the ELM (Error Location Module) to detect errors and their location
 */
int elm_error_check(struct etfs_devio *dev, unsigned ecc_mode, uint8_t *syndrome_ecc, uint32_t nibbles, uint32_t *error_count,uint32_t *error_locations)
{
    uint8_t poly = 0;
    uint32_t location_status;
    int i;

    elm_load_syndromes(dev, ecc_mode, syndrome_ecc, nibbles, poly);

    /* start processing */
    out32(base_elm + ELM_SYNDROME_FRAGMENT_6_(0), (in32(base_elm + ELM_SYNDROME_FRAGMENT_6_(0))) | ELM_SYNDROME_FRAGMENT_6_SYNDROME_VALID);

    /* wait for processing to complete */
    while((in32(base_elm + ELM_IRQSTATUS) & (0x1 << 0)) != 0x1);

    /* clear status */
    out32(base_elm + ELM_IRQSTATUS, in32(base_elm + ELM_IRQSTATUS) | (0x1 << 0));

    /* check if correctable */
    location_status = in32(base_elm + ELM_LOCATION_STATUS_(0));

    if(!(location_status & 0x100)) {
        /* clear status */
        out32(base_elm + ELM_IRQSTATUS, in32(base_elm + ELM_IRQSTATUS) | (0x1 << 0));
        return -1;
    }

    /* get error count */
    *error_count = in32(base_elm + ELM_LOCATION_STATUS_(0)) & 0x1f;

    for(i = 0;i < *error_count; i++) {
        error_locations[i] = in32(base_elm + ELM_ERROR_LOCATION_0_(i));
    }

    /* clear status */
    out32(base_elm + ELM_IRQSTATUS, in32(base_elm + ELM_IRQSTATUS));

    return 0;
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/etfs/nand2048/jacinto5_micron/elm.c $ $Rev: 767198 $")
#endif
