/*
 * $QNXLicenseC: 
 * Copyright 2007, 2008 QNX Software Systems.  
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
 * photon.c
 *
 * This file contains the code for the photon interface to the devi
 * drivers.  The photon interface consists of its own thread, which
 * we'll call the photon thread.  When the thread starts up it registers
 * with photon, sets up input device regions and goes to sleep waiting
 * for event bus lines to queue it data.  Data coming up from filter modules 
 * is put on a queue called the PhEventBuffer and the photon thread is
 * woken up.  Once it wakes up it processes the data and injects raw events
 * into photon event space.  Keyboard packets are injected into photon
 * event space as RawKeyboardEvents and absolute and relative packets are 
 * injected into photon event space as RawPointerEvents.
 *
 */

#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/devi.h>

/* devi_enqueue_packet
 *
 * Used by filter layer modules to dispatch a completed packet to
 * the resource manager.
 *
 * Returns  0 on success
 *         -1 on error
 */

int
devi_enqueue_packet (input_module_t * module, char *dptr, unsigned size)
{
	if (playFile) {
		// we are in the playback mode -- ignore all real events...
		return 0;
	}

	if (!module)
		return (-1);

	resmgr_enqueue_packet (module->line, dptr, size);

	if (logFile) {
		struct timespec t;

		clock_gettime (CLOCK_REALTIME, &t);
		// save the time stamp
		fwrite (&t, sizeof (t), 1, logFile);
		// save the data
		fwrite (&module->type, sizeof (module->type), 1, logFile);
		fwrite (&size, sizeof (size), 1, logFile);
		fwrite (dptr, size, 1, logFile);
		fflush (logFile);
	}

	if (logFile && (module->type & DEVI_CLASS_MASK) == DEVI_CLASS_KBD) {
		struct packet_kbd *kbuf = (struct packet_kbd *) dptr;

		if (kbuf->key.key_cap == KEYCODE_MENU) {
			extern void set_abs_pos (int, int);

			set_abs_pos (0, 0);
		}
	}
	return (0);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devi/lib/photon.c $ $Rev: 697744 $")
#endif
