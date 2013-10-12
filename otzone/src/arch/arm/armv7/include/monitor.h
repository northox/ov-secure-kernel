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
 *  Monitor declarations
 */

#include "sw_types.h"

/**
 * @brief 
 *      Calls smc function with appropriate argument
 *      and hence non-secure kernel gets invoked on reaching the 
 *      non-secure side
 */
void invoke_ns_kernel(void);

/**
 * @brief
 *      Calls smc function with appropriate return value
 *      obtained by executing the secure api
 * @param retval
 */
void return_secure_api(u32 retval);

/**
 * @brief 
 *      The function actually executes the smc instruction
 *      It is always called in privileged mode
 * @param smc_arg
 */
void __execute_smc(u32 smc_arg);

/**
* @brief 
*/
void smc_nscpu_context_init(void);
