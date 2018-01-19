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

	.text
	.align 2
	.globl hal_pub_to_sec_dispatcher
	.type hal_pub_to_sec_dispatcher, function

/*
 * uint32_t hal_pub_to_sec_dispatcher(uint32_t func_id, uint32_t p0_in, uint32_t p1_in, uint32_t p2_in)
 */
hal_pub_to_sec_dispatcher:
	PUSH	{r4-r11, lr}	@ save registers - ROM code may pollute
	mov r6, #0xff			@ set new task
	mov r12, #0				@ Secure Service ID
	dsb
	isb
	dmb
	.word	0xe1600070		@ SMC #0 - hand assembled
	POP	 {r4-r11, pc}


#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL: http://svn/product/branches/6.6.0/trunk/hardware/ipl/boards/dra74x/hal_pub_to_sec.S $ $Rev: 758539 $";
.previous
#endif
#endif
