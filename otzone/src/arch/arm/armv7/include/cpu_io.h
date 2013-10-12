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
 * header file for CPU I/O or Memory read/write functions
 * 
 */

#ifndef _TZ_CPU_IO_H__
#define _TZ_CPU_IO_H__

#include <sw_types.h>

#define __swab32(x) \
	((u32)( \
		(((u32)(x) & (u32)0x000000ffUL) << 24) | \
		(((u32)(x) & (u32)0x0000ff00UL) <<  8) | \
		(((u32)(x) & (u32)0x00ff0000UL) >>  8) | \
		(((u32)(x) & (u32)0xff000000UL) >> 24) ))

#define __be32_to_cpu(x) __swab32((x))



/**
 * @brief 
 *
 * @param addr
 *
 * @return 
 */
static inline u8 sw_cpu_in_8(volatile u8 * addr)
{
    return *addr;
}

/**
 * @brief 
 *
 * @param addr
 * @param data
 */
static inline void sw_cpu_out_8(volatile u8 * addr, u8 data)
{
    *addr = data;
}

/**
 * @brief 
 *
 * @param addr
 *
 * @return 
 */
static inline u16 sw_cpu_in_le16(volatile u16 * addr)
{
    return *addr;
}

/**
 * @brief 
 *
 * @param addr
 * @param data
 */
static inline void sw_cpu_out_le16(volatile u16 * addr, u16 data)
{
    *addr = data;
}

/**
 * @brief 
 *
 * @param addr
 *
 * @return 
 */
static inline u32 sw_cpu_in_le32(volatile u32 * addr)
{
    return *addr;
}

/**
 * @brief 
 *
 * @param addr
 * @param data
 */
static inline void sw_cpu_out_le32(volatile u32 * addr, u32 data)
{
    *addr = data;
}

#endif
