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

static void qspi_wait_for_complete(qspi_dev_t *qspi)
{
	volatile int val;
	int timeout = 1000;
	val = in32(qspi->core_base + QSPI_SPI_STATUS);

	while ((val & (QSPI_STATUS_WC | QSPI_STATUS_BUSY)) != QSPI_STATUS_WC ) {
		val = in32(qspi->core_base + QSPI_SPI_STATUS);
		if (--timeout < 0) {
			fprintf(stderr, "%s timeout! \n", __func__);
			return;
		}
	}
}

void qspi_mode_config(qspi_dev_t *qspi, int mode)
{
	if (mode == QSPI_MODE_MMAP) {
		if (qspi->mode != QSPI_MODE_MMAP) {
			// memory mode, only used for memory read
			out32(qspi->core_base + QSPI_SPI_SWITCH, QSPI_SWITCH_MMPT_S);
			out32(qspi->ctrlcore_control_io2, (in32(qspi->ctrlcore_control_io2) & ~(QSPI_MEMMAPPED_CS_MASK)) | (QSPI_CS0 << QSPI_MEMMAPPED_CS_SHIFT));
			qspi->mode = QSPI_MODE_MMAP;
		}
	} else if (mode == QSPI_MODE_CPORT) {
		if (qspi->mode != QSPI_MODE_CPORT) {
			// select configuration port
			out32(qspi->core_base + QSPI_SPI_SWITCH, 0x0);
			out32(qspi->ctrlcore_control_io2, (in32(qspi->ctrlcore_control_io2) & ~(QSPI_MEMMAPPED_CS_MASK)) | (QSPI_MMR << QSPI_MEMMAPPED_CS_SHIFT));
			qspi->mode = QSPI_MODE_CPORT;
		}
	}
}

int qspi_send_command(qspi_dev_t *qspi, uint32_t cmd, int flen, int bw)
{
	// issue status read command
	out32(qspi->core_base + QSPI_SPI_DATA, cmd);
	out32(qspi->core_base + QSPI_SPI_CMD, (qspi->cs << QSPI_CMD_CSNUM_SHIFT)
										| (flen << QSPI_CMD_FLEN_SHIFT)
										| qspi->write_mode
										| ((bw * 8 - 1) << QSPI_CMD_WLEN_SHIFT));
	qspi_wait_for_complete(qspi);
	return EOK;
}

int qspi_read_data(qspi_dev_t *qspi, int flen, int bw, int mode)
{
	out32(qspi->core_base + QSPI_SPI_CMD, (qspi->cs << QSPI_CMD_CSNUM_SHIFT)
						| (flen << QSPI_CMD_FLEN_SHIFT)
						| mode
						| ((bw * 8 - 1) << QSPI_CMD_WLEN_SHIFT));
	qspi_wait_for_complete(qspi);
	return in32(qspi->core_base + QSPI_SPI_DATA);
}

/*
 * Read register from QSPI flash, assuming it's <= 32bits
 */
int qspi_read_register(qspi_dev_t *qspi, uint32_t cmd, int len)
{
	uint32_t val = 0;

	qspi_send_command(qspi, cmd, len, COMMAND_1_BYTE);

	while (len--) {
		val = (val << 8 ) | (qspi_read_data(qspi, len, COMMAND_1_BYTE, qspi->read_mode) & 0xff);
	}
	return val;
}

/*
 * QSPI flash reset
 */
int qspi_flash_reset(qspi_dev_t *qspi)
{
	qspi_send_command(qspi, QSPI_CMD_RST, 0, COMMAND_1_BYTE);
	return (EOK);
}

/*
 * QSPI flash erase suspend
 */
int qspi_flash_erase_suspend(qspi_dev_t *qspi)
{
	int	loop;

	if (qspi->suspended)
		return (EOK);

	// Select configuration port
	qspi_mode_config(qspi, QSPI_MODE_CPORT);

	qspi_send_command(qspi, QSPI_CMD_ERSP, 0, COMMAND_1_BYTE);

	// max 45us according to spec
	for (loop = 64; loop > 0; loop--) {
		if (!(qspi_read_register(qspi, QSPI_CMD_RDSR, COMMAND_1_BYTE) & FLASH_WIP)) {
			if ((qspi_read_register(qspi, QSPI_CMD_RDSR2, COMMAND_1_BYTE) & FLASH_ES) == 0)	// no WIP, no suspend, so erase completed
				return (ECANCELED);

			// erase is suspended
			qspi->suspended = 1;

			return (EOK);
		}
		nanospin_ns(1000);
	}

	return (EIO);
}

/*
 * QSPI flash erase resume
 */
int qspi_flash_erase_resume(qspi_dev_t *qspi)
{
	if (!qspi->suspended)
		return (EOK);

	qspi_send_command(qspi, QSPI_CMD_ERRS, 0, COMMAND_1_BYTE);
	nanospin_ns(100 * 1000);
	qspi->suspended = 0;

	return (EOK);
}

int qspi_flash_read_ident(qspi_dev_t *qspi, uint8_t *buf, int nbyte)
{
	// Select configuration port
	qspi_mode_config(qspi, QSPI_MODE_CPORT);
	qspi_send_command(qspi, QSPI_CMD_RDID, nbyte, COMMAND_1_BYTE);

	while (nbyte--) {
		*buf++ = qspi_read_data(qspi, nbyte, COMMAND_1_BYTE, qspi->read_mode);
	}

	return (EOK);
}

/*
 * Check flash busy bit
 */
int qspi_flash_chip_busy(qspi_dev_t *qspi)
{
	qspi_mode_config(qspi, QSPI_MODE_CPORT);
	return (qspi_read_register(qspi, QSPI_CMD_RDSR, COMMAND_1_BYTE) & FLASH_WIP);
}

/*
 * Wait for EDMA transfer complete
 */
static int qspi_edma_wait(qspi_dev_t *qspi)
{
	struct _pulse	pulse;
	uint64_t		ntime = 1e9;

	for (;;) {
		TimerTimeout(CLOCK_REALTIME, _NTO_TIMEOUT_RECEIVE, NULL, &ntime, NULL);
		if (MsgReceivePulse(qspi->chid, &pulse, sizeof(pulse), NULL) == -1) {
			qspi->irq_expected = 0;
			fprintf(stderr, "PID_%d %s(%d) timeout.\n", getpid(), strerror(errno), errno);
			return (EIO);
		}

		if (pulse.code == QSPI_EDMA_EVENT)
			return EOK;
	}
}

/*
 * QSPI EDMA read
 */
static int qspi_flash_edma_read(qspi_dev_t *qspi, int offset, uint8_t *buf, int size)
{
	edma_param *param;
	uintptr_t base = qspi->edma_base + DRA74X_EDMA_SHADOW0_OFF;
	int rc = EOK;

	/*
	* Initialize DMA channel
	*/
	param = (edma_param *)(qspi->edma_base + DRA74X_PARAM(qspi->edma_chnl));
	param->opt = (0 << 23)	/* ITCCHEN = 0 */
				| (0 << 22)	/* TCCHEN = 0 */
				| (0 << 21)	/* */
				| (1 << 20)	/* Transfer complete interrupt enable */
				| (qspi->edma_event << 12)	/* TCC */
				| (0 << 11)	/* Normal completion */
				| (0 << 3)	/* PaRAM set is not static */
				| (1 << 2)	/* A-synchronizad */
				| (0 << 1)	/* Destination address increment mode */
				| (0 << 0);	/* Source address increment mode */

	param->src = DRA74X_QSPI_SFI_MM_BASE + offset;
	param->abcnt = (1 << 16) | size;
	param->dst = qspi->p_dmabuf;
	param->srcdstbidx = 0;
	param->linkbcntrld = 0xFFFF;
	param->srcdstcidx = 0;
	param->ccnt = 1;

	qspi->irq_expected	= 1;

	CACHE_INVAL(&qspi->cachectl, qspi->v_buf, qspi->p_dmabuf, size);

	/*
	* Trigger transfer event
	*/
	if (qspi->edma_chnl >= 32)
		out32(base + DM6446_EDMA_ESR + 4, 1 << (qspi->edma_chnl - 32));
	else
		out32(base + DM6446_EDMA_ESR, 1 << qspi->edma_chnl);

	rc = qspi_edma_wait(qspi);

	if (rc == EOK) {
		CACHE_INVAL(&qspi->cachectl, qspi->v_buf, qspi->p_dmabuf, size);
		if (buf != qspi->v_buf)
			memcpy(buf, qspi->v_buf, size);

		return size;
	}
	return 0;
}

/*
 * quad read
 */
#define QSPI_WLEN_16BYTE    16   // 128bit using 4 SPI data registers
static int qspi_flash_cport_read(qspi_dev_t *qspi, int offset, uint8_t *buf, int size)
{
	int nbyte = size;

	if (nbyte >= QSPI_WLEN_16BYTE) {
		// max. 16 byte using four SPI data registers
		uint32_t *buf32 = (uint32_t *)buf;
		uint32_t cmd, temp;
		qspi_mode_config(qspi, QSPI_MODE_CPORT);
		qspi_send_command(qspi, QSPI_CMD_QOFR, nbyte/QSPI_WLEN_16BYTE + 2, COMMAND_1_BYTE);
		qspi_send_command(qspi, offset, 0, COMMAND_3_BYTE);
		qspi_send_command(qspi, 0x0, 0, COMMAND_1_BYTE);

		cmd = (qspi->cs << QSPI_CMD_CSNUM_SHIFT)
			| (1 << QSPI_CMD_FLEN_SHIFT)
			| QSPI_CMD_6PIN_READ_QUAD
			| (127 << QSPI_CMD_WLEN_SHIFT);

		while (nbyte >= QSPI_WLEN_16BYTE) {
			out32(qspi->core_base + QSPI_SPI_CMD, cmd);
			qspi_wait_for_complete(qspi);
			temp = in32(qspi->core_base + QSPI_SPI_DATA3);
			*buf32++ = ENDIAN_RET32(temp);
			temp = in32(qspi->core_base + QSPI_SPI_DATA2);
			*buf32++ = ENDIAN_RET32(temp);
			temp = in32(qspi->core_base + QSPI_SPI_DATA1);
			*buf32++ = ENDIAN_RET32(temp);
			temp = in32(qspi->core_base + QSPI_SPI_DATA);
			*buf32++ = ENDIAN_RET32(temp);
			buf    += QSPI_WLEN_16BYTE;
			offset += QSPI_WLEN_16BYTE;
			nbyte  -= QSPI_WLEN_16BYTE;
		}
	}
	if (nbyte > 0) {
		qspi_mode_config(qspi, QSPI_MODE_CPORT);
		qspi_send_command(qspi, QSPI_CMD_QOFR, nbyte + 2, COMMAND_1_BYTE);
		qspi_send_command(qspi, offset, 0, COMMAND_3_BYTE);
		qspi_send_command(qspi, 0x0, 0, COMMAND_1_BYTE);
		while (nbyte--) {
			*buf++ = qspi_read_data(qspi, 0, COMMAND_1_BYTE, QSPI_CMD_6PIN_READ_QUAD);
		}
	}
	return size;
}

int qspi_flash_read(qspi_dev_t *qspi, int offset, uint8_t *buf, int size)
{
	uint8_t *memory;

	if (qspi->nommap) { // read via configuration port
		return qspi_flash_cport_read(qspi, offset, buf, size);
	}

	// memory mapped mode
	qspi_mode_config(qspi, QSPI_MODE_MMAP);

	if ((size >= EDMA_THRESHOLD) && qspi->edma_chnl != -1) {
		return qspi_flash_edma_read(qspi, offset, buf, size);
	}

	memory = qspi->memory + offset;
	memcpy(buf, memory, size);
	return size;
}

/*
 * EDMA interrupt handler
 */
static const struct sigevent *qspi_intr(void *area, int id)
{
	qspi_dev_t	*qspi = area;

	if (qspi->irq_expected) {
		qspi->irq_expected = 0;

		return &qspi->event;
	}

	return NULL;
}

/*
 * Attach EDMA interrupt
 */
int qspi_attach_irq(qspi_dev_t *qspi)
{
	if ((qspi->chid = ChannelCreate(_NTO_CHF_DISCONNECT | _NTO_CHF_UNBLOCK)) == -1)
		return -1;

	if ((qspi->coid = ConnectAttach(0, 0, qspi->chid, _NTO_SIDE_CHANNEL, 0)) == -1)
		goto fail1;

	qspi->event.sigev_notify	= SIGEV_PULSE;
	qspi->event.sigev_coid	= qspi->coid;
	qspi->event.sigev_code	= QSPI_EDMA_EVENT;
	qspi->event.sigev_priority = 21;

	/*
	* Attach interrupt
	*/
	qspi->iid = InterruptAttach(qspi->edma_irq, qspi_intr, qspi, 0, _NTO_INTR_FLAGS_TRK_MSK);
	if (qspi->iid == -1) {
		perror("InterruptAttach");
		goto fail2;
	}

	return EOK;

fail2:
	InterruptDetach(qspi->iid);
fail1:
	ChannelDestroy(qspi->chid);

	return -1;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/6.6.0/trunk/hardware/flash/boards/ti_qspi/qspi_cmds.c $ $Rev: 761898 $")
#endif
