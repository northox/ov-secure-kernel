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
 * TIMER implementation
 *
 */
#ifndef __OTZ_TEE_TIMER_H_
#define __OTZ_TEE_TIMER_H_

#include <otz_tee_core_api.h>

/**
 * @brief 
 */
typedef struct { 
		uint32_t seconds;
		uint32_t milli_seconds;
	}TEE_Time;


/**
 * @brief 
 *
 * @param time
 */
void TEE_GetSystemTime( TEE_Time* time );

/**
 * @brief 
 *
 * @param timeout
 *
 * @return 
 */
TEE_Result TEE_Wait( u32 timeout );

/**
 * @brief 
 *
 * @param time
 *
 * @return 
 */
TEE_Result TEE_SetTAPersistentTime( TEE_Time* time);

/**
 * @brief 
 *
 * @param time
 *
 * @return 
 */
TEE_Result TEE_GetTAPersistentTime( TEE_Time* time);

/**
 * @brief 
 *
 * @param time
 */
void TEE_GetREETime( TEE_Time* time);
