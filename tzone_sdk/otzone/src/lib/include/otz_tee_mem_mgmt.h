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
#ifndef __OTZ_TEE_MEM_MGMT_H_
#define __OTZ_TEE_MEM_MGMT_H_

/**
 * @brief 
 */
enum TEE_mem_access_rights_error_codes {

	TEE_MEMORY_ACCESS_READ = 0xFFFF5002,
	TEE_MEMORY_ACCESS_WRITE = 0xFFFF5003,
	TEE_MEMORY_ACCESS_ANY_OWNER = 0xFFFF5004
};

/*
 * @brief
 *
 * @param accessFlags
 * @param buffer
 * @param size
 */
//TEE_Result TEE_CheckMemoryAccessRights( u32 accessFlags, void* buffer, size_t size );

/*
 * @brief
 *
 * @param instanceData
 */
//void TEE_SetInstanceData( void* instanceData );

/*
 * @brief
 *
 * @return
 */
//void* TEE_GetInstanceData( void );

/*
 * @brief
 *
 * @param size
 * @param hint
 *
 * @return
 */
void* TEE_Malloc( size_t size, u32 hint );

/*
 * @brief
 *
 * @param buffer
 * @param newSize
 *
 * @return
 */
void* TEE_Realloc( void* buffer, u32 newSize );

/*
 * @brief
 *
 * @param buffer
 */
void TEE_Free( void *buffer );

/*
 * @brief
 *
 * @param dest
 * @parm src
 * @param count
 */
void  TEE_MemMove( void* dest, void* src, u32 count );

/*
 * @brief
 *
 * @param src
 * @param dest
 * @param length
 *
 * @return
 */
int TEE_MemCompare( void* src, void* dest, u32 length);

/*
 * @brief
 *
 * @param buffer
 * @param x
 * @param size
 */
void  TEE_MemFill( void* buffer, u32 x, u32 size);

/**
 * @brief 
 *
 * @param dest
 * @param src
 * @param count
 */
void  TEE_MemCpy( void* dest, void* src, u32 count );

#endif
