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
#include "proto.h"

int tto(TTYDEV * ttydev, int action, int arg1)
{
	virtual_dev_t *vdev = (virtual_dev_t *) ttydev;
	DEV_OMAP *dev = vdev->dev;
	const uintptr_t *port = dev->port;
	unsigned char c;
	int linestatus = 0;
	int idx, cnt, status = EOK;
#ifdef WINBT
	if (dev->idle)
	{
		/* Check the client lists for notify conditions */
		for (cnt = 0; cnt < NUM_DEVS; cnt++)
			status |= tto_checkclients(&dev->vdev[cnt].tty);
		return (status);
	}
#endif

	switch (action)
	{
		case TTO_STTY:
			// if (dev->driver_pmd.cur_mode == PM_MODE_ACTIVE)
			ser_stty(vdev);
			return (0);

		case TTO_CTRL:

			if (arg1 & _SERCTL_BRK_CHG)
				set_port(port[OMAP_UART_LCR], OMAP_LCR_BREAK, arg1 & _SERCTL_BRK ? OMAP_LCR_BREAK : 0);

			if (arg1 & _SERCTL_DTR_CHG)
				set_port(port[OMAP_UART_MCR], OMAP_MCR_DTR, arg1 & _SERCTL_DTR ? OMAP_MCR_DTR : 0);

			if (arg1 & _SERCTL_RTS_CHG)
			{
				if (dev->auto_rts_enable)
				{
					/* For auto-rts enable/disable RX & LS interrupts to assert/clear
					 * input flow control (the FIFO will automatically handle the RTS line)
					 */
					if (arg1 & _SERCTL_RTS)
					{
						int x, paged = 0;
						/* Only re-enable RX when all virtual devices can except data */
						for (x = 0; x < NUM_DEVS; x++)
						{
							if (dev->vdev[x].tty.flags & (IHW_PAGED | ISW_PAGED))
								paged = 1;
						}
						if (!paged)
							write_omap(port[OMAP_UART_IER], read_omap(port[OMAP_UART_IER]) | OMAP_IER_RHR | OMAP_IER_LS);
					}
					else
						write_omap(port[OMAP_UART_IER], read_omap(port[OMAP_UART_IER]) & ~(OMAP_IER_RHR | OMAP_IER_LS));
				}
				else
					set_port(port[OMAP_UART_MCR], OMAP_MCR_RTS, arg1 & _SERCTL_RTS ? OMAP_MCR_RTS : 0);
			}
#ifdef WINBT
			if (arg1 & _CTL_TIMED_CHG)
			{
				slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Turning clocks OFF due to CTS glitch\n", __FUNCTION__);
				dev->signal_oband_notification = 0;
				omap_clock_disable(dev);
			}
#endif
			return (0);

		case TTO_LINESTATUS:
			linestatus = (((read_omap(port[OMAP_UART_MSR]) << 8) | read_omap(port[OMAP_UART_MCR])) & 0x7003);
			/* Carrier Detect (CD) is based on BT init sequence being completed */
			if (dev->flags & HCI_CARRIER)
				linestatus |= _LINESTATUS_SER_CD;
			return (linestatus);

		case TTO_DATA:
		case TTO_EVENT:
			if (dev->flags & SCRIPT_MODE)
			{
				write_command(dev, dev->cmd_nbytes);
				return (0);
			}
			break;

		default:
			return (0);
	}

	/* Find active transmitting virtual device */
	if (dev->tx_active == -1 || dev->vdev[dev->tx_active].tx_length <= 0)
	{
		idx = dev->tx_active < 2 ? dev->tx_active + 1 : 0;
		dev->tx_active = -1;
		for (cnt = 0; dev->tx_active == -1 && cnt < NUM_DEVS; cnt++, idx = (idx < 2 ? idx + 1 : 0))
		{
			if (parse_tx_packet(&dev->vdev[idx]) > 0)
			{
				dev->tx_active = idx;
				if (dev->vdev[idx].tty.verbose > EVENT)
					display_data_bytes(dev->vdev[idx].tty.name, dev->vdev[idx].tty.obuf.tail, dev->vdev[idx].tx_length, "TX");
			}
		}
	}

	while (dev->tx_active != -1 && dev->vdev[dev->tx_active].tty.obuf.cnt > 0
		&& !(read_omap(port[OMAP_UART_SSR]) & OMAP_SSR_TXFULL))
	{
		vdev = &dev->vdev[dev->tx_active];

		/*
		 * If the OSW_PAGED_OVERRIDE flag is set then allow
		 * transmit of character even if output is suspended via
		 * the OSW_PAGED flag. This flag implies that the next
		 * character in the obuf is a software flow control
		 * charater (STOP/START).
		 * Note: tx_inject sets it up so that the contol
		 *       character is at the start (tail) of the buffer.
		 *
		 */

		if (vdev->tty.flags & (OHW_PAGED | OSW_PAGED) && !(vdev->tty.xflags & OSW_PAGED_OVERRIDE))
			break;

		/* Get character from obuf and do any output processing */
		dev_lock(&vdev->tty);
		c = tto_getchar(&vdev->tty);
		vdev->tx_length--;
		dev_unlock(&vdev->tty);

		/* Print the character */
		vdev->tty.un.s.tx_tmr = 3;			/* Timeout 3 */
		write_omap(port[OMAP_UART_THR], c);

		/* If we've completed the current HCI packet, then find next vdev
		 * that is ready to start transmitting.
		 */
		if (vdev->tx_length <= 0)
		{
			idx = dev->tx_active < 2 ? dev->tx_active + 1 : 0;
			dev->tx_active = -1;
			/* Find active transmitting virtual device */
			for (cnt = 0; dev->tx_active == -1 && cnt < NUM_DEVS; cnt++, idx = (idx < 2 ? idx + 1 : 0))
			{
				if (parse_tx_packet(&dev->vdev[idx]) > 0)
				{
					dev->tx_active = idx;
					if (dev->vdev[idx].tty.verbose > EVENT)
						display_data_bytes(dev->vdev[idx].tty.name, dev->vdev[idx].tty.obuf.tail, dev->vdev[idx].tx_length, "TX");
				}
			}
		}

		/* Clear the OSW_PAGED_OVERRIDE flag as we only want
		 * one character to be transmitted in this case.
		 */
		if (vdev->tty.xflags & OSW_PAGED_OVERRIDE)
		{
			atomic_clr(&vdev->tty.xflags, OSW_PAGED_OVERRIDE);
			break;
		}

	}

	/* If there is still data in the obuf and we are not in a flow
	 * controlled state then turn TX interrupts back on to notify us
	 * when the hardware is ready for more characters.
	 */
	if (dev->tx_active != -1 && dev->vdev[dev->tx_active].tty.obuf.cnt > 0
		&& !(vdev->tty.flags & (OHW_PAGED | OSW_PAGED)))
	{
		set_port(dev->port[OMAP_UART_IER], OMAP_IER_THR, OMAP_IER_THR);
	}

	/* Check the client lists for notify conditions */
	for (cnt = 0; cnt < NUM_DEVS; cnt++)
		status |= tto_checkclients(&dev->vdev[cnt].tty);
	return (status);
}

void ser_stty(virtual_dev_t * vdev)
{
	DEV_OMAP *dev = vdev->dev;
	unsigned char lcr = 0;
	unsigned char efr = 0;
	const uintptr_t *port = dev->port;
	unsigned brd = 0;
	int multiple, i;

#ifdef WINBT
	if (dev->idle)
	{
		slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Called while idle, ignoring\n", __FUNCTION__);
		return;
	}
#endif

	for (i = 0; i < NUM_DEVS; i++)
	{
		if (&dev->vdev[i] != vdev)
		{
			dev->vdev[i].tty.c_cflag = vdev->tty.c_cflag;
			dev->vdev[i].tty.baud = vdev->tty.baud;
		}
	}

	/* determine data bits */
	switch (vdev->tty.c_cflag & CSIZE)
	{
		case CS8:
			++lcr;
		case CS7:
			++lcr;
		case CS6:
			++lcr;
	}

	/* determine stop bits */
	if (vdev->tty.c_cflag & CSTOPB)
		lcr |= OMAP_LCR_STB2;
	/* determine parity bits */
	if (vdev->tty.c_cflag & PARENB)
		lcr |= OMAP_LCR_PEN;
	if ((vdev->tty.c_cflag & PARODD) == 0)
		lcr |= OMAP_LCR_EPS;

	if (vdev->tty.c_cflag & OHFLOW)
		efr = OMAP_EFR_AUTO_CTS;
	if (vdev->tty.c_cflag & IHFLOW && dev->auto_rts_enable)
		efr |= OMAP_EFR_AUTO_RTS;

	/* Apply EFR value if changed */
	if (dev->efr != efr)
	{
		/* Switch to Config mode B to access the Enhanced Feature Register (EFR) */
		write_omap(port[OMAP_UART_LCR], 0xbf);
		/* turn off S/W flow control, Config AUTO hw flow control, enable writes to MCR[7:5], FCR[5:4], and IER[7:4] */
		set_port(port[OMAP_UART_EFR], efr, efr);
		/* Switch back to operational mode */
		write_omap(port[OMAP_UART_LCR], 0);
		/* Restore LCR config values */
		write_omap(port[OMAP_UART_LCR], lcr);
		dev->lcr = lcr;
		dev->efr = efr;
	}

	if (vdev->tty.baud != dev->baud)
	{
		/* Get acces to Divisor Latch registers */
		write_omap(port[OMAP_UART_LCR], OMAP_LCR_DLAB);

#ifdef OMAP5910
		/*
		 * Oscillator frequency is fixed at 12MHz. This normally wouldn't allow a
		 * 115200 baud rate, since divisor = Fin / (16 * baud), which, for 115200, = 6.5,
		 * which is out of tolerance. There is a special register with a bit which, when
		 * set, automatically enables a 6.5 divisor value in hardware, and the DLH / DLL
		 * registers can just be programmed with a 1.
		 */

		if (vdev->tty.baud == DEFAULT_BAUD_RATE)
		{
			brd = 1;
			write_omap(port[OMAP_UART_DLL], brd);
			write_omap(port[OMAP_UART_DLH], (brd >> 8) & 0xff);
			write_omap(port[OMAP_UART_OSC_12M_SEL], 0x01);
		}
		else
		{
			brd = (vdev->tty.baud == 0)? 0 : (12000000 / (16 * vdev->tty.baud));
			write_omap(port[OMAP_UART_DLL], brd);
			write_omap(port[OMAP_UART_DLH], (brd >> 8) & 0xff);
			write_omap(port[OMAP_UART_OSC_12M_SEL], 0x00);
		}
#else
		/* MODE_SELECT must be DISABLED before modifying the DLL and DLH registers */
		set_port(port[OMAP_UART_MDR1], OMAP_MDR1_MODE_MSK, OMAP_MDR1_MODE_DISABLE);	/* Disable UART */
		run_errata_i202(dev);

		multiple = (vdev->tty.baud > 230400 && vdev->tty.baud != 3000000) ? 13 : 16;

		brd = (vdev->tty.baud == 0)? 0 : (48000000 / (multiple * vdev->tty.baud));
		write_omap(port[OMAP_UART_DLL], brd);
		write_omap(port[OMAP_UART_DLH], (brd >> 8) & 0xff);
		if (multiple == 13)
			set_port(port[OMAP_UART_MDR1], OMAP_MDR1_MODE_MSK, OMAP_MDR1_MODE_13X);	/* Enable UART in 13x mode */
		else
			set_port(port[OMAP_UART_MDR1], OMAP_MDR1_MODE_MSK, OMAP_MDR1_MODE_16X);	/* Enable UART in 16x mode */
		run_errata_i202(dev);
#endif
		write_omap(port[OMAP_UART_LCR], lcr);	 /* Restore LCR config values */
		dev->lcr = lcr;
		dev->baud = vdev->tty.baud;
		dev->brd = brd;
	}

	/* Apply LCR value if changed */
	if (dev->lcr != lcr)
	{
		write_omap(port[OMAP_UART_LCR], lcr);
		dev->lcr = lcr;
	}

	if (dev->kick_maxim)
	{
		unsigned kick_cnt = dev->baud / 10000;
		if (kick_cnt)
			dev->kick_maxim = kick_cnt;
	}

}


int drain_check(TTYDEV * ttydev, uintptr_t * count)
{
	TTYBUF *bup = &ttydev->obuf;
	DEV_OMAP *dev = (DEV_OMAP *) ttydev;
	const uintptr_t *port = dev->port;

#ifdef WINBT
	if (dev->idle)
	{
		slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Called while idle, ignoring\n", __FUNCTION__);
		return 0;
	}
#endif

	// if the device has DRAINED, return 1
	if (bup->cnt == 0 && (read_omap(port[OMAP_UART_LSR]) & OMAP_LSR_TSRE))
		return 1;

	// if the device has not DRAINED, set a timer based on 50ms counts
	// wait for the time it takes for one character to be transmitted
	// out the shift register. We do this dynamically since the
	// baud rate can change.
	if (count != NULL)
		*count = (ttydev->baud == 0) ? 0 : ((IO_CHAR_DEFAULT_BITSIZE * 20) / ttydev->baud) + 1;

	return 0;
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devc/seromap_hci/tto.c $ $Rev: 765635 $")
#endif
