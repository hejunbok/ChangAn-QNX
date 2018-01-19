/*
 * $QNXLicenseC: 
 * Copyright 2011, QNX Software Systems.  
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

#include "dwcotg.h"
 
//////////////////////////////
// Endpoint Commands        //
//////////////////////////////

void epcmd_build_DEPSTARTCFG( cmd_t * cmd, int xfer_rsc_idx ) {
	cmd->cmd = EPCMDTYP_DEPSTARTCFG | EPCMDACT | ( xfer_rsc_idx << EPCMDPARAM_XFER_RSC_IDX_POS );
	cmd->p0 = 0;
	cmd->p1 = 0;
	cmd->p2 = 0;	
}

// dir == 0 : OUT, dir !=0 : IN
void epcmd_build_DEPCFG( cmd_t * cmd, ep_ctx_t * ep, uint8_t dir, uint8_t ignore_seqn ) {
	uint32_t epdir;
	uint32_t nrdy_msk;

	// only use the dir parameter for control endpoints because they are
	// bi-directional and 2 epcmds need to be called: once per direction	
	if ( ep->type == USB_ATTRIB_CONTROL ) {
		epdir = ( dir ) ? EPCMD_PRM_EPDIR_IN : EPCMD_PRM_EPDIR_OUT;
		
		// only enable this event for control enpoints... it is required
		// to correctly implement the control transfer state machine
		nrdy_msk = EPCMD_PRM_XferNRdyEn;
	} else {
		epdir = ( ep->dir ) ? EPCMD_PRM_EPDIR_IN : EPCMD_PRM_EPDIR_OUT;
		nrdy_msk = 0;
	}
	
	cmd->cmd = EPCMDTYP_DEPCFG | EPCMDACT;

	cmd->p0 = 	( ep->type << EPCMD_PRM_EPType_POS ) 			|
				( ep->mps << EPCMD_PRM_MPS_POS )				|
				( ep->fifonum << EPCMD_PRM_FIFONUM_POS )		|
				( ep->burstsz << EPCMD_PRM_BURST_SZ_POS )		|
				(( ignore_seqn ) ? EPCMD_PRM_SEQN_IGNORE : 0 );

	cmd->p1 = 	nrdy_msk | EPCMD_PRM_XferCmplEn | epdir |
				( ep->num  << EPCMD_PRM_EPNUM_POS );

	cmd->p2 = 0;

}		

void epcmd_build_DEPSTRTXFER( cmd_t * cmd, trb_cluster_t * trb_cluster ) {

	cmd->cmd = EPCMDTYP_DEPSTRTXFER | EPCMDACT;
	cmd->p0 = trb_cluster->trb_base_paddr64 >> 32;
	cmd->p1 = trb_cluster->trb_base_paddr64  & 0xffffffff;
	cmd->p2 = 0;
}

void epcmd_build_DEPENDXFER( cmd_t * cmd, uint32_t xfer_rsc_idx ) {
	cmd->cmd = ( xfer_rsc_idx << EVT_EPPARAM_XFER_RSC_IDX_POS )| EPCMDTYP_DEPENDXFER | EPCMDACT;
	cmd->p0 = 0;
	cmd->p1 = 0;
	cmd->p2 = 0;
}


void epcmd_build_DEPXFERCFG( cmd_t * cmd, int n_xfer_resources ) {
	cmd->cmd = EPCMDTYP_DEPXFERCFG | EPCMDACT;
	cmd->p0 = n_xfer_resources;
	cmd->p1 = 0;
	cmd->p2 = 0;
}

void epcmd_build_DEPSETSTALL( cmd_t * cmd ) {
	cmd->cmd = EPCMDTYP_DEPSETSTALL | EPCMDACT;
	cmd->p0 = 0;
	cmd->p1 = 0;
	cmd->p2 = 0;	
}

void epcmd_build_DEPCSTALL( cmd_t * cmd ) {
	cmd->cmd = EPCMDTYP_DEPCSTALL | EPCMDACT;
	cmd->p0 = 0;
	cmd->p1 = 0;
	cmd->p2 = 0;	
}


// !!! Assumption !!! : epcm_send() is NOT thread safe, so it is assumed that
// the caller will synchronize commands... If we need to make this thread safe, I suggest
// adding a command mutex per endpoint
int epcmd_send( dctrl_t * dc, 	ep_ctx_t *ep, uint32_t epidx, cmd_t * cmd, int is_blocking )  {
	int err;	
	int timeout = 10;

	DWCOTG_SLOGF_DBG(dc, _SLOG_DEBUG2, "%s: cmd: ep->num = 0x%x epidx = %d", __func__, ep->num, epidx );
	DWCOTG_SLOGF_DBG(dc, _SLOG_DEBUG2, "%s: cmd: cmd-reg = 0x%x p0-reg = 0x%x p1-reg = 0x%x p2-reg = 0x%x", __func__, cmd->cmd, cmd->p0, cmd->p1, cmd->p2 );

	// reset the result event.. this will be set by the event handler
	if ( is_blocking ) {
		ep->cmd_complete_event = 0;
		cmd->p0 |= EPCMDIOC;
	}
		
	HW_Write32( dc->IoBase, DEPCMDPAR0(epidx), cmd->p0 );
	HW_Write32( dc->IoBase, DEPCMDPAR1(epidx), cmd->p1 );
	HW_Write32( dc->IoBase, DEPCMDPAR2(epidx), cmd->p2 );
	HW_Write32( dc->IoBase, DEPCMD(epidx), cmd->cmd  );
	
	// NOTE: This delay should be tuned such that the polling loop is never entered,  but not
	// overly long to slow down performace
	
	// wait for command to be accepted by the controller... this should never 
	while ( --timeout && ( HW_Read32( dc->IoBase, DEPCMD(epidx) ) & EPCMDACT ) ) {
		nanospin_ns(1000);
	}
	
	if ( timeout == 0 ) {
		dwcotg_slogf(dc, _SLOG_ERROR, "%s: timeout epidx = %d", __func__, epidx );
		dwcotg_slogf(dc, _SLOG_ERROR, "%s: cmd-reg = 0x%x cmd-orig = 0x%x", __func__, HW_Read32( dc->IoBase, DEPCMD(epidx) ), cmd->cmd );
		dwcotg_slogf(dc, _SLOG_ERROR, "%s: p0-reg = 0x%x p0-orig = 0x%x", __func__, HW_Read32( dc->IoBase, DEPCMDPAR0(epidx) ), cmd->p0 );
		dwcotg_slogf(dc, _SLOG_ERROR, "%s: p1-reg = 0x%x p1-orig = 0x%x", __func__, HW_Read32( dc->IoBase, DEPCMDPAR1(epidx) ), cmd->p1 );
		dwcotg_slogf(dc, _SLOG_ERROR, "%s: p2-reg = 0x%x p2-orig = 0x%x", __func__, HW_Read32( dc->IoBase, DEPCMDPAR2(epidx) ), cmd->p2 );
		err = ETIME;
		goto error;
	} 
	
	if ( is_blocking ) {
		pthread_sleepon_lock();
		while ( ep->cmd_complete_event == 0 ) {
			pthread_sleepon_wait( &ep->cmd_complete_event );
		}
		cmd->result_event = ep->cmd_complete_event;
		pthread_sleepon_unlock( );
	} else {
		// only valid for start xfer
		cmd->xfer_rsc_idx = ( HW_Read32( dc->IoBase, DEPCMD(epidx) ) & EVT_EPPARAM_XFER_RSC_IDX_MSK ) >> EVT_EPPARAM_XFER_RSC_IDX_POS;
	}

	return EOK;
	
error:
	return err;	
}


//////////////////////////////
// Device Generic Commands  //
//////////////////////////////



// incomplete... finish me if needed


void dgcmd_build_FIFO_ALL_FLUSH( cmd_t * cmd ) {
	cmd->cmd = CMDTYP_ALLFIFO_FLUSH | CMDACT;
	cmd->p0 = 0;
}

void dgcmd_build_SETPERI_PARAM( cmd_t * cmd, uint16_t latency ) {
	cmd->cmd = CMDTYP_SETPERI_PARAM | CMDACT;
	cmd->p0 = latency;
}


// !!! Assumption !!! : dgcmd_send() is NOT thread safe, so it is assumed that
// the caller will synchronize commands... If we need to make this thread safe, I suggest
// adding a command mutex per endpoint
int dgcmd_send( dctrl_t * dc, 	cmd_t * cmd  )  {
	int err;	
	int timeout = 100;

	DWCOTG_SLOGF_DBG(dc, _SLOG_DEBUG2, "%s: cmd: cmd-reg = 0x%x p0-reg = 0x%x ", __func__, cmd->cmd, cmd->p0 );

		
	HW_Write32( dc->IoBase, DGCMDPAR, cmd->p0 );
	HW_Write32( dc->IoBase, DGCMD, cmd->cmd  );
	
	
	// wait for command to be accepted by the controller
	while ( --timeout && ( HW_Read32( dc->IoBase, DGCMD ) & CMDACT ) ) {
		nanospin_ns(1000);
	}
	
	if ( timeout == 0 ) {
		dwcotg_slogf(dc, _SLOG_ERROR, "%s: timeout ", __func__ );
		dwcotg_slogf(dc, _SLOG_ERROR, "%s: cmd-reg = 0x%x cmd-orig = 0x%x", __func__, HW_Read32( dc->IoBase, DGCMD ), cmd->cmd );
		dwcotg_slogf(dc, _SLOG_ERROR, "%s: p0-reg = 0x%x p0-orig = 0x%x", __func__, HW_Read32( dc->IoBase, DGCMDPAR ), cmd->p0 );
		err = ETIME;
		goto error;
	} 

	return EOK;
	
error:
	return err;	
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devu/dc/dwcotg3/cmd.c $ $Rev: 735878 $")
#endif
