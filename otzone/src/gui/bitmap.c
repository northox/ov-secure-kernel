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
 * bit map implementation
 */
#include <gui_manager.h>

/*Function to draw bitmap on given coordinates*/
/**
* @brief 
*
* @param x
* @param y
* @param width
* @param height
* @param bmp_array
* @param id
*/
void bitmap_on_given_coord(s32int x, s32int y, s32int width, 
		s32int height, u32 *bmp_array, u8* id)
{
	s32int i, j, k = 0, offset = -1;
	for(i = 0; i < height; i++) {
		for(j = 0; j < width; j++) {
			if(gui_info.orientation == LANDSCAPE)
				sw_window_set_pixel(((i+y)*gui_info.screen_width+(j+x)), 
						bmp_array[k++]);
			else
				sw_window_set_pixel(((gui_info.screen_height-(j+x)) * 
							gui_info.screen_width+(i+y)), bmp_array[k++]);
		}
	}
	set_object_values_given_coord(id, "", x, y, width, height, "bmp\0", 0);
}

/*Function to draw bitmap*/
/**
* @brief 
*
* @param width
* @param height
* @param bmp_array
* @param id
*/
void bitmap(s32int width, s32int height, u32* bmp_array, u8* id)
{
	s32int i, j, k = 0, offset = -1;
	if(!obj_val.end_inserting_obj) {
		if(check_space_for_object("", width, height)) {

			for(i = 0; i < height; i++) {
				for(j = 0; j < width; j++) {
					if(gui_info.orientation == LANDSCAPE)
						offset = ((i+obj_val.obj_y)*gui_info.screen_width+
								(j+obj_val.obj_x));
					else
						offset = ((gui_info.screen_height - (j+obj_val.obj_x))*
								gui_info.screen_width+(i+obj_val.obj_y));

					sw_window_set_pixel(offset, bmp_array[k++]);	
				}
			}
			set_object_values(id, "", width, height, "bmp\0", 0);
		}
	}
}

