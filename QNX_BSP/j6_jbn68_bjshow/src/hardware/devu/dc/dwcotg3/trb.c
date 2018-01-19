
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

/*
 * These funcstion are used to allocate and configure TRBs
 */
 
#include "dwcotg.h"


trb_cluster_t *  trb_cluster_alloc( dctrl_t * dc, int n_trb_in_cluster ) {
	trb_cluster_t * trb_cluster;
	int             err;
	int             sz = n_trb_in_cluster * sizeof( *trb_cluster );
	
	trb_cluster = calloc(1, sizeof( trb_cluster_t ) );
	if ( trb_cluster == NULL ) {
		dwcotg_slogf( dc, _SLOG_ERROR, "%s failed to calloc trb_cluster struct",__func__);
		goto error;
	}
	trb_cluster->n_trb_in_cluster = n_trb_in_cluster;
	
	
	/* map physical memory for the trb cluster, and get the physical address */
	trb_cluster->trb_arr = 
		mmap64( 	NULL, 
					sz, 
					PROT_READ | PROT_WRITE | PROT_NOCACHE, 
					MAP_PRIVATE | MAP_ANON | MAP_PHYS,
					NOFD,
					0 									);
	if ( trb_cluster->trb_arr == MAP_FAILED ) {
		dwcotg_slogf( dc, _SLOG_ERROR, "%s failed to mmap trb_cluster",__func__);
		goto error2;
	}
	
	err = mem_offset64( 	trb_cluster->trb_arr,
							NOFD,
							sz,
							&trb_cluster->trb_base_paddr64,
							NULL					);
	if ( err != EOK ) {
		dwcotg_slogf( dc, _SLOG_ERROR, "%s failed to calc paddr64 for trb_cluster",__func__ );
		goto error3;
	}
	
	dwcotg_slogf( dc, _SLOG_DEBUG1, "%s allocated cluster at paddr = 0x%llx",__func__, trb_cluster->trb_base_paddr64 );	
	
	return trb_cluster;
error3:
	munmap( trb_cluster->trb_arr,  sz );
error2:
	free( trb_cluster );	
error:
	return NULL;	
	
}

void trb_cluster_free( dctrl_t * dc, trb_cluster_t * trb_cluster ) {
	munmap( trb_cluster->trb_arr, trb_cluster->n_trb_in_cluster * sizeof(*trb_cluster) );
	free( trb_cluster );
}


/* Control Packet: Setup Phase */
void trb_build_ctrl_setup( trb_cluster_t * trb_cluster, uint64_t setup_packet_paddr ) {
	volatile trb_t * trb = trb_cluster->trb_arr;
	
	trb->bufptr_low = setup_packet_paddr & 0xffffffff;
	trb->bufptr_high = setup_packet_paddr >> 32;
	trb->bufsz_sts = SETUP_PACKET_SIZE;
	trb->control = TRB_CTL_HWO | TRB_CTL_LST | TRBCTL_CONTROL_SETUP | TRB_CTL_IOC;
}

/* Control Packet: Data Phase */
void trb_build_ctrl_data( trb_cluster_t * trb_cluster, uint64_t buf_paddr64, uint32_t size ) {
	volatile trb_t * trb = trb_cluster->trb_arr;
	
	trb->bufptr_low = buf_paddr64 & 0xffffffff;
	trb->bufptr_high = buf_paddr64 >> 32;
	trb->bufsz_sts = size;
	trb->control = TRB_CTL_HWO | TRB_CTL_LST | TRBCTL_CONTROL_DATA | TRB_CTL_IOC;
}

/* Control Packet: Status Phase ( without data phase ) */
void trb_build_ctrl_status2( trb_cluster_t * trb_cluster ) {
	volatile trb_t * trb = trb_cluster->trb_arr;

	trb->bufptr_low = 0;
	trb->bufptr_high = 0;
	trb->bufsz_sts = 0;
	trb->control = TRB_CTL_HWO | TRB_CTL_LST | TRBCTL_CONTROL_STATUS2 | TRB_CTL_IOC;
}

/* Control Packet: Status Phase ( with data phase ) */
void trb_build_ctrl_status3( trb_cluster_t * trb_cluster ) {
	volatile trb_t * trb = trb_cluster->trb_arr;

	trb->bufsz_sts = 0;
	trb->bufptr_low = 0;
	trb->bufptr_high = 0;
	trb->control = TRB_CTL_HWO | TRB_CTL_LST | TRBCTL_CONTROL_STATUS3 | TRB_CTL_IOC;
}


/* Control Packet: Status Phase ( with data phase ) */
void trb_build_normal( trb_cluster_t * trb_cluster, uint64_t buf_paddr64, uint32_t size ) {
	volatile trb_t * trb = trb_cluster->trb_arr;

	trb->bufptr_low = buf_paddr64 & 0xffffffff;
	trb->bufptr_high = buf_paddr64 >> 32;
	trb->bufsz_sts = size;
	trb->control = TRB_CTL_HWO | TRB_CTL_LST | TRBCTL_NORMAL | TRB_CTL_IOC;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devu/dc/dwcotg3/trb.c $ $Rev: 680332 $")
#endif
