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
 * Header for cache implementation
 */

#ifndef __ARMV7_CACHE_H__
#define __ARMV7_CACHE_H__

#include <sw_types.h>

/**
 * @brief 
 */
void v7_flush_dcache_all (void);

/**
 * @brief 
 *
 * @param l1_pte
 */
void flush_page_table_entry (u32 *l1_pte);

/**
 * @brief 
 *
 * @param va_addr
 */
void invalidate_data_cache_line (va_t va_addr);

/**
 * @brief 
 *
 * @param l2_pte
 * @param size
 */
void clean_dcache_area (u32* l2_pte, int size);

/**
 * @brief 
 */
void clean_invalidate_dcache (void);

/**
 * @brief 
 */
void flush_cache_all(void);

/**
 * @brief 
 */
void flush_tlb_all(void);
#endif
