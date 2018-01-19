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

#ifndef PROTO_H
#define PROTO_H

DEV_OMAP *create_device(TTYINIT_OMAP *dip, unsigned unit, unsigned maxim_xcvr_kick);
void ser_stty(virtual_dev_t *dev);
void ser_ctrl(DEV_OMAP *dev, unsigned flags);
void set_port(unsigned port, unsigned mask, unsigned data);
void *query_default_device(TTYINIT_OMAP *dip, void *link);
unsigned options(int argc, char *argv[]);
int seromap_interrupt_event_handler (message_context_t *msgctp, int code, unsigned flags, void *handle);
const struct sigevent *seromap_interrupt_handler ( void *area, int id);
void display_data_bytes(char *name, _uint8 * ptr, int len, const char *dir);
void run_errata_i202(DEV_OMAP *dev);

/* SHARED TRANSPORT */
int seromap_custom_event_handler (TTYDEV *tty);
int write_command (DEV_OMAP *dev, int size);
int wait_rx_ack(DEV_OMAP *dev, uint32_t ms_timeout, EVT_CALLBACK callback);
int wait_tx_complete(DEV_OMAP *dev);
int init_script_pulse(DEV_OMAP *dev);
FILE *open_script_file(const char *filename, int *script_type, unsigned long *version);
int run_init_script(DEV_OMAP *dev);
void change_soc_baud_rate(DEV_OMAP *dev, int baud_rate);
#ifdef QCABT
int run_qca_init( void *data );
int bthci_qcomm_handler_rx(DEV_OMAP *dev, uint8_t *buf, int length, int *parsed_bytes, EVT_CALLBACK callback);
uint8_t* bt_qsoc_read_bin_file(DEV_OMAP *dev, char* file_name, int* ptr_num_bytes_read, uint32_t extra_len);
#endif

int reset_device( DEV_OMAP *dev );
int parse_tx_packet(virtual_dev_t * vdev);
int parse_rx_header(DEV_OMAP * dev);

#define BT_ON		0
#define BT_OFF		1

int bt_init(DEV_OMAP *dev);
int bt_deinit(DEV_OMAP *dev);
int bt_toggle(DEV_OMAP *dev, int state);

#ifdef WINBT

int winbt_devctl(resmgr_context_t * ctp, io_devctl_t * msg, iofunc_ocb_t * ocb);

void omap_clock_enable(DEV_OMAP* dev);
void omap_clock_disable(DEV_OMAP* dev);
int omap_clock_toggle_init(DEV_OMAP* dev);
void omap_clock_enable_isr(DEV_OMAP* dev);
void omap_clock_disable_isr(DEV_OMAP* dev);
int omap_force_rts_init(struct dev_omap* dev);
void omap_force_rts(struct dev_omap* dev, int level);

#endif

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devc/seromap_hci/proto.h $ $Rev: 765635 $")
#endif
