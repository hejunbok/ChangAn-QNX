/*
 * $QNXLicenseC:
 * Copyright 2013-2014, QNX Software Systems.
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

#include <arm/omap.h>
#include "dra74x_startup.h"
#include "board.h"

int ddr_size = 0;
int gpmc_nand = 0;
int gpmc_nor = 0;
int gpmc_vout3 = 0;
int abe_dpll_196m = 0;
int wifi = 0;
int jamr3 = 0;

extern struct callout_rtn reboot_dra74x;

const struct callout_slot callouts[] =
{
	{ CALLOUT_SLOT( reboot, _dra74x ) },
};

const struct armv_chip *armv_list[] = {
	&armv_chip_a15
};

const struct debug_device debug_devices[] =
{
	{
		"8250",
		{
			"0x4806A000^2.115200.48000000.16"	// UART 1, use the baud rate set by boot loader
		},
		init_omap,
		put_omap,
		{
			&display_char_8250,
			&poll_key_8250,
			&break_detect_8250,
		}
	}
};

void allocate_dsplink_memory(paddr_t linkmem_addr, size_t linkmem_size)
{
	alloc_ram(linkmem_addr, linkmem_size, 1);
	hwi_add_device(HWI_ITEM_BUS_UNKNOWN, HWI_ITEM_DEVCLASS_DMA, "misc", 0);
	hwi_add_location(linkmem_addr, linkmem_size, 0, hwi_find_as(linkmem_addr, 0));
	as_add_containing(linkmem_addr, linkmem_addr + linkmem_size - 1, AS_ATTR_RAM, "dsplink", "ram");
}

void init_hwinfo()
{
	set_syspage_section(&lsp.cpu.arm_boxinfo, sizeof(*lsp.cpu.arm_boxinfo.p));

	/* Initialize the Hwinfo section of the Syspage */
	hwi_dra74x();
}

/*
 * main()
 *	Startup program executing out of RAM
 *
 * 1. It gathers information about the system and places it in a structure
 *	called the system page. The kernel references this structure to
 *	determine everything it needs to know about the system. This structure
 *	is also available to user programs (read only if protection is on)
 *	via _syspage->.
 *
 * 2. It (optionally) turns on the MMU and starts the next program
 *	in the image file system.
 */
int main(int argc, char **argv, char **envv)
{
	int	opt;
	char *p;
	paddr_t linkmem_addr = 0;
	size_t linkmem_size = 0;
	int	link_present = 0;

	/*
	 * Initialize debugging output
	 */
	select_debug(debug_devices, sizeof(debug_devices));

	/* early hw initialization required for everything */
	init_clocks();

	/* Disable the WatchDog Timer 2 */
	wdt_disable();

	/* add reboot callout */
	add_callout_array(callouts, sizeof(callouts));

	while ((opt = getopt(argc, argv, COMMON_OPTIONS_STRING "WL:c:d:n:GFgbJVa")) != -1) {
		switch (opt) {
		case 'c':
			kprintf("Warning: Cannot change frequency from startup. Request Ignored!\n");
			break;

		case 'd':
			ddr_size = atoi(optarg);
			break;

		case 'L':
			linkmem_addr = getsize(optarg, &p);
			if (*p == ',') {
				linkmem_size = getsize(p + 1, &p);
			}
			link_present = 1;
			break;

		case 'n':
			hundred_loop_time = (unsigned) strtoul(optarg, &optarg, 0);
			if (*optarg != ',') {
				crash("missing comma in -n option near '%s'.\n", optarg);
			}

			++optarg;
			overhead = (unsigned) strtoul(optarg, &optarg, 0);
			break;

		case 'b':
			wifi = 1;
			kprintf ("WiFi enabled\n");
			break;

		case 'W':
			/* Enable WDT */
			wdt_enable();
			break;

		case 'G':
			/* GPMC NAND flash */
			gpmc_nand = 1;
			break;

		case 'g':
			/* GPMC NOR flash */
			gpmc_nor = 1;
			break;

		case 'V':
			/*
			 * Enable VOUT3 FDP interface
			 * Either 'G' or 'g' can't be specified with 'V' at the same time
			 */
			gpmc_vout3 = 1;
			break;

		case 'F':
			/*
			 * By default GPMC pads connect to GPMC devices
			 * If in order to enable FPD-Link (vout3)
			 * need to configure PINMUX and configure PCF8575 to drive SEL_GPMC_AD_VID_S0 high
			 */
			break;

		case 'J':
			/*
			 * JAMR3 application board is attached
			 * also SW2 needs to be at OFF-OFF
			 */
			jamr3 = 1;
			break;

		case 'a':
			/*
			 * Change ABE DPLL to 196MHz, instead the default 361Mhz,
			 * also feed propriate functional clock to McASP devices
			 * so that they can work at 48Mhz or 8Khz sampling rate
			 * Note: this option breaks the radio which works at 44.1Khz
			 */
			abe_dpll_196m = 1;
			break;
		default:
			handle_common_option(opt);
			break;
		}
	}

	/*
	 * Collect information on all free RAM in the system
	 */
	init_raminfo();

	/* These allocations MUST be completed in this order */
	if (link_present) {
		allocate_dsplink_memory(linkmem_addr, linkmem_size);
	}

	/*
	 * Set CPU frequency
	 */
	if (cpu_freq == 0) {
		unsigned processor_speed;
		unsigned max_processor_speed;
		calc_omap_dpll_and_set_freq(&processor_speed, &max_processor_speed);
		cpu_freq = processor_speed;
	}

	/*
	 * Remove RAM used by modules in the image
	 */
	alloc_ram(shdr->ram_paddr, shdr->ram_size, 1);

	/*
	 * Initialise SMP
	 */
	init_smp();

	if (shdr->flags1 & STARTUP_HDR_FLAGS1_VIRTUAL) {
		init_mmu();
	}

	init_intrinfo();

	init_qtime_dra74x();

	init_cacheattr();

	init_cpuinfo();

	/* board-specific hw init */
	hw_init();

	/* Initialize the Hwinfo section of the Syspage */
	init_hwinfo();

	/* Update system page with nanospin calibration values entered */
	if (hundred_loop_time != 0) {
		init_nanospin();
	}

	// The string has to contain "DRA7" or "J6" in it since some drivers (USB/sdmmc)
	// rely on this to identify the SoC.
	add_typed_string(_CS_MACHINE, "TI-DRA74X-EVM");

	dump_clocks();

	/*
	 * Load bootstrap executables in the image file system and Initialise
	 * various syspage pointers. This must be the _last_ initialisation done
	 * before transferring control to the next program.
	 */
	init_system_private();

	/*
	 * This is handy for debugging a new version of the startup program.
	 * Commenting this line out will save a great deal of code.
	 */
	print_syspage();

	return 0;
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/boards/dra74x/vayu-evm/main.c $ $Rev: 765418 $")
#endif
