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




/*
 * non-XIP configuration header.
 * Should be put at the beginning of boot image.
 * CHSETTINGS item is mandatory, but not used in this project.
 * The total size of CH header must be 512 bytes.
 */


	.text
	.align	2
_CH:
	.word	0x00000040
	.word	0x0000000c
	.word	0x00000000
	.word	0x00000000
	.word	0x00000000
	.word	0x45534843
	.word	0x4e495454
	.word	0x00005347
	.word	0xffffffff
	.word	0xffffffff
	.word	0xffffffff
	.word	0xffffffff
	.word	0xffffffff
	.word	0xffffffff
	.word	0xffffffff
	.word	0xffffffff
	.word	0xc0c0c0c1
	.word	0x00000100
	.word	0x00000000
	.word	0x00000000
	.set	XCount, 0
	.rept	108
	.word	0x00000000
	.set	XCount, XCount + 1
	.endr


#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL: http://svn/product/branches/6.6.0/trunk/hardware/ipl/boards/dra74x/ch_header.S $ $Rev: 742206 $";
.previous
#endif
#endif 
