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
 * memory management implementation
 *
 */

#include <sw_buddy.h>
#include <sw_types.h>
#include <sw_debug.h>
#include <libc_sos.h>
#include <sw_mem_functions.h>
#include <otz_tee_mem_mgmt.h>
#include <otz_tee_common.h>
#include <sw_types.h>

static void *Data;

/*TEE_Result TEE_CheckMemoryAccessRights(	u32 accessFlags, void* buffer, size_t size) {
    return TEE_SUCCESS;
}

void TEE_SetInstanceData( void* instanceData ) {
    Data = instanceData;
}

void* TEE_GetInstanceData( void ) {

	return Data;
}
*/
void* TEE_Malloc( size_t size, u32 hint ) {
	void* buffer;
	if((buffer = malloc(size))==NULL) {
		sw_printf("Memory Allocation Failed\n");
		return NULL;
	}
		
	if(hint == 0x0) {
		sw_memset(buffer, 0, size);
	}
	/*else {
	  hint in the range [0x00000001, 0xFFFFFFFF] can be used in future versions.
	}*/
	return buffer;
}

/**
 * @brief 
 *
 * @param buffer
 * @param newSize
 *
 * @return 
 */
void* TEE_Realloc( void* buffer, u32 newSize ) {
    return(realloc(buffer,newSize));
}

/**
 * @brief 
 *
 * @param buffer
 */
void TEE_Free( void *buffer ) {
    sw_free(buffer);
}

/**
 * @brief 
 *
 * @param dest
 * @param src
 * @param count
 */
void  TEE_MemMove( void* dest, void* src, u32 count ) {
    sw_memmove(dest, src, count);
}

/**
 * @brief 
 *
 * @param src
 * @param dest
 * @param length
 *
 * @return 
 */
int TEE_MemCompare( void* src, void* dest, u32 length) {
    return(sw_memcmp(src, dest, length));
}

/**
 * @brief 
 *
 * @param buffer
 * @param x
 * @param size
 */
void  TEE_MemFill( void* buffer, u32 x, u32 size) {
    sw_memset(buffer, x, size);
}

/**
 * @brief 
 *
 * @param dest
 * @param src
 * @param count
 */
void  TEE_MemCpy( void* dest, void* src, u32 count ) {
    sw_memcpy(dest, src, count);
}
