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
 * polled serial I/O
 */

#include "callout.ah"
#include <arm/scif.h>

/*
 * -----------------------------------------------------------------------
 * Routine to patch callout code
 *
 * On entry:
 *	r0	- physical address of syspage
 *	r1	- virtual  address of syspage
 *	r2	- offset from start of syspage to start of the callout routine
 *	r3	- offset from start of syspage to read/write data used by callout
 *	[sp]	- data registered with callout via callout_register_data()
 *	[sp+4]	- address of callout definition (CALLOUT_START)
 * -----------------------------------------------------------------------
 */
patch_debug:
	stmdb		sp!, {r4, lr}
	add		r4, r0, r2			// address of callout routine

	// Map registers
	ldr		r1, [sp, #8]			// debug_device[...]
	ldr		r1, [r1, #DDI_BASE]

	mov		r0, #0x28
	bl		callout_io_map

	/* Patch the callout routine */
	CALLOUT_PATCH		r4, r0, r1, r2, ip
	ldmia		sp!, {r4, pc}

/*
 * -----------------------------------------------------------------------
 * void	display_char_scif(struct sypage_entry *, char)
 * -----------------------------------------------------------------------
 */
CALLOUT_START(display_char_scif, 0, patch_debug)
	mov		ip, #0x000000ff			// register base (= tx holding register)
	orr		ip, ip, #0x0000ff00
	orr		ip, ip, #0x00ff0000
	orr		ip, ip, #0xff000000
loop1:
	ldrh		r3, [ip, #SH_SCIF_SCFSR_OFF]
	tst		r3, #SH_SCIF_SCFSR_TDFE
	beq		loop1

	strb		r1, [ip, #SH_SCIF_SCFTDR_OFF]
	mov		r3,#0
	strh		r3, [ip, #SH_SCIF_SCFSR_OFF]

loop2:
	ldrh		r3, [ip, #SH_SCIF_SCFSR_OFF]
	tst		r3, #SH_SCIF_SCFSR_TEND
	beq		loop2
	mov		pc, lr
CALLOUT_END(display_char_scif)

/*
 * -----------------------------------------------------------------------
 * char	poll_key_scif(struct sypage_entry *)
 * -----------------------------------------------------------------------
 */
CALLOUT_START(poll_key_scif, 0, patch_debug)
	mov		ip, #0x000000ff
	orr		ip, ip, #0x0000ff00
	orr		ip, ip, #0x00ff0000
	orr		ip, ip, #0xff000000

	mvn		r0, #1
	ldrh		r3, [ip, #SH_SCIF_SCFDR_OFF]
	tst		r3, #0x7f
	bne		0f
	ldrb		r0, [ip, #SH_SCIF_SCFRDR_OFF]

0:
	mov		pc, lr
CALLOUT_END(poll_key_scif)

/*
 * -----------------------------------------------------------------------
 * void	break_detect_scif(struct sypage_entry *, char)
 * -----------------------------------------------------------------------
 */
CALLOUT_START(break_detect_scif, 0, patch_debug)
	mov		ip, #0x000000ff
	orr		ip, ip, #0x0000ff00
	orr		ip, ip, #0x00ff0000
	orr		ip, ip, #0xff000000

	ldr		r0, [ip, #SH_SCIF_SCFSR_OFF]
	and		r0, #SH_SCIF_SCFSR_BRK
	tst		r0, #0
	bne		0f
	bic		r0,r0,#SH_SCIF_SCFSR_BRK
	str		r0, [ip, #SH_SCIF_SCFSR_OFF]
0:
	mov		pc, lr
CALLOUT_END(break_detect_scif)

#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL$ $Rev$";
.previous
#endif
#endif
