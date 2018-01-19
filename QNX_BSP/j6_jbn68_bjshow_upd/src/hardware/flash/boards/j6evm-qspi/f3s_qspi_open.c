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


extern paddr_t mphys(void *);

char *bsargs = NULL;

static char *supported_opts[] = {
    "edma",
    "event",
    "clk",
    NULL
};

static void qspi_board_option(qspi_dev_t *qspi)
{
    char    *value, *freeptr, *options;
    int     opt;

    qspi->edma_chnl  = -1;
    qspi->edma_event = -1;
    qspi->edma_irq   = -1;
    qspi->clk        = 64000000;

    if (bsargs == NULL)
        return;

    freeptr = strdup(bsargs);

    options = freeptr;

    while (options && *options != '\0') {
        opt = getsubopt(&options, supported_opts, &value);
        switch (opt) {
            case 0:
                qspi->edma_chnl  = strtoul(value, 0, 0);
                break;
            case 1:
                qspi->edma_event = strtoul(value, 0, 0);
                break;
            case 2:
                qspi->clk        = strtoul(value, 0, 0);
                break;
        }
    }

    if (qspi->edma_event == -1)
        qspi->edma_event = qspi->edma_chnl;

    if (qspi->edma_event != -1)
        qspi->edma_irq = 0x2000 + qspi->edma_event;

    free(freeptr);
}

/*
 * This is the open callout for the SPI serial NOR flash driver.
 */
int32_t f3s_qspi_open(f3s_socket_t *socket, uint32_t flags)
{
    qspi_dev_t  *qspi;
    int         clkdiv;

    if (socket->memory)
        return EOK;

    if (f3s_socket_option(socket)) {
        socket->address = J6QSPI_SFI_MM_BASE;
        socket->window_size = J6QSPI_SFI_MM_SIZE;
        socket->array_size = J6QSPI_SFI_MM_SIZE;
    }

    socket->name = (unsigned char*)"QSPI serial flash";

    socket->memory = calloc(1, sizeof(qspi_dev_t));
    qspi = (qspi_dev_t *)socket->memory;

    // check command line options
    qspi_board_option(qspi);

    qspi->memory = mmap_device_memory(0, socket->window_size,
                        PROT_READ|PROT_WRITE|PROT_NOCACHE, 0, socket->address);

    if ((qspi->core_base = mmap_device_io(J6QSPI_CORE_SIZE, J6QSPI_CORE_BASE)) == MAP_DEVICE_FAILED)
        goto fail5;
    if ((qspi->ctrlcore_control_io2 = mmap_device_io(4, CONTROL_CORE_CONTROL_IO_2)) == MAP_DEVICE_FAILED)
        goto fail7;
    if ((qspi->edma_base = mmap_device_io(DRA7XX_EDMA_SIZE, DRA7XX_EDMA_BASE)) == MAP_DEVICE_FAILED)
        goto fail8;

    qspi->mode = QSPI_MODE_UNKNOWN;

    out32(qspi->core_base + QSPI_SYSCONFIG, 2 << 2);     // smart idle

    clkdiv = 192000000 / qspi->clk;
    if (clkdiv) --clkdiv;

    // disable/clear interrupts
    out32(qspi->core_base + QSPI_INTR_ENABLE_CLEAR, QSPI_INTR_WIRQ | QSPI_INTR_FIRQ);
    out32(qspi->core_base + QSPI_INTR_STATUS_CLEAR, QSPI_INTR_WIRQ | QSPI_INTR_FIRQ);
    out32(qspi->core_base + QSPI_SPI_DC, (0 << 3) | (1 << 2) | (0 << 1) | (1 << 0));  
    out32(qspi->core_base + QSPI_SPI_CLOCK_CNTRL, clkdiv);
    in32 (qspi->core_base + QSPI_SPI_CLOCK_CNTRL);
    out32(qspi->core_base + QSPI_SPI_CLOCK_CNTRL, QSPI_CLKEN | clkdiv);
    out32(qspi->core_base + QSPI_SPI_SETUP0,  QSPI_CMD_4QOR << QSPI_SETUP_RCMD_SHIFT
                                          | QSPI_SETUP_ADDR_4BYTE
                                          | QSPI_SETUP_DUMMY_8BITS
                                          | QSPI_SETUP_READ_QUAD
                                          | QSPI_SETUP_READ_NORM
                                          | QSPI_CMD_4PP << QSPI_SETUP_WCMD_SHIFT
                                          | 0x0 << 24);

    // send command to device to enable QUAD mode
    if (qspi_flash_quad_enable(qspi) != EOK) {
        fprintf(stderr, "Quad enable failed\n");
        goto fail9;
    }

    if (qspi->edma_chnl == -1) {
        qspi->v_buf = malloc(EDMA_BUFLEN);
        return (EOK);
    }

    // map in DMA buffer
    if ((qspi->v_buf = mmap(NULL, EDMA_BUFLEN, PROT_READ | PROT_WRITE,   // | PROT_NOCACHE,
                                        MAP_PRIVATE | MAP_ANON | MAP_PHYS, NOFD, 0)) == MAP_FAILED) {
        goto fail9;
    }

    qspi->cachectl.fd = NOFD;
    if (cache_init(0, &qspi->cachectl, NULL) == -1) {
        fprintf(stderr, "cache init failed\n");
        goto fail10;
    }

    qspi->p_dmabuf = mphys(qspi->v_buf);

    // attach qspi interrupt
    if (qspi_attach_irq(qspi) == EOK)
        return (EOK);

fail10:
    munmap(qspi->v_buf, EDMA_BUFLEN);
fail9:
    munmap_device_io(qspi->edma_base, DRA7XX_EDMA_SIZE);
fail8:
    munmap_device_io(qspi->ctrlcore_control_io2, CONTROL_CORE_CONTROL_IO_2);
fail7:
    munmap_device_io(qspi->core_base, J6QSPI_CORE_SIZE);
fail5:
    munmap_device_memory(qspi->memory, socket->window_size);

    return (-1);
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/flash/boards/j6evm-qspi/f3s_qspi_open.c $ $Rev: 738509 $")
#endif
