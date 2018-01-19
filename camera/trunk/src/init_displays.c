/*
 * $QNXLicenseC:
 * Copyright 2012, QNX Software Systems Limited. All Rights Reserved.
 *
 * This software is QNX Confidential Information subject to 
 * confidentiality restrictions. DISCLOSURE OF THIS SOFTWARE 
 * IS PROHIBITED UNLESS AUTHORIZED BY QNX SOFTWARE SYSTEMS IN 
 * WRITING.
 *
 * You must obtain a written license from and pay applicable license 
 * fees to QNX Software Systems Limited before you may reproduce, modify
 * or distribute this software, or any work that includes all or part 
 * of this software. For more information visit 
 * http://licensing.qnx.com or email licensing@qnx.com.
 * 
 * This file may contain contributions from others.  Please review 
 * this entire file for other proprietary rights or license notices, 
 * as well as the QNX Development Suite License Guide at 
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

#include <errno.h>
/**
 ** Include the header files for libraries we are using.
 **/

#include <ctype.h>         /* Header file for isdigit */
#include <stdio.h>         /* Header file for fprintf */
#include <stdlib.h>        /* Header file for EXIT_FAILURE, EXIT_SUCCESS, atoi */
#include <string.h>        /* Header file for strncmp */
#include <sys/keycodes.h>  /* Header file for KEYCODE_ESCAPE */
#include <time.h>          /* Header file for clock_gettime, timespec2nsec */
#include <screen/screen.h> /* Header file for all screen API calls */

/* preferred resolution list from RIM */
static struct _preferred_mode {
        int  width;
        int  height;
        int  refresh;
        int  interlaced;
} preferred_modes [] = {
        { 1920, 1080, 60, 0 },
        { 1920, 1080, 50, 0 },
        { 1280, 720, 60, 0 },
        { 1280, 720, 50, 0 },
        { 1920, 1080, 60, 1 },
        { 1920, 1080, 50, 1 },
        { 720, 480, 60, 0 },
        { 720, 576, 50, 0 },
        { 720, 480, 60, 0 },
        { 720, 576, 50, 0 },
        { 640, 480, 60, 0 },
};


int find_preferred_mode(screen_display_mode_t *modes, int modesSize)
{
        int i, j;
        int found_preferred = 0;
	int preferred_idx = 0;

        for (i=0; i < sizeof (preferred_modes); i++) {
                for (j=0; j < modesSize; j++) {
                        if (modes[j].width == preferred_modes[i].width &&
                                modes[j].height == preferred_modes[i].height &&
                                modes[j].refresh == preferred_modes[i].refresh &&
                                modes[j].interlaced == preferred_modes[i].interlaced) {
				preferred_idx = j;
                                found_preferred = 1;
                                break;
                        }
                }

                if (found_preferred) {
                        break;
                }
        }

	return preferred_idx;
}



int display_setup(screen_context_t screen_ctx, int idx, int verbose)
{
	int val;                       		
	int rval = -1;       		
	int rc;                        		
	int i;                         		
	screen_display_t *displays;
	int display_count = 0, port;
	int m=0;

	rc = screen_get_context_property_iv(screen_ctx, SCREEN_PROPERTY_DISPLAY_COUNT, &display_count);

	if (idx > display_count) {
		goto fail;
	}

	displays = (screen_display_t *)malloc(display_count * sizeof(screen_display_t));

	rc = screen_get_context_property_pv(screen_ctx, SCREEN_PROPERTY_DISPLAYS, (void **)displays);

	i = idx;
	if (verbose) {
		char dev_id[100];
		rc = screen_get_display_property_cv(displays[i], SCREEN_PROPERTY_ID_STRING, sizeof(dev_id), dev_id);
		if ( rc != -1 ) {
			printf("display devid %s\n", dev_id );
		}
	}

	screen_get_display_property_iv(displays[i], SCREEN_PROPERTY_ATTACHED, &val);
	if (verbose) {
		screen_get_display_property_iv(displays[i], SCREEN_PROPERTY_ID, &port);
		printf("display %d (port %d) is %stached\n", i, port, val?"at":"de");
	}
	if ( val ) {
		screen_get_display_property_iv(displays[i], SCREEN_PROPERTY_MODE_COUNT, &val);
		if (verbose)
			printf("\tmode count is %d\n", val );
	   	screen_display_mode_t *modes = (screen_display_mode_t *)malloc(val * sizeof(*modes));
		rc = screen_get_display_modes(displays[i], val, modes);
		if (verbose) {
			printf("modes is %p, rc is %d\n", modes, rc );
			for ( m = 0; m < val; m++ ) {
				printf("%d: %dx%d%s @ %dHz %d:%d flags %#08x index %d\n", m, modes[m].width, modes[m].height, 
					modes[m].interlaced ? "i":"p", 
					modes[m].refresh,
					modes[m].aspect_ratio[0], modes[m].aspect_ratio[1], modes[m].flags, modes[m].index );
			}
		}
			
		if ( val > 1 ) {
			m = find_preferred_mode(modes, val);

			rc = screen_set_display_property_iv(displays[i], SCREEN_PROPERTY_MODE, &m );
			if (verbose) {
					printf("set size %dx%d, rc = %d\n", modes[m].width, modes[m].height, rc );
			}
			rc = screen_flush_context(screen_ctx, 0);
			if (verbose) {
				printf("flushed, rc = %d\n", rc );
			}
		}
	}

	rval = 0;

fail:
	return rval;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/6.6.0/trunk/apps/screen/demos/display_image/init_displays.c $ $Rev: 715919 $")
#endif
