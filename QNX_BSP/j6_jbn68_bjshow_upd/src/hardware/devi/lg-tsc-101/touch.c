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

  Driver for the LG I2C touch screen controller.
  A Combination of Device and Protocol modules.
=============================================================================*/


#include <sys/devi.h>
#include "touch.h"

//This controls polling mode vs. irq wait
//This is controls code behaviour
#define POLL

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
        "vi:a:P:p:rX",          /* command line parameters */
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


/*=============================================================================

	Name:         dump_i2c_range

  Description: check to ensure that the input_module_t *module is not NULL

=============================================================================*/
#if 0
static void dump_i2c_range(private_data_t *dp, int address, int start, int end)
{
	int error = 0;
	uint16_t i = 0;
	uint8_t val = 0;
	slogf (_SLOGC_INPUT, _SLOG_ERROR, "\t%s",__FUNCTION__);

	slogf (_SLOGC_INPUT, _SLOG_ERROR, "\t 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");

	//not using address at this time

	for (i=start; i<=end; i++)
	{
		if ((i%16)==0)
		error = TLC59108IPW_read_reg(dp->fd, i, 1, &val);
		if (!error)
			slogf (_SLOGC_INPUT, _SLOG_ERROR, "%02X ",val);
		else
			slogf (_SLOGC_INPUT, _SLOG_ERROR, "!ERR");
	}

	return;
}
#endif

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

/*=============================================================================

  Name:         touch_poll_for_irq

  Description: Poll on GPIO pin to read IRQ status from TSC

=============================================================================*/
static int touch_poll_for_irq(private_data_t *dp)
{
	unsigned pin2_mask = (1 << (dp->irq - GPIO_IRQ_BASE_VECTOR)); //mask for GPIO pin2
	unsigned gpio_in = in32((dp->gpio1_regbase+GPIO_OE));
	unsigned data = 0;

	//somehow this is not catching the wrong config
	if ((gpio_in & pin2_mask) != pin2_mask) {
		out32((dp->gpio1_regbase+GPIO_OE), (gpio_in | pin2_mask));
	}

	//Read the Data In register
	data = in32(dp->gpio1_regbase + GPIO_DATAIN);
	//slogf (_SLOGC_INPUT, _SLOG_ERROR, "\t GPIO_DATAIN=0x%X ",data);

	if (data & pin2_mask)
		return 1;
	else
		return 0;
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

	slogf (_SLOGC_INPUT, _SLOG_ERROR, "\t%s",__FUNCTION__);

	if (!module->data) {
		if (!(dp = module->data = scalloc(sizeof *dp))) {
			slogf (_SLOGC_INPUT, _SLOG_ERROR, "%s: Data pointer scalloc failed (%s)", __FUNCTION__, strerror (errno));
			return (-1);
		}

		//Clear the newly allocated memory. This helps catch any un-init'ed pointers and data members
		memset (module->data, 0, sizeof(*dp));

		/* Enable IO capability.*/
		if (ThreadCtl(_NTO_TCTL_IO, NULL) == -1) {
			perror("ThreadCtl: ");
			exit(EXIT_FAILURE);
		}

		dp->skip_idle_disable = 0;
		dp->rotate = 0;
		dp->flags = FLAG_RESET;
		dp->irq = TOUCH_INT;
		dp->irq_pc = DEVI_PULSE_ALLOC;
		dp->param.sched_priority = PULSE_PRIORITY;
		dp->event.sigev_priority = dp->param.sched_priority;
		dp->verbose = LVL_INFO;

		dp->irq_count = 0;

		dp->speed = TSC_I2C_SPEED;
		dp->slave.addr = TSC_ADDRESS;
		dp->slave.fmt = I2C_ADDRFMT_7BIT;

		dp->i2c = malloc(strlen(TSC_I2C_DEVICE) + 1);
		if (!dp->i2c) {
			slogf (_SLOGC_INPUT, _SLOG_ERROR, "%s: malloc failed for dev name", __FUNCTION__);
			free(dp);
			return (-1);
		}

		dp->gpio1_reglen = DRA74X_GPIO1_REGSIZE;
		dp->gpio1_physbase = DRA74X_GPIO1_BASE;

		dp->gpio1_regbase = mmap_device_io(dp->gpio1_reglen, dp->gpio1_physbase);
		if (dp->gpio1_regbase == (uintptr_t)MAP_FAILED) {
			slogf (_SLOGC_INPUT, _SLOG_ERROR, "%s: mmap_device_io failed for GPIO", __FUNCTION__);
			free(dp->i2c);
			free(dp);
			return (-1);
		}

		strcpy(dp->i2c, TSC_I2C_DEVICE);
		pthread_mutex_init (&dp->mutex, NULL);
	}

	return (0);
}

/*=============================================================================

  Name:		     touch_parm

  Description:	This callback is called by the input runtime system to parse
                command line options to the 'touch' module of driver.

=============================================================================*/
static int touch_parm(input_module_t *module, int opt, char *optarg)
{
	private_data_t *dp = module->data;
	long int v = 0;

	check_module(module);
	slogf (_SLOGC_INPUT, _SLOG_ERROR, "\t%s",__FUNCTION__);

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
				else
					slogf (_SLOGC_INPUT, _SLOG_ERROR, "%s: invalid slave address ", __FUNCTION__);
				break;
		case 'p':
				if (optarg && (strcmp(optarg, dp->i2c) != 0)) {
					free(dp->i2c);
					dp->i2c = malloc(strlen(optarg) + 1);

					if (!dp->i2c) {
						slogf (_SLOGC_INPUT, _SLOG_ERROR, "%s: malloc failed for dev name", __FUNCTION__);
						return -1;
					}
					strcpy(dp->i2c, optarg);
				}
				break;
		case 'P':
				dp->param.sched_priority = atoi (optarg);
				dp->event.sigev_priority = dp->param.sched_priority;
				break;
		case 'r':
				dp->rotate = 1;
				slogf (_SLOGC_INPUT, _SLOG_ERROR, "%s: Rotate in effect now", __FUNCTION__);
				break;
		case 'X':
				dp->skip_idle_disable = 1;
				slogf (_SLOGC_INPUT, _SLOG_ERROR, "%s: Skip disable IDLE mode", __FUNCTION__);
				break;
		default:
				slogf (_SLOGC_INPUT, _SLOG_ERROR, "Unknown option %c", opt);
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
	slogf (_SLOGC_INPUT, _SLOG_ERROR, "\t%s",__FUNCTION__);

	if ((dp->flags & FLAG_INIT) == 0) {
		/* Initialize I2C interface */
		dp->fd = open(dp->i2c, O_RDWR);
		if (dp->fd == -1)	{
			slogf (_SLOGC_INPUT, _SLOG_ERROR, "%s: failure in opening I2C device %s", __FUNCTION__, dp->i2c);
			exit (-1);
		}

		sts = devctl(dp->fd, DCMD_I2C_SET_BUS_SPEED, &dp->speed, sizeof(dp->speed), NULL);
		if (sts != EOK)	{
			slogf (_SLOGC_INPUT, _SLOG_ERROR, "%s: failed to set speed", __FUNCTION__);
			exit (-1);
		}

		//Init the touch hardware to read fw_id etc.
		ldc3001_initialize(dp);

		/* Create touch release timer */
		dp->timerid = devi_register_timer (module, 15, &dp->irq_pc, NULL);

		/* Setup the interrupt handler thread */
		if ((dp->chid = ChannelCreate (_NTO_CHF_DISCONNECT | _NTO_CHF_UNBLOCK)) == -1)	{
			slogf (_SLOGC_INPUT, _SLOG_ERROR, "%s: ChannelCreate failed: %s", __FUNCTION__, strerror (errno));
			free (dp);
			exit (-1);
		}

		if ((dp->coid = ConnectAttach (0, 0, dp->chid, _NTO_SIDE_CHANNEL, 0)) == -1) {
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

		dp->event.sigev_notify = SIGEV_PULSE;
		dp->event.sigev_coid = dp->coid;
		dp->event.sigev_code = 1;

		/* Create interrupt handler thread */
		if (pthread_create (NULL, &dp->pattr, (void *)intr_thread, module)) {
			slogf (_SLOGC_INPUT, _SLOG_ERROR, "%s: pthread_create failed: %s", __FUNCTION__, strerror (errno));
			free (dp);
			exit (-1);
		}

#ifdef POLL
		slogf (_SLOGC_INPUT, _SLOG_INFO, "devi-lg-tsc driver - Using Polling mode");
#else
		slogf (_SLOGC_INPUT, _SLOG_INFO, "devi-lg-tsc driver - Using Interrupt mode");

		/* Attach interrupt. */
		if (dp->verbose >= 3) {
			slogf (_SLOGC_INPUT, _SLOG_INFO, "devi-lg-tsc driver - Attaching to interrupt: %d", dp->irq);
		}

		if ((dp->iid = InterruptAttachEvent (dp->irq, &dp->event, _NTO_INTR_FLAGS_TRK_MSK)) == -1) {
			slogf (_SLOGC_INPUT, _SLOG_ERROR, "%s: InterruptAttachEvent failed: %s", __FUNCTION__, strerror (errno));
			free (dp);
			exit (-1);
		}

		//Now configure GPIO Pin for irq generation
		setup_gpio_as_tch_irq(dp);
#endif

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
	private_data_t *dp = module->data;

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
			//range->max = dp->pdata.x_size - 1; //resolution of touch is 1280x800, same as display
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
static void *intr_thread(void *data, int argid)
{
	input_module_t *module = (input_module_t *) data;
	private_data_t *dp = module->data;
	input_module_t *up = module->up;
	struct _pulse pulse;
	iov_t iov;
	int error = 0;
	struct tch_data	message;
	int rcvid;

	check_module(module);

	SETIOV (&iov, &pulse, sizeof(pulse));

	//init tp[] for first use
	memset (&dp->tp[0], 0, MAX_TOUCH_POINTS*sizeof(dp->tp[0]));

	/* Enable IO capability.*/
	if (ThreadCtl(_NTO_TCTL_IO, NULL) == -1) {
		perror("ThreadCtl: ");
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "\t %s: thread IO failed",__FUNCTION__);
		exit(EXIT_FAILURE);
	}

	while (1)
	{
#ifdef POLL
		//polling delay - observations noted touch events come in at max 1/10ms rate
		delay(5);

		//read what GPIO1 pin2 has to say abt this
		rcvid = touch_poll_for_irq(dp);
		if (!rcvid)
		{
			continue;
		}
#else
		if ((rcvid = MsgReceivev (dp->chid, &iov, 1, NULL)) == -1)	{
			if (errno == ESRCH) {
				slogf (_SLOGC_INPUT, _SLOG_ERROR, "\t Error:Exiting %s",__FUNCTION__);
				pthread_exit (NULL);
			}
			continue;
		}

		switch (pulse.code)
		{
			case PULSE_CODE:
				pthread_mutex_lock (&dp->mutex);
#endif
				error = touch_read_message(dp, &message);
				if (error) {
					slogf (_SLOGC_INPUT, _SLOG_ERROR, "%s:Failed to read message", __FUNCTION__);
					goto end;
				}

				//process the touch event
				touch_input_touchevent(dp, &message, up);

end:
				/* Reset 'tp' for next use */
				memset (&dp->tp[0], 0, MAX_TOUCH_POINTS*sizeof(dp->tp[0]));
#ifndef POLL
				pthread_mutex_unlock (&dp->mutex);
				InterruptUnmask (dp->irq, dp->iid);
				break;

			default:
				slogf (_SLOGC_INPUT, _SLOG_ERROR, "\t%s:Default Case",__FUNCTION__);
				if (rcvid) {
					MsgReplyv (rcvid, ENOTSUP, &iov, 1);
				}
				break;
			}//end of switch
#endif
		}//end of while

	slogf (_SLOGC_INPUT, _SLOG_ERROR, "\t %s:Exiting...",__FUNCTION__);
	return(NULL);
}

/*=============================================================================

  Name:         touch_shutdown

  Description:  This callback preforms the cleanup of the driver when it is
                signaled to quit.

=============================================================================*/
static int touch_shutdown(input_module_t *module, int shutdown_delay)
{
	private_data_t *dp = module->data;

	slogf (_SLOGC_INPUT, _SLOG_ERROR, "\t%s ",__FUNCTION__);

	check_module(module);

	InterruptDetach(dp->iid);

	munmap_device_io(dp->gpio1_regbase, dp->gpio1_reglen);
	dp->gpio1_regbase = NULL;

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
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "%s: devctl failed %s for X", __FUNCTION__, strerror (errno));
		return -1;
	}

	for (i=0;i<len;i++, val++) {
		*val = rd_data.buf[i];
	}

	return 0;
}

/*=============================================================================

	Name:    touch_read_reg

=============================================================================*/
static int touch_read_reg(int fd, uint16_t reg, uint8_t *val)
{
	return read_reg(fd, reg, 1, val);
}

/*=============================================================================

	Name:         touch_write_reg

=============================================================================*/
static int touch_write_reg(int fd, uint16_t reg, uint8_t val)
{
	struct {
 		i2c_send_t hdr;
 		uint8_t buf[3];
 	}rd_data;

	slogf (_SLOGC_INPUT, _SLOG_ERROR, "\t%s: reg=0x%X val=0x%X",__FUNCTION__,reg,val);

	rd_data.hdr.slave.addr = TSC_ADDRESS;
	rd_data.hdr.slave.fmt = I2C_ADDRFMT_7BIT;
	rd_data.hdr.len = 3;
	rd_data.hdr.stop = 1;
	rd_data.buf[0] = reg & 0xff;
	rd_data.buf[1] = (reg >> 8) & 0xff;
	rd_data.buf[2] = val;

	if (devctl(fd, DCMD_I2C_SEND, &rd_data, sizeof(rd_data), NULL)) {
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "%s: devctl failed for %s", __FUNCTION__, strerror (errno));
		return -1;
	}

	return 0;
}

/*=============================================================================

  Name:         TLC59108IPW_read_reg

=============================================================================*/
static int TLC59108IPW_read_reg(int fd, uint16_t reg, uint16_t len, uint8_t *val)
{
	struct {
 		i2c_sendrecv_t hdr;
 		uint8_t buf[128];
 	} rd_data;
	int i;

	rd_data.hdr.slave.addr = TLC59108IPW_ADDRESS;
	rd_data.hdr.slave.fmt = I2C_ADDRFMT_7BIT;
	rd_data.hdr.send_len = 2;
	rd_data.hdr.recv_len = len;
	rd_data.hdr.stop = 1;
	rd_data.buf[0] = reg & 0xff;
	rd_data.buf[1] = (reg >> 8) & 0xff;

	if (devctl(fd, DCMD_I2C_SENDRECV, &rd_data, sizeof(rd_data), NULL)) {
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "%s: devctl failed %s for X", __FUNCTION__, strerror (errno));
		return -1;
	}

	for (i=0; i<len; i++,val++) {
		*val = rd_data.buf[i];
	}

	return 0;
}

/*=============================================================================

	Name:         TLC59108IPW_write_reg

=============================================================================*/
static int TLC59108IPW_write_reg(int fd, uint16_t reg, uint8_t val)
{
	struct {
 		i2c_send_t hdr;
 		uint8_t buf[3];
 	} rd_data;

	rd_data.hdr.slave.addr = TLC59108IPW_ADDRESS;
	rd_data.hdr.slave.fmt = I2C_ADDRFMT_7BIT;
	rd_data.hdr.len = 3;
	rd_data.hdr.stop = 1;
	rd_data.buf[0] = reg & 0xff;
	rd_data.buf[1] = (reg >> 8) & 0xff;
	rd_data.buf[2] = val;

	if (devctl(fd, DCMD_I2C_SEND, &rd_data, sizeof(rd_data), NULL)) {
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "%s: devctl failed for %s", __FUNCTION__, strerror (errno));
		return -1;
	}

	return 0;
}

/*=============================================================================

	Name:  touch_read_message

=============================================================================*/
static int touch_read_message(private_data_t *dp, struct tch_data *message)
{
	int error = 0;
	uint8_t	length = 0;

	//Clear the message so that we do not get false positive
	memset((uint8_t *)message, 0, sizeof(*message));

	//Read the Length of the touch data
	error = touch_read_reg(dp->fd, TCH_TOUCH_DATA_LENGTH, &length);
	if (error) {
		slogf (_SLOGC_INPUT, _SLOG_ERROR, " Error:%s: line %d",__FUNCTION__, __LINE__);
		return error;
	}

	//Now we know how long the touch packet is going to be. Read all the info directly into tch_data entity
	if (length > 2) {
		error = read_reg(dp->fd, TCH_TOUCH_DATA, length, (uint8_t *)message);
		if (error) {
			slogf (_SLOGC_INPUT, _SLOG_ERROR, " Error:%s: line %d",__FUNCTION__, __LINE__);
			return error;
		}
	}

	return 0;
}

/*=============================================================================

	Name:  touch_input_touchevent
	*
	*

=============================================================================*/
static void touch_input_touchevent(private_data_t *dp, struct tch_data *message, input_module_t *up)
{
	struct lg_tch_platform_data *pdata = &dp->pdata;
	uint16_t touchIndex = FULL_TCH_ID(message->touch_id);
#if 0
	int i = 0;

	for (i=0; i<MAX_TOUCH_POINTS; i++)
	{
		/* tracking id has a touch event */
		if (touchIndex & (1 << i)) {
			dp->tp[i].buttons = _POINTER_BUTTON_LEFT;
			dp->tp[i].x = GET_ABS_TCH_X(message->touch_points[i]);
			dp->tp[i].y = GET_ABS_TCH_Y(message->touch_points[i]);

			//rotate by 180 degrees
			if (dp->rotate == 1) {
				dp->tp[i].x = pdata->x_size - dp->tp[i].x;
				dp->tp[i].y = pdata->y_size - dp->tp[i].y ;
			}

			//Update time stamp
			clk_get(&(dp->tp[i].timestamp));
		} else {
			dp->tp[i].buttons = 0L;
			clk_get(&(dp->tp[i].timestamp));
		}
	}
#else
	/* Processing only the first touch event*/
	if (touchIndex & (1 << 0)) {
		dp->tp[0].buttons = _POINTER_BUTTON_LEFT;
		dp->tp[0].x = GET_ABS_TCH_X(message->touch_points[0]);
		dp->tp[0].y = GET_ABS_TCH_Y(message->touch_points[0]);

		//rotate by 180 degrees
		if (dp->rotate == 1) {
			dp->tp[0].x = pdata->x_size - dp->tp[0].x;
			dp->tp[0].y = pdata->y_size - dp->tp[0].y ;
		}

		//Update time stamp
		clk_get(&(dp->tp[0].timestamp));
	} else {
		dp->tp[0].buttons = 0L;
		clk_get(&(dp->tp[0].timestamp));
	}
#endif

	if (verbosity > 3) {
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "Touch [%d] x: %d, y: %d b: %d",
				touchIndex, dp->tp[0].x, dp->tp[0].y, dp->tp[0].buttons);
	}

	// currently sent only one touch point
	(up->input)(up, 1, &dp->tp[0]);

	return;
}

/*=============================================================================

	Name:	    setup_gpio_as_tch_irq

  Description: Setup GPIO pin as level irq. Note we may get more than one ISR call by making it level

=============================================================================*/
#ifndef POLL
static void setup_gpio_as_tch_irq(private_data_t *dp)
{
	slogf (_SLOGC_INPUT, _SLOG_ERROR, "\t%s: dp->irq=%d",__FUNCTION__,dp->irq);

	unsigned pin = (1 << (dp->irq - GPIO_IRQ_BASE_VECTOR));

	//Padconf is already set in hw_init.c.

	//Make the pin input
	out32((dp->gpio1_regbase + GPIO_OE), in32(dp->gpio1_regbase + GPIO_OE) | pin);
	//reset low level irq detect
	out32(dp->gpio1_regbase + GPIO_LEVELDETECT0, in32(dp->gpio1_regbase + GPIO_LEVELDETECT0) & ~pin);
	//set high level irq detect
	out32(dp->gpio1_regbase + GPIO_LEVELDETECT1, in32(dp->gpio1_regbase + GPIO_LEVELDETECT1) | pin);
	//no edge detect
	out32(dp->gpio1_regbase + GPIO_RISINGDETECT, in32(dp->gpio1_regbase + GPIO_RISINGDETECT) & ~pin);
	out32(dp->gpio1_regbase + GPIO_FALLINGDETECT, in32(dp->gpio1_regbase + GPIO_FALLINGDETECT) | pin);

	//Reset any stray status lurking
	out32(dp->gpio1_regbase + GPIO_IRQSTATUS_0, in32(dp->gpio1_regbase + GPIO_IRQSTATUS_0) | pin);

#if 1
	slogf (_SLOGC_INPUT, _SLOG_ERROR, "\t%s: GPIO_OE is %d",__FUNCTION__, in32(dp->gpio1_regbase + GPIO_OE));
	slogf (_SLOGC_INPUT, _SLOG_ERROR, "\t%s: GPIO_LEVELDETECT0 is %d",__FUNCTION__, in32(dp->gpio1_regbase + GPIO_LEVELDETECT0));
	slogf (_SLOGC_INPUT, _SLOG_ERROR, "\t%s: GPIO_LEVELDETECT1 is %d",__FUNCTION__, in32(dp->gpio1_regbase + GPIO_LEVELDETECT1));
	slogf (_SLOGC_INPUT, _SLOG_ERROR, "\t%s: GPIO_RISINGDETECT is %d",__FUNCTION__, in32(dp->gpio1_regbase + GPIO_RISINGDETECT));
	slogf (_SLOGC_INPUT, _SLOG_ERROR, "\t%s: GPIO_FALLINGDETECT is %d",__FUNCTION__, in32(dp->gpio1_regbase + GPIO_FALLINGDETECT));
	slogf (_SLOGC_INPUT, _SLOG_ERROR, "\t%s: GPIO_IRQSTATUS_0 is %d",__FUNCTION__, in32(dp->gpio1_regbase + GPIO_IRQSTATUS_0));
#endif
	return;
}
#endif

/*=============================================================================

	Name:  tlc_wakeup_ldc

=============================================================================*/
static int tlc_wakeup_ldc(private_data_t *dp)
{
	int error = 0;
	uint8_t val = 0;

	//Now raise the LED6 pin high
	val = 0;
	error = TLC59108IPW_read_reg(dp->fd, TLC59108IPW_LEDOUT1, 1, &val);
	if (error) {
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "Error:%s: line %d",__FUNCTION__, __LINE__);
		return error;
	} else {
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "\t%s: TLC59108IPW_LEDOUT1 value is %d",__FUNCTION__, val);
	}

	//Do not loose previous values, just add ours
	val |= TOUCH_WAKE_HIGH;
	error = TLC59108IPW_write_reg(dp->fd, TLC59108IPW_LEDOUT1, val);
	if (error) {
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "Error:%s: line %d",__FUNCTION__, __LINE__);
		return error;
	}

#if 0	/* JI:701844 */
	error = TLC59108IPW_read_reg(dp->fd, TLC59108IPW_LEDOUT1, 1, &val);
	if (error) {
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "Error:%s: line %d",__FUNCTION__, __LINE__);
		return error;
	} else {
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "\t%s: TLC59108IPW_LEDOUT1 value is %d",__FUNCTION__, val);
	}
#endif

	delay(50);

	return error;
}

/*=============================================================================

	Name:  ldc3001_initialize

=============================================================================*/
static int ldc3001_initialize(private_data_t *dp)
{
	int error = 0;
	uint8_t val = 0;
	struct lg_tch_platform_data *pdata = &dp->pdata;
	struct lg_tch_info *info = &dp->info;

	slogf (_SLOGC_INPUT, _SLOG_ERROR, "\t%s ",__FUNCTION__);

	// Wakup the LDC
	tlc_wakeup_ldc(dp);

	// read firmware id
	error = touch_read_reg(dp->fd, TCH_FIRMWARE_ID_R, &val);
	if (error) {
		slogf (_SLOGC_INPUT, _SLOG_ERROR, " Error:%s: line %d",__FUNCTION__, __LINE__);
		return error;
	}

	info->fw_id = val;
	slogf (_SLOGC_INPUT, _SLOG_ERROR, "\t--- FW id: %d.%d", (info->fw_id/100), (info->fw_id%100));

	/* (JI:701844) */
	if (!dp->skip_idle_disable) {
		// read idle mode
		error = touch_read_reg(dp->fd, TCH_IDLE_READ_R, &val);
		if (error) {
			slogf (_SLOGC_INPUT, _SLOG_ERROR, " Error:%s: line %d",__FUNCTION__, __LINE__);
			return error;
		}
		info->idle_read = val;
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "\t--- idle_read: 0x%x ", info->idle_read);

		// disable idle mode
		val = ((val & ~TCH_IDLE_MASK) | TCH_IDLE_OFF);
		error = touch_write_reg(dp->fd, TCH_IDLE_CONTROL_W, val);
		if (error) {
			slogf (_SLOGC_INPUT, _SLOG_ERROR, " Error:%s: line %d",__FUNCTION__, __LINE__);
			return error;
		}

		// re-read idle mode
		error = touch_read_reg(dp->fd, TCH_IDLE_READ_R, &val);
		if (error) {
			slogf (_SLOGC_INPUT, _SLOG_ERROR, " Error:%s: line %d",__FUNCTION__, __LINE__);
			return error;
		}
		info->idle_read = val;
		slogf (_SLOGC_INPUT, _SLOG_ERROR, "\t--- idle_read: 0x%x ", info->idle_read);
	}

	// read calibration check
	error = touch_read_reg(dp->fd, TCH_CALIBRATION_CHECK_R, &val);
	if (error) {
		slogf (_SLOGC_INPUT, _SLOG_ERROR, " Error:%s: line %d",__FUNCTION__, __LINE__);
		return error;
	}
	info->calibration_check = val;
	slogf (_SLOGC_INPUT, _SLOG_ERROR, "\t--- calibration_check: 0x%x ", info->calibration_check);

	/* init _platform_data */
	pdata->x_size = 1280;
	pdata->y_size = 800;

	return 0;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devi/lg-tsc-101/touch.c $ $Rev: 759883 $")
#endif
