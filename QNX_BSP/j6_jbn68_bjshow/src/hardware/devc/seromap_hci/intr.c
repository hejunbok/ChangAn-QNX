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

/*
 * Process data in a line status register
 */
static int process_lsr(DEV_OMAP * dev, unsigned char lsr)
{
	unsigned key = 0;
	int rx_idx = (dev->rx_active != -1) ? dev->rx_active : 0;

	// Return immediately if no errors.
	if ((lsr & (OMAP_LSR_BI | OMAP_LSR_OE | OMAP_LSR_FE | OMAP_LSR_PE)) == 0)
		return (0);

	// Save the error as out-of-band data which can be retrieved via devctl().
	dev->vdev[rx_idx].tty.oband_data |= (lsr >> 1) & 0x0f;
	atomic_set(&dev->vdev[rx_idx].tty.flags, OBAND_DATA);

	if (lsr & OMAP_LSR_BI)
		key |= TTI_BREAK;
	else if (lsr & OMAP_LSR_OE)
		key |= TTI_OVERRUN;
	else if (lsr & OMAP_LSR_FE)
		key |= TTI_FRAME;
	else if (lsr & OMAP_LSR_PE)
		key |= TTI_PARITY;
	dev->flags |= RX_ERROR;

	return (key);
}

/*
 * Serial interrupt handler
 */
int do_interrupts(DEV_OMAP * dev)
{
	uint32_t status = 0;
	int cnt, i, something_happened = 1;
	unsigned char msr, lsr;
	unsigned iir;
	unsigned key = 0;
#ifdef WINBT
	unsigned ssr;
#endif
	uintptr_t *port = dev->port;

	do
	{
#ifdef WINBT
		if (dev->idle)
		{
			omap_clock_enable_isr(dev);
			omap_force_rts(dev, 0);

			// once we are in idle mode the only interrupt that can wake us up is from am CTS line change
			tti(&dev->vdev[0].tty, TTI_OHW_STOP);
			tti(&dev->vdev[1].tty, TTI_OHW_STOP);
			tti(&dev->vdev[2].tty, TTI_OHW_STOP);

			// start a spare timer for debouncing, if this timer actually
			// expires, then this was a CTS glitch, if the timer
			// is cleared by having data on the RX line, it will send up the
			// oband notification to wake up the host.
			dev->signal_oband_notification = 1;

			if (dev->vdev[0].tty.un.s.spare_tmr == 0)
			{
				atomic_set(&dev->vdev[0].tty.eflags, EVENT_TIMER_QUEUE);
				dev->vdev[0].tty.un.s.spare_tmr = 4;
				status |= 1;
			}
		}

		ssr = read_omap(port[OMAP_UART_SSR]);
		if (ssr & OMAP_SSR_WAKEUP_STS)
		{
			/* Clear the wake up interrupt */
			set_port(port[OMAP_UART_SCR], OMAP_SCR_WAKEUPEN, 0);
		}
#endif
		iir = read_omap(port[OMAP_UART_IIR]) & OMAP_II_MASK;

		switch (iir)
		{
			case OMAP_II_RX:					// Receive data
			case OMAP_II_RXTO:					// Receive data timeout
			case OMAP_II_LS:					// Line status change
				cnt = 0;
				while (((lsr = read_omap(port[OMAP_UART_LSR])) & OMAP_LSR_RXRDY) && cnt < FIFO_SIZE)
				{
					if (dev->rx_active != -1 && dev->vdev[dev->rx_active].rx_length > 0)
					{
						dev->rx_buffer[dev->rx_buffer_nbytes++] = read_omap(port[OMAP_UART_RHR]);
						key = process_lsr(dev, lsr);
						/* Push accumulated data block up to io-char and complete the pkt (reset rx_active)
						 * if any of these conditions occur:
						 *  1) Completed the HCI packet
						 *  2) Detected a receive error
						 */
						if (--dev->vdev[dev->rx_active].rx_length <= 0 || (dev->flags & RX_ERROR))
						{
							/* Only slog data if not running in an ISR */
							if (dev->isr == 0 && dev->vdev[dev->rx_active].tty.verbose > EVENT)
								display_data_bytes(dev->vdev[dev->rx_active].tty.name, dev->rx_buffer, dev->rx_buffer_nbytes, "RESPONSE");

							/* Only pass data up to io-char if CREAD is set */
							if (dev->vdev[dev->rx_active].tty.c_cflag & CREAD)
								status |= tti2(&dev->vdev[dev->rx_active].tty, dev->rx_buffer, dev->rx_buffer_nbytes, key);
							dev->rx_buffer_nbytes = 0;

							if (dev->flags & SCRIPT_MODE || (dev->flags & RX_ERROR))
							{
								atomic_set(&dev->vdev[dev->rx_active].tty.eflags, EVENT_CUSTOM);
							}
							dev->rx_active = -1;
						}
						else if (dev->rx_buffer_nbytes == dev->rx_buffer_size)	/* If we've filled our buffer then pass it up to io-char */
						{
							/* Only slog data if not running in an ISR */
							if (dev->isr == 0 && dev->vdev[dev->rx_active].tty.verbose > EVENT)
								display_data_bytes(dev->vdev[dev->rx_active].tty.name, dev->rx_buffer, dev->rx_buffer_nbytes, "RESPONSE");
							/* Only pass data up to io-char if CREAD is set */
							if (dev->vdev[dev->rx_active].tty.c_cflag & CREAD)
								status |= tti2(&dev->vdev[dev->rx_active].tty, dev->rx_buffer, dev->rx_buffer_nbytes, key);
							dev->rx_buffer_nbytes = 0;
						}
					}
					else
					{
						/* Discard zero length HCI packets */
						if (dev->rx_active != -1 && dev->vdev[dev->rx_active].rx_length == 0)
						{
							dev->rx_active = -1;
							dev->rx_buffer_nbytes = 0;
						}

						dev->rx_buffer[dev->rx_buffer_nbytes++] = read_omap(port[OMAP_UART_RHR]);
						if ((key = process_lsr(dev, lsr)) != EOK)
						{
							int rx_idx = (dev->rx_active != -1) ? dev->rx_active : 0;
							/* Set EVENT_CUSTOM to signal error (re-init BT chip at thread time) */
							atomic_set(&dev->vdev[rx_idx].tty.eflags, EVENT_CUSTOM);

#ifdef QCABT
							/* Line break is expected */
							if ((key == TTI_BREAK) && (dev->flags & SCRIPT_MODE))
							{
								/* Clear received bytes */
								dev->rx_buffer_nbytes = 0;
								dev->flags &= ~RX_ERROR;
							}
#endif
						}
						else
							status |= parse_rx_header(dev);
					}
					cnt++;
				}
#ifdef WINBT
				if (cnt && dev->signal_oband_notification)
				{

					// received data after a CTS wake up
					// notify the host that it's a valid CTS wakeup.
					dev->signal_oband_notification = 0;
					dev->vdev[dev->rx_active].tty.oband_data |= _OBAND_SER_MS;
					atomic_set(&dev->vdev[dev->rx_active].tty.flags, OBAND_DATA);
					atomic_set(&dev->vdev[dev->rx_active].tty.flags, EVENT_NOTIFY_OBAND);
					status |= 1;
				}

				if (cnt && dev->vdev[0].tty.un.s.spare_tmr)
				{
					// received data, clear spare timer
					dev->vdev[0].tty.un.s.spare_tmr = 0;
				}
#endif
				/* Give the system a chance to do something else */
				if (cnt >= FIFO_SIZE)
					something_happened = 0;
				break;

			case OMAP_II_TX:					// Transmit buffer empty
				// disable thr interrupt
				set_port(dev->port[OMAP_UART_IER], OMAP_IER_THR, 0);

				dev->vdev[0].tty.un.s.tx_tmr = 0;
				dev->vdev[1].tty.un.s.tx_tmr = 0;
				dev->vdev[2].tty.un.s.tx_tmr = 0;
				/* Send event to io-char, tto() will be processed at thread time */
				atomic_set(&dev->vdev[0].tty.flags, EVENT_TTO);
				status |= 1;
				break;

			case OMAP_II_MS:					// Modem change
				msr = read_omap(port[OMAP_UART_MSR]);

				for (i = 0; i < NUM_DEVS; i++)
				{
					if (msr & OMAP_MSR_DDCD)
						status |= tti(&dev->vdev[i].tty, (msr & OMAP_MSR_DCD) ? TTI_CARRIER : TTI_HANGUP);

					if ((msr & OMAP_MSR_DCTS) && (dev->vdev[i].tty.c_cflag & OHFLOW))
						status |= tti(&dev->vdev[i].tty, (msr & OMAP_MSR_CTS) ? TTI_OHW_CONT : TTI_OHW_STOP);
					/* OBAND notification of Modem status change */
					dev->vdev[i].tty.oband_data |= _OBAND_SER_MS;
					atomic_set(&dev->vdev[i].tty.flags, OBAND_DATA);
					atomic_set(&dev->vdev[i].tty.flags, EVENT_NOTIFY_OBAND);
				}
				status |= 1;
				break;

			case OMAP_II_NOINTR:				// No interrupt
				if (read_omap(port[OMAP_UART_SSR]) & OMAP_SSR_WAKEUP_STS)	// Wake up interrupt
				{
					set_port(port[OMAP_UART_SCR], OMAP_SCR_WAKEUPEN, 0);	// clear wakeup interrupt
					set_port(port[OMAP_UART_SCR], OMAP_SCR_WAKEUPEN, OMAP_SCR_WAKEUPEN);	// re-enable wakeup interrupt
				}
			default:
				something_happened = 0;
				break;
		}
	} while (something_happened);

	return (status);
}

int seromap_interrupt_event_handler(message_context_t * msgctp, int code, unsigned flags, void *handle)
{
	int i;
	uint32_t status = EOK;
	DEV_OMAP *dev = (DEV_OMAP *) handle;

	status = do_interrupts(dev);
	if (status)
	{
		for (i = 0; i < NUM_DEVS; i++)
			iochar_send_event(&dev->vdev[i].tty);
	}

	InterruptUnmask(dev->intr, dev->iid);
	return (EOK);
}

const struct sigevent *seromap_interrupt_handler(void *area, int id)
{
	int i;
	uint32_t status = EOK;
	DEV_OMAP *dev = area;
	struct sigevent *event = NULL;

	status = do_interrupts(dev);

	if (status)
	{
		for (i = 0; i < NUM_DEVS; i++)
		{
			if ((dev->vdev[i].tty.flags & EVENT_QUEUED) == 0)
			{
				event = &ttyctrl.event;
				dev_lock(&ttyctrl);
				ttyctrl.event_queue[ttyctrl.num_events++] = &dev->vdev[i].tty;
				atomic_set(&dev->vdev[i].tty.flags, EVENT_QUEUED);
				dev_unlock(&ttyctrl);
			}
		}
	}
	return (event);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devc/seromap_hci/intr.c $ $Rev: 765635 $")
#endif
