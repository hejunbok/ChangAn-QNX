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

/* the variable size fifo memory is made of 8 byte blocks which can be allocated
 * for an endpoint.  Blocks are number from 0 to n .
 * Block 0-7 are reserved for EP0 ... 64 bytes
 * Block 8-n are reserved for all other endpoints
 */
 
#include <malloc.h>
#include <queue.h>
#include <stdlib.h>

#include "dwcotg.h"


#define POW2_ROUNDUP



/////////////
// MACROS  //
/////////////
#define FIFOMEM_BLK_SIZE			8

#define BLKNUM_EP0_START			0
// 512 bytes + 2 * worst_case_buswidth in bytes
// so ( 512 + ( 2 * 16 ) ) / 8 =  (544 / 8 )  =  68
// where worst_case_buswidth in bytes = 128 bits / 8 = 16
#define EP0_FIFO_SZ					68					

#define BLKNUM_POOL_START			( FIFOMEM_BLK_SIZE + EP0_FIFO_SZ )


/////////////////
// LOCAL TYPES //
/////////////////
struct _blk_descr {
	unsigned int                    start;
	unsigned int                    end;
	LIST_ENTRY(_blk_descr)          link;
};

struct _fifomem_ctrl {
	dctrl_t                       *dc;	
	int                           MDWIDTH_bytes;
	LIST_HEAD(, _blk_descr )      free_list;
	LIST_HEAD(, _blk_descr )      used_ist;
};

////////////////////////////////////////////////////////////////////////////////
//                        PRIVATE FUNCTIONS                                   //
////////////////////////////////////////////////////////////////////////////////

unsigned log2_roundup( unsigned n ) {
	unsigned cnt = 0;
	unsigned n2 =  n;
	
	while( n != 1 ) {
		n = n >> 1;
		cnt++;
	}
	
	// test orignial number to see if it is a power of 2... if not then round up
	if ( n2 & ( n2 - 1) ) {
		cnt++;
	}
	
	return cnt;
}


////////////////////////////////////////////////////////////////////////////////
//                         PUBLIC FUNCTIONS                                   //
////////////////////////////////////////////////////////////////////////////////

void * fifomem_init( dctrl_t  *dc ) {
	struct _fifomem_ctrl 	*ctrl;	
	struct _blk_descr		*freeblock;
	unsigned				fifosz_in_blks;
	
	ctrl = calloc( 1, sizeof( struct _fifomem_ctrl ) );
	if ( ctrl == NULL ) {
		dwcotg_slogf(dc, _SLOG_ERROR, "%s: calloc failed", __func__ );
		goto fail;
	}

	fifosz_in_blks = HW_Read32( dc->IoBase, GHWPARAMS7 ) & RAM1_DEPTH_MSK;
	ctrl->MDWIDTH_bytes = 	( HW_Read32( dc->IoBase, GHWPARAMS0 ) & MDWIDTH_MSK ) >> MDWIDTH_POS;
	ctrl->dc = dc;
	
	
	LIST_INIT( &ctrl->free_list );
	LIST_INIT( &ctrl->used_ist );

	// create free list, reserving blocks  for setup and ep0 fifos
	freeblock = calloc( 1, sizeof( struct _blk_descr ) );
	if ( freeblock == NULL ) {
		dwcotg_slogf(dc, _SLOG_ERROR, "%s: calloc failed", __func__ );
		goto fail2;
	}

	freeblock->start = BLKNUM_POOL_START;
	freeblock->end = fifosz_in_blks - 1;
	LIST_INSERT_HEAD( &ctrl->free_list, freeblock, link );
	
	return ctrl;
	
fail2:
	free( ctrl );
fail:
	return NULL;
}

void fifomem_fini( void * hdl ) {
	struct _fifomem_ctrl 	*ctrl = hdl;
	free(ctrl);
}


// returns a blknum which represents the start of the fifo of specified size
int fifomem_alloc( void * hdl, int n_pkts, int mps, int * n_blks_allocated ) {
	struct _fifomem_ctrl 	*ctrl = hdl;
	unsigned int 			nblk,start;
	struct _blk_descr		*p, *usedblock;
	int						size;
	
	// the size equation is based on SuperSpeed USB 3.0 Controller Databook
	// chapter 6.5.1 which shows that the fifo size is dependent on the buswidth.
	// Experimentally, I have shown that the controller will not work without accouting
	// for MDWIDTH
	size = n_pkts * ( mps + ctrl->MDWIDTH_bytes ) + ctrl->MDWIDTH_bytes;
	if ( size <= FIFOMEM_BLK_SIZE ) {
		size = FIFOMEM_BLK_SIZE;
	} else  {
		size = 1 <<  log2_roundup( size );
	}

	nblk = size / FIFOMEM_BLK_SIZE;
	*n_blks_allocated = nblk;
	
	// walk free list looking to allocate nblk
	for ( p = LIST_FIRST(&ctrl->free_list );
		  p != NULL;
		  p = LIST_NEXT( p, link )                   ) {
		
		if ( ( p->end - p->start + 1 ) >= nblk ) {
			// great, this free list entry has enough blocks 
			// remove from free pool
			start = p->start;
			p->start += nblk;
			
			if ( p->start > p->end ) {
				// this descriptor has no more blocks to give... remove
				LIST_REMOVE( p, link );
				free(p);
			}
			
			// add to used pool
			usedblock = calloc( 1, sizeof( struct _blk_descr ) );
			if ( usedblock == NULL ) {
				// out of descriptors!!!
				dwcotg_slogf(ctrl->dc, _SLOG_ERROR, "%s: calloc failed", __func__ );
				return -1;
			}
			usedblock->start = start;
			usedblock->end = start + nblk - 1;
			LIST_INSERT_HEAD( &ctrl->used_ist, usedblock, link );
			
			return start;
		}
	}
	
	// couldn't find a free block... 
	return -1;
}

// free the blknum allocated with fifomem_alloc
void fifomem_free( void * hdl, int blknum ) {
	struct _fifomem_ctrl 	*ctrl = hdl;
	struct _blk_descr		*p,*prev,*next,*freeblock;
	int						start,end;
	int						found = 0;
	
	// removed  blk from used list	
	for ( p = LIST_FIRST(&ctrl->used_ist );
		  p != NULL;
		  p = LIST_NEXT( p, link )                   ) {

		  if ( p->start == blknum ) {
			  // remove from used list
			  start = p->start;
			  end = p->end;
			  LIST_REMOVE( p, link );
			  free(p);
			  found = 1;
			  break;
		  }
	}
	
	// add newly freed blocks to free list
	if ( found ) {
		for ( p = LIST_FIRST(&ctrl->free_list ),prev=NULL;
			  p != NULL;
			  prev=p, p = LIST_NEXT( p, link )                   ) {
		
			  if ( end == ( p->start - 1 ) ) {
				  // add newly freed blocks to the front-end of this free block
				  p->start = start;
				  //check to see if we need to coalesce with previous free block
				  if ( prev && ( prev->end == ( p->start - 1 ) ) ) {
					  p->start = prev->start;
					  LIST_REMOVE( prev, link );
					  free(prev);
				  }
				  break;

			  } else if ( start == ( p->end + 1 ) ) {
				  p->end = end;
				  //check to see if we need to coalesce with next free block
				  if ( (next = LIST_NEXT(p, link)) && ( next->start == ( p->end + 1 ) ) ) {
					  p->end = next->end;
					  LIST_REMOVE( next, link );
					  free(next);
				  }
				  break;

			  } else if ( end < ( p->start - 1 ) ) {
				  // add a new free block before p
				  freeblock =  calloc( 1, sizeof( struct _blk_descr ) );
				  if ( freeblock == NULL ) {
					  dwcotg_slogf(ctrl->dc, _SLOG_ERROR, "%s: calloc failed", __func__ );
					  return;
				  }
				  freeblock->start = start;
				  freeblock->end = end;
				  LIST_INSERT_BEFORE( p, freeblock, link );
				  break;
			  } else if ( LIST_NEXT( p, link ) == NULL ) {
				  // add a new free block before p
				  freeblock =  calloc( 1, sizeof( struct _blk_descr ) );
				  if ( freeblock == NULL ) {
					  dwcotg_slogf(ctrl->dc, _SLOG_ERROR, "%s: calloc failed", __func__ );
					  return;
				  }
				  freeblock->start = start;
				  freeblock->end = end;
				  LIST_INSERT_AFTER( p, freeblock, link );
				  break;
			  }
		}
	} else {
	  dwcotg_slogf(ctrl->dc, _SLOG_ERROR, "%s: block not found!", __func__ );
	}
	
	return;
}

int fifomem_alloc_ep0( void * hdl, int * n_blks_allocated ) {
	// return dedicated fifo
	*n_blks_allocated = EP0_FIFO_SZ;
	return BLKNUM_EP0_START;
}

void fifomem_free_ep0( void * hdl, int blknum ) {
	//nothing to do fifo is dedicated
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devu/dc/dwcotg3/fifomem.c $ $Rev: 736722 $")
#endif
