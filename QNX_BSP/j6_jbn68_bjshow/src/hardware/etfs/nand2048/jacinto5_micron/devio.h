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


#include <stdint.h>

/*
 * The spare area used for the NAND. It is 64 bytes in size.
 * 2K page parts
 */
struct spare_bch4
{
    /* Sector 0 */
    uint8_t    status[2];          // ecc protected
    uint16_t   nclusters;         // ecc protected

    /* Sector 1 */
    uint32_t   sequence;          // ecc protected

    /* Sector 2 */
    uint32_t   cluster;           // ecc protected

    /* Sector 3 */
    uint16_t   fid;               // ecc protected
    uint16_t   align;             // ecc protected

    /* ECC signature */
    uint8_t    eccv[4][7];      // One 7-byte ECC Syndrome per 512KB (4 total)

    /* non-ECC protected area */
    uint8_t    unused0[4];
    uint32_t   erasesig[2];     // The erase signature created by devio_eraseblk
    uint32_t   crctrans;        // Crc for readtrans().
    uint8_t    unused1[4];
};

struct spare_bch8 {
    uint8_t    status;             // Factory marking for bad blks (0xff == GOOD)
    uint8_t    status2;            // For 16 bit wide parts
    uint8_t    eccv[4][14];        // ecc used in the BCH8 ECC
    uint8_t    unused[6];
};

#ifdef BIOS
struct spare_hamming {
    uint8_t    status;             // Factory marking for bad blks (0xff == GOOD)
    uint8_t    status2;            // For 16 bit wide parts
    uint8_t    ecc[12];
    uint8_t    reserv0[34];
    uint32_t   sequence;
    uint8_t    reserv1[12];
};
#endif

/* Need to read an entire page + upto and including eccv of spare_bch4 spare
 * 2048 + 16 + 28 = 2092
 * */
#define SPARE_BCH4_READSIZE  (2092)
#define SPARE_BCH4_WRITESIZE (2048 + 16)

#define ERASESIG1    0x756c7769
#define ERASESIG2    0x70626d66

/**
 * The lower 128MB of NAND SLC are protected and hence in order
 * to write to it, one must provide the action along with the key
 * to unlocking the lower portion of NAND SLC
 *
 * etfs_trans.fid contains the operation that is requested by the user
 * and etfs_trans.cluster contains the four byte unlock key and finally
 *
 * The variable force_ecc which is used for testing is in the lower nibble
 * of the last byte of etfs_trans.fid.
 */

#define MAGIC_NUMBER                0x3E45AF9B
#define MAGIC_OP_WRITE              0x68AB7140
#define MAGIC_OP_ERASE              0x91FF5720
#define MAGIC_OP_MRK_BLK_BAD        0xAD4E39C0
#define MAGIC_OP_MRK_BLK_GOOD       0x74F53DC0

#define NAND_UPPER_BOUND_MIN        1028
#define NAND_UPPER_BOUND_MAX        2047

/* ECC Mode */
#define ECC_BCH4 0
#define ECC_BCH8 1
#ifdef BIOS
#define ECC_HAMM 2
#endif

/* RAW Partition Type */
#define RAW_IPL  0
#define RAW_IFS  1
#ifdef BIOS
#define RAW_BIOS 2
#endif

/* This struct is included in the chipio structure defined in the low level board driver */
struct _chipio {
    unsigned    addrcycles;
    unsigned    lastcmd;
    unsigned    lastpage;
    unsigned    inspare;
    unsigned    page_2048;
    unsigned    resvblks;            // reserved last n blocks from the top memory end of NAND device
    unsigned    wordlength;
 };

#ifndef CHIPIO
#define CHIPIO struct _chipio
#endif

uintptr_t        base_elm;

/*
 *  Nand device specific data structures
 *  2K page size parts
 *  */
#define NANDCMD_SPAREREAD_2048           0x50
#define NANDCMD_READ_2048                0x00
#define NANDCMD_READCONFIRM_2048         0x30
#define NANDCMD_PROGRAM_2048             0x80
#define NANDCMD_PROGRAMCONFIRM_2048      0x10
#define NANDCMD_ERASE_2048               0x60
#define NANDCMD_ERASECONFIRM_2048        0xD0
#define NANDCMD_IDREAD_2048              0x90
#define NANDCMD_STATUSREAD_2048          0x70
#define NANDCMD_RESET_2048               0xFF
#define NANDCMD_UNLOCK_LOW               0x23
#define NANDCMD_UNLOCK_HIGH              0x24


#define DATASIZE_2048                    2048
#define SPARESIZE_2048                   64
#define PAGESIZE_2048                    (DATASIZE_2048 + SPARESIZE_2048)
#define PAGES2BLK_2048                   64

#define NAND_BLK_PER_IFS                 256

/*
 *  Nand device specific data structures
 *  512 byte pagesize
*/
#define NANDCMD_SPAREREAD_512            0x50
#define NANDCMD_READ_512                 0x00
#define NANDCMD_PROGRAM_512              0x80
#define NANDCMD_PROGRAMCONFIRM_512       0x10
#define NANDCMD_ERASE_512                0x60
#define NANDCMD_ERASECONFIRM_512         0xD0
#define NANDCMD_IDREAD_512               0x90
#define NANDCMD_STATUSREAD_512           0x70
#define NANDCMD_RESET_512                0xFF

#define DATASIZE_512                     512
#define SPARESIZE_512                    16
#define PAGESIZE_512                     (DATASIZE_512 + SPARESIZE_512)
#define PAGES2BLK_512                    32
#define PAGES2CLUSTER_512                2

// These timeouts are very generous.
#define MAX_RESET_USEC                   600            // 600us
#define MAX_READ_USEC                    150            // 150us
#define MAX_POST_USEC                    2000        //   2ms
#define MAX_ERASE_USEC                   10000        //  10ms


/*
 * ELM registers
 */
#define ELM_BASE                               0x48080000
#define ELM_REVISION                           0x000
#define ELM_SYSCONFIG                          0x010
#define ELM_SYSCONFIG_SOFTRESET                    (1<<1)
#define ELM_SYSSTATUS                          0x014
#define ELM_IRQSTATUS                          0x018
#define ELM_IRQSTATUS_PAGE_VALID                   (1<<8)
#define ELM_IRQENABLE                          0x01c
#define ELM_IRQENABLE_PAGE_MASK                    (1<<8)
#define ELM_LOCATION_CONFIG                    0x020
#define ELM_LOCATION_CONFIG_4BIT                   (0<<0)
#define ELM_LOCATION_CONFIG_8BIT                   (1<<0)
#define ELM_LOCATION_CONFIG_ECC_SIZE_2047           (0x7FF<<16)
#define ELM_PAGE_CTRL                          0x080
#define ELM_PAGE_CTRL_SECTOR_0                     (1<<0)
#define ELM_PAGE_CTRL_SECTOR_1                     (1<<1)
#define ELM_PAGE_CTRL_SECTOR_2                     (1<<2)
#define ELM_PAGE_CTRL_SECTOR_3                     (1<<3)
#define ELM_PAGE_CTRL_SECTOR_4                     (1<<4)
#define ELM_PAGE_CTRL_SECTOR_5                     (1<<5)
#define ELM_PAGE_CTRL_SECTOR_6                     (1<<6)
#define ELM_PAGE_CTRL_SECTOR_7                     (1<<7)

#define ELM_SYNDROME_FRAGMENT_0_(i)            (0x400 + (0x40 * i))
#define ELM_SYNDROME_FRAGMENT_1_(i)            (0x404 + (0x40 * i))
#define ELM_SYNDROME_FRAGMENT_2_(i)            (0x408 + (0x40 * i))
#define ELM_SYNDROME_FRAGMENT_3_(i)            (0x40c + (0x40 * i))
#define ELM_SYNDROME_FRAGMENT_4_(i)            (0x410 + (0x40 * i))
#define ELM_SYNDROME_FRAGMENT_5_(i)            (0x414 + (0x40 * i))
#define ELM_SYNDROME_FRAGMENT_6_(i)            (0x418 + (0x40 * i))
#define ELM_SYNDROME_FRAGMENT_6_SYNDROME_VALID     (1<<16)
#define ELM_LOCATION_STATUS_(i)                (0x800 + (0x100 * i))
#define ELM_LOCATION_STATUS_ECC_CRCT_ABBL          (1<<8)
/* supports only syndrome polynomial 0 */
#define ELM_ERROR_LOCATION_0_(i)               0x880 + (i*4)


// Prototypes for chip interface
int  nand_init(struct etfs_devio *dev);
int  nand_wait_busy(CHIPIO *cio, uint32_t usec);
void nand_write_pageaddr(CHIPIO *cio, unsigned page, int addr_cycles, int spare);
void nand_write_blkaddr(CHIPIO *cio, unsigned blk, int addr_cycles);
void nand_write_cmd(CHIPIO *cio, int command);
void nand_write_data(CHIPIO *cio, uint8_t *databuffer, int data_cycles);
void nand_read_data(CHIPIO *cio, uint8_t *databuffer, int data_cycles);
void nand_read_status(CHIPIO *cio, uint8_t *databuffer, int data_cycles);
void nand_reset_hw_ecc(CHIPIO *cio, unsigned ecc_mode, int bWrite);
void nand_get_hw_ecc(CHIPIO *cio, unsigned ecc_mode, uint8_t* ecc_code);
paddr_t nand_dma_buf(CHIPIO *cio, uint8_t **buf);
void nand_write_pageaddr_bch(CHIPIO *cio, unsigned page, int addr_cycles, int col);
uint32_t LowNand(CHIPIO *cio);
uint8_t isIPL(CHIPIO *cio, int cluster);
uint8_t getPartitionEccType(CHIPIO *cio, int cluster);

uint8_t usePRWP(CHIPIO *cio);
unsigned nand_prefetch_read_page(CHIPIO *cio, uint8_t *src, paddr_t paddr, int length);
void nand_prefetch_reset(CHIPIO *cio);
unsigned nand_prefetch_write_page(CHIPIO *cio, uint8_t *src, paddr_t paddr, int length);

extern paddr_t mphys(void *);

int error_correction_code(struct etfs_devio *dev, unsigned ecc_mode ,unsigned page ,uint8_t *buf, uint8_t *sp ,uint8_t *ecc);
void elm_load_syndromes(struct etfs_devio *dev, unsigned ecc_mode, uint8_t *syndrome_ecc, uint32_t nibbles, uint8_t poly);
int elm_error_check(struct etfs_devio *dev, unsigned ecc_mode, uint8_t *syndrome_ecc, uint32_t nibbles, uint32_t *error_count,uint32_t *error_locations);
void fix_errors_bch(struct etfs_devio *dev, unsigned ecc_mode ,uint8_t *data, uint8_t *sp,uint32_t error_count, uint32_t *error_loc);
#ifdef BIOS
int hamming_error_correction(struct etfs_devio *dev, unsigned page ,uint8_t *buf, uint8_t *read_ecc ,uint8_t *calc_ecc);
#endif



#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/etfs/nand2048/jacinto5_micron/devio.h $ $Rev: 767198 $")
#endif
