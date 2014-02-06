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
 * window implementation
 */
#include <gui_manager.h>

/*Function to draw window*/
/**
* @brief 
*
* @param x1
* @param y1
* @param x2
* @param y2
* @param title
* @param color
* @param titlebar
*/
void set_window(s32int x1, s32int y1, s32int x2, s32int y2, 
		u8* title, u32 color, s32int titlebar)
{
	if(titlebar == NO_TITLE_BAR)
	{
		fill_rect(x1, y1, x2, y2, color);
		set_window_values(x1, y1, x2, y2, titlebar, color);
	}
	else
	{
		fill_rect(x1, y1, x2, y2, color);
		draw_line(x1, y1+20, x2, y1+20, calc_contrast_color(color));
		put_string(x1+15, y1+10, title, sw_strlen(title),
				calc_contrast_color(color), 0);
		set_window_values(x1, y1, x2, y2, titlebar, color);
	}
}
