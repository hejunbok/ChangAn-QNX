/*
 * $QNXLicenseC: 
 * Copyright 2009, 2013 QNX Software Systems.  
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
#include <sys/rsrcdbmgr.h>

void omap4430_sdma_disablespi(omap4430_spi_t *omap4430)
{
	int     tx_idx = omap4430->sdma_tx_chid ;
	int     rx_idx = omap4430->sdma_rx_chid;
	
	omap4430->dma4->channel[tx_idx].ccr &= ~DMA4_CCR_ENABLE;
	omap4430->dma4->channel[rx_idx].ccr &= ~DMA4_CCR_ENABLE;
	
	/* Disable interrupt */
	omap4430->dma4->channel[tx_idx].cicr = 0;
	omap4430->dma4->channel[rx_idx].cicr = 0;
	
	/* Clear previous status of the channel */
	omap4430->dma4->channel[tx_idx].csr = DMA4_CSR_MSK;
	omap4430->dma4->channel[rx_idx].csr = DMA4_CSR_MSK;
}

int omap4430_setup_sdma(omap4430_spi_t *omap4430, int device, spi_dma_paddr_t *paddr, int len )
{
	int     tx_idx = omap4430->sdma_tx_chid ;
	int     rx_idx = omap4430->sdma_rx_chid;

	switch (omap4430->dlen) {
		case 1:
			omap4430->dma4->channel[tx_idx].csdp = DMA4_CSDP_DATA_TYPE_8BIT;
			omap4430->dma4->channel[rx_idx].csdp = DMA4_CSDP_DATA_TYPE_8BIT;
			break;
		case 2:
			omap4430->dma4->channel[tx_idx].csdp = DMA4_CSDP_DATA_TYPE_16BIT;
			omap4430->dma4->channel[rx_idx].csdp = DMA4_CSDP_DATA_TYPE_16BIT;
			break;
		default:
			omap4430->dma4->channel[tx_idx].csdp = DMA4_CSDP_DATA_TYPE_32BIT;
			omap4430->dma4->channel[rx_idx].csdp = DMA4_CSDP_DATA_TYPE_32BIT;
			break;
	}

	/********************
	 * Setup Tx channel *
	 ********************/
	/* Config DMA Channel for SPI transmit DMA request */
	omap4430->dma4->channel[tx_idx].ccr = ((omap4430->spi_tx_dma_req + 1) & 0x1f) 
		| DMA4_CCR_SRC_POST_INCR      	/* SRC_AMODE: post-incremental source */
		| DMA4_CCR_DST_CONSTANT   		/* DST_AMODE: constant dst addr	*/
		| (((omap4430->spi_tx_dma_req + 1) & 0x60) << 14);   /* SYNCHRO_CONTROL_UPPER */

	omap4430->dma4->channel[tx_idx].cen = len / omap4430->dlen; /* number of elements per frame (EN) */
	omap4430->dma4->channel[tx_idx].cfn = 1;  /*number of frames (FN) per block*/

	/* Set the Physical Address for the source-side of the DMA channel */
	if (paddr->wpaddr)
		omap4430->dma4->channel[tx_idx].cssa = (uint32_t)paddr->wpaddr;
	else
		omap4430->dma4->channel[tx_idx].cssa =  omap4430->pdmabuf;

	/* Set the Physical Address for the destination-side of the DMA channel */
	omap4430->dma4->channel[tx_idx].cdsa = omap4430->pbase + OMAP4430_MCSPI_CH1_TX_BUFFER_OFFSET + (0x14 * device) ; 

	/* Enable end of frame/fragment interrupt */
	omap4430->dma4->channel[tx_idx].cicr = DMA4_CICR_FRAME_IE;

	/********************
	 * Setup Rx channel *
	 ********************/
	/* Config DMA Channel for SPI receive DMA request */
	omap4430->dma4->channel[rx_idx].ccr = ((omap4430->spi_rx_dma_req + 1) & 0x1f) 
			| ((((omap4430->spi_rx_dma_req +1) & 0x60) << 14)
			| DMA4_CCR_SEL_SRC_DST_SYNC 
			| DMA4_CCR_SRC_CONSTANT 
			| DMA4_CCR_DST_POST_INCR);

	omap4430->dma4->channel[rx_idx].cen = len / omap4430->dlen;	/* number of elements per frame (EN) */
	omap4430->dma4->channel[rx_idx].cfn = 1; /*number of frames (FN) per block*/
	
	/* Set the Physical Address for the source-side of the DMA channel */
	omap4430->dma4->channel[rx_idx].cssa = omap4430->pbase+OMAP4430_MCSPI_CH1_RX_BUFFER_OFFSET + (0x14 * device) ;

	/* Set the Physical Address for the destination-side of the DMA channel */
	if (paddr->rpaddr)  {
		omap4430->dma4->channel[rx_idx].cdsa = (uint32_t)paddr->rpaddr;
	 } else {
		omap4430->dma4->channel[rx_idx].cdsa = omap4430->pdmabuf;
	 }

	/* Enable end of frame/fragment interrupt */
	omap4430->dma4->channel[rx_idx].cicr = DMA4_CICR_FRAME_IE;

	/* Enable SDMA */
	omap4430->dma4->channel[tx_idx].ccr |= DMA4_CCR_ENABLE;
	omap4430->dma4->channel[rx_idx].ccr |= DMA4_CCR_ENABLE;

	return 0;
}

void
omap4430_sdma_detach(omap4430_spi_t *omap4430)
{
	rsrc_request_t req = { 0 };

	if (omap4430->sdma_tx_chid != -1)
	{
		req.length = 1;
		req.start = omap4430->sdma_tx_chid;
		req.flags = RSRCDBMGR_DMA_CHANNEL;
		rsrcdbmgr_detach (&req, 1);
	}
	if (omap4430->sdma_rx_chid != -1)
	{
		memset (&req, 0, sizeof (req));
		req.length = 1;
		req.start = omap4430->sdma_rx_chid;
		req.flags = RSRCDBMGR_DMA_CHANNEL;
		rsrcdbmgr_detach (&req, 1);
	}
	munmap_device_memory (omap4430->dma4, sizeof (dma4_t));

}

int32_t
omap4430_sdma_attach(omap4430_spi_t *omap4430)
{
	int     tx_idx = omap4430->sdma_tx_chid = -1;
	int     rx_idx = omap4430->sdma_rx_chid = -1;
	
	rsrc_request_t req = { 0 };

	/* Map in DMA registers */
	if ((omap4430->dma4 = mmap_device_memory (0, sizeof (dma4_t),
				PROT_READ | PROT_WRITE | PROT_NOCACHE, 0, DMA4_BASE_ADDR)) == MAP_FAILED)
	{
		fprintf(stderr, "Unable to mmap DMA (%s) \n", strerror (errno));
		return (-1);
	}

	/*
	*********** SPI: transmit ****************
	*/
	req.length = 1;
	req.flags = RSRCDBMGR_DMA_CHANNEL;
	if (rsrcdbmgr_attach (&req, 1) == -1)
	{
			fprintf(stderr, "Unable to aquire DMA channels (%s) \n", strerror (errno));
			munmap_device_memory (omap4430->dma4, sizeof (dma4_t));
			return (-1);
	}
	 tx_idx = omap4430->sdma_tx_chid  = req.start;
	
	/*
	*********** SPI: receive ****************
	*/
	memset (&req, 0, sizeof (req));
	req.length = 1;
	req.flags = RSRCDBMGR_DMA_CHANNEL;
	if (rsrcdbmgr_attach (&req, 1) == -1)
	{
			fprintf(stderr, "Unable to aquire DMA channels (%s) \n", strerror (errno));
			omap4430_sdma_detach (omap4430);
			return (-1);
	}
	rx_idx = omap4430->sdma_rx_chid = req.start;

    // do some common initialization
	omap4430->dma4->channel[tx_idx].cse = 1;
	omap4430->dma4->channel[tx_idx].cde = 1;
	omap4430->dma4->channel[rx_idx].cse = 1;
	omap4430->dma4->channel[rx_idx].cde = 1;
   	omap4430->dma4->channel[tx_idx].cdf = 0;
   	omap4430->dma4->channel[rx_idx].csf = 0;

	return 0;
}

int omap4430_init_sdma(omap4430_spi_t *omap4430)
{
	/*
	 * Map in DMA buffer
	 */
	if (omap4430->sdma) {
			omap4430->dmabuf = mmap(0, OMAP4430_SPI_MAXDMALEN, PROT_READ|PROT_WRITE|PROT_NOCACHE, 
							MAP_ANON|MAP_PHYS|MAP_PRIVATE, NOFD, 0);
			if (omap4430->dmabuf != MAP_FAILED) {
				omap4430->pdmabuf =  mphys(omap4430->dmabuf);
				if (omap4430->pdmabuf == -1)
					omap4430->sdma = 0;
			}
			else 
					omap4430->sdma = 0;
	}

	if(omap4430_sdma_attach(omap4430))  return -1;
	
	omap4430_sdma_disablespi(omap4430);
	return 0;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/spi/omap4430/sdma.c $ $Rev: 716138 $")
#endif
