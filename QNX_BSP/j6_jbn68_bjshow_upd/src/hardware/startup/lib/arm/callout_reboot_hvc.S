.ifdef VARIANT_v7

#
# Copyright 2007, 2008, QNX Software Systems. 
# 
# Licensed under the Apache License, Version 2.0 (the "License"). You 
# may not reproduce, modify or distribute this software except in 
# compliance with the License. You may obtain a copy of the License 
# at: http://www.apache.org/licenses/LICENSE-2.0 
# 
# Unless required by applicable law or agreed to in writing, software 
# distributed under the License is distributed on an "AS IS" basis, 
# WITHOUT WARRANTIES OF ANY KIND, either express or implied.
#
# This file may contain contributions from others, either as 
# contributors under the License or as licensors under other terms.  
# Please review this entire file for other proprietary rights or license 
# notices, as well as the QNX Development Suite License Guide at 
# http://licensing.qnx.com/license-guide/ for other information.
#
/*
 * HVC domain reboot
 */

#include "callout.ah"

CALLOUT_START(reboot_hvc, 0, 0)
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 5)
	.arch_extension virt
	hvc	#0x1234
#else
	.word 0xe1412374	// hvc #0x1234
#endif
CALLOUT_END(reboot_hvc)

#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL$ $Rev$";
.previous
#endif
#endif 
.endif
