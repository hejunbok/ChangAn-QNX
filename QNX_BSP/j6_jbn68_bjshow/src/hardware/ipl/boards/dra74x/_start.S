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

#define STACK_SIZE		16384

	.text
	.align 2
	.globl _start

	/* In order to boot High Security devices, we don't need this header */
#ifndef HS_BOOT
	/*
	* non-xip boot header, tell the boot ROM the size and load address of IPL.
	* Should be put at the beginning of IPL boot image.
	* These 8 bytes will be stripped for XIP booting,
	* It's worth noting that the first instruction is always @ 0x40300008
	*/
	.word	_edata - _start
	.word	_start
#endif

_start:

	/*
	 * Save r0 pointer to Booting Parameter Structure from TI ROM code.
	 */
	ldr		r8, ptrr0
	str		r0, [r8]

	/*
	 * Set the cpu to SVC32 mode
	 */
	mrs		r0, cpsr
	bic		r0, r0, #0x1f
	orr		r0, r0, #0xd3
	msr		cpsr, r0

	/*
	 * Invalidate L1 I/D
	 */
	mov		r0, #0					@ set up for MCR
	mcr		p15, 0, r0, c8, c7, 0	@ invalidate TLBs
	mcr		p15, 0, r0, c7, c5, 0	@ invalidate icache

	/*
	 * disable MMU stuff and caches
	 */
	mrc		p15, 0, r0, c1, c0, 0
	bic		r0, r0, #0x00002000		@ clear bits 13 (--V-)
	bic		r0, r0, #0x00000007		@ clear bits 2:0 (-CAM)
	orr		r0, r0, #0x00000002		@ set bit 1 (--A-) Align
	orr		r0, r0, #0x00000800		@ set bit 11 (Z---) BTB
	mcr		p15, 0, r0, c1, c0, 0

	/* determine if IPL boot from SRAM */
	mov		r0, pc					@ Are we runing in SRAM
	and		r0, r0, #0xff000000
	cmp		r0, #0x40000000
	beq		_in_sram

	/****************************************************/
	/* IPL is not in any other boot device, hence hang */
	/****************************************************/
hang:
	b		hang

_in_sram:
	/*
	 * Set stack pointer
	 */
	ldr		sp, =_stack_bottom + STACK_SIZE
	bl		main

	/*
	 * In case return from main
	 */
	b		_start

ptrr0: .word _TI_ROM_r0

	.bss
	.globl _TI_ROM_r0
_TI_ROM_r0: .space 4

	/*
	 * Reserved IPL stack
	 */
	.section .stack
_stack_bottom:
	.space STACK_SIZE

	.text


#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL: http://svn/product/branches/6.6.0/trunk/hardware/ipl/boards/dra74x/_start.S $ $Rev: 745291 $";
.previous
#endif
#endif 
