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

static int qspi_edma_wait(qspi_dev_t *qspi);

static inline void qspi_wait_for_complete(qspi_dev_t *qspi)
{
    while ((in32(qspi->core_base + QSPI_SPI_STATUS) & QSPI_STATUS_BUSY))
        ;
}

static void qspi_mode_config(qspi_dev_t *qspi, int mode)
{
    if (mode == QSPI_MODE_MMAP) {
        if (qspi->mode != QSPI_MODE_MMAP) {
            // memory mode
            out32(qspi->core_base + QSPI_SPI_SWITCH, QSPI_SWITCH_MMPT_S); 
            out32(qspi->ctrlcore_control_io2, (in32(qspi->ctrlcore_control_io2) & ~(QSPI_MEMMAPPED_CS_MASK)) | (QSPI_CS0 << QSPI_MEMMAPPED_CS_SHIFT));
            qspi->mode = QSPI_MODE_MMAP;
        }
    } else if (mode == QSPI_MODE_CPORT) {
        if (qspi->mode != QSPI_MODE_CPORT) {
            // select configuration port
            out32(qspi->core_base + QSPI_SPI_SWITCH, 0x0); 
            out32(qspi->ctrlcore_control_io2, (in32(qspi->ctrlcore_control_io2) & ~(QSPI_MEMMAPPED_CS_MASK)) | (QSPI_MMR << QSPI_MEMMAPPED_CS_SHIFT));
            qspi->mode = QSPI_MODE_CPORT;
        }
    }
}

static int qspi_send_command(qspi_dev_t *qspi, uint8_t cmd, int flen)
{
    // issue status read command
    out32(qspi->core_base + QSPI_SPI_DATA, cmd);
    out32(qspi->core_base + QSPI_SPI_CMD, (0 << QSPI_CMD_CSNUM_SHIFT)   // CS0
                                       | (7 << QSPI_CMD_WLEN_SHIFT)     // 8 bit word
                                       | (flen << QSPI_CMD_FLEN_SHIFT)  // 2 words frame
                                       | QSPI_CMD_4PIN_WRITE_SINGLE);   // 4 pin write single

    qspi_wait_for_complete(qspi);

    return EOK;
}

/*
 * Send 4 bytes address to QSPI flash
 */
static int qspi_send_address(qspi_dev_t *qspi, uint32_t offset, int flen)
{
    // issue 4 bytes sector address
    out32(qspi->core_base + QSPI_SPI_DATA, offset);
    out32(qspi->core_base + QSPI_SPI_CMD, (0 << QSPI_CMD_CSNUM_SHIFT)   // CS0
                                       | (31 << QSPI_CMD_WLEN_SHIFT)    // 32 bit word
                                       | (flen << QSPI_CMD_FLEN_SHIFT)  // 1 words frame
                                       | QSPI_CMD_4PIN_WRITE_SINGLE);   // 4 pin write single
    qspi_wait_for_complete(qspi);

    return EOK;
}

/*
 * Read register from QSPI flash
 */
static int qspi_read_register(qspi_dev_t *qspi, uint8_t cmd)
{
    qspi_send_command(qspi, cmd, 1);

    out32(qspi->core_base + QSPI_SPI_CMD, (0 << QSPI_CMD_CSNUM_SHIFT)   // CS0
                                       | (7 << QSPI_CMD_WLEN_SHIFT)     // 8 bit word
                                       | (1 << QSPI_CMD_FLEN_SHIFT)     // 2 words frame
                                       | QSPI_CMD_4PIN_READ_SINGLE);    // 4 pin read ingle

    qspi_wait_for_complete(qspi);

    return (in32(qspi->core_base + QSPI_SPI_DATA));
}

/*
 * QSPI flash reset
 */
int qspi_flash_reset(qspi_dev_t *qspi)
{
    qspi_send_command(qspi, QSPI_CMD_RST, 0);

    return (EOK);
}

/*
 * QSPI flash erase suspend
 */
int qspi_flash_erase_suspend(qspi_dev_t *qspi)
{
    int     loop;

    if (qspi->suspended)
        return (EOK);

    // Select configuration port
    qspi_mode_config(qspi, QSPI_MODE_CPORT);

    qspi_send_command(qspi, QSPI_CMD_ERSP, 0);

    // max 45us according to spec
    for (loop = 64; loop > 0; loop--) {
        if (!(qspi_read_register(qspi, QSPI_CMD_RDSR) & FLASH_WIP)) {
            if ((qspi_read_register(qspi, QSPI_CMD_RDSR2) & FLASH_ES) == 0)   // no WIP, no suspend, so erase completed
                return (ECANCELED);

            // erase is suspended
            qspi->suspended = 1;

            return (EOK);
        }
        nanospin_ns(1000);
    }

    return (EIO);
}

/*
 * QSPI flash erase resume
 */
int qspi_flash_erase_resume(qspi_dev_t *qspi)
{
    if (!qspi->suspended)
        return (EOK);

    qspi_send_command(qspi, QSPI_CMD_ERRS, 0);

    nanospin_ns(100 * 1000);

    qspi->suspended = 0;

    return (EOK);
}

/*
 * Read identification code from flash
 */
int qspi_flash_read_ident(qspi_dev_t *qspi, uint8_t *buf, int nbyte)
{
    // Select  configuration port
    qspi_mode_config(qspi, QSPI_MODE_CPORT);

    qspi_send_command(qspi, QSPI_CMD_RDID, nbyte);

    while (nbyte--) {
        out32(qspi->core_base + QSPI_SPI_CMD, (0 << QSPI_CMD_CSNUM_SHIFT)   // CS0
                                           | (nbyte << QSPI_CMD_FLEN_SHIFT) // frame size
                                           | (QSPI_CMD_4PIN_READ_SINGLE)    // 4 pin read single
                                           | (7 << QSPI_CMD_WLEN_SHIFT));   // 8 bit word
        qspi_wait_for_complete(qspi);
        *buf = in32(qspi->core_base + QSPI_SPI_DATA);
        buf++;
    }

    return (EOK);
}

/*
 * Check flash busy bit
 */
int qspi_flash_chip_busy(qspi_dev_t *qspi)
{
    // configuration port is selected
    qspi_mode_config(qspi, QSPI_MODE_CPORT);

    return (qspi_read_register(qspi, QSPI_CMD_RDSR) & FLASH_WIP);
}

/*
 * QSPI sector erase
 */
int qspi_flash_sector_erase(qspi_dev_t *qspi, int offset, int sa_bits)
{
    uint32_t    erase_cmd;

    if (sa_bits > 12)
        erase_cmd = QSPI_CMD_4SE;
    else
        erase_cmd = QSPI_CMD_4P4E;

    // configuration port is selected
    qspi_mode_config(qspi, QSPI_MODE_CPORT);

    // enable write
    qspi_send_command(qspi, QSPI_CMD_WREN, 0);

    // send erase command
    qspi_send_command(qspi, erase_cmd, 1);

    // issue 4 bytes sector address
    return qspi_send_address(qspi, offset, 2);
}

/*
 * QSPI page program
 */
int qspi_flash_page_program(qspi_dev_t *qspi, uint32_t offset, int size, uint8_t *buf)
{
    int     loop;

    // configuration port is selected
    qspi_mode_config(qspi, QSPI_MODE_CPORT);

    qspi_send_command(qspi, QSPI_CMD_WREN, 0);

    if ((((uint32_t)buf) & 3) || (offset & 3) || (size & 3)) {
        uint8_t    *pbuf = buf;
        int         flen = size + 1;

        // issue 4 bytes address page program command
        qspi_send_command(qspi, QSPI_CMD_4PP, flen);

        // issue 4 bytes address
        qspi_send_address(qspi, offset, flen);

        // write data
        while (size) {
            out32(qspi->core_base + QSPI_SPI_DATA, *pbuf++);
            out32(qspi->core_base + QSPI_SPI_CMD, (0 << QSPI_CMD_CSNUM_SHIFT)   // CS0
                                            | (flen << QSPI_CMD_FLEN_SHIFT)
                                            | (QSPI_CMD_4PIN_WRITE_SINGLE)      // 4 pin write single
                                            | (7 << QSPI_CMD_WLEN_SHIFT));      // 7 bit word
            qspi_wait_for_complete(qspi);
            --size;
        }
    } else {
        uint32_t    *pbuf = (uint32_t *)buf;
        uint32_t    flen = (size >> 2) + 1;
        uint32_t    tmp;

        // issue 4 bytes address page program command
        qspi_send_command(qspi, QSPI_CMD_4PP, flen);

        // issue 4 bytes address
        qspi_send_address(qspi, offset, flen);

        // write data
        while (size) {
            tmp = *pbuf++;
            out32(qspi->core_base + QSPI_SPI_DATA, ENDIAN_BE32(tmp));
            out32(qspi->core_base + QSPI_SPI_CMD, (0 << QSPI_CMD_CSNUM_SHIFT)    // CS0
                                       | (flen << QSPI_CMD_FLEN_SHIFT)
                                       | (QSPI_CMD_4PIN_WRITE_SINGLE)       // 4 pin write single
                                       | (31 << QSPI_CMD_WLEN_SHIFT));      // 32 bit word
            qspi_wait_for_complete(qspi);
            size -= 4;
        }
    }

    // max page program time 750us
    for (loop = 1000; loop > 0; loop--) {
        if (!qspi_flash_chip_busy(qspi))
            return (EOK);

        nanospin_ns(1000);
    }

    return (EIO);
}

/*
 * QSPI EDMA read
 */
static int qspi_flash_edma_read(qspi_dev_t *qspi, int offset, uint8_t *buf, int size)
{
    edma_param  *param;
    uintptr_t   base = qspi->edma_base + DRA7XX_SHADOW0_OFF;
    int         rc = EOK;

#if 0
    // in case there is pending event
    if (qspi->edma_chnl >= 32)
        out32(base + DRA7XX_EDMA_ECR + 4, 1 << (qspi->edma_chnl - 32));
    else
        out32(base + DRA7XX_EDMA_ECR, 1 << qspi->edma_chnl);
#endif

    /*
     * Initialize DMA channel
     */
    param = (edma_param *)(qspi->edma_base + DRA7XX_PARAM(qspi->edma_chnl));
    param->opt =  (0 << 23)     /* ITCCHEN = 0 */
                | (0 << 22)     /* TCCHEN = 0 */
                | (0 << 21)     /* */
                | (1 << 20)     /* Transfer complete interrupt enable */
                | (qspi->edma_event << 12)    /* TCC */
                | (0 << 11)     /* Normal completion */
                | (0 << 3)      /* PaRAM set is not static */
                | (1 << 2)      /* A-synchronizad */
                | (0 << 1)      /* Destination address increment mode */
                | (0 << 0);     /* Source address increment mode */

    param->src          = J6QSPI_SFI_MM_BASE + offset;
    param->abcnt        = (1 << 16) | size;
    param->dst          = qspi->p_dmabuf;
    param->srcdstbidx   = 0;
    param->linkbcntrld  = 0xFFFF;
    param->srcdstcidx   = 0;
    param->ccnt         = 1;

    qspi->irq_expected  = 1;

    CACHE_INVAL(&qspi->cachectl, qspi->v_buf, qspi->p_dmabuf, size);

    /*
     * Trigger transfer event
     */
    if (qspi->edma_chnl >= 32)
        out32(base + DRA7XX_EDMA_ESR + 4, 1 << (qspi->edma_chnl - 32));
    else
        out32(base + DRA7XX_EDMA_ESR, 1 << qspi->edma_chnl);

    rc = qspi_edma_wait(qspi);

    if (rc == EOK) {
        CACHE_INVAL(&qspi->cachectl, qspi->v_buf, qspi->p_dmabuf, size);
        if (buf != qspi->v_buf)
            memcpy(buf, qspi->v_buf, size);

        return size;
    }

    return 0;
}

/*
 * quad read
 */
int qspi_flash_read(qspi_dev_t *qspi, int offset, uint8_t *buf, int size)
{
    uint8_t *memory;

    // memory mode
    qspi_mode_config(qspi, QSPI_MODE_MMAP);

    if ((size >= EDMA_THRESHOLD) && qspi->edma_chnl != -1)
        return qspi_flash_edma_read(qspi, offset, buf, size);

    memory = qspi->memory + offset;

    memcpy(buf, memory, size);

    return size;    // return number of bytes read
}

/*
 * Enable chip quad mode
 */
int qspi_flash_quad_enable(qspi_dev_t *qspi)
{
    unsigned sr1, cr;

    // Select configuration port
    qspi_mode_config(qspi, QSPI_MODE_CPORT);

    // enable write
    qspi_send_command(qspi, QSPI_CMD_WREN, 0);

    // Read Status register 1
    sr1 = qspi_read_register(qspi, QSPI_CMD_RDSR);

    // Read Configuration Register
    cr  = qspi_read_register(qspi, QSPI_CMD_RDCR);

    // Quad already enabled?
    if ((cr & 0x02))
        return (EOK);

    // Set Configuration register 2nd bit to 1 for Quad enable
    cr |= 0x2;
    
    // Write CR register
    out32(qspi->core_base + QSPI_SPI_DATA, (QSPI_CMD_WRREG << 16) | (sr1 << 8 ) | cr);
    qspi_wait_for_complete(qspi);
    out32(qspi->core_base + QSPI_SPI_CMD, (0 << QSPI_CMD_CSNUM_SHIFT)   // CS0
                                       | (0x0 << QSPI_CMD_FLEN_SHIFT)   // 1 word frame
                                       | (QSPI_CMD_4PIN_WRITE_SINGLE)   // 4 pin write single
                                       | (23 << QSPI_CMD_WLEN_SHIFT));  // 24 bit word
    qspi_wait_for_complete(qspi);

    // Verify Configuration Register
    cr = qspi_read_register(qspi, QSPI_CMD_RDCR);

    // Quad already enabled?
    if ((cr & 0x02))
        return (EOK);

    return (ENODEV);
}

/*
 * EDMA interrupt handler
 */
static const struct sigevent *qspi_intr(void *area, int id)
{
    qspi_dev_t  *qspi = area;

    if (qspi->irq_expected) {
        qspi->irq_expected = 0;

        return &qspi->event;
    }

    return NULL;
}

/*
 * Attach EDMA interrupt
 */
int qspi_attach_irq(qspi_dev_t *qspi)
{
    if ((qspi->chid = ChannelCreate(_NTO_CHF_DISCONNECT | _NTO_CHF_UNBLOCK)) == -1)
        return -1;

    if ((qspi->coid = ConnectAttach(0, 0, qspi->chid, _NTO_SIDE_CHANNEL, 0)) == -1)
        goto fail1;

    qspi->event.sigev_notify   = SIGEV_PULSE;
    qspi->event.sigev_coid     = qspi->coid;
    qspi->event.sigev_code     = QSPI_EDMA_EVENT;
    qspi->event.sigev_priority = 21;

    /*
     * Attach interrupt
     */
    qspi->iid = InterruptAttach(qspi->edma_irq, qspi_intr, qspi, 0, _NTO_INTR_FLAGS_TRK_MSK);
    if (qspi->iid == -1) {
        perror("InterruptAttach");
        goto fail2;
    }

    return EOK;

fail2:
    InterruptDetach(qspi->iid);
fail1:
    ChannelDestroy(qspi->chid);

    return -1;
}

/*
 * Wait for EDMA transfer complete
 */
static int qspi_edma_wait(qspi_dev_t *qspi)
{
    struct _pulse   pulse;
    uint64_t        ntime = 1e9;

    for (;;) {
        TimerTimeout(CLOCK_REALTIME, _NTO_TIMEOUT_RECEIVE, NULL, &ntime, NULL);
        if (MsgReceivePulse(qspi->chid, &pulse, sizeof(pulse), NULL) == -1) {
            qspi->irq_expected = 0;
            fprintf(stderr, "PID_%d %s(%d) timeout.\n", getpid(), strerror(errno), errno);
            return (EIO);
        }

        if (pulse.code == QSPI_EDMA_EVENT)
            return EOK;
    }
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/flash/boards/j6evm-qspi/qspi_cmds.c $ $Rev: 738509 $")
#endif
