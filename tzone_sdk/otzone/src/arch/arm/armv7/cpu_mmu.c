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
 */

#include <cache.h>
#include <cpu.h>
#include <page_table.h>
#include <cpu_mmu.h>
#include <sw_mem_functions.h>
#include <sw_assert.h>

extern u32* pagetable_addr;

/**
 * @brief MMU initialization routine
 *
 */
void mmu_init(void)
{
    dmb();
    clear_icache();
    clear_data_cache();
    flush_tlb_all();
    dmb();
    isb();
    set_ttbcr(0);
}

/**
 * @brief Set access domain
 *
 * @param domain: Domain 
 * @param access: Domain access type 
 */
void set_domain(u8 domain, access_type access)
{
    /* Domain is a two bit field 
        00 = no access, 
        01=client, 
        10=reserved, 
        11=manager 
    */
    u32 value;
    value = read_domain();  
    //clear the current domain
    u32 mask = ~(0b11 << (domain*2));
    value = value & mask;
    //Set the domain
    value = value | (access << ((domain)*2));
    write_domain(value);
}

/**
 * @brief Set TTBR0 register
 *
 * @param addr Value to set for TTBR0 register
 */
void mmu_insert_pt0(u32 addr)
{

    addr |= TTBR_FLAGS;
    write_ttbr0(addr);
}

/**
 * @brief Set TTBR1 register
 *
 * @param addr Value to set for TTBR1 register
 */
void mmu_insert_pt1(u32* addr)
{
    write_ttbr1(addr);
}


/**
 * @brief Get TTBR0 register value
 *
 * @return TTBR0 register value
 */
u32* mmu_get_pt0()
{
    u32 regval = 0;
    regval = read_ttbr0();
    return (u32*)regval;
}

/**
 * @brief Get TTBR1 register value
 *
 * @return TTBR0 register value
 */
u32* mmu_get_pt1()
{
    u32 regval = 0;
    regval = read_ttbr1();
    return (u32*)regval;
}


/**
 * @brief Enable virtual address space for secure kernel
 */
void mmu_enable_virt_addr()
{
    u32 reg_val;
    reg_val = read_sctlr();
    reg_val |= SCTLR_M;
    reg_val |= SCTLR_C;
    reg_val |= SCTLR_I;
    write_sctlr(reg_val);
}

/**
 * @brief Disable virtual address space of secure kernel
 */
void mmu_disable_virt_addr()
{
    u32 reg_val, mask;
    reg_val = read_sctlr();
    
    /* clear MMU, I-Cache and D-Cache */    
    mask = (SCTLR_M | SCTLR_C | SCTLR_I);
    reg_val = reg_val & (~mask);
    write_sctlr(reg_val);
}

/**
 * @brief Returns whether MMU is enabled or not
 *
 * @return MMU enabled flag
 */
bool is_mmu_enabled()
{
    u32 reg_val;
    reg_val = read_sctlr();
    return (reg_val & SCTLR_M) ? TRUE : FALSE;
}

/**
 * @brief Returns the physical address of virtual address based on secure world
 * page table
 *
 * @param va: Virtual address  
 * @return Physical address
 */
pa_t va_to_pa(va_t va)
{
    pa_t pa;
    asm volatile("mcr p15, 0, %0, c7, c8, 0\n"::"r"(va):"memory", "cc");


    asm volatile("isb \n\t" 
                 " mrc p15, 0, %0, c7, c4, 0\n\t" 
                 : "=r" (pa) : : "memory", "cc"); 

    return (pa & 0xfffff000) | (va & 0xfff);
}

/**
 * @brief Returns the physical address of virtual address based on  
 * non-secure world page table
 *
 * @param va: Virtual address  
 * @return Physical address
 */
pa_t va_to_pa_ns(va_t va)
{
    pa_t pa;
    asm volatile("mcr p15, 0, %0, c7, c8, 4\n"::"r"(va):"memory", "cc");


    asm volatile("isb \n\t" 
                 " mrc p15, 0, %0, c7, c4, 0\n\t" 
                 : "=r" (pa) : : "memory", "cc"); 

    return (pa & 0xfffff000) | (va & 0xfff);
}

/**
 * @brief Initialize and enable MMU
 *
 */
void cpu_mmu_enable(void)
{

    mmu_init();
    set_ttbcr(0);
    mmu_insert_pt0(pagetable_addr);
    /*set the domain (access control) for the secure pages */
    set_domain(SECURE_ACCESS_DOMAIN, client);
    dsb();
    isb();
    mmu_enable_virt_addr();
}


