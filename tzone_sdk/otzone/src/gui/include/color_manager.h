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
 * Header for color manager implementation
 */

#ifndef __COLOR_MANAGER_H__
#define __COLOR_MANAGER_H__

#include <sw_types.h>

#define MAX_COLOR_VALUES 8
#define BLACK 0
#define WHITE 1
#define RED 2
#define GREEN 3
#define BLUE 4
#define GRAY 5
#define NAVY 6
#define DARK_BLUE 7

#define BPP_32_VALUE 32
#define BPP_24_VALUE 24
#define BPP_16_VALUE 16

/**
* @brief 
*
* @param input_color
*
* @return 
*/
u32 calc_contrast_color (u32 input_color);

/**
* @brief 
*
* @param col
*
* @return 
*/
u32 color(s32int col);

/**
* @brief 
*
* @param color
*
* @return 
*/
u16 bit32_to_bit16(u32 color);

/**
* @brief 
*
* @param color
*
* @return 
*/
u16 bit24_to_bit16(u32 color);

#endif
