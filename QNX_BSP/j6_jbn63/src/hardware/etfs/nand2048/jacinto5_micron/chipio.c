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


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/slog.h>
#include <sys/mman.h>
#include <sys/cache.h>
#include <string.h>
#include <sys/neutrino.h>
#include <hw/inout.h>
#include <fs/etfs.h>
#include <unistd.h>
#include <arm/dm6446.h>

#define DM816X_GPMC_BASE        0x50000000
#define DM816X_GPMC_SIZE        0x1000000

#define DM816X_ELM_BASE         0x48080000
#define DM816X_ELM_SIZE         0x1000000

#define DM816x_EDMA_BASE        0x49000000
#define DM816x_EDMA_BASE_IRQ    0x200
#define DM816x_EDMA_GPMC_EVENT  52

struct chipio;
#define CHIPIO    struct chipio
#include "./devio.h"


/* NAND Address split up macros */
#define NAND_ADDR_COL1(addr)    ((addr) & 0xff)                  // Column address CA[7:0]
#define NAND_ADDR_COL2(addr)    (((addr) & 0xf00) >> 8)          // Column address CA[11:8], CA[11]=1, then CA[10:6] must be "0"
#define NAND_ADDR_ROW1(page)    ((page) & 0xff)                  // Page address PA[5:0] and Block address BA[7:6]
#define NAND_ADDR_ROW2(page)    (((page) & 0xff00) >> 8)         // Block address BA[15:8]
#define NAND_ADDR_ROW3(page)    (((page) & 0x70000) >> 16)       // Block address BA[18:16]

/*
 * GPMC NAND Access Macros
 * Jacinto5 has a General-Purpose Memory Controller (GPMC)
 *
 * The GPMC includes command, address and data registers for accessing the NAND.
 *
 * NAND devices require multiple address programming phases. The software driver is responsible for
 * issuing the correct number of command and address program accesses, according to the device
 * command set and the device address-mapping scheme.
 */

/*
 *GPMC Macros
 */
/* The configuration register allows global configuration of the GPMC */
#define OMAP_GPMC_SYSCONFIG         (*((volatile uint32_t *)(cio->gpmc_vbase + 0x10)))
#define OMAP_GPMC_SYSSTATUS         (*((volatile uint32_t *)(cio->gpmc_vbase + 0x14)))
#define OMAP_GPMC_IRQSTATUS         (*((volatile uint32_t *)(cio->gpmc_vbase + 0x18)))
#define OMAP_GPMC_IRQENABLE         (*((volatile uint32_t *)(cio->gpmc_vbase + 0x1c)))
#define OMAP_GPMC_CONFIG            (*((volatile uint32_t *)(cio->gpmc_vbase + 0x50)))
#define OMAP_GPMC_STATUS            (*((volatile uint32_t *)(cio->gpmc_vbase + 0x54)))
#define OMAP_GPMC_CONFIG1           (*((volatile uint32_t *)(cio->gpmc_vbase + 0x60 + (0x30 * cio->cs))))
#define OMAP_GPMC_CONFIG2           (*((volatile uint32_t *)(cio->gpmc_vbase + 0x64 + (0x30 * cio->cs))))
#define OMAP_GPMC_CONFIG3           (*((volatile uint32_t *)(cio->gpmc_vbase + 0x68 + (0x30 * cio->cs))))
#define OMAP_GPMC_CONFIG4           (*((volatile uint32_t *)(cio->gpmc_vbase + 0x6C + (0x30 * cio->cs))))
#define OMAP_GPMC_CONFIG5           (*((volatile uint32_t *)(cio->gpmc_vbase + 0x70 + (0x30 * cio->cs))))
#define OMAP_GPMC_CONFIG6           (*((volatile uint32_t *)(cio->gpmc_vbase + 0x74 + (0x30 * cio->cs))))
#define OMAP_GPMC_CONFIG7           (*((volatile uint32_t *)(cio->gpmc_vbase + 0x78 + (0x30 * cio->cs))))
#define OMAP_NAND_COMMAND           (*((volatile uint16_t *)(cio->gpmc_vbase + 0x7C + (0x30 * cio->cs))))
#define OMAP_NAND_ADDRESS           (*((volatile uint16_t *)(cio->gpmc_vbase + 0x80 + (0x30 * cio->cs))))
#define OMAP_NAND_DATA              (*((volatile uint16_t *)(cio->gpmc_vbase + 0x84 + (0x30 * cio->cs))))
#define OMAP_NAND_COMMAND_8BIT      (*((volatile uint8_t *)(cio->gpmc_vbase + 0x7C + (0x30 * cio->cs))))
#define OMAP_NAND_ADDRESS_8BIT      (*((volatile uint8_t *)(cio->gpmc_vbase + 0x80 + (0x30 * cio->cs))))
#define OMAP_NAND_DATA_8BIT         (*((volatile uint8_t *)(cio->gpmc_vbase + 0x84 + (0x30 * cio->cs))))
#define OMAP_GPMC_ECC_CONFIG        (*((volatile uint32_t *)(cio->gpmc_vbase + 0x1F4)))
#define OMAP_GPMC_ECC_CONTROL       (*((volatile uint32_t *)(cio->gpmc_vbase + 0x1F8)))
#define OMAP_GPMC_ECC_SIZE_CONFIG   (*((volatile uint32_t *)(cio->gpmc_vbase + 0x1FC)))
#define GPMC_ECC_RESULT(x)          (*((volatile uint32_t *)(cio->gpmc_vbase + 0x200 + (x) * 4)))
#define GPMC_BCH_RESULT0(x)         (*((volatile uint32_t *)(cio->gpmc_vbase + 0x240 + (x) * 0x10)))
#define GPMC_BCH_RESULT1(x)         (*((volatile uint32_t *)(cio->gpmc_vbase + 0x244 + (x) * 0x10)))
#define GPMC_BCH_RESULT2(x)         (*((volatile uint32_t *)(cio->gpmc_vbase + 0x248 + (x) * 0x10)))
#define GPMC_BCH_RESULT3(x)         (*((volatile uint32_t *)(cio->gpmc_vbase + 0x24C + (x) * 0x10)))
#define GPMC_PREFETCH_CONFIG1       (*((volatile uint32_t *)(cio->gpmc_vbase + 0x1e0)))
#define GPMC_PREFETCH_CONFIG2       (*((volatile uint32_t *)(cio->gpmc_vbase + 0x1e4)))
#define GPMC_PREFETCH_CONTROL       (*((volatile uint32_t *)(cio->gpmc_vbase + 0x1ec)))
#define GPMC_PREFETCH_STATUS        (*((volatile uint32_t *)(cio->gpmc_vbase + 0x1f0)))

#define ECC_P1_128_E(val)           ((val)  & 0x000000FF)           // Bit 0 to 7
#define ECC_P512_2048_E(val)        (((val) & 0x00000F00) >> 8)     // Bit 8 to 11
#define ECC_P1_128_O(val)           (((val) & 0x00FF0000) >> 16)    // Bit 16 to Bit 23
#define ECC_P512_2048_O(val)        (((val) & 0x0F000000) >> 24)    // Bit 24 to Bit 27

#define OMAP_GPMC_CONFIG1_DEVICESIZE_MSK       0x3

/* OMAP_GPMC_ECC_CONFIG bits */
#define OMAP_GPMC_ECC_CONFIG_ECCENABLE          (1 << 0)
#define OMAP_GPMC_ECC_CONFIG_ECCDISABLE         (0 << 0)
#define OMAP_GPMC_ECC_CONFIG_ECCCS(val)         (val << 1)
#define OMAP_GPMC_ECC_CONFIG_ECCTOPSECTOR(val)  (val << 4)
#define OMAP_GPMC_ECC_CONFIG_ECC16B(val)        (val << 7)
#define OMAP_GPMC_ECC_CONFIG_ECCWRAPMODE(val)   (val << 8)
#define OMAP_GPMC_ECC_CONFIG_ECCBCHT4           (0 << 12)
#define OMAP_GPMC_ECC_CONFIG_ECCBCHT8           (1 << 12)
#define OMAP_GPMC_ECC_CONFIG_ECCBCHT16          (2 << 12)
#define OMAP_GPMC_ECC_CONFIG_ECCALGORITHM(val)  (val << 16)

/* OMAP_GPMC_ECC_CONFIG definitions */
#define HAMMING_CODE                            0
#define BCH_CODE                                1
#define ECC_SECTORS_1                           0x0
#define ECC_SECTORS_2                           0x1
#define ECC_SECTORS_4                           0x3

/* OMAP_GPMC_ECC_CONTROL bits */
#define OMAP_GPMC_ECC_CONTROL_ECCCLEAR          (1 << 8)
#define OMAP_GPMC_ECC_CONTROL_ECCPOINTER        (1 << 0)

/* OMAP_GPMC_ECC_SIZE_CONFIG bits */
#define OMAP_GPMC_ECC_SIZE_CONFIG_ECCSIZE0(val) (val << 12)
#define OMAP_GPMC_ECC_SIZE_CONFIG_ECCSIZE1(val) (val << 22)

/* GPMC_PREFETCH_CONFIG1 bits*/
#define GPMC_PREFETCH_CONFIG1_ACCESSMODE(val)       (val << 0)
#define GPMC_PREFETCH_CONFIG1_DMAMODE               (1 << 2)
#define GPMC_PREFETCH_CONFIG1_SYNCHROMODE           (0 << 3)
#define GPMC_PREFETCH_CONFIG1_ENABLEENGINE          (1 << 7)
#define GPMC_PREFETCH_CONFIG1_FIFOTHRESHOLD(val)    (val << 8)
#define GPMC_PREFETCH_CONFIG1_ENABLEOPTACCESS       (1 << 27)
#define GPMC_PREFETCH_CONFIG1_CYCOPTIMIZATION(val)  (val << 28)

/* GPMC_PREFETCH_CONFIG1 definitions*/
#define ACCESSMODE_PREFETCH     0
#define ACCESSMODE_WRITEPORT    1

/* GPMC_PREFETCH_CONTROL bits*/
#define GPMC_PREFETCH_CONTROL_STARTENGINE            (1 << 0)

/* GPMC_PREFETCH_STATUS bits*/
#define GPMC_PREFETCH_STATUS_FIFOPOINTER             0x7F000000

#define NUM_OF_SECTORS      4

/* CS0 access register (used to read/write to prefetch engine FIFO */
#define OMAP_CS0_REGISTER       (*((volatile uint32_t *)(cio->gpmc_cs0_vbase)))

#define OMAP_MAX_WAIT_PINS           4
#define OMAP_MAX_CHIP_SELECT         7
#define OMAP_MAX_BUSY_LOOP           10
#define OMAP_GPMC_WAITPIN0           0x100

typedef struct {
    volatile uint32_t   opt;
    volatile uint32_t   src;
    volatile uint32_t   abcnt;
    volatile uint32_t   dst;
    volatile uint32_t   srcdstbidx;
    volatile uint32_t   linkbcntrld;
    volatile uint32_t   srcdstcidx;
    volatile uint32_t   ccnt;
} edma_param_t;

#define EDMA_PRIORITY   22
#define EDMA_EVENT      1
#define GPMC_EVENT      1

#define DMA_BUF_SIZE    (PAGESIZE_2048 * 2)

static const struct sigevent *nand_intr(void *area, int id);
static void edma_disable(CHIPIO *cio);
static int edma_setup(CHIPIO *cio, paddr_t paddr, int len, int bWrite);
static int edma_init(CHIPIO *cio);
static int edma_attach_intr(CHIPIO *cio);
static void edma_dinit(void *hdl);
static int edma_wait(CHIPIO *cio, int len, int dtime);
static void dumpRegisters(CHIPIO *cio);

uint8_t verify_raw_partitions(CHIPIO *cio);

/*
 * Device specific data structure for the Jacinto5 board, with 2048 byte Micron NAND.
 */

struct chipio {
    struct _chipio  chip;
    uintptr_t       gpmc_pbase;
    uintptr_t       gpmc_vbase;
    uintptr_t       elm_pbase;
    uintptr_t       elm_vbase;
    uint8_t         cs;                 // GPMC chip select
    uint8_t         wp;                 // GPMC Wait pin
    uint8_t         cfg;
    uint8_t         noPFWP;             // Do not use prefetch/writepost engine
    uint8_t         cache;              // use cache buffer for DMA transfer

    uint8_t         ecc;                // Used for ecc mode id

    /* prefetch/write-post cs0 address*/
    uintptr_t       gpmc_cs0_pbase;
    uintptr_t       gpmc_cs0_vbase;

    uint32_t        ipl_maxblk;         // Used to indicate the max block used for IPL
    uint32_t        ifs_maxblk;         // Used to indicate the max block used for IFS file system
#ifdef BIOS
    uint32_t        bios_maxblk;        // Used to indicate the max block used for BIOS partition
#endif
    uint32_t        lowNAND;            // Used to indicate how many block used for IPL and IFS

    /* GPMC interrupt */
    int             irq_gpmc;
    int             iid_gpmc;
    struct sigevent gpmcevent;

    /* EDMA related */
    int             chid;
    int             coid;
    uint32_t        edmapbase;
    int             chnl_dma;
    int             irq_dma;
    uintptr_t       edmavbase;
    int             edma_iid;
    edma_param_t    *edma_param;
    void            *dmabuf;
    uint32_t        pdmabuf;
    struct sigevent edmaevent;

    struct cache_ctrl cachectl;

    int             reset_gpmc;
    int             wl;

    /* Read and Write optimization */
    int             r_opt;
    int             w_opt;
} chipio;


int
main(int argc, char *argv[])
{
    return(etfs_main(argc, argv));
}

/*
 * Process device specific options (if any).
 * This is always called before any access to the part.
 * It is called by the -D option to the file system. If no -D option is given
 * this function will still be called with "" for optstr.
 */
int devio_options(struct etfs_devio *dev, char *optstr)
{
    struct chipio   *cio;
    char            *value;
    static char     *opts[] ={
                    "use",      // 0
                    "gpmc",     // 1
                    "cs",       // 2
                    "wp",       // 3
                    "cfg",      // 4
                    "ifs",      // 5
                    "ipl",      // 6
                    "nopfwp",   // 7
                    "reset",    // 8
                    "elm",      // 9
                    "edma",     // 10
                    "cache",    // 11
                    "irq",      // 12
                    "read_opt", // 13
                    "write_opt", // 14
#ifdef BIOS
                    "bios",     // 15
#endif
                     NULL
                    } ;

    cio = dev->cio = &chipio;

    cio->gpmc_pbase  = DM816X_GPMC_BASE;
    cio->edmapbase   = DM816x_EDMA_BASE;
    cio->irq_dma     = DM816x_EDMA_BASE_IRQ + DM816x_EDMA_GPMC_EVENT;
    cio->chnl_dma    = DM816x_EDMA_GPMC_EVENT;
    cio->elm_pbase   = DM816X_ELM_BASE;
    cio->cs          = 0;            // Chip select
    cio->wp          = 0;            // Wait pin
    cio->cfg         = 0;            // do we need to touch the OMAP_GPMC_CONFIG reg.
    cio->ifs_maxblk  = 0;            // false by default unless specified in the command line
    cio->ipl_maxblk  = 0;            // false by default unless specified in the command line
#ifdef BIOS
    cio->bios_maxblk = 0;            // false by default unless specified in the command line
#endif
    cio->lowNAND     = 0;
    cio->ecc         = ECC_BCH4;     // indicate BHC4 ECC mode (4 bits ECC)
    cio->noPFWP      = 0;            // by default, prefetch/write-post engine is enabled
    cio->reset_gpmc  = 0;
    cio->cache       = 0;            // by default, use non-cache buffer for DMA transfer
    cio->irq_gpmc    = -1;
    cio->chid        = -1;
    cio->r_opt       = 0;
    cio->w_opt       = 0;
    while(*optstr) {
        switch (getsubopt(&optstr, opts, &value)) {
            case 0:
                fprintf(stderr, "Device specific options:(Prefetch and DMA are enabled by default)\n");
#ifdef BIOS
                fprintf(stderr, "  -D use,gpmc=xxxx,cs=[chip select 0-7],wp=[wait pin0-3],cfg,ifs=xxx,ipl=xxx,bios=xxx,nopfwp,reset,elm=xxxx,edma=xxx.x.x,cache,read_opt,write_opt\n");
#else
                fprintf(stderr, "  -D use,gpmc=xxxx,cs=[chip select 0-7],wp=[wait pin0-3],cfg,ifs=xxx,ipl=xxx,nopfwp,reset,elm=xxxx,edma=xxx.x.x,cache\n");
#endif
                fprintf(stderr, "\n\nnopfwp : disables prefetch engine (also disables dma)\n");
                fprintf(stderr, "edma  : EDMA base address, channel and IRQ\n");
                fprintf(stderr, "cache : use cacheable DMA buffer\n");
                fprintf(stderr, "reset : full reset of GPMC\n\n");
                fprintf(stderr, "read_opt/write_opt: Enables Read/Write optimization (default: off)");
#ifdef BIOS
                fprintf(stderr, "bios  : this is an optional partition in RAW NAND with 1-bit ECC protection (Currently only supported on 16-bit NAND IF)\n\n");
                fprintf(stderr, "NOTE : For non-zero values for ipl,ifs and bios, make sure ipl <ifs < bios\n\n");
#endif
                fprintf(stderr, "NOTE: If switching from no dma (nodma) back to full dma, use \"reset\" option\n\n");

                return (-1);
            case 1:
                cio->gpmc_pbase = strtoul(value, NULL, 0);
                break;
            case 2:
                cio->cs         = strtoul(value, NULL, 0);
                if(cio->cs >= OMAP_MAX_CHIP_SELECT) {
                    fprintf(stderr, "Chip select out of range 0-7\n");
                    return (EINVAL);
                }
                break;
            case 3:
                cio->wp         = strtoul(value, NULL, 10);
                if(cio->wp >= OMAP_MAX_WAIT_PINS) {
                    fprintf(stderr, "Wait pin out of range 0-3\n");
                    return (EINVAL);
                }
                break;
            case 4:
                cio->cfg = 1;
                break;
            case 5:
                cio->ifs_maxblk = strtoul(value, NULL, 0);
                if(cio->ifs_maxblk > cio->lowNAND) {
                    cio->lowNAND = cio->ifs_maxblk;
                }
                break;
            case 6:
                cio->ipl_maxblk = strtoul(value, NULL, 0);
                if(cio->ipl_maxblk > cio->lowNAND) {
                    cio->lowNAND = cio->ipl_maxblk;
                }
                break;
            case 7:
                cio->noPFWP     = 1;
                break;
            case 8:
                cio->reset_gpmc = 1;
                break;
            case 9:
                cio->elm_pbase  = strtoul(value, NULL, 0);
                break;
            case 10:
                {
                    char    *ptr;
                    cio->edmapbase = strtoul(value, &ptr, 0);
                    if(ptr[0] == '^') {
                        cio->chnl_dma = strtoul(&ptr[1], &ptr, 0);
                        if(ptr[0] == '^') {
                            cio->irq_dma = strtoul(&ptr[1], &ptr, 0);
                        }
                    }
                }
                break;
            case 11:
                cio->cache    = 1;
                break;
            case 12:
                cio->irq_gpmc = strtoul(value, NULL, 0);
                break;
            case 13:
                cio->r_opt = 1;
                break;
            case 14:
                cio->w_opt = 1;
                break;
#ifdef BIOS
            case 15:
                cio->bios_maxblk = strtoul(value, NULL, 0);
                if(cio->bios_maxblk > cio->lowNAND) {
                    cio->lowNAND = cio->bios_maxblk;
                }
                break;
#endif
            default:
                dev->log(_SLOG_ERROR, "Invalid -D suboption.");
                return (EINVAL);
        }
    }
    return (EOK);
}

/* Called once at startup */
int nand_init(struct etfs_devio *dev)
{
    struct chipio   *cio = dev->cio;
    uintptr_t       base;
    uint32_t        cfg7;

    /* Map in the device registers */
    base = cio->gpmc_vbase = mmap_device_io(DM816X_GPMC_SIZE, cio->gpmc_pbase);
    if(base == (uintptr_t) MAP_FAILED) {
        dev->log(_SLOG_CRITICAL, "Unable to map in device registers (%d).", errno);
        return -1;
    }

    cfg7 = OMAP_GPMC_CONFIG7;
    if(!(cfg7 & (1 << 6))) {
        dev->log(_SLOG_CRITICAL, "Chip select %d is not enabled (%d).", cio->cs, errno);
        munmap_device_io(cio->gpmc_vbase, DM816X_GPMC_SIZE);
        return -1;
    }

    cio->gpmc_cs0_pbase = ((cfg7 & 0x3f)) << 24;

    if((cio->gpmc_cs0_vbase = mmap_device_io(4, cio->gpmc_cs0_pbase)) == (uintptr_t) MAP_FAILED) {
        dev->log(_SLOG_CRITICAL, "Unable to map CS0 register for GMPC prefetch engine", errno);
        munmap_device_io(cio->gpmc_vbase, DM816X_GPMC_SIZE);
        return -1;
    }

    if(cio->reset_gpmc) {
        /* Store GPMC values */
        uint32_t    cfg[7];

        cfg[0] = OMAP_GPMC_CONFIG1;
        cfg[1] = OMAP_GPMC_CONFIG2;
        cfg[2] = OMAP_GPMC_CONFIG3;
        cfg[3] = OMAP_GPMC_CONFIG4;
        cfg[4] = OMAP_GPMC_CONFIG5;
        cfg[5] = OMAP_GPMC_CONFIG6;
        cfg[6] = OMAP_GPMC_CONFIG7;

        /* reset GPMC */
        OMAP_GPMC_SYSCONFIG = 1 << 1;
        while(OMAP_GPMC_SYSCONFIG != 0);

        OMAP_GPMC_CONFIG = 0x13;
        OMAP_GPMC_CONFIG7 = 0x0;
        delay(1);

        // Restore GPMC value
        OMAP_GPMC_CONFIG1 = cfg[0];
        OMAP_GPMC_CONFIG2 = cfg[1];
        OMAP_GPMC_CONFIG3 = cfg[2];
        OMAP_GPMC_CONFIG4 = cfg[3];
        OMAP_GPMC_CONFIG5 = cfg[4];
        OMAP_GPMC_CONFIG6 = cfg[5];
        OMAP_GPMC_CONFIG7 = cfg[6];

        delay(1);
    }

    /*
     * Select the device size
     * 0 means 8 bit
     * 1 means 16 bit
     */
    if(!((OMAP_GPMC_CONFIG1 >> 12) & OMAP_GPMC_CONFIG1_DEVICESIZE_MSK)) {
            cio->chip.wordlength = 8;
    }

#ifdef BIOS
    /* FIXME: Currently BIOS option is availabe to 16-bit interfaces only, since
     *        1-bit Hamming is not compatible between 16-bit and 8-bit
     */

    if(cio->chip.wordlength == 8 && cio->bios_maxblk > 0) {
        dev->log(_SLOG_CRITICAL, "The optional BIOS partition is currently only supported on 16-bit NAND Interfaces.");
        return -1;
    }

    /*
     * Make sure the raw partitions are in correct order. Otherwise bad stuff
     * might happen
     */
     if(!verify_raw_partitions(cio)){
         dev->log(_SLOG_CRITICAL, "Raw partitions are not in correct order. (IPL<IFS<BIOS)");
         return -1;
     }
#endif
    if(cio->cfg == 0) {
        /*
         * (1 << 4) WRITEPROTECT is high
         * (0 << 0) NANDFORCEPOSTEDWRITE is off
         */
        OMAP_GPMC_CONFIG = (1 << 4) | (0 << 0);
    }

    /*maping the ELM registers */
    base_elm = cio->elm_vbase = mmap_device_io(DM816X_ELM_SIZE, cio->elm_pbase);
    if(cio->elm_vbase == (uintptr_t) MAP_FAILED) {
        dev->log(_SLOG_CRITICAL, "Unable to map ELM registers (%d).", errno);
        return -1;
    }

    if(cio->noPFWP == 0) {
        /* use dma */
        if(edma_init(cio) == 0) {
            if(edma_attach_intr(cio) != 0) {
                cio->noPFWP = 1; /* do not use DMA */
                fprintf(stderr, "Could not attach interrupt...disabling DMA\n");
            } else {
                if(cio->cache) {
                    if(cache_init(0, &cio->cachectl, NULL) == -1) {
                        /* disable it */
                        cio->cache = 0;
                        dev->log(_SLOG_WARNING, "init cache failed %d", errno);
                    } else {
                        dev->flags  |= ETFS_DEVIO_FLAG_DMABUF;
                    }
                }
            }
        } else {
            cio->noPFWP = 1;
            fprintf(stderr, "Could not init EDMA...disabling DMA\n");
        }
    }

    // use IRQ?
    if(cio->irq_gpmc != -1) {
        if(cio->chid == -1) {
            if((cio->chid = ChannelCreate(_NTO_CHF_DISCONNECT | _NTO_CHF_UNBLOCK)) == -1) {
                return -1;
            }

            if((cio->coid = ConnectAttach(0, 0, cio->chid, _NTO_SIDE_CHANNEL, 0)) == -1) {
                ChannelDestroy(cio->chid);
                return -1;
            }
        }

        OMAP_GPMC_IRQENABLE &= ~(OMAP_GPMC_WAITPIN0 << cio->wp);
        OMAP_GPMC_IRQSTATUS |= OMAP_GPMC_WAITPIN0 << cio->wp;

        cio->gpmcevent.sigev_notify   = SIGEV_PULSE;
        cio->gpmcevent.sigev_coid     = cio->coid;
        cio->gpmcevent.sigev_code     = GPMC_EVENT;
        cio->gpmcevent.sigev_priority = EDMA_PRIORITY;

        cio->iid_gpmc = InterruptAttach(cio->irq_gpmc, nand_intr, cio, 0, _NTO_INTR_FLAGS_TRK_MSK);

        if(cio->iid_gpmc == -1) {
            if(cio->noPFWP) {
                ConnectDetach(cio->coid);
                ChannelDestroy(cio->chid);
            }
           cio->irq_gpmc = -1;
        }
    }

    return (0);
}

/* Accessing the command reg automatically sets ALE=0, CLE=1 */
void nand_write_cmd(struct chipio *cio, int command)
{
    if(cio->chip.wordlength == 8) {
        OMAP_NAND_COMMAND_8BIT = command;
    } else {
        OMAP_NAND_COMMAND = command;
    }
}

/* Accessing the address reg automatically sets ALE=1, CLE=0 */
void nand_write_pageaddr(struct chipio *cio, unsigned page, int addr_cycles, int spare)
{
    int               i;
    unsigned short    addr[5];

    addr[0] = 0x00;
    if(cio->chip.wordlength == 8) {
        addr[1] = spare ? (DATASIZE_2048 >> 8) : 0; // assume it's 8 bit width
    } else {
        addr[1] = spare ? (DATASIZE_2048 >> 9) : 0; // assume it's 16 bit width
    }
    addr[2] = NAND_ADDR_ROW1(page);
    addr[3] = NAND_ADDR_ROW2(page);
    addr[4] = NAND_ADDR_ROW3(page);

    for(i = 0; i < addr_cycles; i++) {
        if(cio->chip.wordlength == 8) {
            OMAP_NAND_ADDRESS_8BIT = addr[i];
        } else {
            OMAP_NAND_ADDRESS = addr[i];
        }
    }
}

void nand_write_pageaddr_bch(struct chipio *cio, unsigned page, int addr_cycles, int col)
{
    int         i;
    uint16_t    addr[5];

    if(cio->chip.wordlength != 8) {
        col >>= 1;
    }

    /*assume it's 16 bit width*/
    addr[0] = NAND_ADDR_COL1(col);
    addr[1] = NAND_ADDR_COL2(col);

    addr[2] = NAND_ADDR_ROW1(page);
    addr[3] = NAND_ADDR_ROW2(page);
    addr[4] = NAND_ADDR_ROW3(page);

    for(i = 0; i < addr_cycles; i++) {
        if(cio->chip.wordlength == 8) {
            OMAP_NAND_ADDRESS_8BIT = addr[i];
        } else {
            OMAP_NAND_ADDRESS = addr[i];
        }
    }
}

/* Accessing the address reg automatically sets ALE=1, CLE=0 */
void nand_write_blkaddr(struct chipio *cio, unsigned blk, int addr_cycles)
{
    int         i;
    uint16_t    addr[5];
    uint32_t    page = blk * PAGES2BLK_2048;

    addr_cycles = 3;
    addr[0] = NAND_ADDR_ROW1(page);
    addr[1] = NAND_ADDR_ROW2(page);
    addr[2] = NAND_ADDR_ROW3(page);

    for(i = 0; i < addr_cycles; i++) {
        if(cio->chip.wordlength == 8) {
            OMAP_NAND_ADDRESS_8BIT = addr[i];
        } else {
            OMAP_NAND_ADDRESS = addr[i];
        }
    }
}

/* Accessing the data reg automatically sets ALE=0, CLE=0 */
void nand_write_data(struct chipio *cio, uint8_t *databuffer, int data_cycles)
{
    int i;
    if(cio->chip.wordlength == 8) {
        uint8_t    *p1 = (uint8_t *)databuffer;
        for(i = 0; i < data_cycles; i++) {
            OMAP_NAND_DATA_8BIT = *p1++;
        }
    } else {
        uint16_t *p1 = (uint16_t *)databuffer;
        for(i = 0; i < data_cycles; i += 2) {
            OMAP_NAND_DATA = *p1++;
        }
    }
}

/* Accessing the data reg automatically sets ALE=0,CLE=0 */
void nand_read_data(struct chipio *cio, uint8_t *databuffer, int data_cycles)
{
    int i;

    if(cio->chip.wordlength == 8) {
        uint8_t *p1 = (uint8_t *)databuffer;

        for(i = 0; i < data_cycles; i++) {
            *p1++ = OMAP_NAND_DATA_8BIT;
        }
    } else {
        uint16_t    *p1 = (uint16_t *)databuffer;
        for(i = 0; i < data_cycles; i += 2) {
            *p1++ = OMAP_NAND_DATA;
        }
    }

}

/* Accessing the data reg automatically sets ALE=0,CLE=0 */
void nand_read_status(struct chipio *cio, uint8_t *databuffer, int data_cycles)
{
    int i;

    for(i = 0; i < data_cycles; i++) {
        if(cio->chip.wordlength == 8) {
            databuffer[i]= OMAP_NAND_DATA_8BIT;
        } else {
            databuffer[i]= OMAP_NAND_DATA;
        }
    }
}

/*
 * This function should be called before an HW ECC is performed.
 * For a 16-bit wide NAND chip with page size of 2K and spare size of 64B
 *
 * @eccmode    0: Use ECC_BCH4 Code
 *             1: Use ECC_BCH8 Code
 *             2: Use ECC_HAMMING Code
 *
 * @bWrite:    1: Enables ECC for write mode
 *             0: Enables ECC for read mode
 *
 * For BCH8 (M3 from TRM):
 * For write mode: MODE=1, size0=P,   size1=E+U
 * For read mode:  MODE=1, size0=P+E, size1=U
 * Where:
 *             P: Protected Spare area (in nibbles) [0]
 *             E: ECC (in nibbles) [13 x 2 = 26]
 *             U: unprotected spare size (in nibbles) [1byte * 2 = 2]
 *             S: sectors per page (1 sector, repeated 4 times)
 * For BCH4 (M10 from TRM):
 * For write mode: MODE=6,  size0=P,   size1=1+E
 * For read mode:  MODE=11, size0=P,   size1=E
 * Where:
 *             P: Protected Spare area (in nibbles) [4bytes * 2 = 8] -- 4bytes/sector
 *             E: ECC (in nibbles) [6.5 x 2 = 13]
 *             S: sectors per page (4 sectors)
 *
 */
void nand_reset_hw_ecc(struct chipio *cio, unsigned ecc_mode, int bWrite)
{
    if(ECC_BCH8 == ecc_mode) {
        /* 8-bit ECC */
        uint32_t val = 0;
        uint16_t size0, size1;

        OMAP_GPMC_ECC_CONFIG = OMAP_GPMC_ECC_CONFIG & ~0x1;

        /* reset ecc (clear ecc register values and select register 1), common to both bch and hamming */
        OMAP_GPMC_ECC_CONTROL = (OMAP_GPMC_ECC_CONTROL_ECCCLEAR | OMAP_GPMC_ECC_CONTROL_ECCPOINTER);

        size0 = bWrite ? 0 : 26;
        size1 = bWrite ? 28 : 2;

        val = OMAP_GPMC_ECC_SIZE_CONFIG_ECCSIZE0(size0) | OMAP_GPMC_ECC_SIZE_CONFIG_ECCSIZE1(size1);
        OMAP_GPMC_ECC_SIZE_CONFIG = val;
        OMAP_GPMC_ECC_CONFIG = OMAP_GPMC_ECC_CONFIG_ECCALGORITHM(1) | OMAP_GPMC_ECC_CONFIG_ECCBCHT8 |
                               OMAP_GPMC_ECC_CONFIG_ECCWRAPMODE(1) | OMAP_GPMC_ECC_CONFIG_ECC16B(1);

        /* enable ecc */
        OMAP_GPMC_ECC_CONFIG = OMAP_GPMC_ECC_CONFIG | OMAP_GPMC_ECC_CONFIG_ECCENABLE;
    } else if(ECC_BCH4 == ecc_mode) {
        /* 4-bit ECC */
        uint32_t val;
        uint16_t size0, size1;

        /* Reset ECC (clear ECC register values and select register 1) */
        val = (OMAP_GPMC_ECC_CONTROL_ECCCLEAR | OMAP_GPMC_ECC_CONTROL_ECCPOINTER);
        OMAP_GPMC_ECC_CONTROL = val;    // common to both bch and hamming

        /* configure eccsize0 and eccsize1 */
        size0 = bWrite ? 8 : 8;
        size1 = bWrite ? 14 : 13;

        val = OMAP_GPMC_ECC_SIZE_CONFIG_ECCSIZE0(size0) | OMAP_GPMC_ECC_SIZE_CONFIG_ECCSIZE1(size1);
        OMAP_GPMC_ECC_SIZE_CONFIG = val;

        val = OMAP_GPMC_ECC_CONFIG_ECCALGORITHM(BCH_CODE) | OMAP_GPMC_ECC_CONFIG_ECCBCHT4 |
              OMAP_GPMC_ECC_CONFIG_ECC16B(1) | OMAP_GPMC_ECC_CONFIG_ECCTOPSECTOR(ECC_SECTORS_4)|
              OMAP_GPMC_ECC_CONFIG_ECCENABLE;
        if(bWrite) {
            val |= OMAP_GPMC_ECC_CONFIG_ECCWRAPMODE(6);
        } else {
            val |= OMAP_GPMC_ECC_CONFIG_ECCWRAPMODE(11);
        }
        OMAP_GPMC_ECC_CONFIG = val;
    }
#ifdef BIOS
    else {
        //8-bit and 16-bit ARE NOT COMPATIBLE
        /* 1-bit ECC */
        uint32_t val = 0;
        uint16_t size0,size1;

        OMAP_GPMC_ECC_CONTROL = 0;
        OMAP_GPMC_ECC_CONFIG = OMAP_GPMC_ECC_CONFIG & ~0x1;

        //reset ecc (clear ecc register values and select register 1)
        val = (OMAP_GPMC_ECC_CONTROL_ECCCLEAR | OMAP_GPMC_ECC_CONTROL_ECCPOINTER);
        OMAP_GPMC_ECC_CONTROL = val;	// common to both bch and hamming

        size0 = 0xFF; // 512B
        size1 = 0xFF; // 512B
        val = OMAP_GPMC_ECC_SIZE_CONFIG_ECCSIZE0(size0) | OMAP_GPMC_ECC_SIZE_CONFIG_ECCSIZE1(size1);
        OMAP_GPMC_ECC_SIZE_CONFIG = val;

        val = OMAP_GPMC_ECC_CONFIG_ECCALGORITHM(HAMMING_CODE) | OMAP_GPMC_ECC_CONFIG_ECC16B(1) |
              OMAP_GPMC_ECC_CONFIG_ECCENABLE;
        OMAP_GPMC_ECC_CONFIG = val;
    }
#endif
}

/*
 * This function should be called after calling nand_reset_hw_ecc() and
 * performing either a read or write
 *
 * ecc_mode    0: Use BCH4 Code
 *             1: Use BCH8 Code
 *             2: Use Hamming
 *
 * NOTE:     The size of ecc_code depends on the ECC algorithms used (Hamming or BCH)
 *             This function assumes the ecc_code has space to fit all the ECC values
 *             for a given mode
 *
 */
void nand_get_hw_ecc(struct chipio *cio, unsigned ecc_mode, uint8_t* ecc_code)
{
    if(ECC_BCH8 == ecc_mode) {
        int         i, j;
        uint32_t    bch_val[3];

        ecc_code[0] = GPMC_BCH_RESULT3(0) & 0xFF;

        bch_val[0] = GPMC_BCH_RESULT2(0);
        bch_val[1] = GPMC_BCH_RESULT1(0);
        bch_val[2] = GPMC_BCH_RESULT0(0);

        for(i = 1, j = 0; j < 3; j++) {
            ecc_code[i++] = (bch_val[j] >> 24) & 0xFF;
            ecc_code[i++] = (bch_val[j] >> 16) & 0xFF;
            ecc_code[i++] = (bch_val[j] >>  8) & 0xFF;
            ecc_code[i++] = bch_val[j] & 0xFF;
        }

        /*
         * Stop reading anymore ECC values and clear old results
         * enable will be called if more reads are required
         */
        OMAP_GPMC_ECC_CONFIG &= ~0x1;
    } else if(ECC_BCH4 == ecc_mode) {
        int         i;
        uint32_t    bch_val1, bch_val2;

        for(i = 0; i < NUM_OF_SECTORS; i++) {
            bch_val1 = GPMC_BCH_RESULT0(i);     // bits 31 - 0
            bch_val2 = GPMC_BCH_RESULT1(i);     // bits 55 - 32

            *ecc_code++ = (bch_val2 >> 16) & 0x0f;
            *ecc_code++ = (bch_val2 >> 8) & 0xff;
            *ecc_code++ = bch_val2 & 0xff;

            *ecc_code++ = (bch_val1 >> 24) & 0xff;
            *ecc_code++ = (bch_val1 >> 16) & 0xff;
            *ecc_code++ = (bch_val1 >> 8) & 0xff;
            *ecc_code++ = bch_val1 & 0xff;
        }
    }
#ifdef BIOS
    else {
        int i;
        uint32_t val;
        for(i=0; i<4; i++) {
			val = GPMC_ECC_RESULT(i);
            *ecc_code++ = ECC_P1_128_E(val);
            *ecc_code++ = ECC_P1_128_O(val);
            *ecc_code++ = ECC_P512_2048_E(val) | ECC_P512_2048_O(val) << 4;
		}
    }
#endif
}

#ifdef BIOS
/**
 * This function verifies the order of various raw partition types.
 * The driver assumes the following order. The existence of each parition
 * is optional, but if they exist, they need to follow the order below
 *
 *  ________________________________________________________
 * |      |              |     |                             |
 * | IPL  |     IFS      |BIOS |         ETFS FS             |
 * |(BCH8)|    (BCH4)    |(HAM)|         (BCH4)              |
 * |______|______________|____ |_____________________________|
 *
 *
 */
uint8_t verify_raw_partitions(struct chipio *cio)
{
    int ret = 1;
    if(cio->ipl_maxblk > 0) {
        // IPL partition exists
        if(cio->ifs_maxblk > 0) {
            // IFS exists. It must reside above IPL
            if(cio->ifs_maxblk < cio->ipl_maxblk) {
                ret = 0;
            }
        } else if(cio->bios_maxblk > 0) {
            // BIOS exists. It must reside above IPL
            if(cio->bios_maxblk < cio->ipl_maxblk) {
                ret = 0;
            }
        }
    }
    if(cio->ifs_maxblk > 0) {
        // IFS partition exists
        if(cio->bios_maxblk > 0) {
            // BIOS exists. It must reside above IFS
            if(cio->bios_maxblk < cio->ifs_maxblk) {
                ret = 0;
            }
        }
    }

    return ret;
}
#endif
/*
 * This function return the block number of low NAND portion
 */
uint32_t LowNand(struct chipio *cio)
{
    return (cio->lowNAND);
}

/*
 * This function is used to determine IPL partition
 */
uint8_t isIPL(struct chipio *cio, int cluster)
{
    return (cluster < (PAGES2BLK_2048 * cio->ipl_maxblk));
}

#ifdef BIOS
/*
 * This function is used to determine IFS partition
 */
uint8_t isBIOS(struct chipio *cio, int cluster)
{
    uint32_t min = 0, max = 0;
    /* BIOS partition might or might not exist. Furthermore,
     * its location depends on whether ipl and/or ifs paritions
     * exist or not.
     */
    if(cio->bios_maxblk != 0) {
        max = PAGES2BLK_2048 * cio->lowNAND;
        if(cio->ifs_maxblk > 0) {
            min = PAGES2BLK_2048 * cio->ifs_maxblk; //BIOS is always above IFS
        } else {
            //if IPL partition exists, it sits above it, otherwise 0 (ipl_maxblk is zero by default)
            min = PAGES2BLK_2048 * cio->ipl_maxblk;
        }
    }

    return ( (cluster < max) && (cluster >= min) );
}
#endif

uint8_t getPartitionEccType(CHIPIO *cio, int cluster)
{
    if(isIPL(cio, cluster)) {
        return ECC_BCH8;
    }
#ifdef BIOS
    else if(isBIOS(cio, cluster)) {
        return ECC_HAMM;
    }
#endif
    else {
        return ECC_BCH4;
    }
}

/**
 * This function returns true if Prefetch-WritePost engine is being used
 * and false otherwise
 */
uint8_t usePRWP(struct chipio *cio)
{
    return !(cio->noPFWP);
}

void nand_prefetch_reset(struct chipio *cio)
{
    GPMC_PREFETCH_CONTROL = 0;
    GPMC_PREFETCH_CONFIG1 = 0x0;
    GPMC_PREFETCH_CONFIG2 = 0x0;
}

paddr_t nand_dma_buf(struct chipio *cio, uint8_t **buf)
{
    if(buf) {
        *buf = (uint8_t *)cio->dmabuf;
    }
    return ((paddr_t)cio->pdmabuf);
}

/**
 * This function reads a page using the GPMC prefetch engine
 */
unsigned nand_prefetch_read_page(struct chipio *cio, uint8_t *src, paddr_t paddr, int length)
{
    int         timeout;

    if(cio->cache) {
        CACHE_INVAL(&cio->cachectl, src, paddr, length);
    }

    edma_setup(cio, (cio->cache) ? paddr : cio->pdmabuf, length, 0);

    /* start the prefetch engine*/
    GPMC_PREFETCH_CONFIG2 = length;
    if(cio->r_opt) {
        GPMC_PREFETCH_CONFIG1 = (GPMC_PREFETCH_CONFIG1_ENABLEOPTACCESS) | (GPMC_PREFETCH_CONFIG1_CYCOPTIMIZATION(0x1));
    } else {
        GPMC_PREFETCH_CONFIG1 = (GPMC_PREFETCH_CONFIG1_CYCOPTIMIZATION(0x0));
    }
    GPMC_PREFETCH_CONFIG1 = GPMC_PREFETCH_CONFIG1 |
                            (GPMC_PREFETCH_CONFIG1_FIFOTHRESHOLD(4)) | (GPMC_PREFETCH_CONFIG1_ACCESSMODE(ACCESSMODE_PREFETCH)) |
                            (GPMC_PREFETCH_CONFIG1_SYNCHROMODE) | (GPMC_PREFETCH_CONFIG1_ENABLEENGINE) |
                            (GPMC_PREFETCH_CONFIG1_DMAMODE);

    /*
     * Wait for prefetch pointer to reset. In some cases the
     * Prefetch pointer gets stuck (hardware). In these cases, the
     * GPMC needs to get resetted (restart the driver with 'reset' option
     */
    timeout = 10000;
    while(GPMC_PREFETCH_STATUS && timeout--);

    if(timeout <= 0) {
        fprintf(stderr, "Prefetch engine pointer not reset!\n");
        fprintf(stderr, "Please restart the driver with 'reset' option\n");
        return -1;
    }

    GPMC_PREFETCH_CONTROL = GPMC_PREFETCH_CONTROL_STARTENGINE;

    if(edma_wait(cio, length, 1)) {
        fprintf(stderr, "J5 Micron PF_READ: DMA XFER TIMEOUT!!!\n");
        edma_dinit((void*)cio);
        return -1;
    }

    if(cio->cache) {
        CACHE_INVAL(&cio->cachectl, src, paddr, length);
    } else {
        // copy dma buffer to source
        memcpy(src, (uint8_t*)cio->dmabuf, length);
    }

    return 0;
}

/**
 * This function reads a page using the GPMC prefetch engine
 */
unsigned nand_prefetch_write_page(struct chipio *cio, uint8_t *src, paddr_t paddr, int length)
{
    int timeout;

    if(cio->cache) {
        CACHE_FLUSH(&cio->cachectl, src, paddr, length);
    } else {
        memcpy((uint8_t*)cio->dmabuf, src, length); // copy souce to dma buffer
    }

    edma_setup(cio, (cio->cache)? paddr : cio->pdmabuf, length, 1);

    /* start the prefetch engine*/
    GPMC_PREFETCH_CONFIG2 = length;
    if(cio->w_opt) {
        GPMC_PREFETCH_CONFIG1 = (GPMC_PREFETCH_CONFIG1_ENABLEOPTACCESS) | (GPMC_PREFETCH_CONFIG1_CYCOPTIMIZATION(0x1));
    } else {
        GPMC_PREFETCH_CONFIG1 = (GPMC_PREFETCH_CONFIG1_CYCOPTIMIZATION(0x0));
    }
    GPMC_PREFETCH_CONFIG1 = GPMC_PREFETCH_CONFIG1 |
                            (GPMC_PREFETCH_CONFIG1_FIFOTHRESHOLD(4)) | (GPMC_PREFETCH_CONFIG1_ACCESSMODE(ACCESSMODE_WRITEPORT)) |
                            (GPMC_PREFETCH_CONFIG1_SYNCHROMODE) | (GPMC_PREFETCH_CONFIG1_ENABLEENGINE) |
                            (GPMC_PREFETCH_CONFIG1_DMAMODE);

    GPMC_PREFETCH_CONTROL = GPMC_PREFETCH_CONTROL_STARTENGINE;

    /*
     * Wait for prefetch pointer to reset. In some cases the
     * Prefetch pointer gets stuck (hardware). In these cases, the
     * GPMC needs to get resetted (restart the driver with 'reset' option
     */
    timeout = 10000;
    while((in32(cio->gpmc_vbase + 0x1f0) != 0x40010000) && timeout--);

    if(timeout <= 0) {
        fprintf(stderr, "Prefetch engine pointer not reset!\n");
        dumpRegisters(cio);
        fprintf(stderr, "Please restart the driver with 'reset' option\n");
        return -1;
    }

    if(edma_wait(cio, length, 1)) {
        fprintf(stderr, "J5 Micron PF_WRITE: DMA XFER TIMEOUT!!!\n");
        edma_dinit((void*)cio);
        return -1;
    }

    return 0;
}

/*************************
 * EDMA Related Functions
 *************************/
static inline void edma_setbit(uintptr_t base, int reg, int bit)
{
    if(bit > 31) {
        reg += 4, bit -= 32;
    }
    out32(base + reg, 1 << bit);
}

static void edma_disable(struct chipio *cio)
{
    uintptr_t region0base = cio->edmavbase + DM6446_EDMA_REGION0;

    edma_setbit(region0base, DM6446_EDMA_EECR, cio->chnl_dma);
    edma_setbit(region0base, DM6446_EDMA_ECR,  cio->chnl_dma);
    edma_setbit(region0base, DM6446_EDMA_SECR, cio->chnl_dma);
    edma_setbit(cio->edmavbase, DM6446_EDMA_EMCR, cio->chnl_dma);
}

static int edma_setup(struct chipio *cio, paddr_t paddr, int len, int bWrite)
{
    int idx = cio->chnl_dma;

    uint32_t acnt, bcnt, ccnt;
    edma_param_t *param;

    acnt = 4;       // 1 word (4 bytes)
    bcnt = 1;       // FIFO size = 64B (16W)
    ccnt = len / 4; // len/(act*bcnt)

    edma_disable(cio);

    param = (edma_param_t *)(cio->edmavbase + DM6446_EDMA_PARAM_BASE + (0x20 * idx));

    param->opt = (idx << 12) | (1 << 20) | (1 << 2);
    param->abcnt = (bcnt << 16) | acnt;
    param->linkbcntrld = 0xFFFF | (1 << 16);
    param->ccnt = ccnt;

    if(bWrite) {
        // write
        param->src = paddr;
        param->dst = cio->gpmc_cs0_pbase;
        param->srcdstbidx = (0 << 16) | acnt;
        param->srcdstcidx = (0 << 16) | (acnt * bcnt);
    } else {
        // read
        param->src = cio->gpmc_cs0_pbase;
        param->dst = paddr;
        param->srcdstbidx = (acnt << 16) | 0;
        param->srcdstcidx = ((acnt * bcnt) << 16) | 0;
    }

    // Enable EDMA event
    edma_setbit(cio->edmavbase, DM6446_EDMA_REGION0 + DM6446_EDMA_EESR, idx);

    return 0;
}

static const struct sigevent *nand_intr(void *area, int id)
{
    struct chipio *cio = (struct chipio *) area;

    if(id == cio->edma_iid) {
        return (&cio->edmaevent);
    }

    if(id == cio->iid_gpmc) {
        // mask interrupt
        OMAP_GPMC_IRQENABLE &= ~(OMAP_GPMC_WAITPIN0 << cio->wp);
        // clear interrupt status
        OMAP_GPMC_IRQSTATUS |= OMAP_GPMC_WAITPIN0 << cio->wp;
        return (&cio->gpmcevent);
    }

    return NULL;
}

static int edma_init(struct chipio *cio)
{
    /*
     * Map in DMA buffer
     */
    cio->dmabuf = mmap(0, DMA_BUF_SIZE, PROT_READ | PROT_WRITE | ((cio->cache) ? 0 : PROT_NOCACHE),
            MAP_ANON | MAP_PHYS | MAP_PRIVATE, NOFD, 0);

    if(cio->dmabuf != MAP_FAILED) {
        cio->pdmabuf = mphys(cio->dmabuf);
        if(cio->pdmabuf == -1) {
            fprintf(stderr, "Error: Could not obtain dma buffer physical address\n");
            munmap(cio->dmabuf, DMA_BUF_SIZE);
            return -1; // set a flag here to say edma is not available?
        }
    } else {
        fprintf(stderr, "Error: Could not allocate dma buffer\n");
        return -1;
    }

    /* Map in EDMA registers */
    if((cio->edmavbase = (uintptr_t) mmap_device_memory(0, DM6446_EDMA_SIZE,
            PROT_READ | PROT_WRITE | PROT_NOCACHE, 0, cio->edmapbase)) == (uintptr_t) MAP_FAILED) {
        fprintf(stderr, "Error: Could not map EDMA registers\n");
        munmap(cio->dmabuf, DMA_BUF_SIZE);
        return -1;
    }
    memset(cio->dmabuf, 0xaa, DMA_BUF_SIZE);
    return 0;
}

static int edma_attach_intr(struct chipio *cio)
{
    if((cio->chid = ChannelCreate(_NTO_CHF_DISCONNECT | _NTO_CHF_UNBLOCK)) == -1) {
        return -1;
    }

    if((cio->coid = ConnectAttach(0, 0, cio->chid, _NTO_SIDE_CHANNEL, 0)) == -1) {
        ChannelDestroy(cio->chid);
        return -1;
    }

    cio->edmaevent.sigev_notify = SIGEV_PULSE;
    cio->edmaevent.sigev_coid = cio->coid;
    cio->edmaevent.sigev_code = EDMA_EVENT;
    cio->edmaevent.sigev_priority = EDMA_PRIORITY;

    edma_disable(cio);

    cio->edma_iid = InterruptAttach(cio->irq_dma, nand_intr, cio, 0, _NTO_INTR_FLAGS_TRK_MSK);

    if(cio->edma_iid == -1) {
        ConnectDetach(cio->coid);
        ChannelDestroy(cio->chid);
        return -1;
    }

    return 0;
}

static void edma_dinit(void *hdl)
{
    struct chipio *cio = hdl;

    edma_disable(cio);
    InterruptDetach(cio->edma_iid);
    munmap(cio->dmabuf, DMA_BUF_SIZE);
    munmap_device_memory((void*)cio->edmavbase, DM6446_EDMA_SIZE);
    ConnectDetach(cio->coid);
    ChannelDestroy(cio->chid);
}

static int intr_wait(struct chipio *cio, uint64_t to)
{
    struct _pulse pulse;

    if(to) {
        TimerTimeout(CLOCK_REALTIME, _NTO_TIMEOUT_RECEIVE, NULL, &to, NULL);
    }

    if(MsgReceivePulse(cio->chid, &pulse, sizeof(pulse), NULL) == -1) {
        fprintf(stderr, "intr_wait: timeout***!\n");
        return -1;
    }

    return (pulse.code);
}

int nand_wait_busy(struct chipio *cio, uint32_t usec)
{
    int         pc;
    uint64_t    to;

    /*
     * Wait for NAND
     * Wait pins start at bit 8 of GPMC status register
     * WAITxSTATUS Is a copy of input pin WAITx. (Reset value is WAITx input R xpin sampled at IC reset)
     * 0x0: WAITx asserted (inactive state)
     * 0x1: WAITx de-asserted
     */
    if(usec <= MAX_RESET_USEC || cio->irq_gpmc == -1) {
        usec *= 10;
        while(usec--) {
            if(OMAP_GPMC_STATUS & (OMAP_GPMC_WAITPIN0 << cio->wp)) {
                return 0;
            }
            nanospin_ns(100);
        }
        return -1;
    }

    to = usec;
    to *= 1000;
    for(;;) {
        OMAP_GPMC_IRQENABLE |= OMAP_GPMC_WAITPIN0 << cio->wp;

        if((pc = intr_wait(cio, to)) <= 0) {
            OMAP_GPMC_IRQENABLE &= ~(OMAP_GPMC_WAITPIN0 << cio->wp);
            fprintf(stderr, "busy_wait: timeout***!\n");
            return -1;
        }

        if(pc == GPMC_EVENT) {
            return 0;
        }
    }
}

static int edma_wait(struct chipio *cio, int len, int dtime)
{
    uint64_t    to;
    int         pc;

    to = dtime * 1000 * 50;
    to *= len;

    for(;;) {
        if((pc = intr_wait(cio, to)) <= 0) {
            fprintf(stderr, "edma_wait: timeout***!\n");
            dumpRegisters(cio);
            edma_disable(cio);
            return -1;
        }

        if(pc == EDMA_EVENT) {
            return 0;
        }
    }

    return -1;
}

static void dumpRegisters(struct chipio *cio)
{
    edma_param_t *param;
    uintptr_t region0base = cio->edmavbase + DM6446_EDMA_REGION0;

    param = (edma_param_t *)(cio->edmavbase + DM6446_EDMA_PARAM_BASE + (0x20 * cio->chnl_dma));

    fprintf(stderr, "\nDMA REGISTERS:\n----------------\n");
    fprintf(stderr, "abcnt = %x, ccnt = %x \n", param->abcnt, param->ccnt);
    fprintf(stderr, "opt = %x\n", param->opt);
    fprintf(stderr, "srcdstbidx = %x, srcdstcidx = %x, linkbcntrld = %x\n", param->srcdstbidx, param->srcdstcidx, param->linkbcntrld);
    fprintf(stderr, "src = %x, dst = %x\n", param->src, param->dst);
    fprintf(stderr, "ER = %x, ERH = %x\n", in32(region0base + DM6446_EDMA_ER), in32(region0base + DM6446_EDMA_ERH));
    fprintf(stderr, "ECR = %x, ECRH = %x\n", in32(region0base + DM6446_EDMA_ECR), in32(region0base + DM6446_EDMA_ECRH));
    fprintf(stderr, "ESR = %x, ESRH = %x\n", in32(region0base + DM6446_EDMA_ESR), in32(region0base + DM6446_EDMA_ESRH));
    fprintf(stderr, "IER = %x, IERH = %x\n", in32(region0base + DM6446_EDMA_IER), in32(region0base + DM6446_EDMA_IERH));
    fprintf(stderr, "EMR = %x, EMRH = %x\n", in32(cio->edmavbase + DM6446_EDMA_EMR), in32(cio->edmavbase + DM6446_EDMA_EMRH));
    fprintf(stderr, "EMCR = %x, EMCRH = %x\n", in32(cio->edmavbase + DM6446_EDMA_EMCR), in32(cio->edmavbase + DM6446_EDMA_EMCRH));
    fprintf(stderr, "EER = %x, EERH = %x\n", in32(region0base + DM6446_EDMA_EER), in32(region0base + DM6446_EDMA_EERH));
    fprintf(stderr, "EESR = %x, EESRH = %x\n", in32(region0base + DM6446_EDMA_EESR), in32(region0base + DM6446_EDMA_EESRH));
    fprintf(stderr, "config1 = %x \n", in32(cio->gpmc_vbase+0x1e0));
    fprintf(stderr, "config2 = %x \n", in32(cio->gpmc_vbase+0x1e4));
    fprintf(stderr, "status = %x \n", in32(cio->gpmc_vbase+0x1f0));
    fprintf(stderr, "GPMC_ERR_TYPE = %x \n", in32(cio->gpmc_vbase+0x48));
    fprintf(stderr, "GPMC_CONFIG = %x \n", in32(cio->gpmc_vbase+0x50));

    fprintf(stderr, "----------------\n\n");
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/etfs/nand2048/jacinto5_micron/chipio.c $ $Rev: 767198 $")
#endif
