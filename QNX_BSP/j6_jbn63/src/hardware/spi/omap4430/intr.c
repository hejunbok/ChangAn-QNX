/*
 * $QNXLicenseC:
 * Copyright 2009, QNX Software Systems.
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


#include "omap4430spi.h"

#include <sys/neutrino.h>

static intrspin_t spi_lock;

/*
 * We use the same buffer for transmit and receive
 * For exchange, that's exactly what we wanted
 * For Read, it doesn't matter what we write to SPI, so we are OK.
 * For transmit, the receive data is put at the buffer we just transmitted, we are still OK.
 */

static const struct sigevent *spi_intr(void *area, int id)
{
    int            channel_id = 0;
    uint32_t       reg_value = 0;
    uint32_t       temp_irq_enable;
    uintptr_t      base;
    int            i, expected;
    omap4430_spi_t *dev = area;
    base = dev->vbase;
 
    InterruptLock(&spi_lock); 
    temp_irq_enable = in32( base + OMAP4430_MCSPI_IRQ_ENABLE_OFFSET ); 
    reg_value       = in32( base + OMAP4430_MCSPI_IRQ_STATUS_OFFSET );

    /* Disable all interrupts */
    out32( base + OMAP4430_MCSPI_IRQ_ENABLE_OFFSET, 0 );

    /* The code below detects on which channel the interrupt
     * has occured.
     */
    if(reg_value & OMAP4430_MCSPI_IRQ_RESET_CHANNEL1)
        channel_id = OMAP4430_SPI_MASTER_CHANNEL_ONE;
    else {
        if(reg_value & OMAP4430_MCSPI_IRQ_RESET_CHANNEL2)
            channel_id = OMAP4430_SPI_MASTER_CHANNEL_TWO;
        else
        {
            if(reg_value & OMAP4430_MCSPI_IRQ_RESET_CHANNEL3)
                channel_id = OMAP4430_SPI_MASTER_CHANNEL_THREE;
            else 
                channel_id = OMAP4430_SPI_MASTER_CHANNEL_FOUR;
        }
    }
    if(channel_id >= NUM_OF_SPI_DEVS) {
        out32(base + OMAP4430_MCSPI_IRQ_STATUS_OFFSET, reg_value);
        out32(base + OMAP4430_MCSPI_IRQ_ENABLE_OFFSET, temp_irq_enable);
        InterruptUnlock(&spi_lock); 
        return NULL;
    }

    /* The remaining bytes are read from the omap4430_xfer function */
    if (reg_value & INTR_TYPE_EOWKE) {

        /* Clear the interrupt */
        out32(base + OMAP4430_MCSPI_IRQ_STATUS_OFFSET, INTR_TYPE_EOWKE);

        /* do NOT restore interrupts in this case. The IRQ handler is DONE! */
        InterruptUnlock(&spi_lock); 
        return (&dev->spievent);
    }

    /* Is Rx Full? also implies that EOW event is not raised and more data needs to be transfered */
    if(reg_value & (INTR_TYPE_RX0_FULL << (channel_id * OMAP4430_INTERRUPT_BITS_PER_SPI_CHANNEL))) {

		out32(base + OMAP4430_MCSPI_IRQ_STATUS_OFFSET, INTR_TYPE_RX0_FULL << (channel_id * OMAP4430_INTERRUPT_BITS_PER_SPI_CHANNEL));

		/* Bytes available in the Rx FIFO */
        expected = OMAP4430_SPI_RXFIFOLEN * dev->dlen;
        for(i = 0 ; i < expected; i += dev->dlen, dev->rlen += dev->dlen) {
            switch(dev->dlen) {
                case 1:
                    dev->pbuf[dev->rlen] = in32(base + OMAP4430_MCSPI_CH1_RX_BUFFER_OFFSET + (OMAP4430_SPI_DEVICE_OFFSET * channel_id));
                    break;
                case 2:
                    *(uint16_t *)(&dev->pbuf[dev->rlen]) = in32(base + OMAP4430_MCSPI_CH1_RX_BUFFER_OFFSET + (OMAP4430_SPI_DEVICE_OFFSET * channel_id));
                    break;
                default:
                    *(uint32_t *)(&dev->pbuf[dev->rlen]) = in32(base + OMAP4430_MCSPI_CH1_RX_BUFFER_OFFSET + (OMAP4430_SPI_DEVICE_OFFSET * channel_id));
                    break;
            }
        }

		/* Bytes need to be transfered */
		expected = min(OMAP4430_SPI_TXFIFOLEN * dev->dlen, dev->xlen - dev->tlen);
		if (expected) {
			for(i = 0; i < expected; i += dev->dlen) {
				switch(dev->dlen) {
					case 1:
						out32(base + OMAP4430_MCSPI_CH1_TX_BUFFER_OFFSET + (OMAP4430_SPI_DEVICE_OFFSET * channel_id), dev->pbuf[dev->tlen + i]);
						break;
					case 2:
						out32(base + OMAP4430_MCSPI_CH1_TX_BUFFER_OFFSET + (OMAP4430_SPI_DEVICE_OFFSET * channel_id), *(uint16_t *)(&dev->pbuf[dev->tlen + i]));
						break;
					default:
						out32(base + OMAP4430_MCSPI_CH1_TX_BUFFER_OFFSET + (OMAP4430_SPI_DEVICE_OFFSET * channel_id), *(uint32_t *)(&dev->pbuf[dev->tlen + i]));
						break;
				}
			}

			dev->tlen += expected;
		}
    }

    /* Re-enable interrupts */
    out32(base + OMAP4430_MCSPI_IRQ_ENABLE_OFFSET, temp_irq_enable);

    InterruptUnlock(&spi_lock); 
    return NULL;
}


int omap4430_attach_intr(omap4430_spi_t *omap4430)
{
    if ((omap4430->chid = ChannelCreate(_NTO_CHF_DISCONNECT | _NTO_CHF_UNBLOCK)) == -1)
        return -1;

    if ((omap4430->coid = ConnectAttach(0, 0, omap4430->chid, _NTO_SIDE_CHANNEL, 0)) == -1) 
        goto fail0;

    omap4430->spievent.sigev_notify   = SIGEV_PULSE;
    omap4430->spievent.sigev_coid     = omap4430->coid;
    omap4430->spievent.sigev_code     = OMAP4430_SPI_EVENT;
    omap4430->spievent.sigev_priority = OMAP4430_SPI_PRIORITY;

    ThreadCtl( _NTO_TCTL_IO, 0 ); // required for InterruptLock/Unlock abilities
    memset( &spi_lock, 0, sizeof(spi_lock));
    /*
     * Attach SPI interrupt
     */
    omap4430->iid_spi = InterruptAttach(omap4430->irq_spi, spi_intr, omap4430, sizeof(omap4430_spi_t), _NTO_INTR_FLAGS_TRK_MSK);

    if (omap4430->iid_spi != -1)
        return 0;

    ConnectDetach(omap4430->coid);
fail0:
    ChannelDestroy(omap4430->chid);

    return -1;
}


int omap4430_sdma_attach_intr(omap4430_spi_t *omap4430)
{
    int rx_idx = omap4430->sdma_rx_chid ;

    if ((omap4430->sdma_coid = ConnectAttach(0, 0, omap4430->chid, _NTO_SIDE_CHANNEL, 0)) == -1) 
        goto fail0;
    
    omap4430->sdmaevent.sigev_notify   = SIGEV_PULSE;
    omap4430->sdmaevent.sigev_coid     = omap4430->sdma_coid;
    omap4430->sdmaevent.sigev_code     = OMAP4430_SDMA_EVENT;
    omap4430->sdmaevent.sigev_priority = OMAP4430_SPI_PRIORITY+1;

    omap4430->iid_sdma = InterruptAttachEvent(omap4430->irq_sdma+rx_idx, &omap4430->sdmaevent, _NTO_INTR_FLAGS_TRK_MSK);
    
    if (omap4430->iid_sdma != -1)
        return 0;

    ConnectDetach(omap4430->sdma_coid);
fail0:

    return -1;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/spi/omap4430/intr.c $ $Rev: 716138 $")
#endif
