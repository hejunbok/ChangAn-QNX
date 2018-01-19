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

#include "dra74x_startup.h"

/*
 * ES1.0: Vayu EVM revision E and earlier
 * ES2.0: Vayu EVM revision F and G
 */
int pmic_revision = TPS659038_ES1_0;

void read_pmic_revision()
{
	uint8_t buf;
	omap_i2c_dev_t i2c_dev = {
		.base = DRA74X_I2C1_BASE,
		.clock = I2C_SPEED,
		.slave = TPS659038_I2C_SLAVE_ADDR_PG3
	};

	omap_i2c_init(&i2c_dev);

	if (0 != omap_i2c_read(&i2c_dev, TPS659038_REG_ADDR_DESIGNREV, 1, &buf, 1)) {
		kprintf("failed to read PMIC for revision info\n");
	} else {
		pmic_revision = buf;
	}

	if (debug_flag) {
		switch (pmic_revision) {
			case TPS659038_ES1_0:
				kprintf("Vayu EVM hardward Rev E or earlier\n");
				break;
			case TPS659038_ES2_0:
			default:
				kprintf("Vayu EVM hardward Rev F or beyond\n");
				break;
		}
	}

	omap_i2c_deinit(&i2c_dev);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/dra74x/init_pmic.c $ $Rev: 746337 $")
#endif
