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
 * button implementation
 */
#include <gui_manager.h>
#include <color_manager.h>

/*Function to draw button*/
/**
* @brief 
*
* @param id
* @param str
* @param width
* @param height
* @param color
*/
void button(u8* id, u8* str, s32int width, 
		s32int height, u32 color)
{

	if(!obj_val.end_inserting_obj) {
		compute_button_width(str);
		
		/* checking if the given width is greater than the default width. Else 
		 * width will be reassigned*/
		width = (width>obj_val.obj_width)?width:obj_val.obj_width; 

		/* Checking if the given width is less than the window width. Else 
		 * width will be reassigned*/
		width = (width > (obj_val.window_x2 - obj_val.window_x1))?
			(obj_val.window_x2 - obj_val.window_x1):width; 

		if(check_space_for_object(str, width, height)) {
			fill_rect(obj_val.obj_x, obj_val.obj_y, obj_val.obj_x+width,
					obj_val.obj_y+height, color);
			fill_rect(obj_val.obj_x+3, obj_val.obj_y+3, obj_val.obj_x+width,
					obj_val.obj_y+height, color);

			put_string(obj_val.obj_x+(sw_strlen(str)
						+(width-(sw_strlen(str)*10))/2), 
					obj_val.obj_y + (height-8)/2, str, sw_strlen(str),
					calc_contrast_color(color), NO_TEXT_BG);

			set_object_values(id, str, width, height, "button\0", color);
		}
	}
}

/*Function to draw button given x and y coordinates*/
/**
* @brief 
*
* @param x
* @param y
* @param str
* @param width
* @param height
* @param col
* @param id
*/
void button_on_given_coord(s32int x, s32int y, u8* str, s32int width, 
		s32int height, u32 col, u8* id)
{
	fill_rect(x, y, x+width, y+height, color(GRAY));
	fill_rect(x+3, y+3, x+width, y+height, col);

	put_string(x+(sw_strlen(str)+(width-(sw_strlen(str)*10))/2),
			y + (height-8)/2, str, sw_strlen(str), calc_contrast_color(col),
			NO_TEXT_BG);
	set_object_values_given_coord(id, str, x, y, width, height, "button\0", 
			col);
}
