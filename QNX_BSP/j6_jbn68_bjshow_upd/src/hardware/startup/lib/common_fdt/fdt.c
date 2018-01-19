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

void        *fdt;
paddr_t     fdt_paddr;
size_t      fdt_size = 0;

void
fdt_init(paddr_t paddr)
{
    void     *header = 0;

    fdt_paddr = paddr;

    header = startup_memory_map(sizeof(struct fdt_header), fdt_paddr, PROT_READ);

    if(fdt_check_header(header) == 0) {
        fdt_size = fdt_totalsize(header);
    }

    startup_memory_unmap(header);

    if(fdt_size != 0) {
        fdt = startup_memory_map(fdt_size, fdt_paddr, PROT_READ);
    }
}

void
fdt_update(void)
{
    if(fdt != NULL) {
        as_add_containing(fdt_paddr, fdt_paddr + fdt_size - 1, AS_ATTR_ROM, "fdt", "memory");
        alloc_ram(fdt_paddr, fdt_size, 1);
    }
}

void
fdt_raminfo(void)
{
    if (fdt) {
        void            *f = fdt;
        int             node = fdt_path_offset(f, "/memory");
        int             acells = fdt_address_cells(f, 0);
        int             scells = fdt_size_cells(f, 0);
        const struct fdt_property   *prop = fdt_get_property(f, node, "reg", 0);

        if(prop && acells > 0 && scells > 0) {
            const fdt32_t   *mem = (const fdt32_t *)prop->data;
            int             nbanks = fdt32_to_cpu(prop->len) / (acells + scells) / sizeof *mem;

            while(nbanks--) {
                paddr_t         start, size;

                start = (acells == 1) ? fdt32_to_cpu(mem[0]) : fdt64_to_cpu(((fdt64_t *)mem)[0]);
                mem += acells;
                size = (scells == 1) ? fdt32_to_cpu(mem[0]) : fdt64_to_cpu(((fdt64_t *)mem)[0]);
                mem += scells;
                add_ram(start, size);
            }
            return;
        }
    }
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif
