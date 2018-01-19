/*
 * $QNXLicenseC:
 * Copyright 2008, QNX Software Systems.
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

#include "externs.h"
#include <sys/mman.h>

void set_port(unsigned port, unsigned mask, unsigned data)
{
	unsigned char c;

	c = read_omap(port);
	write_omap(port, (c & ~mask) | (data & mask));
}

static void clear_device(const uintptr_t * port)
{
	write_omap(port[OMAP_UART_IER], 0);		// Disable all interrupts
	/* Clear FIFOs */
	set_port(port[OMAP_UART_FCR], OMAP_FCR_RXCLR | OMAP_FCR_TXCLR, OMAP_FCR_RXCLR | OMAP_FCR_TXCLR);
	read_omap(port[OMAP_UART_LSR]);			// Clear Line Status Interrupt
	read_omap(port[OMAP_UART_MSR]);			// Clear Modem Interrupt
}

void run_errata_i202(DEV_OMAP *dev)
{
	int i = 5;

	/* Work around for silicon errata i202 states: */
	/* Need a delay = 5 L4 clock cycles + 5 UART functional clock cycle (@48MHz = ~0.2uS) */
	nanospin_ns(200);

	/* Clear FIFOs */
	set_port(dev->port[OMAP_UART_FCR], OMAP_FCR_RXCLR | OMAP_FCR_TXCLR, OMAP_FCR_RXCLR | OMAP_FCR_TXCLR);

	/* Wait for FIFO to empty: when empty, RX_FIFO_E bit is 0 and TX_FIFO_E bit is 1 */
	while ((read_omap(dev->port[OMAP_UART_LSR]) & (OMAP_UART_LSR_THRE | OMAP_UART_LSR_DR)) != OMAP_UART_LSR_THRE)
	{
		nanospin_ns(200);
		if (--i == 0)
		{
			break; /* No need to do anything drastic if FIFO is still not empty */
		}
	}
}

int seromap_interrupt_attach(DEV_OMAP * dev)
{
	struct sigevent event;

	if (dev->isr)
	{
		dev->iid = InterruptAttach(dev->intr, seromap_interrupt_handler, dev, 0, 0);
		if (dev->iid == -1)
			return -1;
	}
	else
	{
		// Associate a pulse which will call the event handler.
		if ((event.sigev_code =
			pulse_attach(ttyctrl.dpp, MSG_FLAG_ALLOC_PULSE, 0, &seromap_interrupt_event_handler, dev)) == -1)
		{
			fprintf(stderr, "Unable to attach event pulse.\n");
			return -1;
		}

		/* Init the pulse for interrupt event */
		event.sigev_notify = SIGEV_PULSE;
		event.sigev_coid = ttyctrl.coid;
		event.sigev_priority = ttyctrl.event.sigev_priority + 1;		/* service interrupts at a higher priority then client requests */
		event.sigev_value.sival_int = 0;

		dev->iid = InterruptAttachEvent(dev->intr, &event, _NTO_INTR_FLAGS_TRK_MSK);
		if (dev->iid == -1)
			return -1;
	}

	return EOK;
}

//
// Clean up the device then add it to the interrupt list and enable it.
//
int ser_attach_intr(DEV_OMAP * dev)
{
	uintptr_t *port = dev->port;

	// According to the National bug sheet you must wait for the transmit
	// holding register to be empty.
	do
	{
	} while ((read_omap(port[OMAP_UART_LSR]) & OMAP_LSR_TXRDY) == 0);

	clear_device(port);

	if (seromap_interrupt_attach(dev) != EOK)
		return (-1);

	// Enable interrupt sources except transmit interrupt.
	write_omap(port[OMAP_UART_IER], OMAP_IER_RHR | OMAP_IER_LS | OMAP_IER_MS);

	return (EOK);
}

static void destroy_virtual_devices(DEV_OMAP * dev)
{
	int cnt;

	for (cnt = 0; cnt < NUM_DEVS; cnt++)
	{
		if (dev->vdev[cnt].tty.ibuf.buff != NULL)
		{
			free(dev->vdev[cnt].tty.ibuf.buff);
			dev->vdev[cnt].tty.ibuf.buff = NULL;
		}
		if (dev->vdev[cnt].tty.obuf.buff != NULL)
		{
			free(dev->vdev[cnt].tty.obuf.buff);
			dev->vdev[cnt].tty.obuf.buff = NULL;
		}
		if (dev->vdev[cnt].tty.cbuf.buff != NULL)
		{
			free(dev->vdev[cnt].tty.cbuf.buff);
			dev->vdev[cnt].tty.cbuf.buff = NULL;
		}
	}
}

int seromap_devctl(resmgr_context_t * ctp, io_devctl_t * msg, iofunc_ocb_t * ocb)
{
	virtual_dev_t *vdev = (virtual_dev_t *) ocb->attr;
	DEV_OMAP *dev = vdev->dev;
	int status = EOK;
	union
	{
		unsigned int udata32;
	} *dcp = _DEVCTL_DATA (msg->i);


	switch (msg->i.dcmd)
	{
		case DCMD_CHR_RESET:
			run_init_script(dev);
			break;
		case DCMD_CHR_GETOBAND_EXTENDED:
			dcp->udata32 = vdev->oband_data;
			vdev->oband_data = 0;	/* Clear data */
			break;
		default:
			status = EINVAL;
			break;
	}

	return (status);
}

static void destroy_device(DEV_OMAP *dev)
{
	destroy_virtual_devices(dev);
	bt_deinit(dev);
	if (dev->rx_buffer)
		munmap(dev->rx_buffer, dev->rx_buffer_size);
	free(dev);
}

DEV_OMAP *create_device(TTYINIT_OMAP * dip, unsigned unit, unsigned maxim_xcvr_kick)
{
	DEV_OMAP *dev;
	unsigned i, cnt;
	uintptr_t port;
	unsigned char msr;
	unsigned char tlr = 0, tcr = 0;
	int vdev_unit;

	// Get a device entry and the input/output buffers for it.
	if ((dev = malloc(sizeof (*dev))) == NULL)
	{
		slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "io-char: Allocation of device entry failed (%d)", errno);
		return (dev);
	}
	memset(dev, 0, sizeof (*dev));

	//copy the max retry value
	dev->max_retry = dip->max_retry;

	// GPIO base and number used for BT enable
	dev->gpio_base = dip->gpio_base;
	dev->gpio_num = dip->gpio_num;

	dev->baud_mid = dip->baud_mid;
	dev->baud_high = dip->baud_high;

	if (bt_init(dev) != EOK)
	{
		slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Unable to initialize bt interface - %s", __FUNCTION__, strerror(errno));
		free(dev);
		return (NULL);
	}

	port = mmap_device_io(OMAP_UART_SIZE, dip->tty.port);
	for (i = 0; i < OMAP_UART_SIZE; i += 4)
		dev->port[i] = port + i;

	/* Soft reset */
	set_port(dev->port[OMAP_UART_SYSC], OMAP_SYSC_SOFTRESET, OMAP_SYSC_SOFTRESET);
	cnt = 0;
	while (!(read_omap(dev->port[OMAP_UART_SYSS]) & OMAP_SYSS_RESETDONE) && cnt++ < 100)
		delay(10);
	if (cnt >= 100)
	{
		slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: UART Reset did not complete", __FUNCTION__);
		fprintf(stderr, "%s: UART Reset did not complete", __FUNCTION__);
	}

	dev->intr = dip->tty.intr;
	dev->clk = dip->tty.clk;
	dev->div = dip->tty.div;
	dev->isr = dip->isr;

	/* Set auto_rts mode */
	dev->auto_rts_enable = dip->auto_rts_enable;

	// see if we have a maxim rs-232 transceiver that needs to be
	// kicked after it goes to sleep
	dev->kick_maxim = maxim_xcvr_kick;

	/* Alloc the rx buffer */
	dev->rx_buffer_size = min(dip->tty.isize / 2, RX_BUFFER_SIZE);	/* Limit the RX buffer to 1/2 the ibuf size for flow control reasons */
	if ((dev->rx_buffer = mmap (NULL, dev->rx_buffer_size, PROT_READ | PROT_WRITE | PROT_NOCACHE,
		MAP_ANON | MAP_PHYS, NOFD, 0)) == MAP_FAILED)
	{
		slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "io-char: Failed to alloc RX buffer - %s", strerror(errno));
		destroy_device(dev);
		return (NULL);
	}

	/* Create virtual devices */
	for (cnt = 0; cnt < NUM_DEVS; cnt++)
	{
		// Get buffers.
		dev->vdev[cnt].tty.ibuf.head = dev->vdev[cnt].tty.ibuf.tail = dev->vdev[cnt].tty.ibuf.buff =
			malloc(dev->vdev[cnt].tty.ibuf.size = dip->tty.isize);
		if (dev->vdev[cnt].tty.ibuf.buff == NULL)
		{
			slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "io-char: Allocation of input buffer failed (%d)", errno);
			destroy_device(dev);
			return (NULL);
		}

		dev->vdev[cnt].tty.obuf.head = dev->vdev[cnt].tty.obuf.tail = dev->vdev[cnt].tty.obuf.buff =
			malloc(dev->vdev[cnt].tty.obuf.size = dip->tty.osize);
		if (dev->vdev[cnt].tty.obuf.buff == NULL)
		{
			slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "io-char: Allocation of output buffer failed (%d)", errno);
			destroy_device(dev);
			return (NULL);
		}

		dev->vdev[cnt].tty.cbuf.head = dev->vdev[cnt].tty.cbuf.tail = dev->vdev[cnt].tty.cbuf.buff =
			malloc(dev->vdev[cnt].tty.cbuf.size = dip->tty.csize);
		if (dev->vdev[cnt].tty.cbuf.buff == NULL)
		{
			slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "io-char: Allocation of canonical buffer failed (%d)", errno);
			destroy_device(dev);
			return (NULL);
		}

		if (dip->tty.highwater)
			dev->vdev[cnt].tty.highwater = dip->tty.highwater;
		else
			dev->vdev[cnt].tty.highwater = dev->vdev[cnt].tty.ibuf.size - dev->rx_buffer_size;

		switch (cnt)
		{
			case 0:
				strcpy(dev->vdev[cnt].tty.name, "/dev/serbt");
				break;
			case 1:
				strcpy(dev->vdev[cnt].tty.name, "/dev/serfm");
				break;
			case 2:
				strcpy(dev->vdev[cnt].tty.name, "/dev/sergps");
				break;
			default:
				break;
		}
		dev->vdev[cnt].tty.baud = dip->tty.baud;
		dev->vdev[cnt].tty.fifo = dip->tty.fifo;
		dev->vdev[cnt].tty.verbose = dip->tty.verbose;

		dev->vdev[cnt].tty.flags = EDIT_INSERT | LOSES_TX_INTR;
		dev->vdev[cnt].tty.c_cflag = dip->tty.c_cflag;
		dev->vdev[cnt].tty.c_iflag = dip->tty.c_iflag;
		dev->vdev[cnt].tty.c_lflag = dip->tty.c_lflag;
		dev->vdev[cnt].tty.c_oflag = dip->tty.c_oflag;

#ifdef WINBT
		if (cnt == 0)
		{
			dev->physbase = dip->tty.port;
			if (omap_clock_toggle_init(dev) != EOK)
			{
				slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "io-char: Fail to initialize clocks for PM!");
				destroy_device(dev);
				return (NULL);
			}

			if (omap_force_rts_init(dev) != EOK)
			{
				slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "io-char: Fail to initialize force_rts for PM!");
				destroy_device(dev);
				return (NULL);
			}
			omap_clock_enable(dev);
		}
#endif

		// Initialize termios cc codes to an ANSI terminal.
		ttc(TTC_INIT_CC, &dev->vdev[cnt].tty, 0);
		/* The driver defaults the CREAD terminal flag to be off so un-used virtual ports
		 * will not accumulate data and trigger flow control halt.
		 * Client application must explicitly set the CREAD terminal flag if the want
		 * to use the virtual device.
		 */
		dev->vdev[cnt].tty.c_cflag &= ~(CREAD);

		// Initialize the device's name.
		// Assume that the basename is set in device name. This will attach
		// to the path assigned by the unit number/minor number combination
		vdev_unit = SET_NAME_NUMBER(unit) | NUMBER_DEV_FROM_USER;
		ttc(TTC_INIT_TTYNAME, &dev->vdev[cnt].tty, vdev_unit);

		dev->vdev[cnt].dev = dev;
	}

	// Only setup IRQ handler for non-pcmcia devices.
	// Pcmcia devices will have this done later when card is inserted.
	if (dip->tty.port != 0 && dip->tty.intr != _NTO_INTR_SPARE)
	{
		/*
		 * TRM states: Before initializing or modifying clock parameter controls
		 * (DLH, DLL), MODE_SELECT must be set to 0x7 (DISABLE). Failure to observe
		 * this rule can result in unpredictable module behavior.
		 * MDR1 is re-enabled later when ser_stty() is called
		 */
		write_omap(dev->port[OMAP_UART_MDR1], OMAP_MDR1_MODE_DISABLE);
		run_errata_i202(dev);

		// enable and clear fifo's
		write_omap(dev->port[OMAP_UART_LCR], OMAP_LCR_DLAB);
		write_omap(dev->port[OMAP_UART_DLL], 0);
		write_omap(dev->port[OMAP_UART_DLH], 0);

		/* Switch to config mode B to get access to EFR Register */
		write_omap(dev->port[OMAP_UART_LCR], 0xBF);
		/* Enable access to TLR register */
		set_port(dev->port[OMAP_UART_EFR], OMAP_EFR_ENHANCED, OMAP_EFR_ENHANCED);
		/* Switch to operational mode to get acces to MCR register */
		write_omap(dev->port[OMAP_UART_LCR], 0x00);
		/* set MCR bit 6 to enable access to TCR and TLR registers */
		set_port(dev->port[OMAP_UART_MCR], OMAP_MCR_TCRTLR, OMAP_MCR_TCRTLR);
		write_omap(dev->port[OMAP_UART_FCR], OMAP_FCR_ENABLE | OMAP_FCR_RXCLR | OMAP_FCR_TXCLR);
		tcr = 0x0e;							/* set auto-rts assert at 56 bytes, restore at 0 bytes */
		if (dev->vdev[0].tty.fifo)
		{
			/* Set RX fifo trigger level */
			switch (dev->vdev[0].tty.fifo >> 4)
			{
				case FIFO_TRIG_8:
				default:
					tlr = 0x20;
					break;
				case FIFO_TRIG_16:
					tlr = 0x40;
					break;
				case FIFO_TRIG_32:
					tlr = 0x80;
					break;
				case FIFO_TRIG_56:
					tlr = 0xe0;
					break;
				case FIFO_TRIG_60:
					tlr = 0xf0;
					tcr = 0x0f;				/* Ensure auto-rts trigger is not less the RX trigger */
					break;
			}

			/* Set TX fifo trigger level */
			switch (dev->vdev[0].tty.fifo & 0x0f)
			{
				case FIFO_TRIG_8:
				default:
					tlr |= 0x02;
					break;
				case FIFO_TRIG_16:
					tlr |= 0x04;
					break;
				case FIFO_TRIG_32:
					tlr |= 0x08;
					break;
				case FIFO_TRIG_56:
					tlr |= 0x0e;
					break;
				case FIFO_TRIG_60:
					tlr |= 0x0f;
					break;
			}
		}
		write_omap(dev->port[OMAP_UART_TCR], tcr);
		write_omap(dev->port[OMAP_UART_TLR], tlr);
#ifdef WINBT
		write_omap(dev->port[OMAP_UART_SCR], OMAP_SCR_WAKEUPEN);
#else
		write_omap(dev->port[OMAP_UART_SCR], 0x00);
#endif
		/* Switch back to Config mode B to gain access to EFR again */
		write_omap(dev->port[OMAP_UART_LCR], 0xBF);
		/* remove access to TLR register */
		set_port(dev->port[OMAP_UART_EFR], OMAP_EFR_ENHANCED, 0);
		/* Switch to operational mode to get acces to MCR register */
		write_omap(dev->port[OMAP_UART_LCR], 0x00);
		/* clr MCR bit 6 to remove access to TCR and TLR registers */
		set_port(dev->port[OMAP_UART_MCR], OMAP_MCR_TCRTLR, 0);

		ser_stty(&dev->vdev[0]);
		if (ser_attach_intr(dev) != EOK)
			exit(1);

		/* Set modem control lines to a ready state */
		if (dev->auto_rts_enable)
			set_port(dev->port[OMAP_UART_MCR], OMAP_MCR_DTR, OMAP_MCR_DTR);
		else
			set_port(dev->port[OMAP_UART_MCR], OMAP_MCR_DTR | OMAP_MCR_RTS, OMAP_MCR_DTR | OMAP_MCR_RTS);

		if (dip->loopback)
		{
			set_port(dev->port[OMAP_UART_MCR], OMAP_MCR_LOOPBACK, OMAP_MCR_LOOPBACK);
		}
	}

	// get current MSR stat
	msr = read_omap(dev->port[OMAP_UART_MSR]);

	if (msr & OMAP_MSR_DDCD)
	{
		for (cnt = 0; cnt < NUM_DEVS; cnt++)
			tti(&dev->vdev[cnt].tty, (msr & OMAP_MSR_DCD) ? TTI_CARRIER : TTI_HANGUP);
	}

	if ((msr & OMAP_MSR_DCTS) && (dev->vdev[0].tty.c_cflag & OHFLOW))
	{
		for (cnt = 0; cnt < NUM_DEVS; cnt++)
			tti(&dev->vdev[cnt].tty, (msr & OMAP_MSR_CTS) ? TTI_OHW_CONT : TTI_OHW_STOP);
	}

	// Attach the resource manager
	for (cnt = 0; cnt < NUM_DEVS; cnt++)
	{
#ifdef WINBT
		dev->vdev[cnt].tty.io_devctlext = winbt_devctl;
#else
		dev->vdev[cnt].tty.io_devctlext = seromap_devctl;
#endif
		dev->vdev[cnt].tty.custom_event_handler = seromap_custom_event_handler;
		ttc(TTC_INIT_ATTACH, &dev->vdev[cnt].tty, 0);
	}

#ifdef WINBT
	omap_force_rts(dev, 1);
	omap_clock_disable(dev);
#endif

	strncpy(dev->bt_script_name, dip->bt_script_name, _POSIX_PATH_MAX);
	run_init_script(dev);

	return (dev);
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devc/seromap_hci/init.c $ $Rev: 772176 $")
#endif
