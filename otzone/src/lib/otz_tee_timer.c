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

#include <otz_tee_timer.h>
#include <sw_timer.h>
#include <sw_types.h>
#include <otz_tee_core_api.h>


/**
 * @brief 
 *
 * @param time
 */
void TEE_GetSystemTime( TEE_Time* time ) {
	
	timeval_t temp_time;
	temp_time = read_timestamp();
	time.seconds = temp_time.tval.sec;
	time.milli_seconds = temp_time.tval.nsec/1000000;
	
}

/**
 * @brief 
 *
 * @param timeout
 *
 * @return 
 */
TEE Result TEE_Wait( u32 timeout ) {

	//To be implemented
}

/**
 * @brief 
 *
 * @param time
 *
 * @return 
 */
TEE_Result TEE_SetTAPersistentTime( TEE_Time* time) {

	//PersistentTime = time;
	/* To be implemented. Needs RTC implementation */
	/* After this implementation TEE_GetTAPersistentTime has to be modified. */
} 

/**
 * @brief 
 *
 * @param time
 *
 * @return 
 */
TEE_Result TEE_GetTAPersistentTime( TEE_Time* time) {

	/*
	if(PersistentTime == NULL) 
		time = NULL;
		return TEE_ERROR_TIME_NOT_SET;

	if(PersistentTime.seconds >= 0xFFFFFFFF) {
		PersistentTime.seconds = 0x0;
		time = PersistentTime;
		return TEE_ERROR_OVERFLOW;
	}
	// TEE_ERROR_TIME_NEEDS_RESET --> To be implemented. Needs RTC
	// implementation
	 
	time = PersistentTime;
	return TEE_SUCCESS;
	*/
}

/**
 * @brief 
 *
 * @param time
 */
void TEE_GetREETime( TEE_Time* time) {
	/* To be implemented. Needs RTC implemetation.*/
}
