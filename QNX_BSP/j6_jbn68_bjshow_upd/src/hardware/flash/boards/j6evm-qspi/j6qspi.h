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

#ifndef J6QSPI_H_
#define J6QSPI_H_

#include <sys/cache.h>

#define J6QSPI_CORE_BASE            0x4B300000
#define J6QSPI_CORE_SIZE            0x1000
#define J6QSPI_SFI_MM_BASE          0x5C000000
#define J6QSPI_SFI_MM_SIZE          (64 * 1024 * 1024)
#define CONTROL_CORE_CONTROL_IO_2   0x4A002558


#define QSPI_PID                    0x00
#define QSPI_SYSCONFIG              0x10
#define QSPI_INTR_STATUS_SET        0x20
#define QSPI_INTR_STATUS_CLEAR      0x24
#define QSPI_INTR_ENABLE_SET        0x28
#define QSPI_INTR_ENABLE_CLEAR      0x2C
 #define QSPI_INTR_WIRQ             (1 << 1)
 #define QSPI_INTR_FIRQ             (1 << 0)
#define QSPI_SPI_CLOCK_CNTRL        0x40
 #define QSPI_CLKEN                 (1 << 31)
#define QSPI_SPI_DC                 0x44
#define QSPI_SPI_CMD                0x48
 #define QSPI_CMD_FLEN_SHIFT        0
 #define QSPI_CMD_WLEN_SHIFT        19
 #define QSPI_CMD_CSNUM_SHIFT       28
 #define QSPI_CMD_WIRQ              (1 << 14)
 #define QSPI_CMD_FIRQ              (1 << 15)
 #define QSPI_CMD_4PIN_READ_SINGLE  (1 << 16)
 #define QSPI_CMD_4PIN_WRITE_SINGLE (2 << 16)
 #define QSPI_CMD_4PIN_READ_DUAL    (3 << 16)
 #define QSPI_CMD_3PIN_READ_SINGLE  (5 << 16)
 #define QSPI_CMD_3PIN_WRITE_SINGLE (6 << 16)
 #define QSPI_CMD_6PIN_READ_QUAD    (7 << 16)
#define QSPI_SPI_STATUS             0x4C
 #define QSPI_STATUS_BUSY           (1 << 0)
 #define QSPI_STATUS_WC             (1 << 1)
 #define QSPI_STATUS_FC             (1 << 2)
 #define QSPI_STATUS_WDCNT_SHIFT    16
 #define QSPI_STATUS_WDCNT_MASK     0xFFF
#define QSPI_SPI_DATA               0x50
#define QSPI_SPI_SETUP0             0x54
#define QSPI_SPI_SETUP1             0x58
#define QSPI_SPI_SETUP2             0x5C
#define QSPI_SPI_SETUP3             0x60
 #define QSPI_SETUP_READ_NORM       (0 << 12)
 #define QSPI_SETUP_READ_DUAL       (1 << 12)
 #define QSPI_SETUP_READ_QUAD       (3 << 12)
 #define QSPI_SETUP_DUMMY_NONE      (0 << 10)
 #define QSPI_SETUP_DUMMY_8BITS     (1 << 10)
 #define QSPI_SETUP_DUMMY_16BITS    (2 << 10)
 #define QSPI_SETUP_DUMMY_24BITS    (3 << 10)
 #define QSPI_SETUP_ADDR_1BYTE      (0 << 8)
 #define QSPI_SETUP_ADDR_2BYTE      (1 << 8)
 #define QSPI_SETUP_ADDR_3BYTE      (2 << 8)
 #define QSPI_SETUP_ADDR_4BYTE      (3 << 8)
 #define QSPI_SETUP_RCMD_SHIFT      0
 #define QSPI_SETUP_WCMD_SHIFT      16
#define QSPI_SPI_SWITCH             0x64
 #define QSPI_SWITCH_MM_INT_EN      (1 << 1)    // memory mapped mode interrupt enable
 #define QSPI_SWITCH_MMPT_S         (1 << 0)    // MPT select
#define QSPI_SPI_DATA1              0x68
#define QSPI_SPI_DATA2              0x6C
#define QSPI_SPI_DATA3              0x70

#define QSPI_MEMMAPPED_CS_MASK  0x700
#define QSPI_MEMMAPPED_CS_SHIFT 8
#define QSPI_MMR    0    
#define QSPI_CS0    1
#define QSPI_CS1    2
#define QSPI_CS2    3
#define QSPI_CS3    4

// Flash Device Operation Instruction
#define QSPI_CMD_RDSR       0x05    // read status register 1
#define QSPI_CMD_RDSR2      0x07    // read status register 2
#define QSPI_CMD_RDCR       0x35    // read configuration register 1
#define QSPI_CMD_RDID       0x9F    // read JEDEC-ID
#define QSPI_CMD_WREN       0x06    // write enable
#define QSPI_CMD_WRDI       0x04    // write disable
#define QSPI_CMD_4P4E       0x21    // 4KB sector erase, 4B address
#define QSPI_CMD_4SE        0xDC    // 64KB sector erase, 4B address
#define QSPI_CMD_4QOR       0x6C    // Quad read with 4 bytes address
#define QSPI_CMD_4QIOR      0xEC    // Quad I/O read with 4 bytes address
#define QSPI_CMD_4PP        0x12    // page program with 4 bytes address
#define QSPI_CMD_4QPP       0x34    // Quad page program with 4 bytes address
#define QSPI_CMD_WRREG      0x01    // write cr register
#define QSPI_CMD_REMS       0x90    // Read Identification
#define QSPI_CMD_ERSP       0x75    // erase suspend
#define QSPI_CMD_ERRS       0x7A    // erase resume

#define QSPI_CMD_RSTEN      0x66    // reset enable
#define QSPI_CMD_RST        0xF0    // software reset 
#define QSPI_CMD_EQIO       0x38    // enable Quad I/O
#define QSPI_CMD_RSTQIO     0xFF    // reset Quad I/O
#define QSPI_CMD_READ       0x03    // read memory
#define QSPI_CMD_HS_READ    0x0B    // high speed read
#define QSPI_CMD_SET_BURST  0xC0    // set burst
#define QSPI_CMD_READ_BURST 0x0C    // nB burst with wrap
#define QSPI_CMD_READ_PI    0x08    // read page index
#define QSPI_CMD_READ_I     0x09    // 
#define QSPI_CMD_READ_BI    0x10    // 
#define QSPI_CMD_READ_QJID  0xAF    // read Quad JEDEC-ID
#define QSPI_CMD_SEC_ERASE  0x20    // sector erase
#define QSPI_CMD_BLK_ERASE  0xD8    // block erase
#define QSPI_CMD_CHIP_ERASE 0xC7    // chip erase
#define QSPI_CMD_PAGE_PRGM  0x02    // page program
#define QSPI_CMD_WRITE_SUSP 0xB0    // write suspend
#define QSPI_CMD_WRITE_RSUM 0x30    // write resume
#define QSPI_CMD_READ_SID   0x88    // read security ID
#define QSPI_CMD_PRGM_SID   0xA5    // program user security ID area
#define QSPI_CMD_LOCK_SID   0x85    // lock out security ID proframming
#define QSPI_CMD_RBPR       0x72    // read block protection register
#define QSPI_CMD_WBPR       0x42    // write block protection register
#define QSPI_CMD_LBPR       0x8D    // lock down block protection register

#define FLASH_WIP           (1 << 0)    // write in progress
#define FLASH_ES            (1 << 1)    // erase suspended

#define EDMA_BUFLEN         (16 * 1024)
#define EDMA_THRESHOLD      64

typedef struct _qspi_dev_t
{
    uintptr_t   core_base;
    uintptr_t   ctrlcore_control_io2;
    uint8_t     *memory;
    int32_t     chip_size;      /* size of the flash chip */
    int32_t     suspended;
    uintptr_t   edma_base;

    int         edma_chnl;
    int         edma_event;
    int         edma_irq;
    int         clk;

    void        *v_buf;         // virtual address of DMA buffer, or write compare buffer
    uint32_t    p_dmabuf;       // pysical address of DMA buffer
    int         iid;            // EDMA interrupt ID
    int         chid;
    int         coid;
    struct sigevent     event;
#define QSPI_EDMA_EVENT 0x05
    int         irq_expected;
    int32_t     mode;           //
#define QSPI_MODE_MMAP      1   // memory mapped mode
#define QSPI_MODE_CPORT     0   // configuration port
#define QSPI_MODE_UNKNOWN   -1

    struct cache_ctrl   cachectl;
} qspi_dev_t;

int qspi_flash_read_ident(qspi_dev_t *, uint8_t* buf, int bytes);
int qspi_flash_sector_erase(qspi_dev_t *, int offset, int sa_bits); 
int qspi_flash_erase_suspend(qspi_dev_t *);
int qspi_flash_erase_resume(qspi_dev_t *);
int qspi_flash_page_program(qspi_dev_t *, uint32_t offset, int len, uint8_t* data);
int qspi_flash_read(qspi_dev_t *, int offset, uint8_t* buffer, int len);
int qspi_flash_chip_busy(qspi_dev_t *);
int qspi_flash_quad_enable(qspi_dev_t *);
int qspi_flash_reset(qspi_dev_t *);
int qspi_attach_irq(qspi_dev_t *);


typedef struct {
    volatile uint32_t   opt;
    volatile uint32_t   src;
    volatile uint32_t   abcnt;
    volatile uint32_t   dst;
    volatile uint32_t   srcdstbidx;
    volatile uint32_t   linkbcntrld;
    volatile uint32_t   srcdstcidx;
    volatile uint32_t   ccnt;
} edma_param;

#define DRA7XX_EDMA_BASE    0x43300000
#define DRA7XX_EDMA_SIZE    0x5000

#define DRA7XX_SHADOW0_OFF  0x2000
#define DRA7XX_PARAM_OFF    0x4000
#define DRA7XX_PARAM_SIZE   0x20
#define DRA7XX_PARAM(ch)    (DRA7XX_PARAM_OFF + (DRA7XX_PARAM_SIZE * ch))

#define DRA7XX_EDMA_ER      0x0000      /* ER Event Register */
#define DRA7XX_EDMA_ERH     0x0004      /* ER Event Register High */
#define DRA7XX_EDMA_ECR     0x0008      /* ER Event Clear Register */
#define DRA7XX_EDMA_ECRH    0x000C      /* ER Event Clear Register High */
#define DRA7XX_EDMA_ESR     0x0010      /* ER Event Set Register */
#define DRA7XX_EDMA_ESRH    0x0014      /* ER Event Set Register High */
#define DRA7XX_EDMA_EER     0x0020      /* ER Event Enable Register */
#define DRA7XX_EDMA_EERH    0x0024      /* ER Event Enable Register High */
#define DRA7XX_EDMA_EECR    0x0028      /* ER Event Enable Clear Register */
#define DRA7XX_EDMA_EECRH   0x002C      /* ER Event Enable Clear Register High */
#define DRA7XX_EDMA_EESR    0x0030      /* ER Event Enable Set Register */
#define DRA7XX_EDMA_EESRH   0x0034      /* ER Event Enable Set Register High */
#define DRA7XX_EDMA_IPR     0x0068      /* ER Interrupt Pending Register */
#define DRA7XX_EDMA_ICR     0x0070      /* ER Interrupt Clear Register */

#endif /* SPI_CMDS_H_ */


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/flash/boards/j6evm-qspi/j6qspi.h $ $Rev: 738509 $")
#endif
