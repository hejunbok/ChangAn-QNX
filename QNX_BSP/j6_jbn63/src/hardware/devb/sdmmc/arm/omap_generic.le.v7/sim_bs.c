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

// Module Description:  board specific interface

#include <sim_sdmmc.h>
#include <internal.h>
#include <hw/dcmd_sim_mmcsd.h>

/* response flags in mmcsd driver are on bits 4:0,
 * while in sdmmc driver they are on bits 8:4. */
#define MMCSD_SDMMC_VUC_RESP_OFF 4

//#define SIM_BS_DEBUG

extern int sdmmc_bkops( SIM_HBA *hba, int tick );

int sim_bs_args( SIM_HBA *hba, char *options )
{
	char		*value;
	int			opt;
	static char	*opts[] = {
				NULL };

	while (options && *options != '\0') {
		if ((opt = getsubopt( &options, opts, &value)) == -1) {
		}

		switch (opt) {
			default:
				break;
		}
	}

	return( EOK );
}


static void sim_bs_dump_vuc( SIM_SDMMC_EXT *ext, MMCSD_VUC_CMD *vuc )
{
	sdio_slogf( _SLOGC_SDIODI, _SLOG_ERROR, 1, 1, "vuc addr      %p", vuc );
	sdio_slogf( _SLOGC_SDIODI, _SLOG_ERROR, 1, 1, "vuc flags:    0x%08x", vuc->flags );
	sdio_slogf( _SLOGC_SDIODI, _SLOG_ERROR, 1, 1, "vuc arg:      0x%08x", vuc->arg );
	sdio_slogf( _SLOGC_SDIODI, _SLOG_ERROR, 1, 1, "vuc cmd:      %04d", vuc->opcode );
	if( vuc->flags & MMCSD_VUC_RCA ) {
		sdio_slogf( _SLOGC_SDIODI, _SLOG_ERROR, 1, 1, "vuc rca:      0x%04x", ext->dev_inf.rca );
	}
	sdio_slogf( _SLOGC_SDIODI, _SLOG_ERROR, 1, 1, "vuc blksz:    %d", vuc->blk_sz );
	sdio_slogf( _SLOGC_SDIODI, _SLOG_ERROR, 1, 1, "vuc paddr:    0x%08x", vuc->data_ptr );
	sdio_slogf( _SLOGC_SDIODI, _SLOG_ERROR, 1, 1, "vuc data_len: %d", vuc->data_len );
	sdio_slogf( _SLOGC_SDIODI, _SLOG_ERROR, 1, 1, "vuc buf_off:  %d", vuc->buf_off );
	sdio_slogf( _SLOGC_SDIODI, _SLOG_ERROR, 1, 1, "vuc timeout:  %d", vuc->timeout );
	sdio_slogf( _SLOGC_SDIODI, _SLOG_ERROR, 1, 1, "vuc delay:    %d us", vuc->postdelay_us );
}

#ifdef SIM_BS_DEBUG
static void sim_bs_dump_buf( const void *buf, int len )
{
#define LINE 32
	int i,j;
	const unsigned char *data;

	data = (const unsigned char*) buf;

	printf( "devb-sdmmc buf %p\n", buf );

	for( i=0, j=0; i < len; i+=j ) {
		for( j=0; ( j < LINE ) && ( j+i < len ); j++ ) {
			printf( "%02x", data[j+i] );
			if ( ( j+1 ) % 4 == 0 ) {
				printf( " " );
			}
			if ( ( j+1 ) % LINE == 0 ) {
				printf( "\n" );
			}
		}
	}
	printf( "\n" );
#undef LINE
}
#endif


static int sim_bs_vuc_cmd( SIM_HBA *hba, CCB_DEVCTL *ccb )
{
	SIM_SDMMC_EXT		*ext;
	MMCSD_VUC_CMD		*vuc;
	SDMMC_PARTITION		*part;
	sdio_dev_info_t		*di;
	struct sdio_device	*dev;
	struct sdio_cmd		*cmd;
	sdio_sge_t			sge;
	int					status;
	uint32_t			cstatus;
	uint32_t			arg, flags, rsp[4];
	uint8_t				*buf;
	uint8_t				*io;

	ext = (SIM_SDMMC_EXT *)hba->ext;
	vuc	= (MMCSD_VUC_CMD *)ccb->cam_devctl_data;
	dev	= ext->device;
	di	= &ext->dev_inf;
	ccb->cam_devctl_status = EOK;

	/* saving pointer to beginning of command structure */
	buf = (uint8_t *)(vuc);

	/* check if the device is present */
	if( sdmmc_unit_ready( hba, (CCB_SCSIIO *)ccb ) != CAM_REQ_CMP ) {
		ccb->cam_devctl_status = EIO;
		return( CAM_REQ_CMP );
	}

	if( ( cmd = sdio_alloc_cmd( ) ) == NULL ) {
		ccb->cam_devctl_status = ENOMEM;
		return( CAM_REQ_CMP );
	}

	for( ; ; vuc++ )  {
		if( (((void *)vuc) + sizeof(MMCSD_VUC_CMD)) > (ccb->cam_devctl_data + ccb->cam_devctl_size) ) {
			ccb->cam_devctl_status = EINVAL;
			sdio_slogf( _SLOGC_SDIODI, _SLOG_ERROR, 1, 1, "%s(%d): VUC struct out of bounds", 
						__FUNCTION__, __LINE__ );
			goto error;
		}

#ifdef SIM_BS_DEBUG
		sim_bs_dump_vuc( ext, vuc );
#endif
		arg = vuc->arg;
		/* shift VUC flags to correct position in command flag.
		 * Attention: Response flags in the DCMD_MMCSD_VUC_CMD they are on bits 12:8.
		 *            In the sdmmc driver they are on bits 8:4, but in the mmcsd driver they are on bits 4:0,
		 *            Therefore the offset in the sdmmc driver is 4 bits and not 8 as in the mmcsd driver. */
		flags = (vuc->flags & MMCSD_VUC_RESP_MSK) >> (MMCSD_VUC_RESP_OFF - MMCSD_SDMMC_VUC_RESP_OFF);
		vuc->result = MMC_VUC_SUCCESS;

#if 0
		/* check if the command is allowed/supported by DCMD_MMCSD_VUC_CMD
		 * currently only GEN_CMD (CMD56) is supported */
		if ( vuc->opcode != MMC_GEN_CMD ) {
			vuc->result = MMC_VUC_FAILED;
			ccb->cam_devctl_status = ENOTTY;
			sdio_slogf( _SLOGC_SDIODI, _SLOG_ERROR, 1, 1, 
						"%s(%d): CMD %d not supported by DCMD_MMCSD_VUC_CMD", 
						__FUNCTION__, __LINE__, vuc->opcode );
			goto error;
		}
#endif

		/* if command has response, clear response fields */
		if( flags & SCF_RSP_PRESENT ) {
			memset( (void *)&vuc->resp[0], 0, 4*sizeof(uint32_t) );
		}

		/* do we need Application Specific Command (CMD 55)? */
		if( vuc->flags & MMCSD_VUC_ACMD ) {
			flags |= SCF_APP_CMD;
		}

		/* check if we need relative addresses */
		if ( vuc->flags & MMCSD_VUC_RCA ) {
			arg |= (di->rca << 16);
		}

		if( vuc->flags & MMCSD_VUC_DATA_MSK ) {

			flags |= SCF_CTYPE_ADTC;

			/* get partition to check type and permission */
			part = &ext->targets[ccb->cam_ch.cam_target_id].partitions[ccb->cam_ch.cam_target_lun];
			if( ( ext->dev_inf.flags & DEV_FLAG_CARD_LOCKED ) ) {
				vuc->result = MMC_VUC_FAILED;
				sdio_slogf( _SLOGC_SDIODI, _SLOG_ERROR, 1, 1, "%s(%d): card locked", 
							__FUNCTION__, __LINE__ );
				goto error;
			}
			/* not sure if this is correct for CMD56, but since eMMC can't be write protected it can't do any harm */
			if( ( vuc->flags & MMCSD_VUC_DATA_OUT ) && ( part->pflags & SDMMC_PFLAG_WP ) ) {
				vuc->result = MMC_VUC_FAILED;
				sdio_slogf( _SLOGC_SDIODI, _SLOG_ERROR, 1, 1, "%s(%d): write protected", 
							__FUNCTION__, __LINE__ );
				goto error;
			}
			/* no read/write to RPMB */
			if( ( part->config & MMC_PART_MSK ) == MMC_PART_RPMB ) {
				vuc->result = MMC_VUC_FAILED;
				sdio_slogf( _SLOGC_SDIODI, _SLOG_ERROR, 1, 1, "%s(%d): no rd/wr to RPMB partition", 
							__FUNCTION__, __LINE__ );
				goto error;
			}
			if( sdio_set_partition( ext->device, part->config ) != EOK ) {
				vuc->result = MMC_VUC_FAILED;
				sdio_slogf( _SLOGC_SDIODI, _SLOG_ERROR, 1, 1, "%s(%d): set partition failed", 
							__FUNCTION__, __LINE__ );
				goto error;
			}

			if( vuc->flags & MMCSD_VUC_DATA_IN ) {
				flags |= SCF_DIR_IN;
			}else{
				flags |= SCF_DIR_OUT;
			}

			if( vuc->data_len > di->sector_size ) {
				if( !( ext->hc_inf.caps & HC_CAP_ACMD12 ) ) {
					if( ( di->caps & DEV_CAP_CMD23 ) ) {
						flags |= SCF_SBC;
					}
				}
				flags |= SCF_MULTIBLK;
			}

			/* setup scatter gather element */
			sge.sg_count   = vuc->data_len;
			if( vuc->flags & MMCSD_VUC_DATA_PHYS ) {
				/* DMA mode, use physical address */
				flags |= SCF_DATA_PHYS;
				sge.sg_address = vuc->data_ptr;
			} else {
				/* check if bounds of data buffer are within bounds of devctl buffer */
				if( (vuc->buf_off > ccb->cam_devctl_size) || ((vuc->buf_off + vuc->data_len) > ccb->cam_devctl_size) ) {
					vuc->result = MMC_VUC_FAILED;
					ccb->cam_devctl_status = EINVAL;
					sdio_slogf( _SLOGC_SDIODI, _SLOG_ERROR, 1, 1, "%s(%d): IO buffer out of bounds", 
								__FUNCTION__, __LINE__ );
					goto error;
				}
				/* PIO mode, allocate IO buffer and copy out data from devctl buffer into it */
				if( !( io = sdio_alloc( vuc->data_len ) ) ) {
					vuc->result = MMC_VUC_FAILED;
					ccb->cam_devctl_status = ENOMEM;
					goto error;
				}
				if( vuc->flags & MMCSD_VUC_DATA_OUT ) {
					memcpy( io, buf + vuc->buf_off, vuc->data_len );
#ifdef SIM_BS_DEBUG
					sim_bs_dump_buf( buf + vuc->buf_off, vuc->data_len );
#endif
				}
				sge.sg_address = (paddr_t)io;
			}

			sdio_setup_cmd( cmd, flags, vuc->opcode, arg );
			sdio_setup_cmd_io( cmd, flags, vuc->data_len/di->sector_size, di->sector_size, &sge, 1, NULL );
		} else {
             sdio_setup_cmd( cmd, flags, vuc->opcode, arg );
		}

		if( ( status = sdio_send_cmd( dev, cmd, NULL, (vuc->timeout)? vuc->timeout : SDIO_TIME_DEFAULT, 0 ) ) != EOK ) {
			vuc->result = MMC_VUC_FAILED;
		}

		/* check if response is present and check for command status if appropriate
		 * Attention: Response flags in the DCMD_MMCSD_VUC_CMD they are on bits 12:8.
		 *            In the sdmmc driver they are on bits 8:4, but in the mmcsd driver they are on bits 4:0,
		 *            Therefore the offset in the sdmmc driver is 4 bits and not 8 as in the mmcsd driver. */
		if( vuc->flags & (SCF_RSP_PRESENT << (MMCSD_VUC_RESP_OFF - MMCSD_SDMMC_VUC_RESP_OFF) ) ) {
			sdio_cmd_status( cmd, &cstatus, vuc->resp );
			// check for urgent background operations
			if( ( vuc->resp[0] & CDS_URGENT_BKOPS ) ) {
				ext->bkops_status = ECSD_BS_OPERATIONS_CRITICAL;
				sdmmc_bkops( hba, CAM_FALSE );
			}
		}

		/* if PIO mode is used, response data must be copied from IO buffer to devctl buffer
		 * and the IO buffer must be released */
		if( !(vuc->flags & MMCSD_VUC_DATA_PHYS) ) {
			if( vuc->flags & MMCSD_VUC_DATA_IN ) {
				memcpy( buf + vuc->buf_off, io, vuc->data_len );
			}
			sdio_free(io, vuc->data_len);
		}

		if( status ) {
			if( status == ENXIO ) {				/* card has been removed */
			}

			if( sdio_send_status( dev, rsp, 0 ) || 
				( rsp[0] & ( CDS_READY_FOR_DATA | CDS_CUR_STATE_MSK ) ) != ( CDS_READY_FOR_DATA | CDS_CUR_STATE_TRAN ) ) {
				/* in case of error return card status in VUC command response.
				 * but this overwrites command response data. */
				memcpy( vuc->resp, rsp, sizeof( vuc->resp ) );
				if( sdio_stop_transmission( dev, 0 ) ) {
					status = ETIMEDOUT;
				}
			}
		} else {
			if( ( flags & SCF_MULTIBLK ) ) {
				if( ( !( flags & SCF_SBC ) && ( vuc->data_len > di->sector_size ) && 
					  !( ext->hc_inf.caps & HC_CAP_ACMD12 ) ) ) {
					if( sdio_stop_transmission( dev, 0 ) ) {
						status = ETIMEDOUT;
					}
				}
			}

			if( status == EOK /*&& ( flags & SCF_DIR_OUT )*/ ) {
				if( ( status = sdio_wait_card_status( dev, rsp, CDS_READY_FOR_DATA | 
						CDS_CUR_STATE_MSK, CDS_READY_FOR_DATA | CDS_CUR_STATE_TRAN, vuc->timeout ) ) != EOK ) {
					// in case of error return card status in VUC command response.
					// but this overwrites command response data.
					memcpy( vuc->resp, rsp, sizeof( vuc->resp ) );
					sdio_stop_transmission( dev, 0 );
				}
			}
		}
		if( (status != EOK) || (cstatus != CS_CMD_CMP) ) {
			vuc->result = MMC_VUC_FAILED;
			sdio_slogf( _SLOGC_SDIODI, _SLOG_ERROR, 1, 1, "%s(%d): status (%x), cstatus (%x)", 
						__FUNCTION__, __LINE__, status, cstatus );
			goto error;
		}

		/* delay after each VUC */
		if( vuc->postdelay_us ) {
			usleep(vuc->postdelay_us);
		}

		/* check if we reached the last VUC, else jump to next VUC */
		if( vuc->flags & MMCSD_VUC_END ) {
			break;
		}

	}

	sdio_free_cmd( cmd );

	return( CAM_REQ_CMP );

error:
	sim_bs_dump_vuc( ext, vuc );
	sdio_free_cmd( cmd );
	return( CAM_REQ_CMP_ERR );
}


static void sim_bs_copy_sd_csd_fields( MMCSD_CSD *csd, struct sdio_device *dev )
{
	sdio_csd_t *sdio_csd = sdio_get_csd(dev);

	csd->type = MMCSD_CARD_TYPE_SD;
	csd->csd_version = sdio_csd->csd_structure;
	csd->mmcprot_version = 0;

	csd->csd.sd_csd.csd_structure = sdio_csd->csd_structure;
	csd->csd.sd_csd.taac = sdio_csd->taac;
	csd->csd.sd_csd.nsac = sdio_csd->nsac;
	csd->csd.sd_csd.tran_speed = sdio_csd->tran_speed;
	csd->csd.sd_csd.ccc = sdio_csd->ccc;

	csd->csd.sd_csd.read_bl_len = sdio_csd->read_bl_len;
	csd->csd.sd_csd.read_bl_partial = sdio_csd->read_bl_partial;
	csd->csd.sd_csd.write_blk_misalign = sdio_csd->write_blk_misalign;
	csd->csd.sd_csd.read_blk_misalign = sdio_csd->read_blk_misalign;

	csd->csd.sd_csd.dsr_imp = sdio_csd->dsr_imp;
	csd->csd.sd_csd.r2w_factor = sdio_csd->r2w_factor;

	csd->csd.sd_csd.wp_grp_size = sdio_csd->wp_grp_size;
	csd->csd.sd_csd.wp_grp_enable = sdio_csd->wp_grp_enable;

	csd->csd.sd_csd.write_bl_len = sdio_csd->write_bl_len;
	csd->csd.sd_csd.write_bl_partial = sdio_csd->write_bl_partial;
	csd->csd.sd_csd.copy = sdio_csd->copy;

	csd->csd.sd_csd.perm_write_protect = sdio_csd->write_protect & 0x01;
	csd->csd.sd_csd.tmp_write_protect = (sdio_csd->write_protect & 0x02) >> 1;

	csd->csd.sd_csd.erase_blk_en = sdio_csd->erase_blk_en;
	csd->csd.sd_csd.sector_size = sdio_csd->sector_size;

	csd->csd.sd_csd.file_format_grp = 0; /* no equivalent value */
	csd->csd.sd_csd.file_format = 0; /* no equivalent value */

	if (sdio_csd->csd_structure	== CSD_STRUCT_VER_10) {
		csd->csd.sd_csd.csd.csd_ver1.c_size = sdio_csd->c_size;
		csd->csd.sd_csd.csd.csd_ver1.vdd_r_curr_min = sdio_csd->vdd_r_curr_min;
		csd->csd.sd_csd.csd.csd_ver1.vdd_r_curr_max = sdio_csd->vdd_r_curr_max;
		csd->csd.sd_csd.csd.csd_ver1.vdd_w_curr_min = sdio_csd->vdd_w_curr_min;
		csd->csd.sd_csd.csd.csd_ver1.vdd_w_curr_max = sdio_csd->vdd_w_curr_max;
		csd->csd.sd_csd.csd.csd_ver1.c_size_mult = sdio_csd->c_size_mult;
	}
	else {
		csd->csd.sd_csd.csd.csd_ver2.c_size = sdio_csd->c_size;
	}
}


static void sim_bs_copy_mmc_csd_fields( MMCSD_CSD *csd, struct sdio_device *dev )
{
	sdio_csd_t *sdio_csd = sdio_get_csd(dev);
	sdio_ecsd_t *sdio_ecsd = sdio_get_ecsd(dev);

	csd->type = MMCSD_CARD_TYPE_MMC;
	csd->csd_version = sdio_csd->csd_structure;
	csd->mmcprot_version = sdio_csd->spec_vers;

	csd->csd.mmc_csd.csd_structure = sdio_csd->csd_structure;
	csd->csd.mmc_csd.mmc_prot = sdio_csd->spec_vers;
	csd->csd.mmc_csd.taac = sdio_csd->taac;
	csd->csd.mmc_csd.nsac = sdio_csd->nsac;
	csd->csd.mmc_csd.tran_speed = sdio_csd->tran_speed;
	csd->csd.mmc_csd.ccc = sdio_csd->ccc;

	csd->csd.mmc_csd.read_bl_len = sdio_csd->read_bl_len;
	csd->csd.mmc_csd.read_bl_partial = sdio_csd->read_bl_partial;
	csd->csd.mmc_csd.write_blk_misalign = sdio_csd->write_blk_misalign;
	csd->csd.mmc_csd.read_blk_misalign = sdio_csd->read_blk_misalign;

	csd->csd.mmc_csd.dsr_imp = sdio_csd->dsr_imp;
	csd->csd.mmc_csd.r2w_factor = sdio_csd->r2w_factor;

	csd->csd.mmc_csd.wp_grp_size = sdio_csd->wp_grp_size;
	csd->csd.mmc_csd.wp_grp_enable = sdio_csd->wp_grp_enable;

	csd->csd.mmc_csd.write_bl_len = sdio_csd->write_bl_len;
	csd->csd.mmc_csd.write_bl_partial = sdio_csd->write_bl_partial;
	csd->csd.mmc_csd.copy = sdio_csd->copy;

	csd->csd.mmc_csd.perm_write_protect = sdio_csd->write_protect;
	csd->csd.mmc_csd.tmp_write_protect = sdio_csd->write_protect;

	csd->csd.mmc_csd.ecc = sdio_csd->ecc;

	csd->csd.mmc_csd.c_size = sdio_csd->c_size;
	csd->csd.mmc_csd.vdd_r_curr_min = sdio_csd->vdd_r_curr_min;
	csd->csd.mmc_csd.vdd_r_curr_max = sdio_csd->vdd_r_curr_max;
	csd->csd.mmc_csd.vdd_w_curr_min = sdio_csd->vdd_w_curr_min;
	csd->csd.mmc_csd.vdd_w_curr_max = sdio_csd->vdd_w_curr_max;
	csd->csd.mmc_csd.c_size_mult = sdio_csd->c_size_mult;

	if (sdio_csd->spec_vers	<= CSD_SPEC_VER_1) {
		csd->csd.mmc_csd.erase.mmc_v22.sector_size = sdio_csd->sector_size;
		csd->csd.mmc_csd.erase.mmc_v22.erase_grp_size = sdio_csd->erase_grp_size;
	}
	else {
		csd->csd.mmc_csd.erase.mmc_v31.erase_grp_size = sdio_csd->erase_grp_size;
		csd->csd.mmc_csd.erase.mmc_v31.erase_grp_mult = sdio_csd->erase_grp_mult;
	}

	/* extended csd information */
	csd->csd.mmc_csd.ext_csd.hs_max_dtr = sdio_ecsd->dtr_max_hs;
	csd->csd.mmc_csd.ext_csd.sectors = sdio_ecsd->sectors;
	csd->csd.mmc_csd.ext_csd.erase_grp_def = sdio_ecsd->erase_grp_def;
	csd->csd.mmc_csd.ext_csd.hc_erase_group_size = sdio_ecsd->hc_erase_group_size;
	csd->csd.mmc_csd.ext_csd.hc_wp_grp_size = sdio_ecsd->hc_wp_grp_size;
	csd->csd.mmc_csd.ext_csd.user_wp = sdio_ecsd->user_wp;
}


static int sim_bs_get_csd( SIM_HBA *hba, CCB_DEVCTL *ccb )
{
	SIM_SDMMC_EXT *ext;
	MMCSD_CSD *csd;

	ext = (SIM_SDMMC_EXT *)hba->ext;
	csd = (MMCSD_CSD *)ccb->cam_devctl_data;

	csd->pid = getpid();
	csd->flags &= MMCSD_FULL_CID;

	if (!(ext->eflags & SDMMC_EFLAG_PRESENT)) {
		csd->flags |= MMCSD_DEV_NO_MEDIA;
		return( CAM_REQ_CMP );
	}
	if (ext->dev_inf.flags & DEV_FLAG_WP) {
		csd->flags |= MMCSD_DEV_RDONLY;
	}

	csd->flags |= MMCSD_DEV_RDY;
	/* sdio_csd_t contains combined MMC and SD CSD fields so we need
	 * to manually copy each attributes in order to be consistent
	 * with the data structure of the mmcsd driver
	 */
	if (ext->dev_inf.dtype == DEV_TYPE_SD) {
		sim_bs_copy_sd_csd_fields(csd, ext->device);
	}
	if (ext->dev_inf.dtype == DEV_TYPE_MMC) {
		sim_bs_copy_mmc_csd_fields(csd, ext->device);
	}

	return( CAM_REQ_CMP );
}


#define MMCSD_FULL_CID_LEN	16
static int sim_bs_get_cid( SIM_HBA *hba, CCB_DEVCTL *ccb )
{

	SIM_SDMMC_EXT *ext;
	MMCSD_CID *cid;

	ext = (SIM_SDMMC_EXT *)hba->ext;
	cid = (MMCSD_CID *)ccb->cam_devctl_data;

	cid->pid = getpid();
	cid->flags &= MMCSD_FULL_CID;

	if (!(ext->eflags & SDMMC_EFLAG_PRESENT)) {
		cid->flags |= MMCSD_DEV_NO_MEDIA;
		return( CAM_REQ_CMP );
	}

	if (ext->dev_inf.flags & DEV_FLAG_WP) {
		cid->flags |= MMCSD_DEV_RDONLY;
	}

	sdio_csd_t *csd = sdio_get_csd(ext->device);

	cid->flags |= MMCSD_DEV_RDY;
	cid->speed = ext->dev_inf.dtr;
	cid->csd_version = csd->csd_structure;
	/* no equivalent value */
	cid->hwspec_version = 0;
	/* no equivalent value */
	cid->media_change = 0;
	/* no equivalent value */
	memset((void*)cid->slotname, 0, sizeof(cid->slotname));

	if (ext->dev_inf.dtype == DEV_TYPE_SD) {
		cid->type = MMCSD_CARD_TYPE_SD;
		cid->mmcprot_version = 0;
	}
	if (ext->dev_inf.dtype == DEV_TYPE_MMC) {
		cid->type = MMCSD_CARD_TYPE_MMC;
		cid->mmcprot_version = csd->spec_vers;
	}

	if (cid->flags & MMCSD_FULL_CID) {
		uint32_t *raw_cid = sdio_get_raw_cid(ext->device);
		memcpy((void*)&cid->cid.full_cid.cid, (void*)raw_cid, MMCSD_FULL_CID_LEN);
	}
	else { /* parsed CID */
		sdio_cid_t *parsed_cid = sdio_get_cid(ext->device);
		uint16_t year_code = (uint16_t) ((parsed_cid->year < 2000) ?
						parsed_cid->year : (parsed_cid->year-2000));
		/* sdio_cid_t contains combined MMC and SD CID fields so we need
		 * to manually copy each attributes in order to be consistent
		 * with the data structure of the mmcsd driver
		 */
		if (ext->dev_inf.dtype == DEV_TYPE_SD) {
			cid->cid.parsed_cid.sd_cid.oid[2] = 0;
			cid->cid.parsed_cid.sd_cid.oid[1] = (uint8_t)((parsed_cid->oid & 0xff00) >> 8);
			cid->cid.parsed_cid.sd_cid.oid[0] = (uint8_t)(parsed_cid->oid & 0xff);

			cid->cid.parsed_cid.sd_cid.mid = (uint8_t) parsed_cid->mid;
			/* parsed_cid->pnm is 8 bytes */
			memcpy((void*)&cid->cid.parsed_cid.sd_cid.pnm, (void*)&parsed_cid->pnm, 6);
			cid->cid.parsed_cid.sd_cid.prv = parsed_cid->prv;
			cid->cid.parsed_cid.sd_cid.psn = parsed_cid->psn;
			cid->cid.parsed_cid.sd_cid.mdt = ((parsed_cid->month & 0x0f) |
						((year_code & 0xff) << 4));
		}
		if (ext->dev_inf.dtype == DEV_TYPE_MMC) {
			cid->cid.parsed_cid.mmc_cid.mid = parsed_cid->mid;
			cid->cid.parsed_cid.mmc_cid.oid = (uint16_t) parsed_cid->oid;
			memcpy((void*)&cid->cid.parsed_cid.mmc_cid.pnm, (void*)&parsed_cid->pnm, 8);
			cid->cid.parsed_cid.mmc_cid.psn = parsed_cid->psn;
			cid->cid.parsed_cid.mmc_cid.mcd = (uint8_t) parsed_cid->month;
			cid->cid.parsed_cid.mmc_cid.ycd = year_code;
			cid->cid.parsed_cid.mmc_cid.hwr = 0; /* no equivalent value */
			cid->cid.parsed_cid.mmc_cid.fwr= 0; /* no equivalent value */
		}
	}

	return( CAM_REQ_CMP );
}

int sim_bs_devctl( SIM_HBA *hba, CCB_DEVCTL *ccb )
{
	int				cstatus;

	cstatus	= CAM_REQ_CMP;

	ccb->cam_devctl_status = EOK;

	switch( ccb->cam_devctl_dcmd ) {

		case DCMD_MMCSD_VUC_CMD:
			cstatus = sim_bs_vuc_cmd( hba, ccb );
			break;
		case DCMD_MMCSD_GET_CID:
			ccb->cam_ch.cam_status = sim_bs_get_cid(hba, ccb);
			break;
		case DCMD_MMCSD_GET_CSD:
			ccb->cam_ch.cam_status = sim_bs_get_csd(hba, ccb);
			break;


		default:
			ccb->cam_devctl_status = _RESMGR_DEFAULT;
			break;
	}

	return( cstatus );
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devb/sdmmc/arm/omap_generic.le.v7/sim_bs.c $ $Rev: 769090 $")
#endif
