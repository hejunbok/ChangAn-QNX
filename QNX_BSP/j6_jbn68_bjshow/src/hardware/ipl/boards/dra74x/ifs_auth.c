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

#include "board.h"

#define API_HAL_BASE_INDEX			0x0
#define API_HAL_KM_VERIFYCERTIFICATESIGNATURE_INDEX	API_HAL_BASE_INDEX + 0xE
#define CERT_IFS_MAGIC				0x53544255
#define CERT_RIGHTS_OTHER			0x100
#define CL_RSA_KEY_MAX_SIZE			256

#ifdef HS_AUTH_IFS
struct ti_signed_cert {
	unsigned char	signer_info[16];
	unsigned		signature_info;
	unsigned		key_id;
	unsigned char	signature[CL_RSA_KEY_MAX_SIZE];
};

static unsigned get_device_type(void)
{
	return (in32(DRA74X_CTRL_CORE_BASE + CTRL_CORE_STATUS)
		& CTRL_STATUS_DEVICE_TYPE_MASK) >> CTRL_STATUS_DEVICE_TYPE_SHIFT;
}

static unsigned ti_rom_auth(void *cert, unsigned len, void *sign, unsigned rights)
{
	unsigned params[6];

	params[0] = 4;
	params[1] = (unsigned)cert;
	params[2] = len;
	params[3] = (unsigned)sign;
	params[4] = rights;
	params[5] = 0;

	return hal_pub_to_sec_dispatcher(API_HAL_KM_VERIFYCERTIFICATESIGNATURE_INDEX, 0, 0, (unsigned)&params[0]);
}

unsigned authenticate_image_signature(unsigned start_addr, unsigned size)
{
	unsigned cert_size, result;
	struct ti_signed_cert *cert_ptr = 0;
	char buf[5];
	unsigned *tmp = (unsigned *)&buf[0];

	ser_putstr("In authenticate_image_signature\n");

	if (get_device_type() != CTRL_STATUS_DEVICE_TYPE_GP) {
		cert_size = sizeof(struct ti_signed_cert);
		cert_ptr = (struct ti_signed_cert *)(start_addr + size - cert_size);
		if ((unsigned)cert_ptr % 4 != 0) {
			ser_putstr("Size ");
			ser_puthex(size);
			ser_putstr(" from image loaded @ ");
			ser_puthex(start_addr);
			ser_putstr(" is not word aligned! Sign image correctly\n");
			goto exit_error;
		}

		/* Magic unsigned to string */
		*tmp = cert_ptr->signature_info;
		buf[4]='\0';
		ser_putstr(buf);
		ser_putstr(" authenticating!! \n");

		switch (cert_ptr->signature_info) {
			case CERT_IFS_MAGIC:
				result = ti_rom_auth((void *)start_addr, size - cert_size, (void *)cert_ptr, CERT_RIGHTS_OTHER);
				if (result != 0) {
					ser_putstr(buf);
					ser_putstr(" authentication failed! :");
					ser_puthex(result);
					ser_putstr("\n");
					goto exit_error;
				} else {
					ser_putstr(buf);
					ser_putstr(" authentication passed!\n");
				}
			break;
		default:
			ser_putstr(buf);
			ser_putstr(" - Unknown Magic, image not authenticated\n");
			goto exit_error;
		}
	} else {
		ser_putstr("GP device does not support image authentication\n");
		result = 1;
	}
	return result;

exit_error:
	/* loop for ever */
	for (;;);
	return 1;
}
#else
unsigned authenticate_image_signature(unsigned start_addr, unsigned size)
{
	return 1;
}
#endif


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/ipl/boards/dra74x/ifs_auth.c $ $Rev: 758539 $")
#endif
