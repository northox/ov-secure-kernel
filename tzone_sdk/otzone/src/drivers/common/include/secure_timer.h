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
 * secure timer declarations.
 */

#ifndef __SECURE_TIMER_H__
#define __SECURE_TIMER_H__

#include <sw_types.h>


#define FREE_RUNNING_CNTR   0
#define TICK_TIMER          1


/**
* @brief 
*/
enum timer_mode{
    MODE_FREE_RUNNING,
    MODE_PERIODIC,
    MODE_ONESHOT
};

/**
* @brief 
*
* @param timer_base
*/
void secure_timer_init(u32 timer_base);

/**
* @brief 
*
* @param timer_base
* @param timer_module_index
*/
void secure_timer_clearirq(u32 timer_base, u32 timer_module_index);

/**
* @brief 
*
* @param timer_base
* @param timer_module_index
*/
void secure_timer_disable(u32 timer_base, u32 timer_module_index);

/**
* @brief 
*
* @param timer_base
* @param timer_module_index
*/
void secure_timer_enable(u32 timer_base, u32 timer_module_index);

/**
* @brief 
*
* @param timer_base
* @param tim_mod_index
*
* @return 
*/
u32 secure_timer_read_mis(u32 timer_base, u32 tim_mod_index);

/**
* @brief 
*
* @param timer_base
* @param tim_mod_index
*
* @return 
*/
u32 secure_timer_read_ris(u32 timer_base, u32 tim_mod_index);

/**
* @brief 
*
* @param timer_base
* @param tim_mod_index
* @param mode
* @param usecs
*/
void secure_set_timer(u32 timer_base, u32 tim_mod_index, u32 mode, u32 usecs);

/**
* @brief 
*
* @param timer_base
* @param tim_mod_index
*
* @return 
*/
u32 secure_read_timer(u32 timer_base, u32 tim_mod_index);


/**
* @brief 
*
* @param irq
*
* @return 
*/
void secure_timer_irq_handler(int irq);

#endif /* __VE_RS1_TIMER_H__ */

