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


#include "dra74x_ipl.h"

typedef unsigned word_t;

#define PATTERN_LENGTH		3

/*
 * This function executes the Data bus test
 * @param addr	any address in dram address range
 */
int dramtest_walk_data(word_t addr)
{
	word_t pattern_a, pattern_b;

	volatile word_t *px = (word_t *) addr;
	volatile word_t *py = (word_t *) (addr + 8);

	int err_count = 0;

	for (pattern_a = 1; pattern_a; pattern_a <<= 1) {
		pattern_b = ~pattern_a;

		*px = pattern_a;
		*py = pattern_b;

		if (*px != pattern_a) {
			ser_putstr((char *)"\nERROR in Pattern A");
			err_count+= 1;
		}

		*px = 0;

		if (*py != pattern_b) {
			ser_putstr((char *)"\nERROR in Pattern B");
			err_count+= 1;
		}
	}

	return err_count;
}

/*
 * This function executes the Address bus test
 * @param baseAddr	dram start address
 * @param nBytes		size of dram to test
 */
int dramtest_walk_address(volatile word_t * baseAddr, unsigned long nBytes)
{
	unsigned long addrMask = (nBytes / sizeof(word_t) - 1);
	unsigned long offset;
	unsigned long testOffset;
	int err_count = 0;
	word_t pattern = (word_t) 0xAAAAAAAA;
	word_t antipattern = (word_t) 0x55555555;

	// Write the default pattern at each of the power-of-two offsets.
	for (offset = 1; (offset & addrMask) != 0; offset <<= 1) {
		baseAddr[offset] = pattern;
	}

	//Check for address bits stuck high.
	testOffset = 0;
	if (! (((((unsigned long)&baseAddr[testOffset]) >= 0x80000000)
		&& (((unsigned long)&baseAddr[testOffset]) <= 0x8000CFFF))
		|| ((((unsigned long)&baseAddr[testOffset]) >= 0x88000000)
		&& (((unsigned long)&baseAddr[testOffset]) <= 0x88003FFF)))) {
		baseAddr[testOffset] = antipattern;
	}

	for (offset = 1; (offset & addrMask) != 0; offset <<= 1) {
		if (baseAddr[offset] != pattern) {
			ser_putstr((char *)"\nERROR in Address Test");
			err_count++;
		}
	}

	// Check for address bits stuck low or shorted.
	for (testOffset = 1; (testOffset & addrMask) != 0; testOffset <<= 1) {
		baseAddr[testOffset] = antipattern;
		for (offset = 1; (offset & addrMask) != 0; offset <<= 1) {
			if ((baseAddr[offset] != pattern) && (offset != testOffset)) {
				ser_putstr((char *)"\nERROR in Address Test");
				err_count++;
			}
		}
		baseAddr[testOffset] = pattern;
	}

	return err_count;
}

/*
 * This function executes the Inversion test. This may take
 * some time to execte. This function reads and verifies
 * the patterns written to the entire dram memory.
 * @param start_addr	dram start address
 * @param memsize		size of dram to test
 */
int dram_inversion_test(unsigned int start_addr, unsigned int memsize)
{
	word_t pattern_a[PATTERN_LENGTH], pattern_b[PATTERN_LENGTH];
	word_t *ptr;
	int err_count = 0;
	int count;
	int i;

	pattern_a[0] = 0x1f9e587c;
	pattern_a[1] = 0xd35877e0;
	pattern_a[2] = 0xe061a783;

	for (i = 0; i < PATTERN_LENGTH; i++) {
		pattern_b[i] = ~pattern_a[i];
	}

	// Fill entire memory with pattern_a array elements
	ptr = (word_t*) start_addr;

	for (count = memsize / sizeof(word_t); count > 0; count--) {
		i = (unsigned) ptr % PATTERN_LENGTH;
		*ptr = pattern_a[i];
		ptr++;
	}

	watchdog_toggle();

	// Ensure each word is correct, and then write the b_pattern word
	ptr = (word_t*) start_addr;
	for (count = memsize / sizeof(word_t); count > 0; count--) {
		i = (unsigned) ptr % PATTERN_LENGTH;

		if (*ptr != pattern_a[i]) {
			err_count++;
			ser_putstr((char *)"\nERROR in Pattern A");
		}

		*ptr++ = pattern_b[i];
	}

	//	Now the same backwards
	watchdog_toggle();
	ptr-= 1;

	for (count = memsize / sizeof(word_t); count > 0; count--) {
		i = (unsigned) ptr % PATTERN_LENGTH;

		if (*ptr != pattern_b[i]) {
			err_count++;
			ser_putstr((char *)"\nERROR in Pattern B");
		}

		*ptr-- = 0;
	}

	return err_count;
}

int sdram_test(unsigned int dram_addr, unsigned int dram_size)
{
	int dram_result = 0;

	ser_putstr("\nExecuting DRAM TEST, starting from address 0x");
	ser_puthex(dram_addr);
	ser_putstr(" length 0x");
	ser_puthex(dram_size);
	ser_putstr("\nExecuting data walk test ...");

	// DRAM Data Bus Test
	watchdog_toggle();
	dram_result = dramtest_walk_data(dram_addr);

	if (dram_result) {
		ser_putstr("Data walk test failed. Result:	0x");
		ser_puthex(dram_result);
		ser_putstr("\n");
		return dram_result;
	} else {
		ser_putstr("Data walk test succedded. \nExecuting address walk test ... ");
	}

	// DRAM Address Test
	watchdog_toggle();
	dram_result = dramtest_walk_address((word_t *)dram_addr, dram_size);
	if (dram_result) {
		ser_putstr("Address walk test failed. Result:	0x");
		ser_puthex(dram_result);
		ser_putstr("\n");
		return dram_result;
	} else {
		ser_putstr("Address walk test succedded. \n");
	}

	// DRAM Inversion Test
	unsigned int dram_off;
	unsigned int len;
	for (dram_off = dram_addr; dram_off <= dram_addr + dram_size; dram_off += MEG(1)) {
		ser_putstr("Inversion test starting address 0x");
		ser_puthex(dram_off);
		ser_putstr(" ... ");

		if ((dram_addr + dram_size - dram_off) > MEG(1))
			len = MEG(1);
		else
			len = dram_addr + dram_size - dram_off;

		watchdog_toggle();
		dram_result = dram_inversion_test(dram_off, len);
		if (dram_result) {
			ser_putstr("failed. Result:	0x");
			ser_puthex(dram_result);
			ser_putstr("\n");
			return dram_result;
		} else {
			ser_putstr("succedded\n");
		}
	}
	ser_putstr("Inversion walk test succedded\n");
	ser_putstr("\nDRAM TEST done.\n");
	return 0;
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/ipl/boards/dra74x/mem_test.c $ $Rev: 745291 $")
#endif
