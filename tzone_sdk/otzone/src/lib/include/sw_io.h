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

/*
 * @brief header file for common io functions.
 */

#ifndef __LIB_IO_H_
#define __LIB_IO_H_

#include <sw_types.h>
#include <cpu_io.h>


/** Memory read/write legacy functions (Assumed to be Little Endian) */
/**
 * @brief 
 *
 * @param addr
 *
 * @return 
 */
static inline u8 sw_readb(volatile void *addr)
{
    return sw_cpu_in_8((u8 *) addr);
}

/**
 * @brief 
 *
 * @param data
 * @param addr
 */
static inline void sw_writeb(u8 data, volatile void *addr)
{
    sw_cpu_out_8((u8 *) addr, data);
}

/**
 * @brief 
 *
 * @param addr
 *
 * @return 
 */
static inline u16 sw_readw(volatile void *addr)
{
    return sw_cpu_in_le16((u16 *) addr);
}

/**
 * @brief 
 *
 * @param data
 * @param addr
 */
static inline void sw_writew(u16 data, volatile void *addr)
{
    sw_cpu_out_le16((u16 *) addr, data);
}

/**
 * @brief 
 *
 * @param addr
 *
 * @return 
 */
static inline u32 sw_readl(volatile void *addr)
{
    return sw_cpu_in_le32((u32 *) addr);
}

/**
 * @brief 
 *
 * @param data
 * @param addr
 */
static inline void sw_writel(u32 data, volatile void *addr)
{
    sw_cpu_out_le32((u32 *) addr, data);
}

#endif /* __LIB_IO_H_ */
