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
 * custom BMP implementation
 */
#include <virtual_keyboard.h>
#include <gui_manager.h>

static u8 pixel[(271 * 168) * 4 + 1];
static u32 bmp[(271 * 168)+1];

/**
* @brief 
*
* @param width
* @param height
* @param data
* @param str
*/
void calc_bmp_array(s32int width, s32int height, u8* data, u8* str)
{
	s32int i = width * height, c = 0, k = 0, j = 0;
	while(i-- > 0) {
		pixel[k+3] = ((((data[2] - 33) & 0x3) << 6) | ((data[3] - 33)));
		pixel[k+2] = ((((data[1] - 33) & 0xF) << 4) | ((data[2] - 33) >> 2));
		pixel[k+1] = (((data[0] - 33) << 2) | ((data[1] - 33) >> 4));
		pixel[k] = 0;
		data += 4;
		bmp[j] = (pixel[k+3]&0x000000ff) | 
			((pixel[k+2]&0x000000ff)<<8) | ((pixel[k+1]&0x000000ff)<<16) |
			((pixel[k]&0x000000ff)<<24);
		j++;
		k += 4;
	}
	bitmap(width, height, bmp, str);
}

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
		s32int height, u8* data, u8* str)
{
	s32int i = width * height, c = 0, k = 0, j = 0;
	while(i-- > 0) {
		pixel[k+3] = ((((data[2] - 33) & 0x3) << 6) | ((data[3] - 33)));
		pixel[k+2] = ((((data[1] - 33) & 0xF) << 4) | ((data[2] - 33) >> 2));
		pixel[k+1] = (((data[0] - 33) << 2) | ((data[1] - 33) >> 4));
		pixel[k] = 0;
		data += 4;
		bmp[j] = (pixel[k+3]&0x000000ff) | 
			((pixel[k+2]&0x000000ff)<<8) | ((pixel[k+1]&0x000000ff)<<16) |
			((pixel[k]&0x000000ff)<<24);
		j++;
		k += 4;
	}

	bitmap_on_given_coord(x, y, width, height, bmp, str);
}

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
		s32int height, u32 *bmp_array)
{
	s32int i, j, k = 0, offset = -1;
	for(i = height; i > 0; i--) {
		for(j = 0; j < width; j++) {
			if(gui_info.orientation == LANDSCAPE)
				sw_window_set_pixel(((i+y)*gui_info.screen_width+(j+x)),
						bmp_array[k++]);
			else
				sw_window_set_pixel(((gui_info.screen_height-(j+x)) * 
							gui_info.screen_width+(i+y)), bmp_array[k++]);
		}
	}
}
