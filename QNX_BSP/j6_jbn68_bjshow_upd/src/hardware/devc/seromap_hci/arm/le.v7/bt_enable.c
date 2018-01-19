/*
 * $QNXLicenseC:
 * Copyright 2013, QNX Software Systems.
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

#include <sys/mman.h>
#include "externs.h"
#include "proto.h"

#define GPIO_BT_EN(x) (1 << (x - ((x >> 5) << 5)))

typedef struct _gpio_regs {
    volatile uint32_t output_en;
    volatile uint32_t data_in;
    volatile uint32_t data_out;
    volatile uint32_t resv[2];
    volatile uint32_t rising_detect;
    volatile uint32_t falling_detect;
    volatile uint32_t resv2[16];
    volatile uint32_t clr_data_out;
    volatile uint32_t set_data_out;
} gpio_regs_t;

int bt_init(DEV_OMAP *dev)
{
    gpio_regs_t *gpio_regs;
    if ((gpio_regs = mmap_device_memory(0, sizeof (gpio_regs_t), PROT_READ | PROT_WRITE | PROT_NOCACHE, 0, dev->gpio_base + 0x134)) == MAP_FAILED)
        return EFAULT;

    dev->bt_extra = gpio_regs;
    gpio_regs->output_en &= ~(GPIO_BT_EN(dev->gpio_num));		 /* Active low */
    return EOK;

}

int bt_deinit(DEV_OMAP *dev)
{
    gpio_regs_t *gpio_regs = (gpio_regs_t *) dev->bt_extra;
    munmap_device_memory(gpio_regs, sizeof (gpio_regs_t));
    return EOK;
}

int bt_toggle(DEV_OMAP *dev, int state)
{
    gpio_regs_t *gpio_regs = (gpio_regs_t *) dev->bt_extra;
    if(BT_ON == state)
    {
        gpio_regs->set_data_out = GPIO_BT_EN(dev->gpio_num); /* Enable BT */
    }
    else if(BT_OFF == state)
    {
        gpio_regs->clr_data_out = GPIO_BT_EN(dev->gpio_num); /* Disable BT */
    }

    return EOK;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devc/seromap_hci/arm/le.v7/bt_enable.c $ $Rev: 748566 $")
#endif
