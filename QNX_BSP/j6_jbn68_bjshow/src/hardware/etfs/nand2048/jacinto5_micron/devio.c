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
#include <sys/mman.h>
#include <sys/neutrino.h>
#include <fs/etfs.h>
#include <hw/inout.h>
#include "devio.h"

int internal_eraseblk(struct etfs_devio *dev, unsigned blk);
int internal_markBlkBad(struct etfs_devio *dev, unsigned blk);
int internal_markBlkGood(struct etfs_devio *dev, unsigned blk);

/*
 * Initialize the part and stuff physical parameters for the part.
 */
int devio_init(struct etfs_devio *dev)
{
    uint8_t        id[5];
    CHIPIO        *cio = dev->cio;

    /* Allow IO operations */
    if(ThreadCtl(_NTO_TCTL_IO, 0) != EOK) {
        dev->log(_SLOG_CRITICAL, "You must be root.");
        /* We will not return */
    }

    /* Do anything special to get the part ready to use */
    if(nand_init(dev) != 0) {
        dev->log(_SLOG_CRITICAL, "nand_init failed : %s", strerror(errno));
        /* We will not return */
    }

    /* Reset the part */
    nand_write_cmd(cio, NANDCMD_RESET_2048);
    if(nand_wait_busy(cio, MAX_RESET_USEC) != 0) {
        dev->log(_SLOG_CRITICAL, "Timeout on RESET");
    }

    /* Read id info from the part */
    nand_write_cmd(cio, NANDCMD_IDREAD_2048);
    nand_write_pageaddr(cio, 0, 1, 0);
    if(nand_wait_busy(cio, MAX_READ_USEC) != 0) {
        dev->log(_SLOG_CRITICAL, "Timeout on ReadID");
    }

    nand_read_status(cio, id, 5);                // Get Device ID and Configuration Codes

    switch (id[1]) {
        /* 128M */
        case 0xb1: case 0xc1:
            dev->numblks = 1024;
            cio->addrcycles = 5;
            cio->page_2048 = 1;
            break;

        /* 128M - 8bit */
        case 0xa1: case 0xf1:
            dev->numblks = 1024;
            cio->addrcycles = 5;
            cio->page_2048 = 1;
            cio->wordlength = 8;
            break;

        /* 256M */
        case 0xba: case 0xca:
            dev->numblks = 2048;
            cio->addrcycles = 5;
            cio->page_2048 = 1;
            break;

        /* 256M - 8-bit */
        case 0xaa: case 0xda:
            dev->numblks = 2048;
            cio->addrcycles = 5;
            cio->page_2048 = 1;
            cio->wordlength = 8;
            break;

        /* 512M */
        case 0xbc: case 0xcc:
            dev->numblks = 4096;
            cio->addrcycles = 5;
            cio->page_2048 = 1;
            break;

        /* 512M - 8-bit*/
        case 0xac: case 0xdc:
            dev->numblks = 4096;
            cio->addrcycles = 5;
            cio->page_2048  = 1;
            cio->wordlength = 8;
            break;

        /* 1G */
        case 0xb3: case 0xc3:
            dev->numblks = 8192 - cio->resvblks;
            cio->addrcycles = 5;
            cio->page_2048 = 1;
            cio->wordlength = 8;
            break;

        /* 1G - 8-bit */
        case 0xa3: case 0xd3:
            dev->numblks = 8192 - cio->resvblks;
            cio->addrcycles = 5;
            cio->page_2048 = 1;
            cio->wordlength = 8;
            break;

        default:
            dev->log(_SLOG_CRITICAL, "Unsupported NAND device (%2.2x %2.2x)", id[0], id[1]);
            /* We will not return */
    }

    /* These must be initialized here. All other numbers are built from them. */
    dev->memtype       = ETFS_MEMTYPE_NAND;

    dev->clustersize   = DATASIZE_2048;
    dev->sparesize     = SPARESIZE_2048;
    dev->clusters2blk  = PAGES2BLK_2048;
    dev->blksize       = (dev->clustersize + SPARESIZE_2048) * dev->clusters2blk;

    cio->lastpage = ~0;

    return(EOK);
}


unsigned nand_unlock_blocks(struct etfs_devio *dev, unsigned start, unsigned end)
{
    CHIPIO    *cio = dev->cio;

    nand_write_cmd(cio, NANDCMD_UNLOCK_LOW);
    nand_write_blkaddr(cio, start, 3);
    nand_write_cmd(cio, NANDCMD_UNLOCK_HIGH);
    nand_write_blkaddr(cio, end, 3);
    if(nand_wait_busy(cio, MAX_RESET_USEC) != 0) {
        dev->log(_SLOG_ERROR, "Timeout on Block unlock\n");
    }

    return 0;
}


/*
 * Read a cluster of data.
 *
 * Verify CRC for both the spare area and the entire page (data + spare).
 * The passed buffer "buf" is larger than the cluster size. It can hold
 * (PAGESIZE bytes. This is for convenience when reading
 * data from the device and calculating the CRC. The work area after clustersize
 * bytes is ignored by the caller.
 */
int devio_readcluster(struct etfs_devio *dev, unsigned cluster, uint8_t *buf, struct etfs_trans *trp)
{
    CHIPIO    *cio = dev->cio;

    struct    spare_bch4     *sp_bch4;
    struct    spare_bch8     *sp_bch8;
#ifdef BIOS
    struct    spare_hamming  *sp_hamm;
#endif
    unsigned  page;
    int       partition_ecc_type = ECC_BCH4;
    int       ecc_ret_val,i;
    int const eccsize     = 512;
    int const eccbytes    = 14;
    int       eccsteps    = 4;
#ifdef BIOS
    uint8_t   ecc_ham[12];
#endif
    uint8_t   ecc_bch[4][7];
    uint8_t   ecc_bch8[56];
    uint32_t  data_pos, oob_pos, status_pos;
    uint8_t   *pecc;
    uint8_t   *pbuf = buf;
    uint8_t   *spbuf = buf + DATASIZE_2048;
    paddr_t   paddr = 0;

    if(dev->flags & ETFS_DEVIO_FLAG_DMABUF) {
        if(buf) {
            paddr = ETFS_BUF_MPHYS(dev, buf);
        } else {    // use chipio bounce buffer
            paddr = nand_dma_buf(cio, &pbuf);
            buf = pbuf;
            spbuf = buf + DATASIZE_2048;
        }
    }

    /* Determine the of type partition */
    partition_ecc_type = getPartitionEccType(cio, cluster);

    if(ECC_BCH4 == partition_ecc_type) {
        /* The requested page is either IFS or ETFS File system */
        page = cluster;

        nand_write_cmd(cio, NANDCMD_READ_2048);
        nand_write_pageaddr(cio, page, cio->addrcycles, 0);
        nand_write_cmd(cio, NANDCMD_READCONFIRM_2048);

        if(nand_wait_busy(cio,  MAX_READ_USEC) != 0) {
            dev->log(_SLOG_CRITICAL, "Timeout on READ");
        }

        /* Reset/configure ECC engine */
        nand_reset_hw_ecc(cio, ECC_BCH4, 0);

        //read data either directly or through prefetch engine
        if(usePRWP(cio)) {
            nand_prefetch_reset(cio);
            if(nand_prefetch_read_page(cio, buf, paddr, SPARE_BCH4_READSIZE) != 0) {
                dev->log(_SLOG_CRITICAL, "prefetch read error");
                return ETFS_TRANS_DATAERR;
            }
        } else {
            nand_read_data(cio, buf, SPARE_BCH4_READSIZE);
        }

        // read generated ecc values
        nand_get_hw_ecc(cio, ECC_BCH4, &ecc_bch[0][0]);

        // read remaining bytes
        nand_write_cmd(cio, NANDCMD_READ_2048);
        nand_write_pageaddr_bch(cio, page, cio->addrcycles, SPARE_BCH4_READSIZE);
        nand_write_cmd(cio, NANDCMD_READCONFIRM_2048);

        if(nand_wait_busy(cio,  MAX_READ_USEC) != 0) {
            dev->log(_SLOG_CRITICAL, "Timeout on READ");
        }

        //read rest of data -- erase sig and other
        nand_read_data(cio, buf + SPARE_BCH4_READSIZE, (PAGESIZE_2048 - SPARE_BCH4_READSIZE));

        cio->lastpage = page;
        sp_bch4 = (struct spare_bch4 *)(buf + DATASIZE_2048);

        /* Check the spare area first */
        if(sp_bch4->status[0] != 0xff || sp_bch4->status[1] != 0xff) {
            /* Bad block */
            dev->log(_SLOG_ERROR, "readcluster BADBLK on cluster %d", cluster);
            trp->tacode = ETFS_TRANS_BADBLK;
        } else if(((uint64_t*)sp_bch4)[0] == ~0ll && ((uint64_t*)sp_bch4)[1] == ~0ll &&
                 ((uint64_t*)sp_bch4)[2] == ~0ll && ((uint64_t*)sp_bch4)[3] == ~0ll &&
                 ((uint64_t*)sp_bch4)[4] == ~0ll && ((uint64_t*)sp_bch4)[5] == ~0ll &&
                 ((uint64_t*)sp_bch4)[7] == ~0ll) {
            /* Erased block */
            if(sp_bch4->erasesig[0] == ERASESIG1 && sp_bch4->erasesig[1] == ERASESIG2) {
                trp->tacode = ETFS_TRANS_ERASED;
            } else {
                trp->tacode = ETFS_TRANS_FOXES;
            }
        } else {
            trp->tacode = ETFS_TRANS_OK;
        }

        /* Build transaction data from data in the spare area. */
        trp->sequence    = ENDIAN_LE32(sp_bch4->sequence);
        trp->fid         = ENDIAN_LE16(sp_bch4->fid);
        trp->cluster     = ENDIAN_LE32(sp_bch4->cluster);
        trp->nclusters   = sp_bch4->nclusters;

        trp->dacode = ETFS_TRANS_OK;
        if(trp->tacode == ETFS_TRANS_OK) { // Proceed only if spare data is good
            /* Fix data using ECC if needed */
            for(i = 0; i < 28; i += 7, pbuf += eccsize) {
                pecc = &ecc_bch[0][0];
                ecc_ret_val = error_correction_code(dev, ECC_BCH4, page, pbuf, spbuf, &pecc[i]);
                spbuf += 4;

                if(ecc_ret_val == ETFS_ECC_ERROR) {
                    dev->log(_SLOG_ERROR, "readcluster DATAERR on cluster %d", cluster);
                    trp->dacode = ETFS_TRANS_DATAERR;
                    break;
                } else if(ecc_ret_val == ETFS_ECC_CORRECTED) {
                    dev->log(_SLOG_ERROR, "readcluster ECCERR on cluster %d", cluster);
                    trp->dacode = ETFS_TRANS_ECC;
                    continue;
                }
            }
            /* Re-Build transaction data from data in the spare area. */
            trp->sequence    = ENDIAN_LE32(sp_bch4->sequence);
            trp->fid         = ENDIAN_LE16(sp_bch4->fid);
            trp->cluster     = ENDIAN_LE32(sp_bch4->cluster);
            trp->nclusters   = sp_bch4->nclusters;
            return(trp->dacode);
        }
        return(trp->tacode);
    } else if(ECC_BCH8 == partition_ecc_type) {
        /* The requested page is IPL */
        page = cluster;
        pecc = &ecc_bch8[0];
        memset(pecc, 0xff, 56);

        /* reading the Status and Status1 */
        status_pos = (eccsize * eccsteps);
        nand_write_cmd(cio, NANDCMD_READ_2048);
        nand_write_pageaddr_bch(cio, page, cio->addrcycles, status_pos);
        nand_write_cmd(cio, NANDCMD_READCONFIRM_2048);

        if(nand_wait_busy(cio,  MAX_READ_USEC) != 0) {
            dev->log(_SLOG_CRITICAL, "Timeout on READ");
        }

        /* read the status and status2 */
        nand_read_data(cio, &pbuf[eccsize * eccsteps], 2);

        /* reading the last six bytes of spare : (2k of data + Status + Staus2 + ECC) */
        status_pos = (eccsize * eccsteps) + (eccbytes * eccsteps) + 2;
        nand_write_cmd(cio, NANDCMD_READ_2048);
        nand_write_pageaddr_bch(cio, page, cio->addrcycles, status_pos);
        nand_write_cmd(cio, NANDCMD_READCONFIRM_2048);

        if(nand_wait_busy(cio,  MAX_READ_USEC) != 0) {
            dev->log(_SLOG_CRITICAL, "Timeout on READ");
        }

        /* read the status and status2 */
        nand_read_data(cio, &pbuf[status_pos], 6);

        /* reading the data and spare */
        data_pos = 0;
        oob_pos = (eccsize * eccsteps) + 2; /* skipping the status and status2 */

        for(i = 0; eccsteps; eccsteps--, i += eccbytes) {
            nand_write_cmd(cio, NANDCMD_READ_2048);
            nand_write_pageaddr_bch(cio, page, cio->addrcycles, data_pos);
            nand_write_cmd(cio, NANDCMD_READCONFIRM_2048);

            if(nand_wait_busy(cio,  MAX_READ_USEC) != 0) {
                dev->log(_SLOG_CRITICAL, "Timeout on READ");
            }
            /* read the data */
            nand_reset_hw_ecc(cio, ECC_BCH8, 0);
            nand_read_data(cio, &pbuf[data_pos], eccsize);

            nand_write_cmd(cio, NANDCMD_READ_2048);
            nand_write_pageaddr_bch(cio, page, cio->addrcycles,oob_pos);
            nand_write_cmd(cio, NANDCMD_READCONFIRM_2048);

            if(nand_wait_busy(cio,  MAX_READ_USEC) != 0) {
                dev->log(_SLOG_CRITICAL, "Timeout on READ");
            }

            /*read ECC values to the spare */
            nand_read_data(cio, &pbuf[oob_pos], eccbytes-1);
            nand_get_hw_ecc(cio, ECC_BCH8, &pecc[i]);

            /* update the data_position and oob_position */
            data_pos += eccsize;
            oob_pos += eccbytes;
        }

        cio->lastpage = page;
        sp_bch8 = (struct spare_bch8 *)(buf + DATASIZE_2048);

        /* Check the spare area first */
        if(sp_bch8->status != 0xff || sp_bch8->status2 != 0xff) {
            /* Bad block */
            dev->log(_SLOG_ERROR, "readcluster BADBLK on cluster %d", cluster);
            trp->tacode = ETFS_TRANS_BADBLK;
        } else if(((uint64_t*)sp_bch8)[0] == ~0ll && ((uint64_t*)sp_bch8)[1] == ~0ll &&
                 ((uint64_t*)sp_bch8)[2] == ~0ll && ((uint64_t*)sp_bch8)[3] == ~0ll &&
                 ((uint64_t*)sp_bch8)[4] == ~0ll && ((uint64_t*)sp_bch8)[5] == ~0ll &&
                 ((uint64_t*)sp_bch8)[6] == ~0ll && ((uint64_t*)sp_bch8)[7] == ~0ll) {
            trp->tacode = ETFS_TRANS_ERASED;
        } else {
            trp->tacode = ETFS_TRANS_OK;
        }

        trp->dacode = ETFS_TRANS_OK;

        /* Proceed only if spare data is good */
        if(trp->tacode == ETFS_TRANS_OK) {
            for(i = 0; i< 56 ; i += eccbytes, pbuf += eccsize) {
                /* Fix data using ECC if needed */
                ecc_ret_val = error_correction_code(dev, ECC_BCH8, page, pbuf, NULL, &ecc_bch8[i]);

                if(ecc_ret_val == ETFS_ECC_ERROR) {
                    dev->log(_SLOG_ERROR, "Uncorrectable errors: readcluster DATAERR on cluster %d", cluster);
                    trp->dacode = ETFS_TRANS_DATAERR;
                    break;
                } else if(ecc_ret_val == ETFS_ECC_CORRECTED) {
                    dev->log(_SLOG_ERROR, "readcluster ECCERR on cluster %d", cluster);
                    trp->dacode = ETFS_TRANS_ECC;
                    continue;
                }
            }
            return(trp->dacode);
        }
        return(trp->tacode);
    }
#ifdef BIOS
    else {
        /* The requested page is the BIOS partition */
        page = cluster;

        //dev->log(_SLOG_ERROR, "readcluster on cluster %d requested", cluster);

        nand_write_cmd(cio, NANDCMD_READ_2048);
        nand_write_pageaddr(cio, page, cio->addrcycles, 0);
        nand_write_cmd(cio, NANDCMD_READCONFIRM_2048);

        if(nand_wait_busy(cio,  MAX_READ_USEC) != 0) {
            dev->log(_SLOG_CRITICAL, "Timeout on READ");
        }

        /* Reset/configure ECC engine */
        nand_reset_hw_ecc(cio, ECC_HAMM, 0);

        //read data either directly or through prefetch engine
        if(usePRWP(cio)) {
            nand_prefetch_reset(cio);
            if(nand_prefetch_read_page(cio, buf, paddr, PAGESIZE_2048) != 0) {
                dev->log(_SLOG_CRITICAL, "prefetch read error");
                return ETFS_TRANS_DATAERR;
            }
        } else {
            nand_read_data(cio, buf, PAGESIZE_2048);
        }

        // read generated ecc values
        nand_get_hw_ecc(cio, ECC_HAMM, ecc_ham);
        cio->lastpage = page;
        sp_hamm = (struct spare_hamming *)(buf + DATASIZE_2048);

        if(sp_hamm->status != 0xff || sp_hamm->status2 != 0xff) {
            /* Bad block */
            dev->log(_SLOG_ERROR, "readcluster BADBLK on cluster %d", cluster);
            trp->tacode = ETFS_TRANS_BADBLK;
        } else if(((uint64_t*)sp_hamm)[0] == ~0ll && ((uint64_t*)sp_hamm)[1] == ~0ll &&
                 ((uint64_t*)sp_hamm)[2] == ~0ll && ((uint64_t*)sp_hamm)[3] == ~0ll &&
                 ((uint64_t*)sp_hamm)[4] == ~0ll && ((uint64_t*)sp_hamm)[5] == ~0ll &&
                 ((uint64_t*)sp_hamm)[6] == ~0ll && ((uint64_t*)sp_hamm)[7] == ~0ll) {
            trp->tacode = ETFS_TRANS_ERASED;
        } else {
            trp->tacode = ETFS_TRANS_OK;
        }

        trp->dacode = ETFS_TRANS_OK;
        /* Proceed only if spare data is good */
        if(trp->tacode == ETFS_TRANS_OK) {
            trp->dacode = hamming_error_correction(dev, page , buf, &sp_hamm->ecc[0] , &ecc_ham[0]);
            return(trp->dacode);
        }
        return(trp->tacode);
    }
#endif
    /* Should not reach here */
    return ETFS_TRANS_DEVERR;
}

/*
 * Read the spare area of a page (not the data) to return transaction information.
 *
 * This called is used heavily on startup to process the transactions. It is
 * a cheaper call than readcluster() since it reads less data and has a smaller
 * checksum to calculate.
 */
int devio_readtrans(struct etfs_devio *dev, unsigned cluster, struct etfs_trans *trp)
{
    struct spare_bch4    sp;
    int                  partition_ecc_type = ECC_BCH4;
    unsigned             page;
    CHIPIO               *cio = dev->cio;

    /* Determine if it's an IPL read */
    partition_ecc_type = getPartitionEccType(cio, cluster);

    if(ECC_BCH4 == partition_ecc_type) {
        /* The requested page is either IFS or ETFS File system */
        page = cluster;
        nand_write_cmd(cio, NANDCMD_READ_2048);
        nand_write_pageaddr(cio, page, cio->addrcycles, 1);
        nand_write_cmd(cio, NANDCMD_READCONFIRM_2048);

        if(nand_wait_busy(cio, MAX_READ_USEC) != 0) {
            dev->log(_SLOG_CRITICAL, "Timeout on READ");
        }

        nand_read_data(cio, (uint8_t *)&sp, sizeof sp);
        cio->lastpage = page;

        if(sp.status[0] != 0xff || sp.status[1] != 0xff) {
            dev->log(_SLOG_ERROR, "readtrans BADBLK on cluster %d", cluster);
            return(ETFS_TRANS_BADBLK);
        } else {
            /* sp[6] contains the erase signature and thus should not be 0xFF */
            if(((uint64_t*)&sp)[0] == ~0ll && ((uint64_t*)&sp)[1] == ~0ll &&
                ((uint64_t*)&sp)[2] == ~0ll && ((uint64_t*)&sp)[3] == ~0ll &&
                ((uint64_t*)&sp)[4] == ~0ll && ((uint64_t*)&sp)[5] == ~0ll &&
                ((uint64_t*)&sp)[7] == ~0ll) {
                if(sp.erasesig[0] == ERASESIG1 && sp.erasesig[1] == ERASESIG2) {
                    return(ETFS_TRANS_ERASED);
                } else if(sp.erasesig[0] != 0xFF || sp.erasesig[1] != 0xFF) {
                    return(ETFS_TRANS_FOXES);
                }
            } else {
                if(dev->crc32((uint8_t *) &sp, sizeof(sp) - sizeof(sp.crctrans) - 4) != sp.crctrans) {
                    dev->log(_SLOG_ERROR, "readtrans spare error on cluster %d -- attempting to recover...", cluster);

                    // re-read the page through read cluster
                    if(dev->flags & ETFS_DEVIO_FLAG_DMABUF) {
                        // let devio_readcluster() to allocate data buffer
                        return devio_readcluster(dev, cluster, NULL, trp);
                    } else {
                        int     rc;
                        void    *tbuf = malloc(PAGESIZE_2048);
                        rc = devio_readcluster(dev, cluster, tbuf, trp);
                        free(tbuf);
                        return (rc);
                    }
                }

                trp->sequence    = ENDIAN_LE32(sp.sequence);
                trp->fid         = ENDIAN_LE16(sp.fid);
                trp->cluster     = ENDIAN_LE32(sp.cluster);
                trp->nclusters   = sp.nclusters;
                return(ETFS_TRANS_OK);
            }
        }
    } else if(ECC_BCH8 == partition_ecc_type) {
        /*
         * This should not occur since the requested
         * page is in the IPL partition and
         * there is no metadata stored for IPL.
         *
         * If we get here, we could either call readcluster (and use scrach_buf)
         * or simply return error.
         */
        trp->tacode     = ETFS_TRANS_DATAERR;
        return(trp->tacode);
    }
#ifdef BIOS
    else {
        struct spare_hamming sp;
        /* The requested page is the BIOS partition -- Use Hamming ECC */
        page = cluster;
        nand_write_cmd(cio, NANDCMD_READ_2048);
        nand_write_pageaddr(cio, page, cio->addrcycles, 1);
        nand_write_cmd(cio, NANDCMD_READCONFIRM_2048);

        if(nand_wait_busy(cio, MAX_READ_USEC) != 0) {
            dev->log(_SLOG_CRITICAL, "Timeout on READ");
        }

        nand_read_data(cio, (uint8_t *)&sp, sizeof sp);
        cio->lastpage = page;

        if(sp.status != 0xff || sp.status2 != 0xff) {
            dev->log(_SLOG_ERROR, "readtrans BADBLK on cluster %d", cluster);
            return(ETFS_TRANS_BADBLK);
        } else {
            /* Technically, there's no reason (need) for this call for the
             * bios parition.
             * Spare is not protected. We will return the sequence
             * which is the only metadata available. If the sequence number
             * doesn't make sense to the application calling it, it will report
             * an error.
             */
            trp->sequence    = ENDIAN_LE32(sp.sequence);
            return(ETFS_TRANS_OK);
        }
    }
#endif
    return ETFS_TRANS_DEVERR;
}

/*
 * Post a cluster of data.
 *
 * Set CRC for spare area.
 * The passed buffer "buf" is larger than the cluster size. It can hold
 * PAGESIZE bytes. This is for convenience writing
 * data to the device ands calculating the CRC. The work area after clustersize
 * bytes is ignored by the caller.
 *
 * NOTE:      Both erase and write for the lower NAND SLC need to go through this function.
 *            If a post/erase is being performed on the lower NAND (where IPL and IFS are located)
 *            you need to unlock the bottom first. To do that, etfs_trans (trp) needs to contain the action
 *            (MAGIC_OP_ERASE or MAGIC_OP_WRITE) in trp->fid followed by the unlock key in trp->cluster. The
 *            optional force_ecc debug parameter is also added to the last 4 bits of trp->fid if needed.
 *
 *            If operation is erase, the function internally calls internal_devio_eraseblk and erases
 *            the ONE block located in cluster. If operation is write, the function unlocks the block
 *            which cluster belongs to
 */

int devio_postcluster(struct etfs_devio *dev, unsigned cluster, uint8_t *buf, struct etfs_trans *trp)
{
    struct spare_bch4   *sp_bch4;
    uint8_t             status;
    unsigned            page;
    CHIPIO              *cio = dev->cio;
    int                 ret;
    int                 partition_ecc_type = ECC_BCH4;
    int                 bLowNAND = 0;    // Flag used to indicate whether in IFS/IPL partition
    int                 mLowNAND;        // How many blocks of low NAND portion
    paddr_t             paddr = 0;

    mLowNAND = LowNand(cio);
    /* Determine the partition type*/
    partition_ecc_type = getPartitionEccType(cio, cluster);

    if(dev->flags & ETFS_DEVIO_FLAG_DMABUF) {
        paddr = (trp) ? ETFS_BUF_MPHYS(dev, buf) : nand_dma_buf(cio, NULL);
    }

    if(trp) {
        /* Determine whether it's a write/erase to the lower portion of NAND */
        if((MAGIC_NUMBER == trp->cluster) && (cluster < (PAGES2BLK_2048 * mLowNAND))) {
            /* Unlock the block and set flag to true */
            bLowNAND = 1;
            nand_unlock_blocks(dev, cluster>>6, cluster>>6);

            /* Determine whether it is a write, erase or block-marking */
            if(MAGIC_OP_ERASE == (trp->fid & 0xfffffff0)) {
                /* Erase the block */
                if((ret = internal_eraseblk(dev, cluster>>6)) != ETFS_TRANS_OK) {
                    dev->log(_SLOG_ERROR,"Could not erase blk = %d, which is in the lower part of NAND", cluster>>6);
                    nand_unlock_blocks(dev, mLowNAND, (dev->numblks - 1));
                    return ret;
                }
                memset(buf, 0xff, PAGESIZE_2048);
                trp = NULL;
            } else if(MAGIC_OP_MRK_BLK_BAD == (trp->fid & 0xfffffff0)) {
                /* Mark block as bad */
                if((ret = internal_markBlkBad(dev, cluster>>6)) != 0) {
                    dev->log(_SLOG_ERROR, "Could not mark blk = %d as bad", cluster>>6);
                    nand_unlock_blocks(dev, mLowNAND, (dev->numblks - 1));
                    return (ETFS_TRANS_DEVERR);
                }
                return ETFS_TRANS_OK;
            } else if(MAGIC_OP_MRK_BLK_GOOD == (trp->fid & 0xfffffff0)) {
                /* Mark block as good */
                if((ret = internal_markBlkGood(dev, cluster>>6)) != 0) {
                    dev->log(_SLOG_ERROR, "Could not mark blk = %d as good", cluster>>6);
                    nand_unlock_blocks(dev, mLowNAND, (dev->numblks - 1));
                    return (ETFS_TRANS_DEVERR);
                }
                return ETFS_TRANS_OK;
            } else if( MAGIC_OP_WRITE != (trp->fid & 0xfffffff0)) {
                /* Invalid code sent. Lock the lower portions */
                dev->log(_SLOG_ERROR, "Invalid code set. Lower NAND was NOT unlocked");
                nand_unlock_blocks(dev, mLowNAND, (dev->numblks - 1));
                bLowNAND = 0;
            }
        }
    }

    page = cluster;
    if(ECC_BCH4 == partition_ecc_type) {
        /* The requested page is either IFS or ETFS File system */
        /* Build spare area */
        sp_bch4 = (struct spare_bch4 *) (buf + DATASIZE_2048);
        memset((void *)sp_bch4, 0xff, sizeof(*sp_bch4));
        sp_bch4->erasesig[0] = ERASESIG1;
        sp_bch4->erasesig[1] = ERASESIG2;

        if(trp) {
            sp_bch4->sequence   = ENDIAN_LE32(trp->sequence);
            sp_bch4->fid        = ENDIAN_LE16((uint16_t) trp->fid);
            sp_bch4->cluster    = ENDIAN_LE32(trp->cluster);
            sp_bch4->nclusters  = trp->nclusters;
            sp_bch4->status[0]  = 0xff;
            sp_bch4->status[1]  = 0xff;
        }
        nand_write_cmd(cio, NANDCMD_PROGRAM_2048);
        nand_write_pageaddr(cio, page, cio->addrcycles, 0);

        if(trp) {   // Perform hardware ECC on actual writes (and not erases)
            nand_reset_hw_ecc(cio, ECC_BCH4, 1);
        }

        /* write data portion first (either directly or through prefetch engine)*/
        if(usePRWP(cio)) {
            nand_prefetch_reset(cio);
            if(nand_prefetch_write_page(cio, buf, paddr, SPARE_BCH4_WRITESIZE) != 0) {
                dev->log(_SLOG_CRITICAL, "write-post write error (data)");
                return ETFS_TRANS_DATAERR;
            }
        } else {
            nand_write_data(cio, buf, SPARE_BCH4_WRITESIZE);
        }

        if(trp) {
            nand_get_hw_ecc(cio, ECC_BCH4, &sp_bch4->eccv[0][0]);
        }

        if(trp) {
            /* Now that all ECC values and other spare parts are populated, calculate CRC of the spare section */
            sp_bch4->crctrans   = dev->crc32((uint8_t *) sp_bch4, sizeof(*sp_bch4) - sizeof(sp_bch4->crctrans) - 4);

            if((cluster % dev->clusters2blk) == 0) {
                sp_bch4->erasesig[0] = ~0;    // Can only punch bits down once and we did it
                sp_bch4->erasesig[1] = ~0;    // on the erase
            }
        }
        /* Write spare portion (either directly or through prefetch engine */
        if(usePRWP(cio)) {
            if(nand_prefetch_write_page(cio, &buf[SPARE_BCH4_WRITESIZE], paddr+SPARE_BCH4_WRITESIZE, (PAGESIZE_2048 - SPARE_BCH4_WRITESIZE)) != 0) {
                dev->log(_SLOG_CRITICAL, "write-post error (spare)");
                return ETFS_TRANS_DATAERR;
            }
        } else {
            nand_write_data(cio, &buf[SPARE_BCH4_WRITESIZE], (PAGESIZE_2048 - SPARE_BCH4_WRITESIZE));
        }

        nand_write_cmd(cio, NANDCMD_PROGRAMCONFIRM_2048);

        if(nand_wait_busy(cio, MAX_POST_USEC) != 0) {
            dev->log(_SLOG_CRITICAL, "Timeout on POST");
        }

        /* Read status */
        nand_write_cmd(cio, NANDCMD_STATUSREAD_2048);
        nand_read_status(cio, &status, 1);
        if((status & 0xC3) != 0xC0) {
            /* Error on write! Mark the block containing this page as BAD */
            dev->log(_SLOG_ERROR, "Post error on page %d", page);
            if(bLowNAND) {
                nand_unlock_blocks(dev, mLowNAND, (dev->numblks - 1));
            }
            return(ETFS_TRANS_DEVERR);
        }

        if(bLowNAND) {// Unlock the top memory again
            nand_unlock_blocks(dev, mLowNAND, (dev->numblks - 1));
        }
        return(ETFS_TRANS_OK);
    } else if(ECC_BCH8 == partition_ecc_type) {
        /* The requested page is in IPL, perform BCH8 ECC only for IPL */
        struct    spare_bch8    *sp_bch8;
        int        i;
        int        eccsize      = 512;
        int        eccbytes     = 14;
        int        eccsteps     = 4;
        uint8_t    *p           = buf;
        uint8_t    *ecc_calc;

        sp_bch8 = (struct spare_bch8 *) (buf + DATASIZE_2048);
        ecc_calc = &sp_bch8->eccv[0][0];

        if(0 == page % PAGES2BLK_2048) {
            /* Make sure IPL partition has been erased properly */
            dev->log(_SLOG_WARNING, "Erase NAND @ Block%d", page>>6);
            internal_eraseblk(dev, page>>6);
        }

        dev->log(_SLOG_WARNING, "Program IPL @ page%d", page);
        nand_write_cmd(cio, NANDCMD_PROGRAM_2048);
        nand_write_pageaddr(cio, page, cio->addrcycles, 0);

        /* Write the data portion first, and get BCH8 ECC in spare[2:57] */
        for(i = 0; eccsteps; eccsteps--, i += eccbytes, p += eccsize) {
            nand_reset_hw_ecc(cio,ECC_BCH8, 1);                  // Reset ECC
            nand_write_data(cio, p, eccsize);           // Write 512 bytes data
            nand_get_hw_ecc(cio, ECC_BCH8, &ecc_calc[i]);     // Read ECC value to spare
        }
        /* Write spare portion (either directly or through prefetch engine */
        if(usePRWP(cio)) {
            if(nand_prefetch_write_page(cio, &buf[DATASIZE_2048], paddr+DATASIZE_2048, SPARESIZE_2048) != 0) {
                dev->log(_SLOG_CRITICAL, "write-post error (spare)");
                return ETFS_TRANS_DATAERR;
            }
        } else {
            nand_write_data(cio, &buf[DATASIZE_2048], SPARESIZE_2048);
        }

        nand_write_cmd(cio, NANDCMD_PROGRAMCONFIRM_2048);

        if(nand_wait_busy(cio, MAX_POST_USEC) != 0) {
            dev->log(_SLOG_CRITICAL, "Timeout on POST");
        }

        /* Read status */
        nand_write_cmd(cio, NANDCMD_STATUSREAD_2048);
        nand_read_status(cio, &status, 1);
        if((status & 0xC3) != 0xC0) {
            /* Error on write! Mark the block containing this page as BAD */
            dev->log(_SLOG_ERROR, "Post error on page %d", page);
            if(bLowNAND) {
                nand_unlock_blocks(dev, mLowNAND, (dev->numblks - 1));
            }
            return(ETFS_TRANS_DEVERR);
        }

        if(bLowNAND) {            // Unlock the top memory again
            nand_unlock_blocks(dev, mLowNAND, (dev->numblks - 1));
        }
        return(ETFS_TRANS_OK);
    }
#ifdef BIOS
    else {
        /* The requested page is the BIOS partition -- Use Hamming ECC */
        struct spare_hamming *sp_hamm;
        sp_hamm = (struct spare_hamming *) (buf + DATASIZE_2048);

        nand_write_cmd(cio, NANDCMD_PROGRAM_2048);
        nand_write_pageaddr(cio, page, cio->addrcycles, 0);

        /* Perform hardware ECC on actual writes (and not erases) */
        if(trp) {
            nand_reset_hw_ecc(cio, ECC_HAMM, 1);
        }

        /* write data portion first (either directly or through prefetch engine)*/
        if(usePRWP(cio)) {
            nand_prefetch_reset(cio);
            if(nand_prefetch_write_page(cio, buf, paddr, DATASIZE_2048) != 0) {
                dev->log(_SLOG_CRITICAL, "write-post write error (data)");
                return ETFS_TRANS_DATAERR;
            }
        } else {
            nand_write_data(cio, buf, DATASIZE_2048);
        }

        /* Populate the calculated ecc bits + other spare data in the spare area */
        if(trp) {
            nand_get_hw_ecc(cio, ECC_HAMM, &sp_hamm->ecc[0]);
            sp_hamm->sequence = ENDIAN_LE32(trp->sequence);
        }

        /* Write spare portion (either directly or through prefetch engine */
        if(usePRWP(cio)) {
            if(nand_prefetch_write_page(cio, &buf[DATASIZE_2048], paddr+DATASIZE_2048, (SPARESIZE_2048)) != 0) {
                dev->log(_SLOG_CRITICAL, "write-post error (spare)");
                return ETFS_TRANS_DATAERR;
            }
        } else {
            nand_write_data(cio, &buf[DATASIZE_2048], (SPARESIZE_2048));
        }

        nand_write_cmd(cio, NANDCMD_PROGRAMCONFIRM_2048);

        if(nand_wait_busy(cio, MAX_POST_USEC) != 0) {
            dev->log(_SLOG_CRITICAL, "Timeout on POST");
        }

        /* Read status */
        nand_write_cmd(cio, NANDCMD_STATUSREAD_2048);
        nand_read_status(cio, &status, 1);
        if((status & 0xC3) != 0xC0) {
            /* Error on write!  */
            dev->log(_SLOG_ERROR, "Post error on page %d", page);
            if(bLowNAND) {
                nand_unlock_blocks(dev, mLowNAND, (dev->numblks - 1));
            }
            return(ETFS_TRANS_DEVERR);
        }

        if(bLowNAND) {// Unlock the top memory again
            nand_unlock_blocks(dev, mLowNAND, (dev->numblks - 1));
        }
        return(ETFS_TRANS_OK);
    }
#endif
    // should not reach here
    return(ETFS_TRANS_OK);
}


/*
 * Erase a block.
 */
int devio_eraseblk(struct etfs_devio *dev, unsigned blk)
{
    uint8_t     status;
    CHIPIO      *cio = dev->cio;
    uint8_t     *buf;

    /* Cannot erase lower NAND by calling this function. Refer to devio_postcluster for more information */
    if(blk < LowNand(cio)) {
        return ETFS_TRANS_OK;
    }

    /* Erase the blk */
    nand_write_cmd(cio, NANDCMD_ERASE_2048);
    nand_write_blkaddr(cio, blk, cio->addrcycles);        // Actually 3 Address Cycles for NAND2048 device
    nand_write_cmd(cio, NANDCMD_ERASECONFIRM_2048);
    if(nand_wait_busy(cio, MAX_ERASE_USEC) != 0) {
        dev->log(_SLOG_CRITICAL, "Timeout on ERASE");
    }

    /* Read status */
    nand_write_cmd(cio, NANDCMD_STATUSREAD_2048);
    nand_read_status(cio, &status, 1);
    if((status & 0xC3) != 0xC0) {
        dev->log(_SLOG_ERROR, "erase error on blk %d (%2.2x)", blk, status);
        return(ETFS_TRANS_DEVERR);
    }

    /*
     * Write the erase signature. We only write non FFs in the first 16 bytes of the
     * spare area and put FFs everywhere else. This is required for
     * multilevel NAND devices.
     */
    if(dev->flags & ETFS_DEVIO_FLAG_DMABUF) {
        nand_dma_buf(cio, &buf);
    } else {
        buf = malloc(PAGESIZE_2048);
    }

    memset(buf, 0xff, PAGESIZE_2048);

    status = devio_postcluster(dev, blk * dev->clusters2blk, buf, NULL);


    if(!(dev->flags & ETFS_DEVIO_FLAG_DMABUF)) {
        free(buf);
    }

    return(status);
}


/*
 * This is the internal eraseblk function which is not accessible to
 * the outside world. This function is called if an erase is requested
 * to the lower 128MB of the SLC NAND. The request goes through devio_postcluster
 * and if the magic key is provided correctly, this function is called.
 *
 * The locking and unlocking is done in postcluster and not here. This way, if one tries to
 * call this function by mistake, they cannot erase the lower portion because it is still locked.
 */
int internal_eraseblk(struct etfs_devio *dev, unsigned blk)
{
    uint8_t            status;
    CHIPIO            *cio = dev->cio;

    /* Erase the blk */
    nand_write_cmd(cio, NANDCMD_ERASE_2048);
    nand_write_blkaddr(cio, blk, cio->addrcycles);        // Actually 3 Address Cycles for NAND2048 device
    nand_write_cmd(cio, NANDCMD_ERASECONFIRM_2048);

    if(nand_wait_busy(cio, MAX_ERASE_USEC) != 0) {
        dev->log(_SLOG_CRITICAL, "Timeout on ERASE");
    }

    nand_write_cmd(cio, NANDCMD_STATUSREAD_2048);
    nand_read_status(cio, &status, 1);
    if((status & 0xC3) != 0xC0) {
        dev->log(_SLOG_ERROR, "erase error on blk %d (%2.2x)", blk, status);
        return(ETFS_TRANS_DEVERR);
    }

    return ETFS_TRANS_OK;
}


/*
 * This function marks a block as BAD by writing to the first
 * 2 bytes of the spare area of the first page of the input block.
 *
 * This function does not lock/unlock the block and therefore the calling
 * function must make sure the block is unlocked.
 *
 * returns 0 if marking was successful and -1 otherwise.
 */
int internal_markBlkBad(struct etfs_devio *dev, unsigned blk)
{
    uint8_t            status;
    CHIPIO            *cio = dev->cio;
    uint8_t*        blkStatus;

    blkStatus = alloca(2);

    /*     Mark Block as bad. Find the first page of the block
     *  and write zero to the status bytes
     */

    nand_write_cmd(cio, NANDCMD_PROGRAM_2048);
    nand_write_pageaddr(cio, blk*PAGES2BLK_2048, cio->addrcycles, 1);

    blkStatus[0] = 0x00;
    blkStatus[1] = 0x00;

    /* Write only to the status bytes */
    nand_write_data(cio, blkStatus, 2);
    nand_write_cmd(cio, NANDCMD_PROGRAMCONFIRM_2048);
    if(nand_wait_busy(cio, MAX_POST_USEC) != 0) {
        dev->log(_SLOG_CRITICAL, "Timeout on POST");
    }

    nand_write_cmd(cio, NANDCMD_STATUSREAD_2048);
    nand_read_status(cio, &status, 1);
    if((status & 0xC3) != 0xC0) {
        dev->log(_SLOG_ERROR, "Unable to mark blk %d as bad (%x)",blk, status);
        return -1;
    }

    return 0;
}


/*
 * FOR TESTING ONLY!!!!!
 *
 * This function marks a block as GOOD by writing 0xFF to the first
 * 2 bytes of the spare area of the first page of the input block.
 *
 * This function does not lock/unlock the block and therefore the calling
 * function must make sure the block is unlocked.
 *
 * returns 0 if marking was successful and -1 otherwise.
 *
 */
int internal_markBlkGood(struct etfs_devio *dev, unsigned blk)
{
    uint8_t            status;
    CHIPIO            *cio = dev->cio;
    uint8_t*        blkStatus;

    blkStatus = alloca(2);

    /*     Mark Block as bad. Find the first page of the block
     *  and write zero to the status bytes
     */

    nand_write_cmd(cio, NANDCMD_PROGRAM_2048);
    nand_write_pageaddr(cio, blk*PAGES2BLK_2048, cio->addrcycles, 1);

    blkStatus[0] = 0xff;
    blkStatus[1] = 0xff;

    /* write only to the status bytes */
    nand_write_data(cio, blkStatus, 2);
    nand_write_cmd(cio, NANDCMD_PROGRAMCONFIRM_2048);
    if(nand_wait_busy(cio, MAX_POST_USEC) != 0) {
        dev->log(_SLOG_CRITICAL, "Timeout on POST");
    }

    nand_write_cmd(cio, NANDCMD_STATUSREAD_2048);
    nand_read_status(cio, &status, 1);
    if((status & 0xC3) != 0xC0) {
        dev->log(_SLOG_ERROR, "Unable to mark blk %d as good (%x)",blk, status);
        return -1;
    }

    return 0;
}


/*
 * Called to allow the driver to flush any cached data that
 * has not be written to the device. The NAND class driver does
 * not need it.
 */
int devio_sync(struct etfs_devio *dev)
{
    return(-1);
}

void fix_errors_bch(struct etfs_devio *dev, unsigned ecc_mode ,uint8_t *data, uint8_t *sp, uint32_t error_count, uint32_t *error_loc)
{
    uint8_t  count = 0;
    uint32_t error_byte_pos;
    uint32_t error_bit_mask;
    uint32_t last_bit;
    uint8_t  bInSpare = 0;
    uint32_t offset;

    if(ECC_BCH8 == ecc_mode) {
        last_bit = (26 * 4) - 1; //nibbles
    } else {
        last_bit = (13 * 4) - 1; //nibbles
    }

    if(sp == NULL) { // spare is not protected
        offset = 512;
    } else { // spare is protected
        offset = 516;
    }

    /* Flip all bits as specified by the error location array. */
    /* FOR( each found error location flip the bit ) */
    for(count = 0; count < error_count; count++) {
        if(error_loc[count] > last_bit) {
            /* Remove the ECC spare bits from correction. */
            error_loc[count] -= (last_bit + 1);

            /* Offset bit in data region */
            error_byte_pos = ((offset * 8) - (error_loc[count]) - 1) /8;

            if(error_byte_pos > offset) {
                break;
            }

            if(error_byte_pos > 511) {
                bInSpare = 1;
            }

            /* Error Bit mask */
            error_bit_mask = 0x1 << (error_loc[count] % 8);

            /* Toggle the error bit to make the correction. */
            if(bInSpare && sp != NULL) {
                sp[error_byte_pos-512] ^= error_bit_mask;
            } else {
                data[error_byte_pos] ^= error_bit_mask;
            }
            bInSpare = 0;
        }

    }
}

int error_correction_code(struct etfs_devio *dev, unsigned ecc_mode ,unsigned page ,uint8_t *buf, uint8_t *sp ,uint8_t *ecc)
{
    uint8_t  syndrome_ecc[28];
    uint8_t  n_bytes;
    int      nibbles;
    uint32_t error_count = 0;
    uint32_t error_loc[16];
    int      i,j;

    if(ECC_BCH8 == ecc_mode) {
        // 8 bit ecc
        n_bytes = 13;
        nibbles = 26;
    } else {
        // 4 bit ecc
        n_bytes = 7;
        nibbles = 13;
    }

    for(i=0; i < n_bytes; i++) {
        if(ecc[i] != 0) {
            break;
        } else if(i == n_bytes - 1) {
            return ETFS_ECC_GOOD;
        }
    }

    /* rotating the syndrome bytes */
    for(i=0, j=(n_bytes-1);i<n_bytes;i++, j--) {
        syndrome_ecc[i]= ecc[j];
    }

    /* initialize the ELM */
    out32(base_elm + ELM_SYSCONFIG,in32(base_elm + ELM_SYSCONFIG) | ELM_SYSCONFIG_SOFTRESET);
    while(!(in32(base_elm + ELM_SYSSTATUS) & 1));

    /* ECC and the ECC buf size MUST be set to 0x7ff */
    if(ECC_BCH8 == ecc_mode) {
        out32(base_elm + ELM_LOCATION_CONFIG, ELM_LOCATION_CONFIG_8BIT | ELM_LOCATION_CONFIG_ECC_SIZE_2047);
    } else {
        out32(base_elm + ELM_LOCATION_CONFIG, ELM_LOCATION_CONFIG_4BIT | ELM_LOCATION_CONFIG_ECC_SIZE_2047);
    }

    /* Continues mode */
    out32(base_elm + ELM_PAGE_CTRL, 0);

    /* Disable all the interrupts and enable the PAGE_Mask interrupt */
    out32(base_elm + ELM_IRQENABLE, 0x1);

    /* use elm module to check for errors */
    if(elm_error_check(dev, ecc_mode, syndrome_ecc, nibbles, &error_count, error_loc) != 0) {
        return ETFS_ECC_ERROR;
    }

    /* correct bch error */
    if(error_count > 0) {
        dev->log(_SLOG_WARNING, "error_count = %d, page=%d", error_count,page);
        fix_errors_bch(dev, ecc_mode, buf, sp, error_count, error_loc);
        return ETFS_ECC_CORRECTED;
    } else {
        return ETFS_ECC_GOOD;
    }
}

#ifdef BIOS
unsigned int __sw_hweight32(unsigned int w)
{
	unsigned int res = w - ((w >> 1) & 0x55555555);
	res = (res & 0x33333333) + ((res >> 2) & 0x33333333);
	res = (res + (res >> 4)) & 0x0F0F0F0F;
	res = res + (res >> 8);
	return (res + (res >> 16)) & 0x000000FF;
}

int hamming_error_correction(struct etfs_devio *dev, unsigned page ,uint8_t *buf, uint8_t *read_ecc ,uint8_t *calc_ecc)
{
    uint32_t read_ecc_regen;
    uint32_t calc_ecc_regen;
    uint32_t res, hm;
    uint16_t parity_bits, byte;
    uint8_t bit;
    int i,j;
    int ret = ETFS_TRANS_OK;

    for(i=0, j=0; i < 4; i++, j+=3) {
        read_ecc_regen = read_ecc[j+0] | (read_ecc[j+1] << 16) | ((read_ecc[j+2] & 0xF0) << 20) |
                        ((read_ecc[j+2] & 0x0F) << 8);
        calc_ecc_regen = calc_ecc[j+0] | (calc_ecc[j+1] << 16) | ((calc_ecc[j+2] & 0xF0) << 20) |
                        ((calc_ecc[j+2] & 0x0F) << 8);

        res = read_ecc_regen ^ calc_ecc_regen;
		if(res) {
		    hm = __sw_hweight32(res);

		    if(hm == 12) {
		        parity_bits = res>>16;
		        bit = (parity_bits & 0x7);
		        byte = (parity_bits>>3)&0x1FF;
		        byte += (i*512);
		        dev->log(_SLOG_WARNING, "Found 1 bit error in page %d, byte %d, bit offset %d", page, byte, bit);
		        buf[byte] ^= (1<<bit);
		        ret = ETFS_TRANS_ECC;
		    } else {
		        dev->log(_SLOG_WARNING, "Uncorrectable ECC detected on page %d", page);
		        ret = ETFS_TRANS_DATAERR;
		    }
		}
    }

    return ret;
}
#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/etfs/nand2048/jacinto5_micron/devio.c $ $Rev: 767198 $")
#endif
