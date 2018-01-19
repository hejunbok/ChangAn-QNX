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





#include <sys/devi.h>
#include "proto.h"

/* options:
 *
 * Parse command line options.  Create input modules specified on
 * the command line, link them up into event bus lines, and initialize.
 * Initialization consists of allocating the manipulator if necessary,
 * calling its init() callback followed by its parm() callback with
 * any args.  Finally after the module has been linked into an event
 * bus line, the reset() callback is called.
 *
 * Returns  0 on OK
 *         -1 on error
 */

int
options (int argc, char *argv[], uint32_t * reg_flags,
				 unsigned int *opt_flags)
{

	int opt, i, index, rval;
	input_module_t *mp = NULL;
	event_bus_line_t *bp = NULL;
	user_parm = NULL;
	char *p;
	char *found_char = NULL;

	delayMsec = 2000;
	/* default for DISPLAY_RES option */
	xRes = 800;
	yRes = 480;

	if (argc < 2) {
		fprintf (stderr, "type 'use %s' for usage\n", argv[0]);
		return (-1);
	}

	pFull_process_name = strdup (argv[0]);

	/* We no longer use Photon, so default to the resmgr interface */
	*opt_flags |= OPT_NO_PHOTON;
	*opt_flags |= OPT_RESMGR;

	/* parse general devi commmand line options first */

	while ((opt = getopt (argc, argv, "D:U:f:p:Lbld:g:vGPR:rh:")) != -1) {
		switch (opt) {
		case 'D':
			delayMsec = atoi (optarg);
			break;

		case 'U':
			user_parm = strdup(optarg);
			break;

		case 'f':
			fprintf (stderr, "This internal feature has been disabled for now.\n");
			/*
			logFile = fopen (optarg, "w");
			if (!logFile) {
				perror (optarg);
			}
			*/
			break;

		case 'p':
			fprintf (stderr, "This internal feature has been disabled for now.\n");
			/*
			playFile = fopen (optarg, "r");
			if (!playFile) {
				perror (optarg);
			}
			*/
			break;

		case 'L':
			fprintf (stderr, "This internal feature has been disabled for now.\n");
			//loopPlayback = 1;
			break;

			/* disable Ctrl_Alt_Backspace termination of Photon */
		case 'b':
			fprintf (stderr, "Option is depricated and no longer available\n");
			break;

			/* open a different device than /dev/photon to inject
			 * events into.  This is legacy stuff from qnx4, we
			 * don't need this anymore since we have the resmgr
			 * interface in nto.
			 */
		case 'd':
			fprintf (stderr, "Option is depricated and no longer available\n");
			break;

			/* input group for photon */

		case 'g':
			fprintf (stderr, "Option is depricated and no longer available\n");
			break;

			/* useful in debugging touchscreens, doesn't require
			 * the prescense of a grafx driver when starting
			 * up a touchscreen driver
			 */
		case 'G':
			fprintf (stderr, "Option is depricated and no longer available\n");
			break;

			/* don't start the photon interface */
		case 'P':
			fprintf (stderr, "Option is depricated and no longer needed\n");
			break;

			/* start the resource manager interface */
		case 'r':
			fprintf (stderr, "Option is depricated and no longer needed\n");
			break;

			/* display a list of available modules */

		case 'l':
			i = 0;
			while ((mp = ModuleTable[i++])) {

				printf ("%-12s %-12s %c%c%c\n", mp->name, mp->date,
								(mp->type & DEVI_MODULE_TYPE_FILTER) ?
								'F' : ' ',
								(mp->type & DEVI_MODULE_TYPE_PROTO) ?
								'P' : ' ', (mp->type & DEVI_MODULE_TYPE_DEVICE) ? 'D' : ' ');
			}
			exit (EXIT_SUCCESS);

			/* debugging verbose level */

		case 'v':
			verbosity++;
			break;
		case 'h':
			pServ_name = strdup (optarg);
			break;
		case 'R':
			*opt_flags |= OPT_DISPLAY_RES;
			p = optarg;

			found_char = strchr (p, ',');

			if (found_char == NULL) {
				char *pMsgTxt = "Error: Expecting a comma delimiter for screen resolution.\n";

				fprintf (stderr, pMsgTxt);
				slogf (_SLOG_SETCODE (_SLOGC_INPUT, 0), _SLOG_ERROR, pMsgTxt);
				exit (-1);
			}

			while (p && (*p++ != ','));

			*(p - 1) = '\0';
			xRes = atoi (optarg);
			yRes = atoi (p);

			if ((NULL == xRes) || (NULL == yRes)) {
				char *pMsgTxt = "Error: Specify both X and Y screen resolutions.\n";

				fprintf (stderr, pMsgTxt);
				slogf (_SLOG_SETCODE (_SLOGC_INPUT, 0), _SLOG_ERROR, pMsgTxt);
				exit (-1);
			}
		}
	}

	/* set unbuffered output in verbose mode */

	if (verbosity == 0) {
		close (STDIN_FILENO);
		close (STDOUT_FILENO);
	} else {
		setbuf (stdout, NULL);
	}

	/* now process input modules */

	index = optind;
	while (index < argc) {
		if ((mp = module_lookup (argv[index])) == 0) {
			char *pMsgTxt = "Options: Unable to lookup %s in module table\n";

			if (!getuid () && !getgid ()) {
				fprintf (stderr, pMsgTxt, argv[index]);
				slogf (_SLOG_SETCODE (_SLOGC_INPUT, 0), _SLOG_ERROR, pMsgTxt,
							 argv[index]);
			} else {
				fprintf (stderr, pMsgTxt, "module");
			}
			return (-1);
		}

		/* 
		 * Initialize the modules by calling its init() and 
		 * parm() callbacks.
		 */

		if (!mp->init) {
			char *pMsgTxt = "Error: init() callback for module %s does not exist\n";

			fprintf (stderr, pMsgTxt, mp->name);
			slogf (_SLOG_SETCODE (_SLOGC_INPUT, 0), _SLOG_ERROR, pMsgTxt, mp->name);
			return (-1);
		}

		if (mp->init (mp) < 0) {
			char *pMsgTxt = "Error: init() callback to %s failed\n";

			fprintf (stderr, pMsgTxt, mp->name);
			slogf (_SLOG_SETCODE (_SLOGC_INPUT, 0), _SLOG_ERROR, pMsgTxt, mp->name);
			return (-1);
		}

		/* have to reset globals optind,optarg so getopt() will work */
		optind = 1;

		while ((opt = getopt (argc - index, argv + index, mp->args)) != -1) {
			if (!mp->parm) {
				char *pMsgTxt =
					"Error: parm() callback for module %s does not exist\n";
				fprintf (stderr, pMsgTxt, mp->name);
				slogf (_SLOG_SETCODE (_SLOGC_INPUT, 0), _SLOG_WARNING, pMsgTxt,
							 mp->name);
			}
			mp->parm (mp, opt, optarg);
		}

		/* now add module to bus */

		if ((rval = bus_add (bp, mp)) < 0)
			return (-1);

		/* bus_add() will return 1 when we need to create a new bus */

		if (rval == 1) {
			if ((bp = bus_create ()) == NULL) {
				char *pMsgTxt = "Error: unable to create bus line\n";

				fprintf (stderr, pMsgTxt);
				slogf (_SLOG_SETCODE (_SLOGC_INPUT, 0), _SLOG_ERROR, pMsgTxt);
				return (-1);
			}
			if (bus_add (bp, mp) < 0)
				return (-1);
		}
		index += optind;
	}

	/* last one needs to be completed */
	if (bus_complete (bp) < 0)
		return (-1);

	/* now each bus is complete and linked up, call 
	 * each modules reset() function */

	if (bus_reset () < 0)
		return (-1);

	return (0);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devi/lib/options.c $ $Rev: 704024 $")
#endif
