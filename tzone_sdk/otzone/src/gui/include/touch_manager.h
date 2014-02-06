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
 * Header for Touch manager implementation
 */

#ifndef __TOUCH_MANAGER_H__
#define __TOUCH_MANAGER_H__

#include <sw_types.h>

typedef void (*touch)();

/**
* @brief 
*/
struct touch_info {
	u8 last_touched[500];
	u32 x, x2;
	u32 y, y2;
	u8 type[500];
	s32 active_textbox;
}touch_info;

/**
* @brief 
*
* @return 
*/
touch activate_touch();
/**
* @brief 
*/
void on_touch(void);
/**
* @brief 
*/
void on_release(void);

#endif
