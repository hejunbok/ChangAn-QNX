/*
 * $QNXLicenseC:
 * Copyright 2008, QNX Software Systems. 
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

#include "startup.h"
#include <libfdt.h>

static paddr_t  qvm_fabric_base = 0;
static size_t   qvm_fabric_size = 0;
static int      qvm_fabric_irq = -1;
static void     *qvm_fabric = NULL;

void
qvm_init(void)
{
    if(fdt != NULL) {
        int         node = -1;

        while((node = fdt_node_offset_by_compatible(fdt, node, "qvm,qvm")) > 0) {
            int         len;
            const char  *name = fdt_get_name(fdt, node, &len);

            if(name && !strncmp(name, "hypervisor", sizeof("hypervisor")-1)) {
                const struct fdt_property   *prop = fdt_get_property(fdt, node, "reg", 0);

                in_hvc = 1;
                if(prop) {
                    int             acells = fdt_address_cells(fdt, 0);
                    int             scells = fdt_size_cells(fdt, 0);
                    const fdt32_t   *mem = (const fdt32_t *)prop->data;
                    int             ncells = fdt32_to_cpu(prop->len) / sizeof *mem;

                    if(ncells >= acells + scells && acells > 0 && scells > 0) {
                        qvm_fabric_base = (acells == 1) ? fdt32_to_cpu(mem[0]) : fdt64_to_cpu(((fdt64_t *)mem)[0]);
                        mem += acells;
                        qvm_fabric_size = (scells == 1) ? fdt32_to_cpu(mem[0]) : fdt64_to_cpu(((fdt64_t *)mem)[0]);
                        mem += scells;
                        qvm_fabric = startup_memory_map(qvm_fabric_size, qvm_fabric_base, PROT_READ|PROT_WRITE);
                    }
                }
                if((prop = fdt_get_property(fdt, node, "interrupts", 0))) {
                    const fdt32_t   *mem = (const fdt32_t *)prop->data;
                    int             nents = fdt32_to_cpu(prop->len) / sizeof *mem;

                    if(nents >= 3) {
                        qvm_fabric_irq = fdt32_to_cpu(mem[1]) + 16; // Skip over SGIs
                        if(fdt32_to_cpu(mem[0]) == 0) {
                            qvm_fabric_irq += 16;   // Add 16 for SPIs
                        }
                    }
                }
                break;
            }
        }
    }
}

void
qvm_update(void)
{
    if(qvm_fabric_size > 0) {
        hwi_add_device(HWI_ITEM_ROOT_HW, HWI_ITEM_DEVCLASS_MISC, "qvm", 0);
        hwi_add_location(qvm_fabric_base, qvm_fabric_size, 0, hwi_find_as(qvm_fabric_base, 1));
        if(qvm_fabric_irq > 0) {
            hwi_add_irq(qvm_fabric_irq);
        }
        if(debug_flag >= 2) {
            kprintf("qvm_fabric: %x:%x irq=%d\n", qvm_fabric_base, qvm_fabric_size, qvm_fabric_irq);
        }
    }
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif
