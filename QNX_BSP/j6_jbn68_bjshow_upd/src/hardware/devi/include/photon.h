/*
 * $QNXLicenseC: 
 * Copyright 2007, 2008, QNX Software Systems.  
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








#ifndef __PHOTON_H_INCLUDED__
#define __PHOTON_H_INCLUDED__

extern struct 		_Ph_ctrl *_Ph_;

#define MAX_QUERY_RIDS	100

/* injection constants */

#define INJECTED		1
#define NOT_INJECTED	        ~INJECTED
#define MAX_COORD		50

#define SIG_INPUT_EVENT SIGRTMIN
#define SIG_PH_EVENT SIGRTMIN+1


typedef struct _input_data {

  struct {
	unsigned char		type;
	unsigned char		flags;
	long 			reply_pid;
   } hdr;

  union {
	struct packet_kbd	        kbd;
	struct packet_rel	        rel;
	struct packet_abs	        abs;
	char 				data[1];
  } u;
} input_data_t;


typedef struct {

	int			flags;
  	PhRid_t			rid;
  	PhRect_t	        rect;
  	PhRegion_t		region;
  	int			throttle;
} input_region_data_t;

typedef	struct {

  unsigned                      millisec;
  unsigned                      abs_max;

  union {
    PhRawPtrEvent_t		ptr;
    char			padding[offsetof(PhRawPtrEvent_t, coord) + 
					MAX_COORD * sizeof(PhRawPtrCoord_t)];
  } u;

} inject_event_t;


#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devi/include/photon.h $ $Rev: 697744 $")
#endif
