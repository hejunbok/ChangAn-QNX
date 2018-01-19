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
#include "clock_toggle.h"
#include "context_restore.h"

//#define DEBUG
#define DEBUGLEN 4

static char *omap4430_opts[] = {
	"base",			/* Base address for this CSPI controller */
	"bitrate",		/* defines teh bitrate to to assigned to the spi */
	"clock",		/* defined the value of the clock source of the SPI */
	"channel",		/* defines the channel connected */
	"irq",			/* IRQ for this CSPI intereface */
	"force",		/* Force The default CS level */
	"num_cs",		/* number of devices supported */
	"sdma",			/* Disable/enable SDMA for SPI */ 
	"cs_delay",		/* Disable/enable 1 1/2 clock CS to data delay */
	"ctxbase", 		/*save_restore context base register*/
	NULL
};

spi_funcs_t spi_drv_entry = {
	sizeof(spi_funcs_t),
	omap4430_init,		/* init() */
	omap4430_dinit,		/* fini() */
	omap4430_drvinfo,	/* drvinfo() */
	omap4430_devinfo,	/* devinfo() */
	omap4430_setcfg,	/* setcfg() */
	omap4430_xfer,		/* xfer() */
	omap4430_dmaxfer	/* dma_xfer() */
};

/*
 * Note:
 * The devices listed are just examples, users should change
 * this according to their own hardware spec.
 */
static spi_devinfo_t devlist[4] = {
	{
		0x00,				// Device ID, for SS0
		"MICREL8851SNL",	// Description
		{ 
			(8 | SPI_MODE_CKPOL_HIGH), 	// data length 8bit, MSB
			24000000			 		// MICREL8851SNL will function at 24000000
		},
	},
	{
		0x01,			// Device ID, for SS1
		"TSC-2046",	 	// Description
		{ 
			(8 | SPI_MODE_CKPOL_HIGH), 	// data length 8bit, MSB
			125000						// TSC-2046 will fucntion at 125000
		},
	},
	{
		0x02,			// Device ID, for SS2
		"TSC-2046",	 	// Description
		{ 
			(8 | SPI_MODE_CKPOL_HIGH), // data length 8bit, MSB
			125000				// TSC-2046 will fucntion at 125000
		},
	},
	{
		0x03,			// Device ID, for SS3
		"TSC-2046",	 	// Description
		{ 
			(8 | SPI_MODE_CKPOL_HIGH), // data length 8bit, MSB
			125000				// TSC-2046 will fucntion at 125000
		},
	}
};

static uint32_t devctrl[NUM_OF_SPI_DEVS];

void set_port(unsigned port, unsigned mask, unsigned data)
{
	uint32_t c;

	c = read_omap(port);
	write_omap(port, (c & ~mask) | (data & mask));
}

static int omap4430_options(omap4430_spi_t *spi, char *optstring)
{
	int	opt, rc = 0;
	char *options, *freeptr, *c, *value;

	if (optstring == NULL)
		return 0;

	freeptr = options = strdup(optstring);

	while (options && *options != '\0') {
		c = options;
		if ((opt = getsubopt(&options, omap4430_opts, &value)) == -1)
			goto error;

		switch (opt) {
			case 0:
				spi->pbase = strtoull(value, 0, 0);
				continue;
			case 1:
				spi->bitrate = strtoul(value, NULL, 0);
				continue;	
			case 2:
				spi->clock = strtoul(value, NULL, 0);
				continue;	
			case 3:
				spi->channel_num = strtoul(value, NULL, 0);
				continue;	
			case 4:
				spi->irq_spi = strtoul(value, 0, 0);
				continue;
			case 5:
				spi->force = strtoul(value, 0, 0);
				continue;	
			case 6:
				spi->num_cs = strtoul(value, 0, 0);
				continue;	
			case 7:
				spi->sdma = strtoul(value, 0, 0);
				continue;	
			case 8:
				spi->cs_delay = strtoul(value, 0, 0);
				continue;
			case 9:
				spi->mcspi_context_paddr = strtoul(value, 0, 0);
				continue;
		}
error:
		fprintf(stderr, "omap4430-spi: unknown option %s", c);
		rc = -1;
	}

	free(freeptr);
	return rc;
}

void *omap4430_init(void *hdl, char *options)
{
	omap4430_spi_t	*dev;
	uintptr_t		base;
	int			 	i;
	uint32_t		reg;	

	dev = calloc(1, sizeof(omap4430_spi_t));

	if (dev == NULL)
		return NULL;
		
	/* Set defaults */
	dev->pbase = OMAP4430_SPI1_BASE;
	dev->spi_int = OMAP4430_SPI_MASTER_TWO;
	dev->irq_spi = OMAP4430_SPI_SPI1_INTR;
	dev->clock = OMAP4430_SPI_INPUT_CLOCK;
	dev->channel_num = 1;
	dev->force = 0;
	dev->sdmapbase = DMA4_BASE_ADDR;
	dev->irq_sdma = SDMA_BASE_IRQ;	/* We use interrupt of receive channel */
	dev->sdma = 0;
	dev->num_cs = 1;
	dev->cs_delay = 1;

	if (omap4430_options(dev, options))
		goto fail0;
	
	/*
	 * Map in SPI registers
	*/
	if ((base = mmap_device_io(OMAP4430_SPI_REGLEN, dev->pbase)) == (uintptr_t)MAP_FAILED)
		goto fail0;

	dev->vbase = base;

	if (omap_reg_map_init(dev) == -1) {
		goto fail1;
	}

	if (-1 == omap_clock_toggle_init(dev)) {
		goto fail1;
	}

	if (-1 == context_restore_init(dev)) {
		goto fail1;
	}

	omap_clock_enable(dev);

	/* Reset the SPI interface
	 * and wait for the reset to complete
	 */
	set_port(base + OMAP4430_MCSPI_SYS_CONFIG, OMAP4430_MCSPI_CONFIG_SOFT_RESET, OMAP4430_MCSPI_CONFIG_SOFT_RESET);
	while ( !(in32(base + OMAP4430_MCSPI_SYS_CONFIG) & 1)) {
		nanospin_ns(20);
	}

	/* Set Master mode -- single channel mode */
	out32((base + OMAP4430_MCSPI_MODCTRL_OFFSET), (in32(base + OMAP4430_MCSPI_MODCTRL_OFFSET) & OMAP4430_MCSPI_MODCTRL_MASTER_SEL) | OMAP4430_MCSPI_MODCTRL_SINGLE_CH_SEL);
	 
	/*
	 * Calculate all device configuration here
	 */
	for (i = 0; i < dev->num_cs; i++) {
		devctrl[i] = omap4430_cfg(dev, &devlist[i].cfg);

		if(dev->force){
			/* if we need to set the default CSx level to other than defaul low, we need to kick it*/
			out32((base + OMAP4430_MCSPI_CH1_CONFIG_OFFSET + 0x14 * i), devctrl[i] | SPI_COMM_TX_RX << 12);
			set_port(base + OMAP4430_MCSPI_CH1_CTRL_OFFSET + 0x14 * i, OMAP4430_MCSPI_CHANNEL_ENABLE, OMAP4430_MCSPI_CHANNEL_ENABLE);
			/* force CS */
			set_port((base + OMAP4430_MCSPI_CH1_CONFIG_OFFSET + 0x14 * i), OMAP4430_MCSPI_FORCE_MODE_ONE, OMAP4430_MCSPI_FORCE_MODE_ONE);
			delay(1);
			/*un force CS */
			set_port((base + OMAP4430_MCSPI_CH1_CONFIG_OFFSET + 0x14 * i),OMAP4430_MCSPI_FORCE_MODE_ONE, 0);
			set_port(base + OMAP4430_MCSPI_CH1_CTRL_OFFSET + 0x14 * i, OMAP4430_MCSPI_CHANNEL_ENABLE, 0);
		}
	}
	
	/* disable interrupts */
	out32(base + OMAP4430_MCSPI_IRQ_ENABLE_OFFSET, 0);	

	/*
	 * Attach SPI interrupt
	 */
	if (omap4430_attach_intr(dev)) {
		omap_clock_disable(dev);
		goto fail2;
	}
	
	dev->spi.hdl = hdl;

	/* Clear the appropriate Interrupts if any */
	reg = in32(base + OMAP4430_MCSPI_IRQ_STATUS_OFFSET);
	out32((base + OMAP4430_MCSPI_IRQ_STATUS_OFFSET), reg);

	if (dev->sdma) {
		if (omap4430_init_sdma(dev))
			dev->sdma = 0;

		/*
		 * Attach SDMA interrupt
		 */
		if (dev->sdma && omap4430_sdma_attach_intr(dev)) {
			omap_clock_disable(dev);
			goto fail3;
		}
	}

	context_restore_save(dev);
	omap_clock_disable(dev);
	return dev;

fail3:
	munmap_device_memory (dev->dma4, sizeof (dma4_t));
fail2:
	context_restore_fini(dev);
fail1:
	munmap_device_io(dev->vbase, OMAP4430_SPI_REGLEN);
fail0:
	free(dev);
	return NULL;
}

void omap4430_setup(omap4430_spi_t *dev, uint32_t device)
{
	uintptr_t	base = dev->vbase;
	uint32_t	id;

	id = device & SPI_DEV_ID_MASK;
	omap_clock_enable(dev);
	out32((base + OMAP4430_MCSPI_CH1_CONFIG_OFFSET + 0x14 * id), (SPI_COMM_TX_RX << 12) | devctrl[id]);
	omap_clock_disable(dev);

	if (dev->sdma) {
		switch (dev->channel_num) {
			case 1:
				switch (id) {
					case 0:
						dev->spi_tx_dma_req = S_DMA_34;
						dev->spi_rx_dma_req = S_DMA_35;
						break;
					case 1:
						dev->spi_tx_dma_req = S_DMA_36;
						dev->spi_rx_dma_req = S_DMA_37;
						break;
					case 2:
						dev->spi_tx_dma_req = S_DMA_38;
						dev->spi_rx_dma_req = S_DMA_39;
						break;
					case 3:
						dev->spi_tx_dma_req = S_DMA_40;
						dev->spi_rx_dma_req = S_DMA_41;
						break;
					default:
						break;
				}							 
				break;
			case 2:
				switch (id) {
					case 0:
						dev->spi_tx_dma_req = S_DMA_42;
						dev->spi_rx_dma_req = S_DMA_43;
						break;
					case 1:
						dev->spi_tx_dma_req = S_DMA_44;
						dev->spi_rx_dma_req = S_DMA_45;
						break;
					default:
						break;
				}							 
				break;
			case 3:
				switch (id) {
					case 0:
						dev->spi_tx_dma_req = S_DMA_14;
						dev->spi_rx_dma_req = S_DMA_15;
						break;
					case 1:
						dev->spi_tx_dma_req = S_DMA_22;
						dev->spi_rx_dma_req = S_DMA_23;
						break;
					default:
						break;
				}							 
				break;
			case 4:
				dev->spi_tx_dma_req = S_DMA_69;
				dev->spi_rx_dma_req = S_DMA_70;
				break;
			default:
				break;
		}
	}
}

void omap4430_dinit(void *hdl)
{
	omap4430_spi_t	*dev = hdl;
	
	/*
	 * unmap the register, detach the interrupt
	 */
	InterruptDetach(dev->iid_spi);
	munmap_device_io(dev->vbase, OMAP4430_SPI_REGLEN);
	
	if (dev->clkctrlvbase) {
		munmap_device_io(dev->clkctrlvbase, 4);
	}

	/*
	 * Disable SDMA
	 */
	if (dev->sdma) {
		omap4430_sdma_disablespi(dev);
		InterruptDetach(dev->iid_sdma);
		omap4430_sdma_detach (dev);
		munmap_device_memory(dev->dma4, sizeof (dma4_t));
		munmap(dev->dmabuf, OMAP4430_SPI_MAXDMALEN);
		ConnectDetach(dev->sdma_coid);
	}
	ConnectDetach(dev->coid);
	ChannelDestroy(dev->chid);
	
	free(hdl);
}

int omap4430_drvinfo(void *hdl, spi_drvinfo_t *info)
{
	info->version = (SPI_VERSION_MAJOR << SPI_VERMAJOR_SHIFT) | (SPI_VERSION_MINOR << SPI_VERMINOR_SHIFT) | (SPI_REVISION << SPI_VERREV_SHIFT);
	strcpy(info->name, "OMAP4430 SPI");
	info->feature = SPI_FEATURE_DMA;	/* DMA supported, buffer address alignment is 1 */
	
	return (EOK);
}

int omap4430_setcfg(void *hdl, uint16_t device, spi_cfg_t *cfg)
{
	uint32_t	control;
	omap4430_spi_t	*dev = hdl;

	if (device >= dev->num_cs)
		return (EINVAL);

	memcpy(&devlist[device].cfg, cfg, sizeof(spi_cfg_t));

	control = omap4430_cfg(hdl, &devlist[device].cfg);
	if (control == 0)
		return (EINVAL);

	devctrl[device] = control;

	return (EOK);
}

int omap4430_devinfo(void *hdl, uint32_t device, spi_devinfo_t *info)
{
	omap4430_spi_t	*dev = hdl;
	int	id = device & SPI_DEV_ID_MASK;

	if (device & SPI_DEV_DEFAULT) {
		/*
		 * Info of this device
		 */
		if (id >= 0 && id < dev->num_cs)
			memcpy(info, &devlist[id], sizeof(spi_devinfo_t));
		else
			return (EINVAL);
	}
	else {
		/*
		 * Info of next device 
		 */
		if (id == SPI_DEV_ID_NONE)
			id = -1;
		if (id < (dev->num_cs - 1))
			memcpy(info, &devlist[id + 1], sizeof(spi_devinfo_t));
		else
			return (EINVAL);
	}
	
	return (EOK);
}

void *omap4430_xfer(void *hdl, uint32_t device, uint8_t *buf, int *len)
{
	omap4430_spi_t	*dev = hdl;
	uintptr_t		base = dev->vbase;
	uint32_t		id;
	int			 	i;
	int			 	timeout, expected;
	uint32_t		reg_value = 0;

	id = device & SPI_DEV_ID_MASK;
	if (id >= dev->num_cs) {
		*len = -1;
		return buf;
	}

	dev->xlen = *len;
	dev->dlen = ((devlist[id].cfg.mode & SPI_MODE_CHAR_LEN_MASK) + 7) >> 3;

	/*
	 * Cannot set more than 64KB of data at one time
	 * due to the limitation of WCNT field in MCSPI_XFERLEVEL register
	 */
	if(dev->xlen >= (64 * 1024))
	{
		*len = -1;
		return buf;
	}

	/* The transfer len must be integer multiple of the word width */
	if (dev->xlen % dev->dlen) {
		*len = -1;
		return buf;
	}

	dev->rlen = 0;
	dev->tlen = min(OMAP4430_SPI_TXFIFOLEN * dev->dlen, dev->xlen);
	dev->pbuf = buf;

	
	// Estimate transfer time in us... The calculated dtime is only used for
	// the timeout, so it doesn't have to be that accurate.  At higher clock
	// rates, a calculated dtime of 0 would mess-up the timeout calculation, so
	// round up to 1 us
	dev->dtime = dev->dlen * 1000 * 1000 / devlist[id].cfg.clock_rate;
	if (dev->dtime == 0)
		dev->dtime = 1;

	omap_clock_enable(dev);

	omap4430_setup(dev, device);

	/* force CS */
	set_port((base + OMAP4430_MCSPI_CH1_CONFIG_OFFSET + 0x14 * id),OMAP4430_MCSPI_FORCE_MODE_ONE, OMAP4430_MCSPI_FORCE_MODE_ONE);

	/* set FIFO */
	set_port((base + OMAP4430_MCSPI_CH1_CONFIG_OFFSET + 0x14 * id), OMAP4430_MCSPI_FFER | OMAP4430_MCSPI_FFEW, OMAP4430_MCSPI_FFER | OMAP4430_MCSPI_FFEW);

	/*
	 * Set FIFO transfer level
	 * we rely on EOW interrupt to indicate the end of the tranfer
	 * RXx_FULL interrupt as an indication to fill more data to the the Tx FIFO
	 */
	out32(base + OMAP4430_MCSPI_XFERLEVEL_OFFSET, ((dev->xlen / dev->dlen) << 16) | ((OMAP4430_SPI_RXFIFOLEN * dev->dlen - 1) << 8));

	/* Clear any pending interrupts */
	out32(base + OMAP4430_MCSPI_IRQ_STATUS_OFFSET, OMAP4430_MCSPI_IRQ_RESET_CHANNEL(id));

	/* Configue the SPI control register to enable the corresponding channel of the SPI */
	set_port(base + OMAP4430_MCSPI_CH1_CTRL_OFFSET + 0x14 * id, OMAP4430_MCSPI_CHANNEL_ENABLE, OMAP4430_MCSPI_CHANNEL_ENABLE);

	/* start the data transmit.....this happens by writing data to
	* the corresponding transmit register. This module has been 
	* designed for Transmit/Recieve Mode. This part will change
	* according to the design. 
	*/
#ifdef DEBUG
	if (dev->xlen > DEBUGLEN) {
		fprintf(stderr,"%s: Xfer Packet length = %2.2x\nTx: ",__FUNCTION__,dev->xlen);
	}
#endif

	/* Enable Interrupts */
	out32(base + OMAP4430_MCSPI_IRQ_ENABLE_OFFSET, INTR_TYPE_EOWKE | ((INTR_TYPE_RX0_FULL) << (id * OMAP4430_INTERRUPT_BITS_PER_SPI_CHANNEL)));

	for(i = 0; i < dev->tlen; i += dev->dlen) {
		switch(dev->dlen) {
			case 1:
				out32(base + OMAP4430_MCSPI_CH1_TX_BUFFER_OFFSET + 0x14 * id, dev->pbuf[i]);
				break;
			case 2:
				out32(base + OMAP4430_MCSPI_CH1_TX_BUFFER_OFFSET + 0x14 * id, *(uint16_t *)(&dev->pbuf[i]));
				break;
			case 3:
			case 4:
				out32(base + OMAP4430_MCSPI_CH1_TX_BUFFER_OFFSET + 0x14 * id, *(uint32_t *)(&dev->pbuf[i]));
				break;
		}
#ifdef DEBUG
		if (dev->xlen > DEBUGLEN) {
			fprintf(stderr,"%2.2x",dev->pbuf[i]);
			if (i % 2 == 0) { fprintf (stderr," ");}
		}
#endif
	}

#ifdef DEBUG
	if (dev->xlen > DEBUGLEN) {
		fprintf(stderr,"\nRx: %d", dev->xlen);
	}
#endif

	/*
	* Wait for exchange to finish
	*/
	if (omap4430_wait(dev, dev->xlen * 10)) {
		fprintf(stderr, "OMAP4430 SPI: XFER Timeout!!!\n");
		dev->rlen = -1;
	}

	/* Read the last spi words when EOW interrupt is raised */
	if(dev->rlen < dev->xlen && dev->rlen != -1) {
		reg_value = in32(base + OMAP4430_MCSPI_CH1_STATUS_OFFSET + (OMAP4430_SPI_DEVICE_OFFSET * id));
		timeout = 1000;
		while( timeout-- && ((reg_value & OMAP4430_MCSPI_CH_RX_REG_FULL) == 0)) {
			nanospin_ns(100);
			reg_value = in32(base + OMAP4430_MCSPI_CH1_STATUS_OFFSET + (OMAP4430_SPI_DEVICE_OFFSET * id));
		}
		
		if(timeout <= 0) {
			dev->rlen = -1;
		} else {
			// last words to read from buffer
			expected = dev->xlen - dev->rlen;
			for(i = 0; i < expected; ) {
				switch (dev->dlen)
				{
				case 1:
					dev->pbuf[dev->rlen++] = in32(base + OMAP4430_MCSPI_CH1_RX_BUFFER_OFFSET + (OMAP4430_SPI_DEVICE_OFFSET * id));
					i++;
					break;
				case 2:
					*(uint16_t *)(&dev->pbuf[dev->rlen]) = in32(base + OMAP4430_MCSPI_CH1_RX_BUFFER_OFFSET + (OMAP4430_SPI_DEVICE_OFFSET * id));
					dev->rlen += 2;
					i += 2;
					break;
				case 3:
				case 4:
					*(uint32_t *)(&dev->pbuf[dev->rlen]) = in32(base + OMAP4430_MCSPI_CH1_RX_BUFFER_OFFSET + (OMAP4430_SPI_DEVICE_OFFSET * id));
					dev->rlen += 4;
					i += 4;
					break;
				}
			}
		}
	}

	/* disable the channel */
	set_port(base + OMAP4430_MCSPI_CH1_CTRL_OFFSET + 0x14 * id, OMAP4430_MCSPI_CHANNEL_ENABLE, 0);

	/* disable interrupts */
	out32(base + OMAP4430_MCSPI_IRQ_ENABLE_OFFSET, 0);	

	/* un-force CS */
	set_port(base + OMAP4430_MCSPI_CH1_CONFIG_OFFSET + 0x14 * id,OMAP4430_MCSPI_FORCE_MODE_ONE, 0);
	set_port(base + OMAP4430_MCSPI_CH1_CONFIG_OFFSET + 0x14 * id,OMAP4430_MCSPI_FFER | OMAP4430_MCSPI_FFEW, 0);
	out32(base + OMAP4430_MCSPI_XFERLEVEL_OFFSET, 0);

	*len = dev->rlen;

#ifdef DEBUG
	for(i=0; i<min(16,dev->rlen); i++) {
		if (dev->xlen>DEBUGLEN) {
			fprintf(stderr,"%2.2x",dev->pbuf[i]);
			if (i%2 == 0){ fprintf (stderr," ");}
		}
	}	
	if (dev->xlen>DEBUGLEN) {
		fprintf(stderr,"%d\n",dev->xlen);
	}
#endif

	omap_clock_disable(dev);
	return buf;
}

int omap4430_dmaxfer(void *hdl, uint32_t device, spi_dma_paddr_t *paddr, int len)
{
	omap4430_spi_t	*dev = hdl;
	int	id = device & SPI_DEV_ID_MASK;
	uintptr_t	base = dev->vbase;

	/* Is the SDMA disabled? */
	if (dev->sdma == 0 || id >= dev->num_cs)
		return -1;

	dev->dlen = ((devlist[id].cfg.mode & SPI_MODE_CHAR_LEN_MASK) + 7) >> 3;

	/* The transfer len must be integer multiple of the word width */
	if (len % dev->dlen) {
		fprintf(stderr, "OMAP4430 SPI: transfer length must be integer multiple of the word length!\n");
		return -1;
	}

	// Estimate transfer time in us... The calculated dtime is only used for
	// the timeout, so it doesn't have to be that accurate.  At higher clock
	// rates, a calcuated dtime of 0 would mess-up the timeout calculation, so
	// round up to 1 us
	dev->dtime = dev->dlen * 1000 * 1000 / devlist[id].cfg.clock_rate;
	if (dev->dtime == 0)
		dev->dtime = 1;

	omap_clock_enable(dev);

	omap4430_setup(dev, device);
	
	if (omap4430_setup_sdma(hdl, id, paddr, len)){
		omap_clock_disable(dev);
		return -1;
	}
	
	/* Enable sdma request	*/
	set_port(base + OMAP4430_MCSPI_CH1_CONFIG_OFFSET + (0x14 * id), OMAP4430_MCSPI_DMAR | OMAP4430_MCSPI_DMAW, OMAP4430_MCSPI_DMAR | OMAP4430_MCSPI_DMAW);

	/* Configue the SPI control register to enable the corresponding channel of the SPI */ 
	set_port(base + OMAP4430_MCSPI_CH1_CTRL_OFFSET + 0x14 * id, OMAP4430_MCSPI_CHANNEL_ENABLE, OMAP4430_MCSPI_CHANNEL_ENABLE);

	/* force CS */
	set_port((base + OMAP4430_MCSPI_CH1_CONFIG_OFFSET + 0x14 * id), OMAP4430_MCSPI_FORCE_MODE_ONE, OMAP4430_MCSPI_FORCE_MODE_ONE);

	if (omap4430_wait(dev,len * 10)) {
		fprintf(stderr, "OMAP4430 SPI: DMA XFER Timeout!!!");
		len = -1;
	}

	/* un-force CS */
	set_port((base + OMAP4430_MCSPI_CH1_CONFIG_OFFSET + 0x14 * id), OMAP4430_MCSPI_FORCE_MODE_ONE, 0);

	/*
	 * Disable SDMA request and SPI function
	 */
	set_port(base + OMAP4430_MCSPI_CH1_CTRL_OFFSET + 0x14 * id, OMAP4430_MCSPI_CHANNEL_ENABLE, 0);
	set_port(base + OMAP4430_MCSPI_CH1_CONFIG_OFFSET + 0x14 * id, OMAP4430_MCSPI_DMAR | OMAP4430_MCSPI_DMAW, 0);

	omap4430_sdma_disablespi(dev);

	omap_clock_disable(dev);

	return len;	
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/spi/omap4430/omap4430spi.c $ $Rev: 738420 $")
#endif
