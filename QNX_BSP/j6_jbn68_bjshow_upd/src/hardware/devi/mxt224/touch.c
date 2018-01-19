/*
 * $QNXLicenseC:
 * Copyright 2012, QNX Software Systems.
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

/*============================================================================
  touch.c
 
  Driver for the mXT224  I2C touch screen controller.
  A Combination of Device and Protocol modules.
=============================================================================*/
 

#include <sys/devi.h>
#include "touch.h"

/*============================================================================

  Data Structure input_module_t

  Our protocol module is represented by the following input_module_t
  data structure.  We always create one static instance of the module.
  If more than one are needed, ie in multiple bus lines; then the system
  will allocate a new module and copy the contents of the static one
  into it.

=============================================================================*/

input_module_t  touch = {
        NULL,                  /* up, filled in at runtime */
        NULL,                  /* down, filled in at runtime */
        NULL,                  /* line we belong to, filled in at runtime */
        0,                     /* flags, leave as zero */
        DEVI_CLASS_ABS | DEVI_MODULE_TYPE_PROTO | DEVI_MODULE_TYPE_DEVICE,
                               /* Our type, we are a
                                * protocol module or class
                                * relative. This info will
                                * tell the runtime system
                                * which filter module to
                                * link above us
                                */
        "touch",               /* name, must match what you specify
                                * on the command line when invoking this
                                * module
                                */
        __DATE__,              /* date of compilation, used for output when
                                * the cmdline option -l is given to the
                                * driver
                                */
        "vi:a:P:p:D:t:b:r",   /* command line parameters */
        NULL,                  /* pointer to private data, set this up
                                * in the init() callback
                                */
        touch_init,            /* init() callback, required */
        touch_reset,           /* reset() callback, required */
        NULL,                  /* input() callback */
        NULL,                  /* output(), not used */
        NULL,                  /* pulse(), called when the timer expires
                                * used for injecting a release event
                                */
        touch_parm,            /* parm() callback, required */
        touch_devctrl,         /* devctrl() callback */
        touch_shutdown         /* shutdown() callback, required */
};

/* Initial register values recommended from chip vendor */
static const uint8_t init_vals_ver_20[] = {
	/* MXT224_GEN_COMMAND(6) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* MXT224_GEN_POWER(7) */
	0x20, 0xff, 0x32,
	/* MXT224_GEN_ACQUIRE(8) */
	0x08, 0x05, 0x05, 0x00, 0x00, 0x00, 0x05, 0x14,
	/* MXT224_TOUCH_MULTI(9) */
	0x00, 0x00, 0x00, 0x11, 0x0a, 0x00, 0x00, 0x00, 0x02, 0x00,
	0x00, 0x01, 0x01, 0x0e, 0x0a, 0x0a, 0x0a, 0x0a, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x64,
	/* MXT224_TOUCH_KEYARRAY(15) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00,
	/* MXT224_SPT_GPIOPWM(19) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00,
	/* MXT224_PROCI_GRIPFACE(20) */
	0x00, 0x64, 0x64, 0x64, 0x64, 0x00, 0x00, 0x1e, 0x14, 0x04,
	0x1e, 0x00,
	/* MXT224_PROCG_NOISE(22) */
	0x05, 0x00, 0x00, 0x19, 0x00, 0xe7, 0xff, 0x04, 0x32, 0x00,
	0x01, 0x0a, 0x0f, 0x14, 0x00, 0x00, 0xe8,
	/* MXT224_TOUCH_PROXIMITY(23) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00,
	/* MXT224_PROCI_ONETOUCH(24) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* MXT224_SPT_SELFTEST(25) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	/* MXT224_PROCI_TWOTOUCH(27) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* MXT224_SPT_CTECONFIG(28) */
	0x00, 0x00, 0x00, 0x04, 0x08,
};

static const uint8_t init_vals_ver_21[] = {
	/* MXT224_GEN_COMMAND(6) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* MXT224_GEN_POWER(7) */
	0x20, 0xff, 0x32,
	/* MXT224_GEN_ACQUIRE(8) */
	0x0a, 0x00, 0x05, 0x00, 0x00, 0x00, 0x09, 0x23,
	/* MXT224_TOUCH_MULTI(9) */
	0x00, 0x00, 0x00, 0x13, 0x0b, 0x00, 0x00, 0x00, 0x02, 0x00,
	0x00, 0x01, 0x01, 0x0e, 0x0a, 0x0a, 0x0a, 0x0a, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* MXT224_TOUCH_KEYARRAY(15) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00,
	/* MXT224_SPT_GPIOPWM(19) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* MXT224_PROCI_GRIPFACE(20) */
	0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x28, 0x04,
	0x0f, 0x0a,
	/* MXT224_PROCG_NOISE(22) */
	0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x23, 0x00,
	0x00, 0x05, 0x0f, 0x19, 0x23, 0x2d, 0x03,
	/* MXT224_TOUCH_PROXIMITY(23) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00,
	/* MXT224_PROCI_ONETOUCH(24) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* MXT224_SPT_SELFTEST(25) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	/* MXT224_PROCI_TWOTOUCH(27) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* MXT224_SPT_CTECONFIG(28) */
	0x00, 0x00, 0x00, 0x08, 0x10, 0x00,
};

static const uint8_t init_vals_ver_22[] = {
	/* MXT224_GEN_COMMAND(6) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* MXT224_GEN_POWER(7) */
	0x20, 0xff, 0x32,
	/* MXT224_GEN_ACQUIRE(8) */
	0x0a, 0x00, 0x05, 0x00, 0x00, 0x00, 0x09, 0x23,
	/* MXT224_TOUCH_MULTI(9) */
	0x00, 0x00, 0x00, 0x13, 0x0b, 0x00, 0x00, 0x00, 0x02, 0x00,
	0x00, 0x01, 0x01, 0x0e, 0x0a, 0x0a, 0x0a, 0x0a, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00,
	/* MXT224_TOUCH_KEYARRAY(15) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00,
	/* MXT224_SPT_GPIOPWM(19) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* MXT224_PROCI_GRIPFACE(20) */
	0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x28, 0x04,
	0x0f, 0x0a,
	/* MXT224_PROCG_NOISE(22) */
	0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x23, 0x00,
	0x00, 0x05, 0x0f, 0x19, 0x23, 0x2d, 0x03,
	/* MXT224_TOUCH_PROXIMITY(23) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	/* MXT224_PROCI_ONETOUCH(24) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* MXT224_SPT_SELFTEST(25) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	/* MXT224_PROCI_TWOTOUCH(27) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* MXT224_SPT_CTECONFIG(28) */
	0x00, 0x00, 0x00, 0x08, 0x10, 0x00,
};

/*=============================================================================

	Name:         check_module
	
  Description: check to ensure that the input_module_t *module is not NULL
  
=============================================================================*/

static void check_module(input_module_t *module)
{ 
	if (module) {
		if (!module->data) {
			slogf (_SLOGC_INPUT, _SLOG_ERROR, "%s: Module->data is NULL", __FUNCTION__);			
			exit (-1);
		}
	} else {
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "%s: Module is NULL", __FUNCTION__);			
		exit (-1);
	}
	return;
}	

/*============================================================================

  Name:         touch_init 

  Description:	This is our init() callback specified in the input_module_t.
                Just simple init routine.
 
=============================================================================*/

static int touch_init(input_module_t *module)
{
	private_data_t *dp = module->data;

	if (!module) {
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "%s: Module is NULL", __FUNCTION__);	
		exit (-1);
	}

	if(!module->data)
	{
		if (!(dp = module->data = scalloc(sizeof *dp))){
			slogf (_SLOGC_INPUT, _SLOG_ERROR, "%s: Data pointer scalloc failed (%s)", __FUNCTION__, strerror (errno));
			return (-1);
		}
		
		/* Enable IO capability.*/
		if (ThreadCtl(_NTO_TCTL_IO, NULL) == -1) {
			perror("ThreadCtl: ");
			exit(EXIT_FAILURE);
		}

		dp->rotate = 0;
		dp->flags = FLAG_RESET;
		dp->irq = TOUCH_INT;
		dp->irq_pc = DEVI_PULSE_ALLOC;
		dp->last_x = 0;
		dp->last_y = 0;
		dp->param.sched_priority = PULSE_PRIORITY;
		dp->event.sigev_priority = dp->param.sched_priority;
			
		dp->speed = TSC_I2C_SPEED;
		dp->slave.addr = TSC_ADDRESS;
		dp->slave.fmt = I2C_ADDRFMT_7BIT;

		dp->i2c = malloc(strlen(TSC_I2C_DEVICE) + 1);
		if (!dp->i2c){
			slogf(_SLOGC_INPUT, _SLOG_ERROR, "%s: malloc failed for dev name", __FUNCTION__);
			free(dp);
			return (-1);
		}
		strcpy(dp->i2c, TSC_I2C_DEVICE);
		pthread_mutex_init (&dp->mutex, NULL);
	}
	return (0);
}

/*=============================================================================

  Name:		      touch_parm

  Description:	This callback is called by the input runtime system to parse
                command line options to the 'touch' module of driver.
 
=============================================================================*/

static int touch_parm(input_module_t *module, int opt, char *optarg)
{
	private_data_t *dp = module->data;
	long int	v;

	check_module(module);

 	switch (opt)
	{
	
		case 'v':
				dp->verbose++;
				break;
		case 'i':
				dp->irq = atoi (optarg);
				break;
		case 'a':
				v = strtol(optarg, 0, 16);
				if (v > 0 && v < 128)
					dp->slave.addr = v;
				break;
		case 'p':
				if (strcmp(optarg, dp->i2c) != 0){
					free(dp->i2c);
					dp->i2c = malloc(strlen(optarg) + 1);

					if (!dp->i2c){
						slogf(_SLOGC_INPUT, _SLOG_ERROR, "%s: malloc failed for dev name", __FUNCTION__);
						return -1;
					}
					strcpy(dp->i2c, optarg);
				}
				break;
		case 'P':
				dp->param.sched_priority = atoi (optarg);
				dp->event.sigev_priority = dp->param.sched_priority;
				break;
		case 't':
				dp->touch_threshold = atoi(optarg);
				break;
		case 'r':
				dp->rotate = 1;
				break;
 		default:
				fprintf(stderr, "Unknown option %c\n", opt);
				break;
 	}

	return (0);
}

/*=============================================================================

  Name:         touch_reset

  Description:   The reset callback is called when our module is linked into an
		event bus line. In here we will setup our device for action.

=============================================================================*/


static int touch_reset(input_module_t *module)
{
	private_data_t	*dp = module->data;
	int sts;

	check_module(module);

	if((dp->flags & FLAG_INIT) == 0)	{
		/* Initialize I2C interface */
		dp->fd = open(dp->i2c, O_RDWR);
		if (dp->fd == -1)	{
			slogf(_SLOGC_INPUT, _SLOG_ERROR, "%s: failure in opening I2C device %s", __FUNCTION__, dp->i2c);
			exit (-1);
		}

		sts = devctl(dp->fd, DCMD_I2C_SET_BUS_SPEED, &dp->speed, sizeof(dp->speed), NULL);
		if (sts != EOK)	{
			slogf(_SLOGC_INPUT, _SLOG_ERROR, "%s: failed to set speed", __FUNCTION__);
			exit (-1);
		}

		/* get mxt224 firmware info */
		mxt224_initialize(dp);

		/* Create touch release timer */
		dp->timerid = devi_register_timer (module, 15, &dp->irq_pc, NULL);

		/* Setup the interrupt handler thread */
		if ((dp->chid = ChannelCreate (_NTO_CHF_DISCONNECT | _NTO_CHF_UNBLOCK)) == -1)	{
			slogf (_SLOGC_INPUT, _SLOG_ERROR, "%s: ChannelCreate failed: %s", __FUNCTION__, strerror (errno));
			free (dp);
			exit (-1);
		}

		if ((dp->coid = ConnectAttach (0, 0, dp->chid, _NTO_SIDE_CHANNEL, 0)) == -1)	{
			slogf (_SLOGC_INPUT, _SLOG_ERROR, "%s: ConnectAttach failed: %s", __FUNCTION__, strerror (errno));
			free (dp);
			exit (-1);
		}

		pthread_attr_init (&dp->pattr);
		pthread_attr_setschedpolicy (&dp->pattr, SCHED_RR);
		pthread_attr_setschedparam (&dp->pattr, &dp->param);
		pthread_attr_setinheritsched (&dp->pattr, PTHREAD_EXPLICIT_SCHED);
		pthread_attr_setdetachstate (&dp->pattr, PTHREAD_CREATE_DETACHED);
		pthread_attr_setstacksize (&dp->pattr, 4096);

		dp->event.sigev_notify     = SIGEV_PULSE;
		dp->event.sigev_coid       = dp->coid;
		dp->event.sigev_code       = 1;

		/* Create interrupt handler thread */
		if (pthread_create (NULL, &dp->pattr, (void *)intr_thread, module))	{
			slogf (_SLOGC_INPUT, _SLOG_ERROR, "%s: pthread_create failed: %s", __FUNCTION__, strerror (errno));
			free (dp);
			exit (-1);
		}

		/* Attach interrupt. */
		if (dp->verbose >= 3){
			slogf (_SLOGC_INPUT, _SLOG_INFO, "devi-mxt224 driver - Attaching to interrupt: %d", dp->irq);
			fprintf (stderr, "Attaching to interrupt %d\n", dp->irq);
		}
		
		if ((dp->iid = InterruptAttachEvent (dp->irq, &dp->event, _NTO_INTR_FLAGS_TRK_MSK)) == -1) {
			slogf (_SLOGC_INPUT, _SLOG_ERROR, "%s: InterruptAttachEvent failed: %s", __FUNCTION__, strerror (errno));
			free (dp);
			exit (-1);
		}

		dp->flags |= FLAG_INIT;
	}

	return (0);
}

/*=============================================================================

	Name:         touch_devctrl

  Description:  This callback is used by modules in an event bus line to send
                information further up the line to other modules (e.g. abs).

                This allows the other modules to know how many buttons we
                have, pointer coordinates, and the range of the coordinates.

=============================================================================*/

static int touch_devctrl(input_module_t *module, int event, void *ptr)
{
	private_data_t 	*dp = module->data;

	check_module(module);
	
	switch(event)
	{
		case DEVCTL_GETDEVFLAGS:
			*(unsigned short *)ptr = (dp->flags & FLAGS_GLOBAL);
			break;
		case DEVCTL_GETPTRBTNS:
			*(unsigned long *)ptr = 1L;
			break;
		case DEVCTL_GETPTRCOORD:
			*(unsigned char *)ptr = '\02';
			break;
		case DEVCTL_GETCOORDRNG:
		{
			struct devctl_coord_range *range = ptr;

			range->min = 0;
			range->max = 4096;
			break;
		}
		default:
			return (-1);
	}
	
	return (0);
}



/*=============================================================================

  Name:         intr_thread

  Description:  This is the code for the interrupt handler thread.  It simply
                waits on a pulse that is generated by the interrupt and then
                requests the X and Y coordinates from the touch controller over I2C.

=============================================================================*/

static void *intr_thread( void *data )
{ 
	input_module_t    *module = (input_module_t *) data;
	private_data_t    *dp = module->data;
	input_module_t    *up = module->up;
	struct _pulse   pulse;
	iov_t           iov;
	int             rcvid;
	struct mxt224_message message;
	struct mxt224_object *object;
	int id;
	uint8_t reportid;
	uint8_t max_reportid;
	uint8_t min_reportid;

	check_module(module);

	SETIOV (&iov, &pulse, sizeof(pulse));

	while (1)
	{
		if ((rcvid = MsgReceivev (dp->chid, &iov, 1, NULL)) == -1)	{
			if (errno == ESRCH){
				pthread_exit (NULL);
			}
			continue;
		}

		switch (pulse.code)
		{
			case PULSE_CODE:
				
				pthread_mutex_lock (&dp->mutex);
		
				if (mxt224_read_message(dp, &message)) {
					slogf (_SLOGC_INPUT, _SLOG_ERROR, "%s:Failed to read message", __FUNCTION__);			
					goto end;
				}

				reportid = message.reportid;

				if(reportid !=0xff){	
					/* whether reportid is thing of MXT224_TOUCH_MULTI */
					object = mxt224_get_object(dp, MXT224_TOUCH_MULTI);
					if (!object){
						slogf (_SLOGC_INPUT, _SLOG_ERROR, "%s:Failed to get object ", __FUNCTION__);			
						goto end;
					}
					max_reportid = object->max_reportid;
					min_reportid = max_reportid - object->num_report_ids + 1;
					id = reportid - min_reportid;

					if (reportid >= min_reportid && reportid <= max_reportid){
						mxt224_input_touchevent(dp, &message, id, up);
					}	
					else
						mxt224_dump_message(&message);
				}
end:			
				/* Start with a clean packet */
				memset (&dp->tp, 0, sizeof (dp->tp));
				pthread_mutex_unlock (&dp->mutex);
				InterruptUnmask (dp->irq, dp->iid);
				break;

			default:
				if (rcvid) {
					MsgReplyv (rcvid, ENOTSUP, &iov, 1);
				}
				break;
			}
		}
	return( NULL );
}

/*=============================================================================

  Name:         touch_shutdown

  Description:  This callback preforms the cleanup of the driver when it is
                signaled to quit.

=============================================================================*/

static int touch_shutdown(input_module_t *module, int shutdown_delay)
{
	private_data_t  *dp = module->data;

	check_module(module);
	
	delay (shutdown_delay);

	close(dp->fd);
	free(dp->i2c);

	free (module->data);
	return (0);
}


/*=============================================================================

  Name:         read_reg
	
=============================================================================*/
  
static int read_reg(int fd, uint16_t reg, uint16_t len, uint8_t *val)
{
	struct{
  		i2c_sendrecv_t hdr;
  		uint8_t buf[128];
  	}rd_data;	
	int i; 
   
	rd_data.hdr.slave.addr = TSC_ADDRESS;
	rd_data.hdr.slave.fmt = I2C_ADDRFMT_7BIT;
	rd_data.hdr.send_len = 2;
	rd_data.hdr.recv_len = len;
	rd_data.hdr.stop = 1;
	rd_data.buf[0] = reg & 0xff;
	rd_data.buf[1] = (reg >> 8) & 0xff;

	if (devctl(fd, DCMD_I2C_SENDRECV, &rd_data, sizeof(rd_data), NULL)) {
		slogf(_SLOGC_INPUT, _SLOG_ERROR, "%s: devctl failed %s for X", __FUNCTION__,  strerror (errno));
		return -1;
	}

	for(i=0;i<len;i++, val++){
    	*val = rd_data.buf[i];
	}

	return 0;
}

/*=============================================================================

	Name:    mxt224_read_reg     
    
=============================================================================*/

static int mxt224_read_reg(int fd, uint16_t reg, uint8_t *val)
{
	return read_reg(fd, reg, 1, val);
}

/*=============================================================================

	Name:         mxt224_write_reg
	
=============================================================================*/

static int mxt224_write_reg( int fd, uint16_t reg, uint8_t val)
{
	struct{
  		i2c_send_t hdr;
  		uint8_t buf[3];
  	}rd_data;	

	rd_data.hdr.slave.addr = TSC_ADDRESS;
	rd_data.hdr.slave.fmt = I2C_ADDRFMT_7BIT;
	rd_data.hdr.len = 3; 
	rd_data.hdr.stop = 1;
	rd_data.buf[0] = reg & 0xff;
	rd_data.buf[1] = (reg >> 8) & 0xff;
	rd_data.buf[2] = val;

	if (devctl(fd, DCMD_I2C_SEND, &rd_data, sizeof(rd_data), NULL)) {
		slogf(_SLOGC_INPUT, _SLOG_ERROR, "%s: devctl failed %s for X", __FUNCTION__,  strerror (errno));
		return -1;
	}

	return 0;
}

/*=============================================================================

	Name:   mxt224_read_object_table      
	
=============================================================================*/

static int mxt224_read_object_table(int fd, uint16_t reg, uint8_t *object_buf)
{
	return read_reg(fd, reg, MXT224_OBJECT_SIZE, object_buf);
}

/*=============================================================================

	Name:    mxt224_get_object     
	
=============================================================================*/

static struct mxt224_object * mxt224_get_object( private_data_t    *dp, uint8_t type)
{
	struct mxt224_object *object;
	int i;

	for (i = 0; i < dp->info.object_num; i++) {
		object = dp->object_table + i;
		if (object->type == type)
			return object;
	}

	slogf(_SLOGC_INPUT, _SLOG_ERROR, "%s: Invalid object type\n", __FUNCTION__);
	return NULL;
}

/*=============================================================================

	Name:  mxt224_read_message       
	
=============================================================================*/

static int mxt224_read_message(private_data_t    *dp, struct mxt224_message *message)
{
	struct mxt224_object *object;
	uint16_t reg;

	object = mxt224_get_object(dp, MXT224_GEN_MESSAGE);
	if (!object)
		return -EINVAL;

	reg = object->start_address;
	return read_reg(dp->fd, reg, sizeof(struct mxt224_message), (uint8_t *)message);
}

/*=============================================================================

	Name:  mxt224_read_object       
	
=============================================================================*/

static int mxt224_read_object(private_data_t    *dp, uint8_t type, uint8_t offset, uint8_t *val)
{
	struct mxt224_object *object;
	uint16_t reg;

	object = mxt224_get_object(dp, type);
	if (!object)
		return -EINVAL;

	reg = object->start_address;
	return read_reg(dp->fd, reg + offset, 1, val);
}

/*=============================================================================

	Name:  mxt224_write_object       
	
  Description: write mXT224 firmware object 
  
=============================================================================*/

static int mxt224_write_object(private_data_t	*dp, uint8_t type, uint8_t offset, uint8_t val)
{
	struct mxt224_object *object;
	uint16_t reg;

	object = mxt224_get_object(dp, type);
	if (!object)
		return -EINVAL;

	reg = object->start_address;
	return mxt224_write_reg(dp->fd, reg + offset, val);
}

/*=============================================================================

	Name:  mxt224_get_object_table     
	
=============================================================================*/


static int mxt224_get_object_table(private_data_t	*dp)
{
	int error;
	int i;
	uint16_t reg;
	uint8_t reportid = 0;
	uint8_t buf[MXT224_OBJECT_SIZE];
	struct mxt224_object *object;

	for (i = 0; i < dp->info.object_num; i++) {
		object = dp->object_table + i;

		reg = MXT224_OBJECT_START + MXT224_OBJECT_SIZE * i;
		error = mxt224_read_object_table(dp->fd, reg, buf);
		if (error)
			return error;

		object->type = buf[0];
		object->start_address = (buf[2] << 8) | buf[1];
		object->size = buf[3];
		object->instances = buf[4];
		object->num_report_ids = buf[5];

		if (object->num_report_ids) {
			reportid += object->num_report_ids *	(object->instances + 1);
			object->max_reportid = reportid;
		}
		
		if(verbosity){
			slogf (_SLOGC_INPUT, _SLOG_ERROR, "object->type= %d", object->type);			
			slogf (_SLOGC_INPUT, _SLOG_ERROR, "object->start_address= 0x%x", object->start_address);			
			slogf (_SLOGC_INPUT, _SLOG_ERROR, "object->size= 0x%x", object->size);			
			slogf (_SLOGC_INPUT, _SLOG_ERROR, "object->instances= 0x%x", object->instances);			
			slogf (_SLOGC_INPUT, _SLOG_ERROR, "object->num_report_ids= 0x%x", object->num_report_ids);			
			slogf (_SLOGC_INPUT, _SLOG_ERROR, "object->max_reportid = 0x%x", object->max_reportid);			
		}
	}
	return 0;
}

/*=============================================================================

	Name:  mxt224_object_writable     
  
=============================================================================*/

static int mxt224_object_writable(uint8_t type)
{
	switch (type) {
	case MXT224_GEN_COMMAND:
	case MXT224_GEN_POWER:
	case MXT224_GEN_ACQUIRE:
	case MXT224_TOUCH_MULTI:
	case MXT224_TOUCH_KEYARRAY:
	case MXT224_TOUCH_PROXIMITY:
	case MXT224_PROCI_GRIPFACE:
	case MXT224_PROCG_NOISE:
	case MXT224_PROCI_ONETOUCH:
	case MXT224_PROCI_TWOTOUCH:
	case MXT224_SPT_GPIOPWM:
	case MXT224_SPT_SELFTEST:
	case MXT224_SPT_CTECONFIG:
		return 0;
	default:
		return -1;
	}
}

/*=============================================================================

	Name: mxt224_check_reg_init     
  
=============================================================================*/

static int mxt224_check_reg_init(private_data_t	*dp)
{
	struct mxt224_object *object;
	int index = 0;
	int i, j;
	uint8_t version = dp->info.version;
	uint8_t *init_vals;

	switch (version) {
	case MXT224_VER_20:
		init_vals = (uint8_t *)init_vals_ver_20;
		break;
	case MXT224_VER_21:
		init_vals = (uint8_t *)init_vals_ver_21;
		break;
	case MXT224_VER_22:
		init_vals = (uint8_t *)init_vals_ver_22;
		break;
	default:
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "Firmware version %d doesn't support ",  version);			
		return -EINVAL;
	}

	for (i = 0; i < dp->info.object_num; i++) {
		object = dp->object_table + i;

		if(verbosity)
			slogf (_SLOGC_INPUT, _SLOG_ERROR, "object type : %d  ",  object->type);			

		if (mxt224_object_writable(object->type))
			continue;

		for (j = 0; j < object->size + 1; j++)
			mxt224_write_object(dp, object->type, j, init_vals[index + j]);

		index += object->size + 1;
	}

	return 0;
}

/*=============================================================================

	Name:  mxt224_check_matrix_size    
  
=============================================================================*/

static int mxt224_check_matrix_size(private_data_t	*dp)
{
	struct mxt224_platform_data *pdata = &dp->pdata;
	int mode = -1;
	int error;
	uint8_t val;

	switch (pdata->x_line) {
	case 0 ... 15:
		if (pdata->y_line <= 14)
			mode = 0;
		break;
	case 16:
		if (pdata->y_line <= 12)
			mode = 1;
		if (pdata->y_line == 13 || pdata->y_line == 14)
			mode = 0;
		break;
	case 17:
		if (pdata->y_line <= 11)
			mode = 2;
		if (pdata->y_line == 12 || pdata->y_line == 13)
			mode = 1;
		break;
	case 18:
		if (pdata->y_line <= 10)
			mode = 3;
		if (pdata->y_line == 11 || pdata->y_line == 12)
			mode = 2;
		break;
	case 19:
		if (pdata->y_line <= 9)
			mode = 4;
		if (pdata->y_line == 10 || pdata->y_line == 11)
			mode = 3;
		break;
	case 20:
		mode = 4;
	}

	if ((mode < 0) && verbosity) {
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "--------------%s:Invalid X/Y lines ", __FUNCTION__);			
		return -EINVAL;
	}

	error = mxt224_read_object(dp, MXT224_SPT_CTECONFIG, MXT224_CTE_MODE, &val);
	if (error)
		return error;

	if (mode == val)
		return 0;

	/* Change the CTE configuration */
	mxt224_write_object(dp, MXT224_SPT_CTECONFIG, MXT224_CTE_CTRL, 1);
	mxt224_write_object(dp, MXT224_SPT_CTECONFIG, MXT224_CTE_MODE, mode);
	mxt224_write_object(dp, MXT224_SPT_CTECONFIG, MXT224_CTE_CTRL, 0);

	if(verbosity)
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "--------------%s: mode : %d ", __FUNCTION__,mode);			

	return 0;
}

/*=============================================================================

	Name:  mxt224_make_highchg    
	
=============================================================================*/

static int mxt224_make_highchg(private_data_t	*dp)
{
	int count = 10;
	int error;
	uint8_t val;

	/* Read dummy message to make high CHG pin */
	do {
		error = mxt224_read_object(dp, MXT224_GEN_MESSAGE, 0, &val);
		if (error)
			return error;
	} while ((val != 0xff) && --count);

	if (!count) {
		slogf (_SLOGC_INPUT, _SLOG_ERROR,  "CHG pin isn't cleared\n");
		return -EBUSY;
	}

	return 0;
}

/*=============================================================================

	Name:  mxt224_handle_pdata     
	
=============================================================================*/

static void mxt224_handle_pdata(private_data_t	*dp)
{
	struct mxt224_platform_data *pdata = &dp->pdata;
	uint8_t voltage;

	/* Set touchscreen lines */
	mxt224_write_object(dp, MXT224_TOUCH_MULTI, MXT224_TOUCH_XSIZE, pdata->x_line);
	mxt224_write_object(dp, MXT224_TOUCH_MULTI, MXT224_TOUCH_YSIZE, pdata->y_line);

	/* Set touchscreen orient */
	mxt224_write_object(dp, MXT224_TOUCH_MULTI, MXT224_TOUCH_ORIENT, pdata->orient);

	/* Set touchscreen burst length */
	mxt224_write_object(dp, MXT224_TOUCH_MULTI, MXT224_TOUCH_BLEN, pdata->blen);

	/* Set touchscreen threshold */
	mxt224_write_object(dp, MXT224_TOUCH_MULTI, MXT224_TOUCH_TCHTHR, pdata->threshold);

	/* Set touchscreen resolution */
	mxt224_write_object(dp, MXT224_TOUCH_MULTI, MXT224_TOUCH_XRANGE_LSB, (pdata->x_size - 1) & 0xff);
	mxt224_write_object(dp, MXT224_TOUCH_MULTI, MXT224_TOUCH_XRANGE_MSB, (pdata->x_size - 1) >> 8);
	mxt224_write_object(dp, MXT224_TOUCH_MULTI, MXT224_TOUCH_YRANGE_LSB, (pdata->y_size - 1) & 0xff);
	mxt224_write_object(dp, MXT224_TOUCH_MULTI, MXT224_TOUCH_YRANGE_MSB, (pdata->y_size - 1) >> 8);

	/* Set touchscreen voltage */
	if (dp->info.version >= MXT224_VER_21 && pdata->voltage) {
		if (pdata->voltage < MXT224_VOLTAGE_DEFAULT) {
			voltage = (MXT224_VOLTAGE_DEFAULT - pdata->voltage) / MXT224_VOLTAGE_STEP;
			voltage = 0xff - voltage + 1;
		} else
			voltage = (pdata->voltage - MXT224_VOLTAGE_DEFAULT) / MXT224_VOLTAGE_STEP;

		mxt224_write_object(dp, MXT224_SPT_CTECONFIG, MXT224_CTE_VOLTAGE, voltage);
	}
}

/*=============================================================================

	Name:  mxt224_get_info     
  
=============================================================================*/

static int mxt224_get_info(private_data_t	*dp)
{

	struct mxt224_info *info = &dp->info;
	int error;
	uint8_t val;

	error = mxt224_read_reg(dp->fd, MXT224_FAMILY_ID, &val);
	if (error)
		return error;
 	info->family_id = val;

	error = mxt224_read_reg(dp->fd, MXT224_VARIANT_ID, &val);
	if (error)
		return error;
	info->variant_id = val;

	error = mxt224_read_reg(dp->fd, MXT224_VERSION, &val);
	if (error)
		return error;
	info->version = val;

	error = mxt224_read_reg(dp->fd, MXT224_BUILD, &val);
	if (error)
		return error;
	info->build = val;

	error = mxt224_read_reg(dp->fd, MXT224_OBJECT_NUM, &val);
	if (error)
		return error;
	info->object_num = val;

	if(verbosity){
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "--------------famliy id: 0x%x ", info->family_id);			
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "--------------variant id: 0x%x ", info->variant_id);			
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "--------------version: 0x%x ", info->version);			
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "--------------build: 0x%x ", info->build);			
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "--------------object num id: 0x%x ",info->object_num);			
	}
	
	return 0;
}

/*=============================================================================

	Name:  mxt224_dump_message     
	
=============================================================================*/
static void mxt224_dump_message(struct mxt224_message *message)
{
	if(verbosity){
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "reportid:\t0x%x", message->reportid);
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "message1:\t0x%x", message->message[0]);
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "message2:\t0x%x", message->message[1]);
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "message3:\t0x%x", message->message[2]);
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "message4:\t0x%x", message->message[3]);
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "message5:\t0x%x", message->message[4]);
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "message6:\t0x%x", message->message[5]);
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "message7:\t0x%x", message->message[6]);
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "checksum:\t0x%x", message->checksum);
	}	
}

/*=============================================================================

	Name:  mxt224_input_touchevent     
	
=============================================================================*/
static void mxt224_input_touchevent(private_data_t *dp, struct mxt224_message *message, int id, input_module_t *up )
{
	struct mxt224_platform_data *pdata = &dp->pdata;
	uint8_t status = message->message[0];

	/* Check the touch is present on the screen */
	if (!(status & MXT224_DETECT)) {
		if (status & MXT224_RELEASE) {
			if(verbosity)
				slogf (_SLOGC_INPUT, _SLOG_ERROR, "[%d] released, status 0x%x\n", id, status);
			dp->tp.buttons = 0L;
		}
	}

	/* Check only AMP detection */
	if ((status & (MXT224_PRESS | MXT224_MOVE))){
		dp->tp.buttons =  _POINTER_BUTTON_LEFT;
		dp->tp.x = (message->message[1] << 2) | ((message->message[3] & ~0x3f) >> 6);
		dp->tp.y = (message->message[2] << 2) | ((message->message[3] & ~0xf3) >> 2);

		if(dp->rotate ==1){
			dp->tp.x = pdata->x_size - dp->tp.x;
			dp->tp.y = pdata->y_size - dp->tp.y ;
		}

		if(verbosity)
			slogf (_SLOGC_INPUT, _SLOG_ERROR, "[%d] %s x: %d, y: %d, status 0x%x\n", id,
			status & MXT224_MOVE ? "moved" : "pressed",
			dp->tp.x, dp->tp.y, status);
	}

	/* If we have a release, stuff in the last known good values, to eliminate the
		situation where we get a bad release event every now and then */
	if (!dp->tp.buttons) {
		dp->tp.x = dp->last_x;
		dp->tp.y = dp->last_y;
	} else {
		dp->last_x = dp->tp.x;
		dp->last_y = dp->tp.y;
	}

        if((status & (MXT224_PRESS | MXT224_MOVE) || (!(status & MXT224_DETECT) )))
        {
             clk_get(&dp->tp.timestamp);
             (up->input)(up, 1, &dp->tp);
             if(verbosity > 3)  slogf (_SLOGC_INPUT, _SLOG_ERROR, "[%d] status 0x%x",id,status);
	}	
}

/*=============================================================================

	Name:  mxt224_initialize     
	
=============================================================================*/
static int mxt224_initialize(private_data_t	*dp)
{
	struct mxt224_info *info = &dp->info;
	struct mxt224_platform_data *pdata = &dp->pdata;
	int error;
	uint8_t val;

	/* Get register information */
	error = mxt224_get_info(dp);
	if (error)
		return error;
		
	/* Get object table information */
	dp->object_table = malloc(info->object_num *sizeof(struct mxt224_object));
	error = mxt224_get_object_table(dp);
	if (error)
		return error;

	/* Check register init values */
	error = mxt224_check_reg_init(dp);
	if (error)
		return error;

	/* init _platform_data */
	pdata->x_line = 18;
	pdata->y_line = 12;
	pdata->x_size = 800;
	pdata->y_size	= 480;
	pdata->blen	= 0x01;
	pdata->threshold	= 30;
	pdata->voltage = 2800000;
	pdata->orient = MXT224_HORIZONTAL_FLIP;

	/* Check X/Y matrix size */
	error = mxt224_check_matrix_size(dp);
	if (error)
		return error;

	mxt224_handle_pdata(dp);

	/* Backup to memory */
	mxt224_write_object(dp, MXT224_GEN_COMMAND, MXT224_COMMAND_BACKUPNV, MXT224_BACKUP_VALUE);
	delay(MXT224_BACKUP_TIME);

	/* Soft reset */
	if(verbosity)
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "Issuing soft reset to MXT224 ...");			
	mxt224_write_object(dp, MXT224_GEN_COMMAND, MXT224_COMMAND_RESET, 1);
	delay(MXT224_RESET_TIME);

	/* Touch enable */
	mxt224_write_object(dp, MXT224_TOUCH_MULTI, MXT224_TOUCH_CTRL, 0x83);
			
	error = mxt224_make_highchg(dp);
	if (error)
		return error;

	/* Update matrix size at info struct */
	error = mxt224_read_reg(dp->fd, MXT224_MATRIX_X_SIZE, &val);
	if (error)
		return error;
	info->matrix_xsize = val;

	error = mxt224_read_reg(dp->fd, MXT224_MATRIX_Y_SIZE, &val);
	if (error)
		return error;
	info->matrix_ysize = val;

	if(verbosity){
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "Family ID: %d Variant ID: %d Version: %d Build: %d\n",
			info->family_id, info->variant_id, info->version,
			info->build);			

		slogf (_SLOGC_INPUT, _SLOG_ERROR,"Matrix X Size: %d Matrix Y Size: %d Object Num: %d\n",
			info->matrix_xsize, info->matrix_ysize,
			info->object_num);			
	}
	return 0;
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devi/mxt224/touch.c $ $Rev: 718630 $")
#endif
