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
 * Touch manager implementation
 */

#include <gui_manager.h>
#ifdef TOUCH_SUPPORT
#include <sw_touch_screen.h>
#endif
#include <touch_manager.h>
#include <color_manager.h>

/*
#define SW_DEBUG_TOUCH_MANAGER 1
*/

/* Function to activate touch*/
/**
* @brief 
*
* @return 
*/
touch activate_touch()
{
#ifndef TOUCH_SUPPORT
	sw_printf("Touch not defined\n");
	return NULL;
#else
#ifdef SW_DEBUG_TOUCH_MANAGER
	sw_printf("Touch defined\n"); 
#endif

	sw_strcpy(touch_info.last_touched, "\0");
	sw_i2c_init();
	struct __xy xy = get_coordinates();
	static s32int count = -1, previous_count = -1;
	
#ifdef SW_DEBUG_TOUCH_MANAGER
/*
	xy.point_x[0] = 505;
	xy.point_y[0] = 390;
*/
#endif
	xy.point_x[0] = (xy.point_x[0]*1024)/0xfff;
	xy.point_y[0] = (xy.point_y[0]*600)/0xfff;
#ifdef SW_DEBUG_TOUCH_MANAGER
	sw_printf("\(x, y\)  %x, %x\n", (gui_info.screen_height-xy.point_y[0]), 
			xy.point_x[0]);
#endif

	if(count != -1)
		previous_count = touch_info.active_textbox;
	
	count = check_key(gui_info.screen_height-xy.point_y[0], xy.point_x[0]);
	if(count<0) {
		sw_strcpy(touch_info.last_touched, "\0");
		sw_strcpy(touch_info.type, "\0");
		touch_info.x = 0;
		touch_info.y = 0;
		touch_info.x2 = 0;
		touch_info.y2 = 0;
	}
	else if(count >= 0) {
		sw_strcpy(touch_info.last_touched, obj_val.obj_id[count]);
		touch_info.x = obj_val.obj_coord[count][0];
		touch_info.y = obj_val.obj_coord[count][1];
		touch_info.x2 = obj_val.obj_coord[count][2];
		touch_info.y2 = obj_val.obj_coord[count][3];
		sw_strcpy(touch_info.type, obj_val.obj_type[count]);
		if((sw_strcmp(touch_info.type, "textfield\0") == 0) ||
				(sw_strcmp(touch_info.type, "password\0") == 0)) {
			touch_info.active_textbox = count;
			set_inactive_textbox_layout(previous_count);
			set_active_textbox_layout(count);
		}
	}
	if(xy.state == PRESSED)
		return (void *)event.on_touch;
	else
		return (void *)event.on_release;
#endif
}

/*Function to check if the clicked area is inside the window*/
/**
* @brief 
*
* @param x
* @param y
*
* @return 
*/
s32int check_touched_area(s32int x, s32int y)
{
	if((x > obj_val.window_x1) && (x < obj_val.window_x2) 
			&& (y > obj_val.window_y1) && (y < obj_val.window_y2))
		return 1;
	return 0;
}

/*Function to get the key value for the given coordinates*/
/**
* @brief 
*
* @param x
* @param y
*
* @return 
*/
s32int check_key(s32int x, s32int y)
{
	s32int count = obj_val.obj_count-1;
	while(count!=-1)
	{
		if((obj_val.obj_coord[count][0] <= x) 
				&& (obj_val.obj_coord[count][1] <= y)
				&& (obj_val.obj_coord[count][2] >= x) 
				&& (obj_val.obj_coord[count][3] >= y))
			break;

		count--;
	}
	return count;
}
