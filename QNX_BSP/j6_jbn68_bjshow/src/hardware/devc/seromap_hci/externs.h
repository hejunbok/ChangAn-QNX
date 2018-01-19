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

#ifdef DEFN
    #define EXT
    #define INIT1(a)                = { a }
#else
    #define EXT extern
    #define INIT1(a)
#endif

#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <malloc.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include <termios.h>
#include <devctl.h>
#include <sys/dcmd_chr.h>
#include <sys/iomsg.h>
#include <atomic.h>
#include <hw/inout.h>
#include <arm/omap.h>
#include <sys/io-char.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>

#include <sys/trace.h>

char *user_parm;

/* Activate code for BT and Powermanagement */
#ifdef VARIANT_winbt
    #ifndef WINBT
            #define WINBT
    #endif
#endif

/* Activate code for Qualcomm BT */
#ifdef VARIANT_qcabt
    #ifndef QCABT
            #define QCABT
    #endif
#endif

#define NUM_DEVS                            3
#define DEFAULT_BAUD_RATE                   115200
#define DEFAULT_BAUD_RATE_MID               115200
#define DEFAULT_BAUD_RATE_HIGH              3000000
#define DEFAULT_CMD_TIMEOUT                 1000 /* milli seconds */

/* HCI definitions */
#define PROTO_HCI_CMD                       0x01
#define PROTO_ACL_PKT                       0x02
#define PROTO_SCO_PKT                       0x03
#define PROTO_HCI_EVT                       0x04
#define PROTO_FM_CHNL                       0x08
#define PROTO_GPS_CHNL                      0x09

/* One byte HCI type, one byte Event Code and one byte Parameter Length */
#define PROTO_HCI_EVT_HEADER_LEN            2

#define cmd_opcode_pack(ogf, ocf)           (uint16_t)((ocf & 0x03ff) | (ogf << 10))

/* Increase the UART size to include the SYSC register */
#undef  OMAP_UART_SIZE
#define OMAP_UART_SIZE                      0x60
#define OMAP_UART_SYSC                      0x54
#define OMAP_SYSC_SOFTRESET                 (1 << 1)
#define OMAP_UART_SYSS                      0x58
#define OMAP_SYSS_RESETDONE                 (1 << 0)
#define OMAP_UART_WER                       0x5C

#define FIFO_TRIG_8                         1
#define FIFO_TRIG_16                        2
#define FIFO_TRIG_32                        3
#define FIFO_TRIG_56                        4
#define FIFO_TRIG_60                        5

#define FIFO_SIZE                           64  /* size of the rx and tx fifo's */
#define RX_BUFFER_SIZE                      6144

typedef struct _virtual_dev {
    TTYDEV            tty;
    int               tx_length;
    int               rx_length;
    volatile uint32_t oband_data;
    struct dev_omap   *dev;
} virtual_dev_t;

/* Dev flags */
#define SCRIPT_MODE                         (1 << 0)
#define RX_ERROR                            (1 << 1)
#define HCI_CARRIER                         (1 << 2)

/* Custom/Extended OBAND flags */
#define HCI_INIT_FAILED                     (1 << 0)

typedef struct dev_omap {
    virtual_dev_t     vdev[NUM_DEVS];
    struct dev_omap   *next;
    volatile uint32_t flags;
    uint32_t          max_retry;

    uint8_t         *rx_buffer;
    int             rx_buffer_size;
    int             rx_buffer_nbytes;
    int             tx_active;    /* Index of active vdev */
    int             rx_active;    /* Index of active vdev */
    uint8_t         cmd_buf[300];
    uint16_t        cmd_nbytes;
    volatile unsigned response_pending;
    unsigned        intr;
    int             iid;
    int             isr;
    unsigned        clk;
    unsigned        div;
    unsigned char   fifo;
    uintptr_t       port[OMAP_UART_SIZE];

    unsigned        kick_maxim;

    unsigned        brd;        /* Baud rate divisor */
    unsigned        lcr;
    unsigned        efr;
    unsigned        baud;

#define    SEROMAP_PWM_PAGED    0x1    /* Flag to tell tto not to transmit data */
    unsigned        auto_rts_enable;
#ifdef WINBT
    intrspin_t idle_spinlock;
    uint32_t   physbase;
    uint32_t   clkctrl_phys;
    uintptr_t  clkctrl_base;
    uintptr_t  pinmux_base;
    uintptr_t  gpio3_base;
    int        idle;                 /* idle flag signaled by a devctl command */
    int        signal_oband_notification;
#endif
    char bt_script_name[_POSIX_PATH_MAX];
    FILE *bt_script;
#ifdef WINBT
    /* OMAP4430 - Winchester */
    #define GPIO_BASE_ADDR     0x48055000    /* GPIO Bank 2 */
    #define GPIO_PIN           55          /* GPIO 55 */
#else
    /* Jacinto 5 EVM */
    #define GPIO_BASE_ADDR     0x4804C000    /* GPIO Bank 1 */
    #define GPIO_PIN           26            /* GPIO 26 */
#endif

    uint32_t gpio_base;
    uint32_t gpio_num;
    unsigned baud_mid;
    unsigned baud_high;
    void     *bt_extra;
} DEV_OMAP;

typedef struct ttyinit_omap {
    TTYINIT     tty;
    uint32_t    max_retry;
    unsigned    loopback;     /*loopback mode*/
    unsigned    auto_rts_enable;
    unsigned    isr;
    char        bt_script_name[_POSIX_PATH_MAX];
    uint32_t    gpio_base;
    uint32_t    gpio_num;
    unsigned    baud_mid;
    unsigned    baud_high;
}TTYINIT_OMAP;

typedef int (*EVT_CALLBACK)(DEV_OMAP *dev, const uint8_t* event, uint8_t len);

#define    SEROMAP_NUM_POWER_MODES    4

EXT TTYCTRL                        ttyctrl;

/* Should put into arm/omap.h */
#define    OMAP_SCR_WAKEUPEN        (1 << 4)
#define    OMAP_SSR_WAKEUP_STS      (1 << 1)

#define    SEROMAP_NOKICK           55

#include "proto.h"
#include <variant.h>

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devc/seromap_hci/externs.h $ $Rev: 761666 $")
#endif
