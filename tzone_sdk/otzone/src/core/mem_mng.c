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
 * Simple memory manager for secure world
 */

#include <sw_types.h>
#include <sw_board.h>
#include <sw_mem_functions.h>
#include <sw_debug.h>
#include <page_table.h>
#include <cpu_data.h>
#include <cpu_mmu.h>
#include <mem_mng.h>

/**
 * @brief
 *  This structure has information about the 
 *  memory regions in the secure world
 */
struct secure_world_mem_info{
    /*! Start address of virtual memory bitmap */
    u32* vir_mem_bmap;
    /*! Start address of physical memory bitmap */
    u32* phy_mem_bmap;

    /*!Length of virtual memory bitmap*/
    size_t vir_mem_bmap_len;
    /*!Length of physical memory bitmap*/
    size_t phy_mem_bmap_len;

    /*! Start address of virtual memory region */
    u32* vir_addr_start;
    /*! Size of virtual address region*/
    size_t vir_memregion_size;

    /*! Start address of secure RAM */
    u32 phy_addr_start;
    /*! Size of secure RAM */
    size_t phy_memregion_size;

    /*! Number of 4K pages available in virtual memory region configured*/
    int num_phy_pag_free;
    /*! Number of 4K pages available in RAM */
    int num_vir_pag_free;
};

struct secure_world_mem_info sw_mem_info;

/**
 * @brief 
 *  Initializes the address space in secure world by allocating 
 *  and mapping the required pages 
 * @return 
 *      Returns the address of the secure world page table
 */
u32* sw_meminfo_init(void)
{
    u32 sw_code_len = 0;
    u32 map_pa , map_va; 
    u32 alc_phy_add = 0;
    u32 alc_vir_add = 0;
    u32 tot_len_to_alloc; 

    sw_memset( &sw_mem_info, 0, sizeof(sw_mem_info));
    
    sw_mem_info.phy_addr_start      =    get_secure_ram_start_addr();
    sw_mem_info.phy_memregion_size  =    get_secure_ram_end_addr() -
                                      get_secure_ram_start_addr() + 1;
    sw_mem_info.num_phy_pag_free = 
                    ((sw_mem_info.phy_memregion_size) >> PAGE_SHIFT);


    sw_mem_info.vir_addr_start = get_sw_code_start() ;
    sw_mem_info.vir_memregion_size = VIR_ASPACE_SIZE ;
    sw_mem_info.num_vir_pag_free =
                    ((sw_mem_info.vir_memregion_size) >> PAGE_SHIFT);

    if ( sw_mem_info.phy_addr_start & PAGE_OFFSET_MASK){
        sw_mem_info.phy_addr_start +=
            (PAGE_SIZE - (sw_mem_info.phy_addr_start & PAGE_OFFSET_MASK));
        sw_mem_info.phy_memregion_size -=
            (PAGE_SIZE - (sw_mem_info.phy_addr_start & PAGE_OFFSET_MASK));
    }

    if( sw_mem_info.phy_memregion_size & PAGE_OFFSET_MASK){
        sw_mem_info.phy_memregion_size -= 
             (sw_mem_info.phy_memregion_size & PAGE_OFFSET_MASK) ;
    }

    sw_mem_info.phy_mem_bmap_len = (sw_mem_info.phy_memregion_size >> 
                                            (PAGE_SHIFT + 3)) + 1;

    /* memory word alignment */
    if(sw_mem_info.phy_mem_bmap_len & 0x3) {
        sw_mem_info.phy_mem_bmap_len  += 
                    4 - (sw_mem_info.phy_mem_bmap_len & 0x3);
    }

    sw_mem_info.vir_mem_bmap_len = (sw_mem_info.vir_memregion_size >> 
                                            (PAGE_SHIFT + 3)) + 1;

    sw_mem_info.phy_mem_bmap = get_sw_code_end();

    sw_mem_info.vir_mem_bmap = (u32*)((u32)sw_mem_info.phy_mem_bmap
                                + sw_mem_info.phy_mem_bmap_len);



    sw_memset(sw_mem_info.phy_mem_bmap , 0 ,
            (sw_mem_info.phy_mem_bmap_len +sw_mem_info.vir_mem_bmap_len));
    sw_code_len = (u32)get_sw_code_end() - (u32)get_sw_code_start();

    map_pa = sw_mem_info.phy_addr_start; 
    map_va = (u32)sw_mem_info.vir_addr_start; 
    alc_phy_add = 0;
    alc_vir_add = 0;

    tot_len_to_alloc = ( sw_code_len +
                        sw_mem_info.phy_mem_bmap_len +
                        sw_mem_info.vir_mem_bmap_len);

    while( map_pa < (sw_mem_info.phy_addr_start + tot_len_to_alloc) )
    {
        alc_phy_add = ((map_pa - sw_mem_info.phy_addr_start) >> PAGE_SHIFT);

        sw_mem_info.phy_mem_bmap[alc_phy_add >> 5]
                    |= 0x1 << (31 - (alc_phy_add & 0x1F));

        sw_mem_info.num_phy_pag_free--;
        map_pa += PAGE_SIZE;
    }

    while( map_va < ((u32)sw_mem_info.vir_addr_start + tot_len_to_alloc) )
    {
        alc_vir_add = ((map_va - (u32)sw_mem_info.vir_addr_start) >> PAGE_SHIFT);

        sw_mem_info.vir_mem_bmap[alc_vir_add >> 5]
                    |= 0x1 << (31 - (alc_vir_add & 0x1F));


        sw_mem_info.num_vir_pag_free--;
        map_va += PAGE_SIZE;
    }
    return map_secure_page_table();
}

/**
 * @brief 
 *      Allocates physical pages for the required size
 *
 * @param size
 *      Size of the memory to be allocated
 *
 * @return 
 *  Returns the address of the memory allocated
 */
pa_t sw_phy_page_alloc(u32 size )
{
    int req_cnt,start = 0, iter=0, available;
    int avl_cnt=0, cnt; 
    u32 map_pa;  
    u32 phy_addr;

	/*Align to next page size */
	if(size & (PAGE_SIZE - 1)) 	
			size = (size & (~PAGE_SIZE - 1)) + PAGE_SIZE;
    req_cnt = size >> PAGE_SHIFT;

    if(req_cnt > sw_mem_info.num_phy_pag_free){
        serial_puts("SW: Page allocation failed \n");
        return 0;
    }

    available = 0;

    for(start = 0; 
        start < ((sw_mem_info.phy_mem_bmap_len << 3) - req_cnt);
        start++)
    {
        avl_cnt = 0;
        for(iter = start; iter < ( start + req_cnt) ; iter++){
            if(sw_mem_info.phy_mem_bmap[iter>>5] & (1 << (31-(iter & 0x1F))))
                    break;
            avl_cnt++;
        }
       if(avl_cnt == req_cnt){
           available = 1;
           break;
       }
    }

    if(!available)
        return 0;

    
    map_pa = start;
    
    for(cnt = 0;cnt < req_cnt; cnt++)
    {
        sw_mem_info.phy_mem_bmap[(start+cnt) >> 5] 
                |= 0x1 << (31 - ((start+cnt) & 0x1F));
        sw_mem_info.num_phy_pag_free--;
    }

    phy_addr = ((start <<  PAGE_SHIFT) + sw_mem_info.phy_addr_start);

    return phy_addr;
}

/**
 * @brief 
 *      Allocates physical pages for the required size
 *
 * @param size
 *      Size of the memory to be allocated
 *
 * @return 
 *  Returns the address of the memory allocated
 */
u32* sw_vir_page_alloc(u32 size )
{
    int req_cnt,start = 0, iter=0, available;
    int avl_cnt=0, cnt; 
    u32 map_va;  
    u32 vir_addr;
    u32 phy_addr;
	/*Align to next page size */
	if(size & (PAGE_SIZE - 1)) 	
			size = (size & (~(PAGE_SIZE -1) )) + PAGE_SIZE;
    req_cnt = size >> PAGE_SHIFT;

    if(req_cnt > sw_mem_info.num_vir_pag_free){
        serial_puts("SW: Page allocation failed \n");
        return NULL;
    }

    available = 0;

    for(start = 0; start < ((sw_mem_info.vir_mem_bmap_len << 3) - req_cnt);
                                                                     start++)
    {
        avl_cnt = 0;
        for(iter = start; iter < ( start + req_cnt) ; iter++){
            if(sw_mem_info.vir_mem_bmap[iter>>5] & (1 << (31-(iter & 0x1F))))
                    break;
            avl_cnt++;
        }
       if(avl_cnt == req_cnt){
           available = 1;
           break;
       }
    }

    if(!available)
        return NULL;
    
    phy_addr = (u32)sw_phy_page_alloc(size);
    if(phy_addr == 0){
        return NULL;
    }
    
    map_va = start;
    
    for(cnt = 0;cnt < req_cnt; cnt++)
    {
        sw_mem_info.vir_mem_bmap[(start+cnt) >> 5] 
               |= 0x1 << (31 - ((start+cnt) & 0x1F));
        sw_mem_info.num_vir_pag_free--;
    }

    vir_addr = ((start << PAGE_SHIFT) + (u32)sw_mem_info.vir_addr_start);
    u32 sec_phy_addr = phy_addr;
    u32 sec_vir_addr = vir_addr;

    u32 end_addr = ((phy_addr + size) & SECTION_MASK);
    u32 start_addr = (phy_addr & SECTION_MASK);

    while(end_addr >= start_addr){
        map_to_secure(sec_phy_addr, sec_vir_addr);
        size-= SECTION_SIZE;
        start_addr += SECTION_SIZE;
        sec_phy_addr += SECTION_SIZE;
        sec_vir_addr += SECTION_SIZE;
    }

    return (u32*)vir_addr;
}

/**
 * @brief 
 *      Reserves the required pages in the bitmap
 *
 * @param phy_addr
 *      start address of the area to be reserved
 * @param size
 *      size of the area to be reserved
 *
 * @return 
 *  Returns zero on success
 */
int sw_phy_addr_reserve(pa_t phy_addr , int size)
{
    u32 res_pa;
    u32 temp_sz,pg_cnt;
    int avl_cnt;

    if(phy_addr < sw_mem_info.phy_addr_start ||
           phy_addr > ( sw_mem_info.phy_addr_start + sw_mem_info.phy_memregion_size)){
        return -1;
    }

    pg_cnt = (size >> PAGE_SHIFT);
    res_pa = (phy_addr - sw_mem_info.phy_addr_start) >> PAGE_SHIFT;
    temp_sz = size;
 
    avl_cnt = 0;

    u32 chk_pa = res_pa;
    for( ;chk_pa < (res_pa + pg_cnt); chk_pa++)
    {
        if(sw_mem_info.phy_mem_bmap[chk_pa >> 5] & 
                        ( 0x1 << (31 - (chk_pa & 0x1F)))){
            break;
        }
        avl_cnt++;
    }

    if(avl_cnt != pg_cnt)
        return -1;

    while(temp_sz > 0){

        sw_mem_info.phy_mem_bmap[res_pa >> 5] |=
            0x1 << (31 - (res_pa & 0x1F));
        temp_sz -= PAGE_SIZE;
        sw_mem_info.num_phy_pag_free--;
    }

    return 0;

}

/**
 * @brief 
 *      Reserves the required pages in the bitmap
 *
 * @param phy_addr
 *      start address of the area to be reserved
 * @param size
 *      size of the area to be reserved
 *
 * @return 
 *  Returns zero on success
 */
int sw_vir_addr_reserve(u32* vir_addr , int size)
{
    u32 res_va;
    u32 temp_sz,pg_cnt;
    int avl_cnt;

    if(vir_addr < sw_mem_info.vir_addr_start ||
           vir_addr > ( sw_mem_info.vir_addr_start + sw_mem_info.vir_memregion_size)){
        return -1;
    }

    pg_cnt = (size >> PAGE_SHIFT);
    res_va = (vir_addr - sw_mem_info.vir_addr_start) >> PAGE_SHIFT;
    temp_sz = size;
 
    avl_cnt = 0;

    u32 chk_va = res_va;
    for( ;chk_va < (res_va + pg_cnt); chk_va++)
    {
        if(sw_mem_info.vir_mem_bmap[chk_va >> 5] & 
                        ( 0x1 << (31 - (chk_va & 0x1F)))){
            break;
        }
        avl_cnt++;
    }

    if(avl_cnt != pg_cnt)
        return -1;

    while(temp_sz > 0){

        sw_mem_info.vir_mem_bmap[res_va >> 5] |=
            0x1 << (31 - (res_va & 0x1F));
        temp_sz -= PAGE_SIZE;
        sw_mem_info.num_vir_pag_free--;
    }

    return 0;

}

/**
 * @brief 
 *      Frees the pages
 * @param phy_addr
 *      start address of the pages to be freed
 * @param size
 *      Size of the memory to be freed
 *
 * @return 
 *      Returns zero on success 
 */
int sw_phy_addr_free(pa_t phy_addr , int size)
{
    u32 fr_phy;
    int tmp_cnt;

    if(phy_addr < sw_mem_info.phy_addr_start ||
        phy_addr > (sw_mem_info.phy_addr_start + sw_mem_info.phy_memregion_size))
    {
        serial_puts("SW: Memory out of range\n");
        return -1;
    }


    fr_phy = (phy_addr - sw_mem_info.phy_addr_start) >> PAGE_SHIFT;

    tmp_cnt = fr_phy + (size >> PAGE_SHIFT);

    for( ; fr_phy < tmp_cnt ; fr_phy++)
    {
        sw_mem_info.phy_mem_bmap[fr_phy >> 5] &=
            ~(0x1 << (31 - (fr_phy & 0x1F)));
        sw_mem_info.num_phy_pag_free++;
    }
    return 0;

}

/**
 * @brief 
 *      Reserves the required pages in the bitmap
 *
 * @param vir_addr:
 *      start address of the area to be reserved
 * @param size:
 *      size of the area to be reserved
 *
 * @return 
 *  Returns zero on success
 */
int sw_vir_addr_free(u32 vir_addr , int size)
{
    u32 fr_vir;
    int tmp_cnt;
    pa_t phy_addr;

    if(vir_addr < (u32)sw_mem_info.vir_addr_start ||
        vir_addr > ((u32)sw_mem_info.vir_addr_start + sw_mem_info.vir_memregion_size))
    {
        serial_puts("SW: Memory out of range\n");
        return -1;
    }


    fr_vir = (vir_addr - (u32)sw_mem_info.vir_addr_start) >> PAGE_SHIFT;

    tmp_cnt = fr_vir + (size >> PAGE_SHIFT);

    for( ; fr_vir < tmp_cnt ; fr_vir++)
    {
        sw_mem_info.vir_mem_bmap[fr_vir >> 5] &=
            ~(0x1 << (31 - (fr_vir & 0x1F)));
        sw_mem_info.num_vir_pag_free++;
    }

    phy_addr = va_to_pa(vir_addr);

    /* TODO: The address should be unmapped from the page table
     *
     */

    sw_phy_addr_free(phy_addr , size);

    return 0;
}
