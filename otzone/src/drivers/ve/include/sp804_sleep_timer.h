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
 * SP804 timer declarations for use with various sleep functions
 * */

#ifndef __SP804_SLEEP_TIMER_H__
#define __SP804_SLEEP_TIMER_H__

#include <sw_types.h>
#include <sp804_timer.h>
#include <sw_io.h>
#include <gic.h>
#include <sw_board.h>

#define SLEEP_TIMER_OFFSET 0x20
#define SLEEP_TIMER_DURATION 100000 /*current clock speed is 100Mhz.*/

/**
* @brief 
*
* @param timer_base
*/
void sp804_sleep_timer_init(u32 timer_base);

/**
* @brief 
*
* @param timer_base
*/
void sp804_sleep_timer_enable(u32 timer_base);

/**
* @brief 
*
* @param timer_base
*
* @return 
*/
u32 sp804_sleep_timer_read_value(u32 timer_base);

/**
* @brief 
*
* @param timer_base
*/
void sp804_sleep_timer_irq_handler(u32 timer_base);

/**
* @brief 
*
* @return 
*/
u64 get_current_ticks();

#endif
