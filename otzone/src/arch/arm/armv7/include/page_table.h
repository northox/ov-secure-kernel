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
 * Header for secure page table initalization
 */

#ifndef __CPU_PAGETABLE_H__
#define __CPU_PAGETABLE_H__

#include <sw_types.h>
#include <sw_list.h>

#define PAGE_TABLE_ENTRIES 4096
#define PAGE_TABLE_ENTRY_WIDTH 4
#define PAGE_TABLE_SIZE (PAGE_TABLE_ENTRIES * PAGE_TABLE_ENTRY_WIDTH)


#define SECURE_ACCESS_DOMAIN    0

#define L1_SECTION_AP_MASK  0x3

/* XN Bit Values */
#define L1_EXECUTE_NEVER    1
#define L1_EXECUTE_ALLOW    0

#define L1_TYPE_MASK            0x3
#define L1_TYPE_FAULT           0x0
#define L1_TYPE_COARSE          0x1
#define L1_TYPE_SECTION         0x2

#define L1_SECTION_B_SHIFT  2
#define L1_SECTION_C_SHIFT  3
#define L1_SECTION_XN_SHIFT 4
#define L1_DOMAIN_SHIFT     5
#define L1_SECTION_AP_SHIFT 10
#define L1_SECTION_TEX_SHIFT    12
#define L1_SECTION_AP2_SHIFT    15
#define L1_SECTION_S_SHIFT  16
#define L1_SECTION_NG_SHIFT 17  /* Non-Global bit*/
#define L1_SECTION_NS_SHIFT 19  /* Non-Secure bit*/
#define L1_SECTION_NS_MASK  0x1 
#define L1_SECTION_NS_IDX_MASK  (L1_SECTION_NS_MASK << L1_SECTION_NS_SHIFT)
#define L1_DOMAIN_MASK          0xf
#define L1_DOMAIN_IDX_MASK      (L1_DOMAIN_MASK << L1_DOMAIN_SHIFT)

#define SECTION_BASE_MASK       0xfff00000

#define FSR_TYPE_MASK           0x40f
#define FSR_ALIGN_FAULT         0x1
#define FSR_EXT_ABORT_L1        0xc
#define FSR_EXT_ABORT_L2        0xe
#define FSR_TRANS_SEC           0x5
#define FSR_TRANS_PAGE          0x7
#define FSR_DOMAIN_SEC          0x9
#define FSR_DOMAIN_PAG          0xb
#define FSR_PERM_SEC            0xd
#define FSR_PERM_PAGE           0xf

#define FSR_DOMAIN_MASK         0xf0
#define FSR_WNR_MASK            0x800
#define FSR_EXT_MASK            0x1000

/* 
 * Page table flags used by memory mapping APIs 
 * 0 - 2 : protection 
 * 3     : exec flag
 * 4 - 31: unused
 */
#define PTF_PROT_KRW   (PRIV_RW_USR_NO)
#define PTF_PROT_KRO   (PRIV_RO_USR_NO)
#define PTF_PROT_URW   (PRIV_RW_USR_RW)
#define PTF_PROT_URO   (PRIV_RW_USR_RO)
#define PTF_EXEC       (1 << PTF_EXEC_SHIFT)


#define PTF_PROT_MASK  0x7
#define PTF_EXEC_MASK  0x8
#define PTF_EXEC_SHIFT 0x3

#define ptf_to_ap(x)   ((x) & PTF_PROT_MASK)
#define ptf_to_xn(x)   ((~(x) & PTF_EXEC_MASK) >> PTF_EXEC_SHIFT)

struct devmap {
	va_t dv_va;
	pa_t dv_pa;
	u32  dv_size;
};

/**
 * @brief Memory access control constants
 */
enum mem_access_ctrl
{
/*!Priv no access, usr no access */
  PRIV_NO_USR_NO = 0b000,   
/*!Priv read/write, usr no access */
  PRIV_RW_USR_NO = 0b001,   
/*!Priv read/write, usr read only */
  PRIV_RW_USR_RO = 0b010,   
/*!Priv read/write, usr read/write */
  PRIV_RW_USR_RW = 0b011,   
/*!Reserved */
  AP_RESERVED = 0b100,
/*!Priv read only, usr no access */
  PRIV_RO_USR_NO= 0b101,    
/*!Deprecated */
  DEPRECATED=0b110,         
/*!Priv read only, usr read only */
  PRIV_RO_USR_RO= 0b111,    
};
typedef enum mem_access_ctrl ACCESS_TYPE;

/**
 * @brief Section page table entry
 */
struct cpu_section {
/*! Virtual address */
    va_t va;
/*! Physical address */
    pa_t pa;
/*! Size */
    size_t sz;
/*! NS bit */
    u32 ns:1;
/*! NG bit */
    u32 ng:1;
/*! Shared bit */
    u32 s:1;
/*! Tex bits */
    u32 tex:3;
/*! AP bit */
    u32 ap:3;
/*! Implementation defined */
    u32 imp:1;
/*! Domain field */
    u32 dom:4;
/*! XN bit */
    u32 xn:1;
/*! Cache bit */
    u32 c:1;
/*! Bufferable bit */
    u32 b:1;
/*! Padding */
    u32 pad:15;
};

/**
* @brief Page table reference
*/
struct sw_page_ref {
/*! List head */
    struct list head;
/*! Page table entry */
    u32 l1_pte;
/*! Reference count */
    u32 ref_cnt;
};

/**
 * @brief Returns the secure page table pointer
 *
 * @return Pointer to page table
 */
u32* get_secure_ptd(void);
/**
 * @brief Initialize page table entries
 *
 * This function creates the initial page table entries for secure kernel
 * @return Pointer to the page table
 */
u32* map_secure_page_table(void);
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
int __map_to_ns(pa_t phy_addr, va_t *va_addr);

/**
 * @brief 
 *  Wrapper function for __map_to_ns
 * @param phy_addr
 * @param va_addr
 *
 * @return 
 */
int map_to_ns(pa_t phy_addr, va_t *va_addr);

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
int __unmap_from_ns(va_t va_addr);
/**
 * @brief 
 *      Wrapper function for __unmap_from_ns
 * @param va_addr
 *
 * @return 
 */
int unmap_from_ns(va_t va_addr);

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

int map_device_table(const struct devmap *dt);

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

int map_user_access_device_table(const struct devmap *dt);

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

int map_secure_memory(va_t va, pa_t pa, u32 size, u32 ptf);

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

int unmap_secure_memory(va_t va, u32 size);



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

int map_nsmemsect_normal(va_t, pa_t, u32);


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

int unmap_nsmemsect_normal(va_t, u32);

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
int map_section_entry(u32 *pgd, struct cpu_section *entry);
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
int map_ns_section_entry(u32 *pgd, struct cpu_section *entry);

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
int unmap_ns_section_entry(u32 *pgd, va_t va);


/**
* @brief 
*
* @param phy_addr
* @param va_addr
*
* @return 
*/
int map_to_secure(pa_t phy_addr, va_t va_addr);

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
int page_ref_add(u32 l1_pte);

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
int page_ref_release(u32 l1_pte);
#endif
