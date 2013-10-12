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
 */

#ifndef __INCLUDE_FB_H__
#define __INCLUDE_FB_H__

#define BPP 1
#define SCREEN_WIDTH 2
#define SCREEN_HEIGHT 3

/**
* @brief 
*
* @param info_num
*
* @return 
*/
int get_sw_window_info(int info_num);

/**
* @brief 
*/
void sw_fb_init();

/**
* @brief 
*/
void sw_fb_exit();

/**
* @brief 
*
* @param offset
* @param color
*/
void sw_fb_write(int offset, unsigned int color);

/**
* @brief 
*
* @param width
* @param height
*
* @return 
*/
int init_lcd(int width, int height);

/**
* @brief 
*
* @param width
* @param height
*
* @return 
*/
void write_media_player_data(void);

#endif
