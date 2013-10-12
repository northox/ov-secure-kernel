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
 * Heap functions
 */

#include <sw_debug.h>
#include <sw_mem_functions.h>
#include <sw_buddy.h>
#include <mem_mng.h>

/**
 * @brief 
 *
 * @param ptr
 */
void sw_malloc_free(void* ptr)
{
    sw_free(ptr);
}


/**
 * @brief 
 */
void sw_dump_mallocs(void)
{
    sw_buddy_print_state();
}

/**
 * @brief 
 *
 */
void sw_malloc_int(void)
{
    u32 start_addr;
    u32 size = HEAP_SIZE; 

    int index;

    for(index = 0; index < NUM_OF_HEAPS; index++){
        
        start_addr = (u32)sw_vir_page_alloc(size);
        if(!(start_addr > 0))
            DIE_NOW(0,"Allocation for heap int failed\n");

#ifdef SW_MALLOC_DEBUG
  sw_printk("SW:sw_malloc_int(%08x, %x);\n", start_addr, size);
#endif

      if(sw_heap_init(index ,(void *)start_addr, size) < 0)
            DIE_NOW(0, "heap init failed\n");
    }
}

