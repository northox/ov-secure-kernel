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
 * Simple Memory manager for secure world
 */

#ifndef __MEM_MNG_H_
#define __MEM_MNG_H_

#include <sw_types.h>

#define PAGE_SIZE               4096
#define PAGE_SHIFT              12
#define PAGE_MASK               0xFFFFF000
#define PAGE_OFFSET_MASK        0x00000FFF

#define SECTION_SIZE            (1 << 20)
#define SECTION_SHIFT           20
#define SECTION_MASK            0xFFF00000
#define SECTION_OFFSET_MASK     0x000FFFFF

#define VIR_ASPACE_SIZE     (1 << 26)

/**
 * @brief 
 *
 * @return 
 */
u32* sw_meminfo_init(void);

/**
 * @brief 
 *
 * @param size
 *
 * @return 
 */
pa_t sw_phy_page_alloc(u32 size );

/**
 * @brief 
 *
 * @param size
 *
 * @return 
 */
u32* sw_vir_page_alloc(u32 size );

/**
 * @brief 
 *
 * @param phy_addr
 * @param size
 *
 * @return 
 */
int sw_phy_addr_reserve(pa_t phy_addr , int size);

/**
 * @brief 
 *
 * @param vir_addr
 * @param size
 *
 * @return 
 */
int sw_vir_addr_reserve(u32* vir_addr , int size);

/**
 * @brief 
 *
 * @param phy_addr
 * @param size
 *
 * @return 
 */
int sw_phy_addr_free(pa_t phy_addr , int size);

/**
 * @brief 
 *
 * @param vir_addr
 * @param size
 *
 * @return 
 */
int sw_vir_addr_free(u32 vir_addr , int size);

#endif
