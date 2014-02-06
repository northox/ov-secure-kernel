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
 * Header for virtual keyboard implementation
 */

#ifndef __KEYS_H_
#define __KEYS_H_

#include <sw_types.h>

/*
#define DEBUG_VK 1
*/

/**
* @brief 
*
* @param vk_active
*
* @return 
*/
int virtual_keyboard(s32int vk_active);

/**
* @brief 
*
* @return 
*/
int vk_app(void);

/**
* @brief 
*
* @param x
* @param y
* @param key
*/
void draw_key(s32int x, s32int y, s32int key);

/**
* @brief 
*
* @param width
* @param height
* @param data
* @param str
*/
void calc_bmp_array(s32int width, s32int height, u8* data, u8* str);

/**
* @brief 
*
* @param x
* @param y
* @param width
* @param height
* @param data
* @param str
*/
void calc_bmp_array_on_given_coord(s32int x, s32int y, s32int width, 
		s32int height, u8* data, u8* str);

/**
* @brief 
*
* @param x
* @param y
* @param width
* @param height
* @param bmp_array
*/
void custom_bmp(s32int x, s32int y, s32int width, 
		s32int height, u32 *bmp_array);
#endif
