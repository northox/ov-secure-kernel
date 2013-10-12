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
 * Header for Memory management functions
 *
 */

#ifndef __LIB__MEM_FUNCTIONS_H__
#define __LIB__MEM_FUNCTIONS_H__

#include <sw_types.h>
#include <sw_buddy.h>

/**
 * @brief 
 *
 * @param ptr
 */
void sw_malloc_free(void *ptr);

/** Initialization function for head managment */

/**
 * @brief 
 *
 */
void sw_malloc_int(void);

/**
 * @brief 
 *
 * @param size
 *
 * @return 
 */
u32 sw_malloc_bytes(u32 size);

/**
 * @brief 
 *
 * @param dest
 * @param src
 * @param count
 *
 * @return 
 */
void * sw_memmove(void * dest,const void *src, u32 count);

/**
 * @brief 
 *
 * @param dest
 * @param c
 * @param count
 *
 * @return 
 */
void * sw_memset(void * dest, u32 c, u32 count);

/**
 * @brief 
 *
 * @param dst
 * @param src
 * @param count
 *
 * @return 
 */
void * sw_memcpy(void *dst, const void *src, u32 count);

/**
 * @brief 
 */
void sw_dump_mallocs(void);

/**
 * @brief 
 *
 * @param pointer
 */
void sw_free(void *pointer);

/**
* @brief 
*
* @param src
* @param dest
* @param length
*
* @return 
*/
int sw_memcmp(void *src, void *dest, u32 length);

/**
* @brief
* Copies the values of Num bytes from the location pointed by source directly 
* to the memory block pointed by destination By checking the values of 
* Num bytes with destination and source Maximum Length
*
* @param dst - Pointer to the destination array where the content is to be
*              copied
* @param src - Pointer to the source of data to be copied
* @param count - Number of bytes to copy
* @param dstLen - destination Maximum Length
* @param srcLen - source Maximum Length
*
* @return 
*/
void *sw_memncpy(void *dst, const void *src, u32 count,u32 dstLen,u32 srcLen);

#endif
