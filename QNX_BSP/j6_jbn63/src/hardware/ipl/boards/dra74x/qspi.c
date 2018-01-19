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

#include <stdlib.h>
#include <string.h>
#include "board.h"
#include "qspi.h"

qspi_dev_t qspi;

static inline void qspi_wait_for_complete()
{
	while ((in32(qspi.core_base + QSPI_SPI_STATUS) & QSPI_STATUS_BUSY))
		;
}

static int qspi_send_command(uint32_t cmd, int flen, int bw)
{
	/* issue status read command */
	out32(qspi.core_base + QSPI_SPI_DATA, cmd);
	out32(qspi.core_base + QSPI_SPI_CMD, (0 << QSPI_CMD_CSNUM_SHIFT)
									| ((bw * 8 - 1) << QSPI_CMD_WLEN_SHIFT)
									| (flen << QSPI_CMD_FLEN_SHIFT)
									| QSPI_CMD_4PIN_WRITE_SINGLE);
	qspi_wait_for_complete();
	return 0;
}

static int qspi_read_data(int flen, int bw)
{
	out32(qspi.core_base + QSPI_SPI_CMD, (0 << QSPI_CMD_CSNUM_SHIFT)
									| (flen << QSPI_CMD_FLEN_SHIFT)
									| QSPI_CMD_4PIN_READ_SINGLE
									| ((bw * 8 - 1) << QSPI_CMD_WLEN_SHIFT));
	qspi_wait_for_complete();
	return in32(qspi.core_base + QSPI_SPI_DATA);
}

static int qspi_read_register(uint32_t cmd, int len)
{
	uint32_t val = 0;

	qspi_send_command(cmd, len, COMMAND_1_BYTE);

	while (len--) {
		val = (val << 8 ) | (qspi_read_data(len, COMMAND_1_BYTE) & 0xff);
	}
	return val;
}

void qspi_mode_config(int mode)
{
	if (mode == QSPI_MODE_MMAP) {
		/* memory mode */
		out32(qspi.core_base + QSPI_SPI_SWITCH, QSPI_SWITCH_MMPT_S);
		out32(DRA74X_CTRL_CORE_BASE + CTRL_CORE_CONTROL_IO_2, (in32(DRA74X_CTRL_CORE_BASE + CTRL_CORE_CONTROL_IO_2) & ~(QSPI_MEMMAPPED_CS_MASK)) | (QSPI_CS0 << QSPI_MEMMAPPED_CS_SHIFT));
	} else if (mode == QSPI_MODE_CPORT) {
		/* select configuration port */
		out32(qspi.core_base + QSPI_SPI_SWITCH, 0x0);
		out32(DRA74X_CTRL_CORE_BASE + CTRL_CORE_CONTROL_IO_2, (in32(DRA74X_CTRL_CORE_BASE + CTRL_CORE_CONTROL_IO_2) & ~(QSPI_MEMMAPPED_CS_MASK)) | (QSPI_MMR << QSPI_MEMMAPPED_CS_SHIFT));
	}
}

static int qspi_s25fl_quad_enable(void)
{
	uint32_t	sr1, cr;

	/* enable write */
	qspi_send_command(QSPI_CMD_WREN, 0, COMMAND_1_BYTE);

	/* Read Status register 1 */
	sr1 = qspi_read_register(QSPI_CMD_RDSR, COMMAND_1_BYTE);

	/* Read Configuration Register */
	cr = qspi_read_register(QSPI_CMD_RDCR, COMMAND_1_BYTE);

	/* Quad already enabled? */
	if ((cr & CR_QUAD))
		return (0);

	/* Set Configuration register 2nd bit to 1 for Quad enable */
	cr |= CR_QUAD;

	/* write CR register */
	qspi_send_command((QSPI_CMD_WRREG << 16) | (sr1 << 8 ) | cr, 0, COMMAND_3_BYTE);

	/* Read Configuration Register */
	cr = qspi_read_register(QSPI_CMD_RDCR, COMMAND_1_BYTE);

	/* Quad already enabled? */
	if ((cr & CR_QUAD))
		return (0);

	ser_putstr("Failed to set QSPI to Quad mode\n");
	return (-1);
}

static int qspi_read_ident(uint8_t *buf, int nbyte)
{
	qspi_send_command(QSPI_CMD_RDID, nbyte, COMMAND_1_BYTE);

	while (nbyte--) {
		*buf++ = qspi_read_data(nbyte, COMMAND_1_BYTE);
	}

	return (0);
}

int qspi_s25fl_ident()
{
	uint8_t buf[20] = {0};

	qspi_read_ident(buf, 0x13);

	if (buf[0x10] != 'Q' || buf[0x11] != 'R' || buf[0x12] != 'Y') {
		return -1;
	}

	if (buf[0] == MID_SPANSION) {
		if (buf[1] == DID_S25FL128) {
			qspi.size = 128 * 1024 *1024;
			log_putstr("S25FL128S detected.\n");

			return 0;
		}

		if (buf[1] == DID_S25FL256) {
			qspi.size = 256 * 1024 *1024;
			log_putstr("S25FL256S detected.\n");
			return 0;
		}
	}

	ser_putstr("No QSPI NOR device detected.\n");
	return -1;
}

static int qspi_n25q_ident()
{
	uint8_t buf[4] = {0};

	qspi_read_ident(buf, 4);

	if (buf[0] == MID_MICRON) {
		if (buf[1] == DID_N25Q128) {
			qspi.size = 128 * 1024 *1024;
			log_putstr("N25Q128A detected.\n");
			return 0;
		}
	}

	ser_putstr("No QSPI NOR device detected.\n");
	return -1;
}

int qspi_read(int offset, uint32_t addr, int size)
{
	edma_param *param;
	uint32_t	acnt, bcnt, shift;

	if (qspi.verbose) {
		ser_putstr("qspi_read(): loading from offset 0x");
		ser_puthex(offset);
		ser_putstr(" to 0x");
		ser_puthex(addr);
		ser_putstr(" size 0x");
		ser_puthex(size);
		ser_putstr(" bytes\n");
	}

	qspi_mode_config(QSPI_MODE_MMAP);

	for (bcnt = 128, shift = 7; ;bcnt <<= 1, shift++) {
		acnt = (size + bcnt - 1) >> shift;
		if (acnt <= 32 * 1024)
			break;
	}

	/* in case there is an pending event */
	omap3_edma_set_bit(qspi.edma_base, OMAP3_EDMA_ECR, qspi.edma_chnl);
	omap3_edma_set_bit(qspi.edma_base, OMAP3_EDMA_ICR, qspi.edma_chnl);

	/*
	 * Initialize DMA channel
	 */
	param = (edma_param *)(qspi.edma_base + DRA74X_PARAM(qspi.edma_chnl));
	param->opt = (0 << 23)	/* ITCCHEN = 0 */
				| (0 << 22)	/* TCCHEN = 0 */
				| (0 << 21)
				| (1 << 20)	/* Transfer complete interrupt enable */
				| (qspi.edma_chnl << 12)	/* TCC */
				| (0 << 11)	/* Normal completion */
				| (0 << 3)	/* PaRAM set is not static */
				| (1 << 2)	/* AB-synchronizad */
				| (0 << 1)	/* Destination address increment mode */
				| (0 << 0);	/* Source address increment mode */

	param->src = DRA74X_QSPI_SFI_MM_BASE + offset;
	param->abcnt = (bcnt << 16) | acnt;
	param->dst = addr;
	param->srcdstbidx = (acnt << 16) | acnt;
	param->linkbcntrld = 0xFFFF;
	param->srcdstcidx = 0;
	param->ccnt = 1;

	/*
	 * Trigger transfer event
	 */
	omap3_edma_set_bit(qspi.edma_base, OMAP3_EDMA_ESR, qspi.edma_chnl);

	while (1) {
		if (omap3_edma_read_bit(qspi.edma_base, OMAP3_EDMA_IPR, qspi.edma_chnl)) {
			omap3_edma_set_bit(qspi.edma_base, OMAP3_EDMA_ICR, qspi.edma_chnl);
			return size;
		}
	}

	return 0;
}

static int qspi_flash_chip_busy()
{
	return (qspi_read_register(QSPI_CMD_RDSR, COMMAND_1_BYTE) & FLASH_WIP);
}

static int qspi_flash_n25q_sector_erase(int offset)
{
	int loop;

	/* enable write */
	qspi_send_command(QSPI_CMD_WREN, 0, COMMAND_1_BYTE);

	/* issue 3 bytes sector address */
	qspi_send_command(((QSPI_CMD_BLK_ERASE << 24) | (offset & 0xffffff)), 0, COMMAND_4_BYTE);

	/* It could take up to 3 second, normally ~700ms */
	for (loop = 3000; loop > 0; loop--) {
		if (!qspi_flash_chip_busy()) {
			return (0);
		}
		omap_usec_delay(1000);
	}

	return 0;
}

static int qspi_flash_s25fl_sector_erase(int offset)
{
	/* enable write */
	qspi_send_command(QSPI_CMD_WREN, 0, COMMAND_1_BYTE);

	/* send erase command */
	qspi_send_command(QSPI_CMD_4SE, 1, COMMAND_1_BYTE);

	/* issue 4 bytes sector address */
	return qspi_send_command(offset, 2, COMMAND_4_BYTE);
}

static int qspi_flash_sector_erase(int offset)
{
	if (qspi.verbose) {
		ser_putstr("qspi_flash_sector_erase(): starts at 0x");
		ser_puthex(offset);
		ser_putstr(" size 0x");
		ser_puthex(qspi.sector_size);
		ser_putstr(" bytes\n");
	}

	if (qspi.wdt_toggle)
		qspi.wdt_toggle();

	if (qspi.qspi_part == PART_S25FL) {
		return (qspi_flash_s25fl_sector_erase(offset));
	}

	if (qspi.qspi_part == PART_N25Q) {
		return (qspi_flash_n25q_sector_erase(offset));
	}

	return -1;
}

static int qspi_flash_n25q_page_program(uint32_t offset, uint32_t size, uint8_t *buf)
{
	int loop;
	uint32_t *pbuf = (uint32_t *)buf;

	qspi_send_command(QSPI_CMD_WREN, 0, COMMAND_1_BYTE);

	/* issue page program command plus 3 bytes address */
	qspi_send_command((QSPI_CMD_PAGE_PRGM << 24) | (offset & 0xffffff), size, COMMAND_4_BYTE);

	/* write data */
	while (size--) {
		uint32_t tmp = *pbuf++;
		qspi_send_command(ENDIAN_BE32(tmp), size, COMMAND_4_BYTE);
	}

	/* It could take up to 5ms, normally ~500us */
	for (loop = 5000; loop > 0; loop--) {
		if (!qspi_flash_chip_busy()) {
			return (0);
		}
		omap_usec_delay(1);
	}

	return (-1);
}

static int qspi_flash_s25fl_page_program(uint32_t offset, uint32_t size, uint8_t *buf)
{
	int loop;
	uint32_t *pbuf = (uint32_t *)buf;

	qspi_send_command(QSPI_CMD_WREN, 0, COMMAND_1_BYTE);

	/* issue page program command plus 4 bytes address */
	qspi_send_command(QSPI_CMD_4PP, size + 1, COMMAND_1_BYTE);
	qspi_send_command(offset, size + 1, COMMAND_4_BYTE);

	/* write data */
	while (size--) {
		uint32_t tmp = *pbuf++;
		qspi_send_command(ENDIAN_BE32(tmp), size, COMMAND_4_BYTE);
	}

	/* max page program time 750us */
	for (loop = 750; loop > 0; loop--) {
		if (!qspi_flash_chip_busy()) {
			return (0);
		}
		omap_usec_delay(1);
	}

	return (-1);
}

/* We assume offset, size, address all are at 4 bytes boundary */
static int qspi_flash_page_program(uint32_t offset, uint32_t size, uint8_t *buf)
{
	if (qspi.wdt_toggle)
		qspi.wdt_toggle();

	if (qspi.verbose) {
		ser_putstr("qspi_flash_page_program(): starts at 0x");
		ser_puthex(offset);
		ser_putstr(" size 0x");
		ser_puthex(size);
		ser_putstr(" bytes\n");
	}

	size = (size >> 2);

	if (qspi.qspi_part == PART_S25FL) {
		return qspi_flash_s25fl_page_program(offset, size, buf);
	}

	if (qspi.qspi_part == PART_N25Q) {
		return qspi_flash_n25q_page_program(offset, size, buf);
	}
	return -1;
}

int qspi_initialize(qspi_dev_t *dev)
{
	uint32_t clkdiv;

	memcpy(&qspi, dev, sizeof(qspi_dev_t));

	for (clkdiv = 1; clkdiv <= QSPI_DCLK_DIV_MSK; clkdiv ++) {
		if (qspi.clock * clkdiv >= qspi.input_frq) {
			clkdiv -= 1;
			break;
		}
	}

	out32(qspi.core_base + QSPI_SYSCONFIG, 2 << 2);

	/* disable/clear interrupts */
	out32(qspi.core_base + QSPI_INTR_ENABLE_CLEAR, QSPI_INTR_WIRQ | QSPI_INTR_FIRQ);
	out32(qspi.core_base + QSPI_INTR_STATUS_CLEAR, QSPI_INTR_WIRQ | QSPI_INTR_FIRQ);
	out32(qspi.core_base + QSPI_SPI_DC, (0 << QSPI_DC_DD(0))	/* 0 clock cycle delay for chip select */
						| (1 << QSPI_DC_CKPH(0))				/* Clock phase, data shifted out on rising edge; input on falling edge */
						| (0 << QSPI_DC_CSP(0))					/* Chip select polarity: active low */
						| (1 << QSPI_DC_CKP(0)));				/* Clock polarity: no data transfer, qspi1_sclk is '1' */
	out32(qspi.core_base + QSPI_SPI_CLOCK_CNTRL, clkdiv);
	in32 (qspi.core_base + QSPI_SPI_CLOCK_CNTRL);
	out32(qspi.core_base + QSPI_SPI_CLOCK_CNTRL, QSPI_CLKEN | clkdiv);

	/* Select configuration port */
	qspi_mode_config(QSPI_MODE_CPORT);

	if (qspi.qspi_part == PART_S25FL) {
		out32(qspi.core_base + QSPI_SPI_SETUP0, QSPI_CMD_4QOR << QSPI_SETUP_RCMD_SHIFT
						| QSPI_SETUP_ADDR_4BYTE
						| (1 << QSPI_SETUP_NUM_D_BYTES_SHIFT)
						| QSPI_SETUP_READ_QUAD | QSPI_SETUP_READ_NORM
						| QSPI_CMD_4PP << QSPI_SETUP_WCMD_SHIFT
						| 0x0 << QSPI_SETUP_NUM_D_BITS_SHIFT);
		if (!qspi_s25fl_ident()) {
			return qspi_s25fl_quad_enable();
		}
	} else if (qspi.qspi_part == PART_N25Q) {
		out32(qspi.core_base + QSPI_SPI_SETUP0, QSPI_CMD_DOFR << QSPI_SETUP_RCMD_SHIFT
						| QSPI_SETUP_ADDR_3BYTE
						| (0 << QSPI_SETUP_NUM_D_BYTES_SHIFT)
						| QSPI_SETUP_READ_DUAL
						| 0 << QSPI_SETUP_WCMD_SHIFT
						| (8 << QSPI_SETUP_NUM_D_BITS_SHIFT));

		return qspi_n25q_ident();
	}

	/* Unsupprted Flash part */
	return -1;
}

int qspi_write(uint32_t offset, uint32_t size, uint8_t *buf)
{
	uint32_t sector_addr, buffer_count;

	qspi_mode_config(QSPI_MODE_CPORT);

	if (qspi.verbose) {
		ser_putstr("qspi_write(): starts at 0x");
		ser_puthex(offset);
		ser_putstr(" size 0x");
		ser_puthex(size);
		ser_putstr(" bytes\n");
	}

	/*
	 * size round to integer multiple of 4 bytes
	 * we assume starting offset is at 4 bytes boundary
	 */
	size = (size + 3) & ~3;

	if (offset + size > qspi.size) {
		ser_putstr("Failed: exceed the size of QSPI\n");
		return -1;
	}

	/* Get the sector base address */
	sector_addr = offset & ~(qspi.sector_size - 1);

	/* Erase the needed sectors before programming */
	for (; sector_addr < (offset + size); sector_addr += qspi.sector_size) {
		if (qspi_flash_sector_erase(sector_addr)) {
			ser_putstr("failed to erase QSPI NOR at offset 0x\n");
			ser_puthex(sector_addr);
			ser_putstr("\n");
			return -1;
		}
	}

	while (size) {
		/* One operation no more than buff_size bytes */
		buffer_count = min(qspi.buff_size, size);

		/* align buffer with write buffer page */
		buffer_count = min(buffer_count, qspi.buff_size - (offset & (qspi.buff_size - 1)));

		if (qspi_flash_page_program(offset, buffer_count, buf)) {
			ser_putstr("Failed to program to QSPI Flash at offset 0x");
			ser_puthex(offset);
			ser_putstr("\n");
			return -1;
		}

		size -= buffer_count;
		offset += buffer_count;
		buf += buffer_count;
	}
	return 0;
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/ipl/boards/dra74x/qspi.c $ $Rev: 746250 $")
#endif
