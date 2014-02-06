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
 * secure page table initalization
 */

#include <sw_types.h>
#include <sw_debug.h>
#include <sw_assert.h>
#include <sw_mem_functions.h>
#include <cache.h>
#include <cpu_mmu.h>
#include <page_table.h>
#include <mem_mng.h>
#include <global.h>
#include <sw_list.h>

#include <uart.h>

#include <sw_board.h>

#ifdef CONFIG_KSPACE_PROTECTION
#include <sw_syscall.h>
#endif

#include <cpu_data.h>



/**
 * @brief Get secure page table pointer
 *
 * This function returns the page table pointer
 *
 * @return Pointer to the page table
 */
u32* get_secure_ptd(void)
{
    return secure_page_table;
}

/**
 * @brief Initialize page table entries
 *
 * This function creates the initial page table entries for secure kernel
 * @return Pointer to the page table
 */
u32* map_secure_page_table(void)
{

    /* clearing the page table */
    sw_memset(secure_page_table, 0, PAGE_TABLE_SIZE);

    board_map_secure_page_table(secure_page_table);
    
    return secure_page_table;
}

/**
* @brief 
*
* @param phy_addr
* @param va_addr
*
* @return 
*/
int map_to_secure(pa_t phy_addr, va_t va_addr)
{
    struct cpu_section desc;

    /* Normal cacheable memory */
    desc.va =  va_addr;
    desc.pa =  phy_addr;
    desc.dom = SECURE_ACCESS_DOMAIN;
    desc.ap = PRIV_RW_USR_RW;
    desc.xn = L1_EXECUTE_ALLOW;
    /* Outer and inner cache write back and write allocate */
    desc.tex = 0x1; 
    desc.c = 0x1;
    desc.b = 0x1;
    desc.ns = 0;
    desc.ng = 1;
#ifdef CONFIG_SW_MULTICORE
    desc.s = 1;
#else
    desc.s = 0;
#endif
    return map_section_entry(secure_page_table, &desc);
}

/**
 * @brief Create shared memory mapping between secure and non-secure kernel
 *
 * This function creates the page table entry with ns bit set. So that 
 * this section of the non-secure memory act like a shared memory.
 *
 * @param phy_addr: Physical address of the non-secure memory
 * @param va_addr:  Virtual address of the shared memory
 *
 * @return otz_return_t:
 * OTZ_OK \n
 * OTZ_FAIL \n
 */
int __map_to_ns(pa_t phy_addr, va_t *va_addr)
{
    int ret_val;
    struct cpu_section desc;
    desc.va =  phy_addr;
    desc.pa =  phy_addr;
    desc.dom = SECURE_ACCESS_DOMAIN;
    desc.ap = PRIV_RW_USR_RW;
    desc.xn = L1_EXECUTE_ALLOW;
    desc.tex = 0x1; 
    desc.c = 0x1;
    desc.b = 0x1;  
    desc.ns = 1;
    desc.ng = 0;
#ifdef CONFIG_SW_MULTICORE
    desc.s = 1;
#else
    desc.s = 0;
#endif

    ret_val = map_ns_section_entry((u32*)secure_page_table, &desc);
    if(!ret_val) {
        *va_addr = desc.va;
        return 0;
    }
    return -1;
}


/**
* @brief 
*
* @param phy_addr
* @param va_addr
*
* @return 
*/
int map_to_ns(pa_t phy_addr, va_t *va_addr)
{
#ifdef CONFIG_KSPACE_PROTECTION
    return __asm_map_to_ns(phy_addr,va_addr);
#else
    return  __map_to_ns(phy_addr,va_addr);
#endif
}

/**
 * @brief Map devices
 *
 * This function assumes that the start address and size are section aligned. 
 *
 * @param va: virtual address
 * @param pa: physical address
 * @size size: size
 *
 * @return otz_return_t:
 * OTZ_OK \n
 * OTZ_FAIL \n
 */

int map_device(va_t va, pa_t pa, u32 size)
{
	    struct cpu_section desc;

    if (size & (SECTION_SIZE - 1))
	    goto error;

    if (va & (SECTION_SIZE - 1))
	    goto error;
    
    if (pa & (SECTION_SIZE -1))
	    goto error;

    desc.dom = SECURE_ACCESS_DOMAIN;
    desc.ap = PRIV_RW_USR_NO;
    desc.xn = L1_EXECUTE_NEVER;
    /* 
     * Device 
     */
    desc.tex = 0x0; 
    desc.c = 0x0;
    desc.b = 0x1;  
    desc.ns = 0;
    desc.ng = 0;
#ifdef CONFIG_SW_MULTICORE
    desc.s = 1;
#else
    desc.s = 0;
#endif

    while(size) {
        desc.va =  va;
        desc.pa =  pa;
        map_section_entry(secure_page_table, &desc);

        va += SECTION_SIZE;
	pa += SECTION_SIZE;
	size -= SECTION_SIZE;
    }
    return 0;

error:
    return -1;

}

/**
 * @brief Map user access devices
 *
 * This function assumes that the start address and size are section aligned. 
 *
 * @param va: virtual address
 * @param pa: physical address
 * @size size: size
 *
 * @return otz_return_t:
 * OTZ_OK \n
 * OTZ_FAIL \n
 */

int map_user_access_device(va_t va, pa_t pa, u32 size)
{
	    struct cpu_section desc;

    if (size & (SECTION_SIZE - 1))
	    goto error;

    if (va & (SECTION_SIZE - 1))
	    goto error;
    
    if (pa & (SECTION_SIZE -1))
	    goto error;

    desc.dom = SECURE_ACCESS_DOMAIN;
    desc.ap = PRIV_RW_USR_RW;
    desc.xn = L1_EXECUTE_NEVER;
    /* 
     * Device 
     */
    desc.tex = 0x0; 
    desc.c = 0x0;
    desc.b = 0x1;  
    desc.ns = 0;
    desc.ng = 0;
#ifdef CONFIG_SW_MULTICORE
    desc.s = 1;
#else
    desc.s = 0;
#endif

    while(size) {
        desc.va =  va;
        desc.pa =  pa;
        map_section_entry(secure_page_table, &desc);

        va += SECTION_SIZE;
	pa += SECTION_SIZE;
	size -= SECTION_SIZE;
    }
    return 0;

error:
    return -1;

}


/**
 * @brief Map device table
 *
 *
 * @param dt: pointer to array of device mappings
 *            Array has to terminated with zero.
 *
 * @return otz_return_t:
 * OTZ_OK \n
 * OTZ_FAIL \n
 */

int map_device_table(const struct devmap *dt)
{
	while (*(u32 *)dt != 0) {
		map_device(dt->dv_va, dt->dv_pa, dt->dv_size);
		dt++;
	};
	return;
}


/**
 * @brief Map User Access device table
 *
 *
 * @param dt: pointer to array of device mappings
 *            Array has to terminated with zero.
 *
 * @return otz_return_t:
 * OTZ_OK \n
 * OTZ_FAIL \n
 */

int map_user_access_device_table(const struct devmap *dt)
{
	while (*(u32 *)dt != 0) {
		map_user_access_device(dt->dv_va, dt->dv_pa, dt->dv_size);
		dt++;

	};
	return;
}

/**
 * @brief Map a range of secure memory area
 *
 * This function assumes that the start address and size are section aligned. 
 *
 * @param va: virtual address
 * @param pa: physical address
 * @param flags: flags for access permission and exec
 *
 * @return otz_return_t:
 * OTZ_OK \n
 * OTZ_FAIL \n
 */

int map_secure_memory(va_t va, pa_t pa, u32 size, u32 ptf)
{
    struct cpu_section desc;

    if (size & (SECTION_SIZE - 1)) 
	    goto error;

    if (va & (SECTION_SIZE - 1))
	    goto error;
    
    if (pa & (SECTION_SIZE -1))
	    goto error;


    desc.dom = SECURE_ACCESS_DOMAIN;
    desc.ap = ptf_to_ap(ptf);
    desc.xn = ptf_to_xn(ptf);
    /* Default memory attributes:
     * - Outer and inner cache write back and write allocate
     * - Global 
     * - sharable attribute based on the number of cores
     */
    desc.tex = 0x1; 
    desc.c = 0x1;
    desc.b = 0x1;  
    desc.ns = 0;
    desc.ng = 0;
#ifdef CONFIG_SW_MULTICORE
    desc.s = 1;
#else
    desc.s = 0;
#endif

    while(size) {
        desc.va =  va;
        desc.pa =  pa;
        /* Normal cacheable memory */
        map_section_entry(secure_page_table, &desc);

        va += SECTION_SIZE;
	pa += SECTION_SIZE;
	size -= SECTION_SIZE;
    }

    return 0;

error:
    return -1;
}

/**
 * @brief Unmap a range of secure memory area 
 *
 * This function assumes that the start address and size are section aligned. 
 *
 * @param va: virtual address
 * @param size: size
 *
 * @return otz_return_t:
 * OTZ_OK \n
 * OTZ_FAIL \n
 */

int unmap_secure_memory(va_t va, u32 size)
{

    if (size & (SECTION_SIZE - 1))
	    return -1;

    if (va & (SECTION_SIZE - 1))
	    return -1;
    
    while(size) {
        unmap_section_entry(secure_page_table, va);
        va += SECTION_SIZE;
	size -= SECTION_SIZE;
    }
    return 0;
}


/**
 * @brief Map a range of non secure memory area as normal memory with 
 * attributes Outer and Inner write back, write allocate. 
 *
 * This function assumes that the start address and size are section aligned. 
 *
 * @param va: virtual address
 * @param pa: physical address
 * @param size: size
 *
 * @return otz_return_t:
 * OTZ_OK \n
 * OTZ_FAIL \n
 */

int map_nsmemsect_normal(va_t va, pa_t pa, u32 size)
{
    struct cpu_section desc;

    if (size & (SECTION_SIZE - 1))
	    goto error;

    if (va & (SECTION_SIZE - 1))
	    goto error;
    
    if (pa & (SECTION_SIZE -1))
	    goto error;

    /* Normal cacheable memory */
    desc.dom = SECURE_ACCESS_DOMAIN;
    desc.ap = PRIV_RW_USR_NO;
    desc.xn = L1_EXECUTE_NEVER;
    /* Outer and inner cache write back and write allocate */
    desc.tex = 0x1; 
    desc.c = 0x1;
    desc.b = 0x1;  
    desc.ns = 1;
    desc.ng = 1;
    desc.s = 0;

    while(size) {
        desc.va =  va;
        desc.pa =  pa;
        /* Normal cacheable memory */
        map_ns_section_entry(secure_page_table, &desc);

        va += SECTION_SIZE;
	pa += SECTION_SIZE;
	size -= SECTION_SIZE;
    }
    return 0;

error:
    return -1;

}


/**
 * @brief Unmap a range of non secure memory area 
 *
 * This function assumes that the start address and size are section aligned. 
 *
 * @param va: virtual address
 * @param size: size
 *
 * @return otz_return_t:
 * OTZ_OK \n
 * OTZ_FAIL \n
 */

int unmap_nsmemsect_normal(va_t va, u32 size)
{

    if (size & (SECTION_SIZE - 1))
	    return -1;

    if (va & (SECTION_SIZE - 1))
	    return -1;
    
    while(size) {
	/* Normal cacheable memory */
        unmap_ns_section_entry(secure_page_table, va);

        va += SECTION_SIZE;
	size -= SECTION_SIZE;
    }
    return 0;
}

/**
 * @brief Create a section map entry
 *
 * This function creates section map entry in supplied page table
 *
 * @param pgd: Pointer to page table
 * @param entry: Pointer to section entry
 *
 * @return otz_return_t:
 * OTZ_OK \n
 * OTZ_FAIL \n
 */
int map_section_entry(u32 *pgd, struct cpu_section *entry)
{
    u32 l1_index = 0;
    u32 *l1_pte;

    l1_index = entry->va >> 20;
    l1_pte = pgd + l1_index;

    switch ((*l1_pte) & L1_TYPE_MASK) {
    case (L1_TYPE_FAULT):
        *l1_pte = ( entry->pa & SECTION_BASE_MASK) | L1_TYPE_SECTION;
        *l1_pte |= (entry->dom << L1_DOMAIN_SHIFT);
        *l1_pte |= (entry->b << L1_SECTION_B_SHIFT);
        *l1_pte |= (entry->c << L1_SECTION_C_SHIFT);
        *l1_pte |= (entry->tex << L1_SECTION_TEX_SHIFT);
        *l1_pte |= (entry->xn << L1_SECTION_XN_SHIFT);
        *l1_pte |= ((entry->ap & L1_SECTION_AP_MASK) << L1_SECTION_AP_SHIFT);
        *l1_pte |= (((entry->ap >> 2) & 0x1) << L1_SECTION_AP2_SHIFT);
        *l1_pte |= (entry->s << L1_SECTION_S_SHIFT);
        *l1_pte |= (entry->ng << L1_SECTION_NG_SHIFT);
        *l1_pte |= (entry->ns << L1_SECTION_NS_SHIFT);

/*		
		flush is not needed - page table is cacheable 
*/
        break;
    case (L1_TYPE_SECTION):
/*
	Change in flags need to be handled  
*/
        break;
    default:
        sw_printk("SW: section_map_hva_to_hpa: This function supports "
                      "only section mappings.\n");
        sw_printk("SW:   L1 descriptor: %08x\n", *l1_pte);
        return -1;
    }

    return 0;
}

/**
 * @brief Map the non-secure memory in secure page table for shared memory 
 * access
 *
 * This function creates the section mapping with 'NS' bit set to create a 
 * shared memory access.
 *
 * @param pgd: Pointer to page table
 * @param entry: Pointer to section entry
 *
 * @return otz_return_t:
 * OTZ_OK \n
 * OTZ_FAIL \n
 */
int map_ns_section_entry(u32 *pgd, struct cpu_section *entry)
{
    u32 l1_index = 0;
    u32 *l1_pte;

    l1_index = entry->va >> 20;
    l1_pte = pgd + l1_index;

    switch ((*l1_pte) & L1_TYPE_MASK) {
    case (L1_TYPE_FAULT):
        *l1_pte = ( entry->pa & SECTION_BASE_MASK) | L1_TYPE_SECTION;
        *l1_pte |= (entry->dom << L1_DOMAIN_SHIFT);
        *l1_pte |= (entry->b << L1_SECTION_B_SHIFT);
        *l1_pte |= (entry->c << L1_SECTION_C_SHIFT);
        *l1_pte |= (entry->tex << L1_SECTION_TEX_SHIFT);
        *l1_pte |= (entry->xn << L1_SECTION_XN_SHIFT);
        *l1_pte |= ((entry->ap & L1_SECTION_AP_MASK) << L1_SECTION_AP_SHIFT);
        *l1_pte |= (((entry->ap >> 2) & 0x1) << L1_SECTION_AP2_SHIFT);
        *l1_pte |= (entry->s << L1_SECTION_S_SHIFT);
        *l1_pte |= (entry->ng << L1_SECTION_NG_SHIFT);
        *l1_pte |= (entry->ns << L1_SECTION_NS_SHIFT);
/*		
		flush is not needed - page table is cacheable 
*/
        page_ref_add((u32)l1_pte);
        break;
    case (L1_TYPE_SECTION):
        if(!(*l1_pte & L1_SECTION_NS_IDX_MASK)) {
            *l1_pte |= (1 << L1_SECTION_NS_SHIFT);
/*		
		flush is not needed - page table is cacheable 
*/
        }
        page_ref_add((u32)l1_pte);
        break;
    default:
        sw_printf("SW: section_map_hva_to_hpa: This function supports "
                      "only section mappings.\n");
        sw_printf("SW:   L1 descriptor: %08x\n", *l1_pte);
        return -1;
    }

    return 0;
}

/**
* @brief 
*
* @param pgd
* @param va
*
* @return 
*/
int unmap_section_entry(u32* pgd, va_t va)
{
    u32 l1_index = 0;
    u32* l1_pte;

    l1_index = va >> 20;

    l1_pte = pgd + l1_index;

    switch((*l1_pte) & L1_TYPE_MASK) {
        case (L1_TYPE_FAULT):
            break;
        case (L1_TYPE_SECTION):
            *l1_pte = 0;
/*		
		flush is not needed - page table is cacheable 
*/
            break;
        default :
            break;
    }
    return 0;
}

/**
 * @brief Unmap the shared section entry from secure page table
 *
 * This function unmaps the secure memory section from secure page table. 
 * i.e. set 'NS' bit to zero
 *
 * @param pgd: Pointer to page table
 * @param va: Virtual address to be unmapped
 *
 * @return otz_return_t:
 * OTZ_OK \n
 */
int unmap_ns_section_entry(u32 *pgd, va_t va)
{
    u32 l1_index = 0;
    u32 *l1_pte;

    l1_index = va >> 20;
    l1_pte = pgd + l1_index;

    switch ((*l1_pte) & L1_TYPE_MASK) {
        case (L1_TYPE_FAULT):
            break;
        case (L1_TYPE_SECTION):
            if(page_ref_release((u32)l1_pte) == 0) {
                if((*l1_pte & L1_SECTION_NS_IDX_MASK)) {
                      *l1_pte = 0;
/*		
		flush is not needed - page table is cacheable 
*/
                }
            }
            break;
        default:
            break;
    }
    return 0;
}

/**
 * @brief 
 *
 * @param va_addr
 *
 * @return 
 */
int unmap_from_secure(va_t va_addr)
{
    int ret_val;
    ret_val = unmap_section_entry((u32*)secure_page_table, va_addr);
    return ret_val;
}

/**
 * @brief Unmap the non-secure memory from secure page table
 *
 * This function unmaps the non-secure memory from secure page table. 
 * i.e. set 'NS' bit to zero
 *
 * @param va_addr: Virtual address to be unmapped
 *
 * @return otz_return_t:
 * OTZ_OK \n
 */
int __unmap_from_ns(va_t va_addr)
{
    int ret_val;
    ret_val = unmap_ns_section_entry((u32*)secure_page_table, va_addr);
    return ret_val;
}

/**
* @brief 
*
* @param va_addr
*
* @return 
*/
int unmap_from_ns(va_t va_addr)
{
#ifdef CONFIG_KSPACE_PROTECTION 
    return __asm_unmap_from_ns(va_addr);
#else
    return __unmap_from_ns(va_addr);
#endif
}


/**
* @brief Increment page table reference of shared memory
*
* This function increment the reference count of mapped shared memory
*
* @param l1_pte: Page table entry
*
* @return otz_return_t:
* OTZ_OK \n
* OTZ_FAIL \n
*/
int page_ref_add(u32 l1_pte)
{
    struct list *l;
    struct sw_global *pglobal = &global_val;
    struct sw_page_ref *page_ref = NULL;
    bool found = FALSE;

    if (!list_empty(&pglobal->page_ref_list)) {
        list_for_each(l, &pglobal->page_ref_list) {
            page_ref = list_entry(l, struct sw_page_ref, head);
            if (page_ref->l1_pte == l1_pte) {
                found = TRUE;
                break;
            }
        }
    }

    if (!found) {
        page_ref = (struct sw_page_ref*)sw_malloc_private(COMMON_HEAP_ID,
                                        sizeof(struct sw_page_ref));
        if(!page_ref) {
            sw_printf("SW: page reference allocation: malloc failed\n");
            return OTZ_EFAIL; 
        }
        page_ref->l1_pte = l1_pte;
        page_ref->ref_cnt = 0;
        list_add_tail(&pglobal->page_ref_list, &page_ref->head);
    } 

    page_ref->ref_cnt++;
#ifdef PAGETABLE_DBG
    sw_printf("SW: add refer count 0x%x and l1_pte 0x%x\n", page_ref->ref_cnt, page_ref->l1_pte);
#endif
    return OTZ_OK;   
}

/**
* @brief Decrement page table reference of shared memory
*
* This function decrement the reference count of mapped shared memory
*
* @param l1_pte: Page table entry
*
* @return otz_return_t:
* OTZ_OK \n
* OTZ_FAIL \n
*/
int page_ref_release(u32 l1_pte)
{   
    int ref_cnt;
    struct list *l;
    struct sw_global *pglobal = &global_val;
    struct sw_page_ref *page_ref = NULL;
    bool found = FALSE;

    if (!list_empty(&pglobal->page_ref_list)) {
        list_for_each(l, &pglobal->page_ref_list) {
            page_ref = list_entry(l, struct sw_page_ref, head);
            if (page_ref->l1_pte == l1_pte) {
                found = TRUE;
                break;
            }
        }
    }

    if (!found) {
        sw_printf("SW: no page reference found\n");
        return OTZ_EFAIL;
    } 

    page_ref->ref_cnt--;

    ref_cnt = page_ref->ref_cnt;

#ifdef PAGETABLE_DBG
    sw_printf("SW: refer count 0x%x and l1_pte 0x%x\n", ref_cnt, page_ref->l1_pte);
#endif

    if(page_ref->ref_cnt == 0)
    {
        list_del(&page_ref->head);  
        sw_free_private(COMMON_HEAP_ID, page_ref);
    }
    return ref_cnt;   
}



