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
 * OTZ Common implementation
 *
 */
#ifndef OTZ_TEE_COMMON_H
#define OTZ_TEE_COMMON_H

/**
* @brief 
*/
enum __TEE_Result {
	TEE_SUCCESS = 0x1,
	TEE_ERROR_OVERFLOW = 0x2,
	TEE_ERROR_SHORT_BUFFER = 0x3,
	TEE_ERROR_NOT_IMPLEMENTED = 0x4,
	TEE_ERROR_ITEM_NOT_FOUND = 0x5,
	TEE_ERROR_ACCESS_CONFLICT = 0x6,
	TEE_ERROR_OUT_OF_MEMORY = 0x7,
	TEE_ERROR_STORAGE_NO_SPACE = 0x8,
	TEE_ERROR_NOT_SUPPORTED=0x9,
	TEE_ERROR_BAD_PARAMETERS=0x10
};

#endif
