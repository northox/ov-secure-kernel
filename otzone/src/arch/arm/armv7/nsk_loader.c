/* 
 * OpenVirtualization: 
 * For additional details and support contact developer@sierraware.com.
 * Additional documentation can be found at www.openvirtualization.org
 * 
 * Copyright (C) 2011 SierraWare
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * Non secure Kernel Loader information
 */

/* Non secure Kernel Loader */

#include <nsk_boot.h>
#include <sw_types.h>
#include <page_table.h>
#include <mem_mng.h>
#include <sw_mem_functions.h>
#include <sw_debug.h>
#include <system_context.h>
#include <monitor.h>
#include <cpu.h>

#define MAP_SIZE SECTION_SIZE
#define ALIGN_MASK (~(MAP_SIZE - 1))

static void nsk_initrd_load(struct nsk_boot_info *);

/**
 * @brief 
 *
 * @return 
 */
int nsk_load(struct nsk_boot_info *ni)
{
	int i, error;
	u32 *addr;
	u32 ns_map_size, s_map_size, aligned_size;
	
	sw_printf("nsk load:");

	aligned_size = ni->nskbi_size;
	/* Align it to the next 1MB */
	if (aligned_size & (MAP_SIZE - 1)) { 
		aligned_size = (aligned_size & ALIGN_MASK) + MAP_SIZE;
	}
	
	ns_map_size = aligned_size;
	if (ni->nskbi_loadaddr & (MAP_SIZE - 1)) {
		ns_map_size += MAP_SIZE;
	}

	/* map the non secure area */
	error = map_nsmemsect_normal(ni->nskbi_loadaddr & ALIGN_MASK,
				     ni->nskbi_loadaddr & ALIGN_MASK,
				     ns_map_size);
	if (error < 0) {
		sw_printf("nsk_load: mapping failed\n");
		return error;
	}

	s_map_size = aligned_size;
	/* start address is assumed to be aligned */
	error = map_secure_memory(ni->nskbi_srcaddr, ni->nskbi_srcaddr, 
				  s_map_size, PTF_PROT_KRW);
	if (error < 0) {
		sw_printf("nsk_load: mapping failed\n");
		return error;
	}

	sw_printf("load addr = %x, startaddr = %x, size = %x\n", 
		  ni->nskbi_loadaddr, ni->nskbi_srcaddr, ni->nskbi_size);

	/* Move the ns kernel to the load address */
	sw_memcpy((void*)ni->nskbi_loadaddr, (void*)ni->nskbi_srcaddr, 
		  ni->nskbi_size);

	dsb();

	flush_cache_all();

	/* Unmap the non secure area */
	unmap_nsmemsect_normal(ni->nskbi_loadaddr, ns_map_size);

	unmap_secure_memory(ni->nskbi_srcaddr, s_map_size);

	/* Load initrd */
	if (ni->nskbi_initrd_flag) {
		nsk_initrd_load(ni);
	}

	sw_printf("Done\n");

	return 0;
}

/**
 * @brief 
 *
 * @return 
 */
static void nsk_initrd_load(struct nsk_boot_info *ni)
{
	int error;
	u32 aligned_size;

	sw_printf("initrd load:");

	aligned_size = ni->nskbi_size;
	/* Align it to the next 1MB */
	if (aligned_size & (MAP_SIZE - 1)) { 
		aligned_size = (aligned_size & ALIGN_MASK) + MAP_SIZE;
	}

	error = map_nsmemsect_normal(ni->nskbi_initrd_la, 
				     ni->nskbi_initrd_la, 
				     aligned_size);
	if (error < 0) {
		sw_printf("nsk_load: mapping failed\n");
	}

	error = map_secure_memory(ni->nskbi_initrd_sa, ni->nskbi_initrd_sa, 
				  aligned_size, PTF_PROT_KRW);
	if (error < 0) {
		sw_printf("nsk_load: mapping failed\n");
	}

	sw_printf("load addr = %x, startaddr = %x, size = %x\n", 
		  ni->nskbi_initrd_la, ni->nskbi_initrd_sa, 
		  ni->nskbi_initrd_size);
	
	/* Move the initrd to the load address */
	sw_memcpy((void*)ni->nskbi_initrd_la, 
		  (void*)ni->nskbi_initrd_sa,
		  ni->nskbi_initrd_size);
	
	unmap_nsmemsect_normal(ni->nskbi_initrd_la, 
			       aligned_size);

	unmap_secure_memory(ni->nskbi_initrd_sa, aligned_size);
}
