/*
 * $QNXLicenseC:
 * Copyright 2014, QNX Software Systems.
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


#include "f3s_qspi.h"
#include <hw/sysinfo.h>
#include <sys/rsrcdbmgr.h>

extern paddr_t mphys(void *);
extern unsigned hwi_find_bus(const char *name, unsigned unit);

char *bsargs = NULL;

static int spi_dc_val = 0; // delay 0, data output on falling edge;input on rising edge
static char *supported_opts[] = {
	"edma",
	"event",
	"clk",
	"nommap",
	"dc",
	NULL
};

static void qspi_hwi_info(qspi_dev_t *qspi)
{
	hwi_tag *tag;
	char *name;
	int occurence = 0;
	int offset;

	while ((offset = hwi_find_bus( "qspi", occurence++)) != HWI_NULL_OFF) {
		while ((offset = hwi_next_tag(offset, 1)) != HWI_NULL_OFF) {
			tag	= hwi_off2tag(offset);
			name = __hwi_find_string(((hwi_tag *)tag)->prefix.name);

			if (!strcmp(name, HWI_TAG_NAME_location)) {
				qspi->core_pbase = tag->location.base;
				qspi->core_size = tag->location.len;
			} else if (!strcmp(name, HWI_TAG_NAME_irq)) {
				qspi->edma_irq = tag->irq.vector;
			} else if (!strcmp(name, HWI_TAG_NAME_inputclk)) {
				qspi->input_clk = tag->inputclk.clk;
			} else if (!strcmp(name, HWI_TAG_NAME_dma)) {
				qspi->edma_chnl = tag->dma.chnl;
			}
		}
	}
}

static void qspi_board_option(qspi_dev_t *qspi)
{
	char *value, *freeptr, *options;
	int	opt;

	qspi->edma_chnl	= -1;
	qspi->edma_event = -1;
	qspi->edma_irq = -1;
	qspi->input_clk = 192000000;
	qspi->clk = 64000000;

	qspi->core_pbase = DRA74X_QSPI_CORE_BASE;
	qspi->core_size = DRA74X_QSPI_CORE_SIZE;

	qspi->read_mode = QSPI_CMD_4PIN_READ_SINGLE;
	qspi->write_mode = QSPI_CMD_4PIN_WRITE_SINGLE;
	qspi->nommap = 0;

	/* By default it's on CS0 */
	qspi->cs = 0;

	/* HWI info could be overwritten by command line options */
	qspi_hwi_info(qspi);

	if (bsargs == NULL)
		return;

	freeptr = strdup(bsargs);

	options = freeptr;

	while (options && *options != '\0') {
		opt = getsubopt(&options, supported_opts, &value);
		switch (opt) {
			case 0:
				qspi->edma_chnl	= strtoul(value, 0, 0);
				break;
			case 1:
				qspi->edma_event = strtoul(value, 0, 0);
				break;
			case 2:
				qspi->clk = strtoul(value, 0, 0);
				break;
			case 3:
				qspi->nommap = 1;
				break;
			case 4:
				spi_dc_val = strtoul(value, 0, 0);
				break;
		}
	}

	free(freeptr);
}

/*
 * This is the open callout for the SPI serial NOR flash driver.
 */
int32_t f3s_qspi_open(f3s_socket_t *socket, uint32_t flags)
{
	qspi_dev_t	*qspi;
	int clkdiv;

	if (socket->socket_handle)
		return EOK;

	if (f3s_socket_option(socket)) {
		socket->address = DRA74X_QSPI_SFI_MM_BASE;
		socket->window_size = DRA74X_QSPI_SFI_MM_SIZE;
		socket->array_size = DRA74X_QSPI_SFI_MM_SIZE;
	}

	socket->name = (unsigned char*)"QSPI serial flash";

	socket->socket_handle = calloc(1, sizeof(qspi_dev_t));
	qspi = (qspi_dev_t *)socket->socket_handle;

	// check command line options
	qspi_board_option(qspi);

	/* 
	 * If we already have the EDMA channel,
	 * Let the kernel know which one we are using
	 */
	if (qspi->edma_chnl != -1) {
		rsrc_request_t req = { 0 };

		req.start = qspi->edma_chnl;
		req.end = qspi->edma_chnl;
		req.length = 1;

		req.flags = RSRCDBMGR_FLAG_RANGE | RSRCDBMGR_DMA_CHANNEL;
		if (rsrcdbmgr_attach( &req, 1 ) != EOK) {
			fprintf(stderr, "(devf t%d::%s) failed to revered EDMA channel or EDMA channels were not seeded by resource_seed\n", 
				pthread_self(), __func__);
		}
	}

	if (qspi->edma_event == -1)
		qspi->edma_event = qspi->edma_chnl;

	if ((qspi->edma_event != -1) && (qspi->edma_irq == -1))
		qspi->edma_irq = 0x2000 + qspi->edma_event;

	qspi->memory = mmap_device_memory(0, socket->window_size,
						PROT_READ|PROT_WRITE|PROT_NOCACHE, 0, socket->address);

	if ((qspi->core_base = mmap_device_io(qspi->core_size, qspi->core_pbase)) == MAP_DEVICE_FAILED)
		goto fail5;
	if ((qspi->ctrlcore_control_io2 = mmap_device_io(4, DRA74X_CTRL_CORE_BASE + CTRL_CORE_CONTROL_IO_2)) == MAP_DEVICE_FAILED)
		goto fail7;

	if (qspi->edma_chnl != -1) {
		if ((qspi->edma_base = mmap_device_io(DRA74X_EDMA_SIZE, DRA74X_EDMA_BASE)) == MAP_DEVICE_FAILED)
			goto fail8;
	}

	qspi->mode = QSPI_MODE_UNKNOWN;

	out32(qspi->core_base + QSPI_SYSCONFIG, 2 << 2);	// smart idle

	clkdiv = qspi->input_clk / qspi->clk;
	if (clkdiv) --clkdiv;

	// disable/clear interrupts
	out32(qspi->core_base + QSPI_INTR_ENABLE_CLEAR, QSPI_INTR_WIRQ | QSPI_INTR_FIRQ);
	out32(qspi->core_base + QSPI_INTR_STATUS_CLEAR, QSPI_INTR_WIRQ | QSPI_INTR_FIRQ);
	out32(qspi->core_base + QSPI_SPI_DC, spi_dc_val);
	out32(qspi->core_base + QSPI_SPI_CLOCK_CNTRL, clkdiv);
	in32 (qspi->core_base + QSPI_SPI_CLOCK_CNTRL);
	out32(qspi->core_base + QSPI_SPI_CLOCK_CNTRL, QSPI_CLKEN | clkdiv);
#if 0	//for s25fl256s
	out32(qspi->core_base + QSPI_SPI_SETUP0, QSPI_CMD_4QOR << QSPI_SETUP_RCMD_SHIFT
								| QSPI_SETUP_ADDR_4BYTE
								| (1 << QSPI_SETUP_NUM_D_BYTES_SHIFT)
								| QSPI_SETUP_READ_QUAD
								| QSPI_CMD_4PP << QSPI_SETUP_WCMD_SHIFT
								| (0x0 << QSPI_SETUP_NUM_D_BITS_SHIFT));
#else	//for s25fl032p
	out32(qspi->core_base + QSPI_SPI_SETUP0, QSPI_CMD_3QOR_032p << QSPI_SETUP_RCMD_SHIFT
								| QSPI_SETUP_ADDR_3BYTE
								| (1 << QSPI_SETUP_NUM_D_BYTES_SHIFT)
								| QSPI_SETUP_READ_QUAD
								| QSPI_CMD_3PP_032p << QSPI_SETUP_WCMD_SHIFT
								| (0x0 << QSPI_SETUP_NUM_D_BITS_SHIFT));
#endif

	if (qspi->edma_chnl == -1) {
		qspi->v_buf = malloc(EDMA_BUFLEN);
		return (EOK);
	}

	// map in DMA buffer
	if ((qspi->v_buf = mmap(NULL, EDMA_BUFLEN, PROT_READ | PROT_WRITE,	// | PROT_NOCACHE,
										MAP_PRIVATE | MAP_ANON | MAP_PHYS, NOFD, 0)) == MAP_FAILED) {
		goto fail9;
	}

	qspi->cachectl.fd = NOFD;
	if (cache_init(0, &qspi->cachectl, NULL) == -1) {
		fprintf(stderr, "cache init failed\n");
		goto fail10;
	}

	qspi->p_dmabuf = mphys(qspi->v_buf);

	// attach qspi interrupt
	if (qspi_attach_irq(qspi) == EOK)
		return (EOK);

fail10:
	munmap(qspi->v_buf, EDMA_BUFLEN);
fail9:
	if (qspi->edma_base)
		munmap_device_io(qspi->edma_base, DRA74X_EDMA_SIZE);
fail8:
	munmap_device_io(qspi->ctrlcore_control_io2, 4);
fail7:
	munmap_device_io(qspi->core_base, qspi->core_size);
fail5:
	munmap_device_memory(qspi->memory, socket->window_size);

	return (-1);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/6.6.0/trunk/hardware/flash/boards/ti_qspi/f3s_qspi_open.c $ $Rev: 762641 $")
#endif
