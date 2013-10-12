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
 *  Draw object implementation
 */

#include <gui_manager.h>

/*Function to draw pixel with the given color*/
/**
* @brief 
*
* @param x
* @param y
* @param color
*/
void put_pixel(s32int x, s32int y, u32 color) {

	s32int offset;
	if(gui_info.orientation == LANDSCAPE)
 		offset = (y*gui_info.screen_width+x);
	else
		offset = (gui_info.screen_height - x) * gui_info.screen_width + y;
	
	sw_window_set_pixel(offset, color);
}

/*Function to draw rect and fill it with the given color*/
/**
* @brief 
*
* @param x1
* @param y1
* @param x2
* @param y2
* @param color
*/
void fill_rect(s32int x1, s32int y1, s32int x2, s32int y2, u32 color)
{
    s32int x, y = y1;

	for(y = y1; y < y2; y++) {
   	    for (x = x1; x < x2; x++) {
			if(gui_info.orientation == LANDSCAPE)
				sw_window_set_pixel((y*gui_info.screen_width+x), color);
			else
				sw_window_set_pixel(((gui_info.screen_height - x) * 
							gui_info.screen_width+y), color);
       	}
	}
} 

/*Function to draw line*/
/**
* @brief 
*
* @param x1
* @param y1
* @param x2
* @param y2
* @param color
*/
void draw_line(s32int x1, s32int y1, s32int x2, s32int y2, u32 color)
{
    s32int x = 0, offset = -1;
	for(x = x1; x< x2; x++) {
		if(gui_info.orientation == LANDSCAPE)
			offset = y1*gui_info.screen_width+x;
		else
			offset = (gui_info.screen_height - x) * gui_info.screen_width + y1;
		
		sw_window_set_pixel(offset, color);
	}
}
