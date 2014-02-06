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
 * put string implementation
 */

#include <gui_manager.h>
#include <color_manager.h>

/*Putting a char on the framebuffer*/
/**
* @brief 
*
* @param x
* @param y
* @param c
* @param color
*/
void put_char(s32int x, s32int y, u8 c,
		u32 color)
{
	s32int i, j, bits, offset;
	for (i = 0; i < FONT_SIZE; i++) {
		bits = fontdata_8x8[FONT_SIZE * c + i];

		for (j = 0; j < FONT_SIZE; j++) {
			if(gui_info.orientation == LANDSCAPE)
				offset = ((x + j) * (gui_info.bits_per_pixel / (FONT_SIZE * 2))
					+ (y + i) * gui_info.line_length)/2;
			else
				offset = ((y + i) * (gui_info.bits_per_pixel / (FONT_SIZE * 2))
					+ (gui_info.screen_height -x - j) * gui_info.line_length)/2;


			if (offset >= 0 && offset < (gui_info.smem_length) &&
					((bits >> (FONT_SIZE - j - 1)) & 1)) {
				sw_window_set_pixel(offset, color);	
			}
		}
	}
}

/*Putting a string on the framebuffer*/
/**
* @brief 
*
* @param x
* @param y
* @param str
* @param length
* @param col
* @param clear
*/
void put_string(s32int x, s32int y, u8 *str, s32int length,
		u32 col, s32int clear)
{
	s32int i;
	s32int w = 0;

	if (clear)
		fill_rect(x, y, x + (length * FONT_SIZE), y + FONT_SIZE, color(BLACK));

	for (i = 0; i < strlen(str) && i < length; i++) {
		put_char((x + FONT_SIZE * i), y, str[i], col);
		w += FONT_SIZE;
	}
}
