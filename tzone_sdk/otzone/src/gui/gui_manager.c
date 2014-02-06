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
 * GUI manager implementation
 */

#include <gui_manager.h>
#include <sw_fb.h>
//#include <sw_info.h>
#include <color_manager.h>

s32int gui_open = 1;

/*Function to set orientation. Default orientation is Landscape*/
/**
* @brief 
*
* @param orient
*/
void set_orientation(s32int orient)
{
	gui_info.orientation = orient;
}

/*Function to assign default values of the objects*/
/**
* @brief 
*/
void assign_obj_values()
{
	s32int i;
	for(i = 0; i <500; i++)
		sw_strcpy(clicked_text[i], "\0");

	obj_val.total_textbox = 0;
	obj_val.button_width = 60;
	obj_val.textfield_width = 400;
	obj_val.label_width = 20;
	obj_val.obj_height = 30;
	obj_val.end_inserting_obj = 0;
	obj_val.obj_count = 0;
	obj_val.space_bw_obj = 10;
	obj_val.curr_height = 30;
}

/*Function to start secure window and to get screen details*/
/**
* @brief 
*/
void sw_window_init()
{
	gui_open = 1;
	sw_fb_init();
	assign_obj_values();
#ifdef TOUCH_SUPPORT
	set_event_handler(NULL, NULL);
#endif
	gui_info.orientation = LANDSCAPE;
	gui_info.bits_per_pixel = get_sw_window_info(BPP);
	gui_info.screen_width = get_sw_window_info(SCREEN_WIDTH);
	gui_info.screen_height = get_sw_window_info(SCREEN_HEIGHT);
	gui_info.line_length = gui_info.screen_width * 2;
	gui_info.smem_length = gui_info.line_length * gui_info.screen_height;
}

/*Function to exit secure window*/
/**
* @brief 
*/
void sw_window_exit()
{
/*	if(gui_info.orientation == PORTRAIT) {
		sw_printf("EXITING: PORTRAIT\n");
		fill_rect(0, 0, 600, 1024, color(BLACK));
	}
	else {
		sw_printf("EXITING: LANDSCAPE\n");
		fill_rect(0, 0, 1024, 600, color(BLACK));
	}*/
	sw_fb_exit();
}

/*Function to set pixel of the secure window*/
/**
* @brief 
*
* @param offset
* @param color
*/
void sw_window_set_pixel(s32int offset, u32 color)
{
	sw_fb_write(offset, color);
}

/* Function to check if the given width has enough room for the text. Else width
 * will be reassigned.*/
/**
* @brief 
*
* @param str
*/
void compute_button_width(u8* str)
{
	s32int len = sw_strlen(str);
	obj_val.obj_width = (obj_val.button_width < (FONT_SIZE+2)*len)?
		((FONT_SIZE+2)*len):(obj_val.button_width);
}

/* Function to check if the given width has enough room for the text. Else width
 * will be reassigned.*/
/**
* @brief 
*
* @param str
*/
void compute_label_width(u8* str)
{
	s32int len = sw_strlen(str);
	obj_val.obj_width = (obj_val.label_width < (FONT_SIZE+2)*len)?
		((FONT_SIZE+2)*len):(obj_val.label_width);
}

/* Function to check if the given width has enough room for the text. Else width
 * will be reassigned.*/
/**
* @brief 
*
* @param str
*/
void compute_textfield_width(u8* str)
{
	s32int len = sw_strlen(str);
	obj_val.obj_width = (obj_val.textfield_width < (FONT_SIZE+2)*len)?
		((FONT_SIZE+2)*len):(obj_val.textfield_width);
}

/*Function to check if the window has space for the new object*/
/**
* @brief 
*
* @param str
* @param width
* @param height
*
* @return 
*/
s32int check_space_for_object(u8* str, s32int width, s32int height)
{
	if((obj_val.obj_x + width) > obj_val.window_x2) {
		obj_val.max_height = obj_val.curr_height;
		obj_val.curr_height = obj_val.obj_height;
		obj_val.obj_x = obj_val.window_x1+10;
		obj_val.obj_y += obj_val.max_height + obj_val.space_bw_obj;
		if(obj_val.obj_y > obj_val.window_y2) {
			obj_val.end_inserting_obj = 1;
			return 0;
		}
	}
	return 1;
}

/*Function to set window details like corner coordinates and object starting
 * coordinate*/
/**
* @brief 
*
* @param x1
* @param y1
* @param x2
* @param y2
* @param titlebar
* @param color
*/
void set_window_values(s32int x1, s32int y1, s32int x2, s32int y2, s32int
		titlebar, u32 color)
{
	obj_val.window_x1 = x1+obj_val.space_bw_obj/2;
	obj_val.window_y1 = y1+obj_val.space_bw_obj/2;
	obj_val.window_x2 = x2-obj_val.space_bw_obj/2;
	obj_val.window_y2 = y2-obj_val.space_bw_obj/2;
	obj_val.window_color = color;

	obj_val.obj_x = obj_val.window_x1 + obj_val.space_bw_obj;
	if(titlebar == NO_TITLE_BAR)
		obj_val.obj_y = obj_val.window_y1 + obj_val.space_bw_obj;
	else
		obj_val.obj_y = obj_val.window_y1 + (4 * obj_val.space_bw_obj);
}

/*Function to set starting coordinate for the next object*/
/**
* @brief 
*
* @param id
* @param str
* @param width
* @param height
* @param type
* @param color
*/
void set_object_values(u8* id, u8* str, s32int width, s32int height, u8* type, 
		u32 color)
{
	obj_val.obj_str[obj_val.obj_count] = str;
	obj_val.obj_type[obj_val.obj_count] = type;
	obj_val.obj_color[obj_val.obj_count] = color;
	sw_strcpy(obj_val.obj_id[obj_val.obj_count], id);
	
	obj_val.obj_coord[obj_val.obj_count][0] = obj_val.obj_x;
	obj_val.obj_coord[obj_val.obj_count][1] = obj_val.obj_y;
	obj_val.obj_coord[obj_val.obj_count][2] = obj_val.obj_x+width;
	obj_val.obj_coord[obj_val.obj_count][3] = obj_val.obj_y+height;
	
	obj_val.obj_count++;
	obj_val.obj_width = 0;
	obj_val.obj_x += width+obj_val.space_bw_obj;
	obj_val.curr_height = (obj_val.curr_height<height)?
		height:obj_val.curr_height;
	if((sw_strcmp(type, "textfield\0") == 0) || (sw_strcmp(type, "password\0")
				== 0))
		obj_val.total_textbox++;
}

/*Function to set object values given starting coordinates*/
/**
* @brief 
*
* @param id
* @param str
* @param x
* @param y
* @param width
* @param height
* @param type
* @param color
*/
void set_object_values_given_coord(u8* id, u8* str, s32int x, s32int y, 
		s32int width, s32int height, u8* type, u32 color)
{
	obj_val.obj_str[obj_val.obj_count] = str;
	obj_val.obj_type[obj_val.obj_count] = type;
	obj_val.obj_color[obj_val.obj_count] = color;
	sw_strcpy(obj_val.obj_id[obj_val.obj_count], id);
	
	obj_val.obj_coord[obj_val.obj_count][0] = x;
	obj_val.obj_coord[obj_val.obj_count][1] = y;
	obj_val.obj_coord[obj_val.obj_count][2] = x+width;
	obj_val.obj_coord[obj_val.obj_count][3] = y+height;
	obj_val.obj_count++;
	if((sw_strcmp(type, "textfield\0") == 0) || (sw_strcmp(type, "password\0")
				== 0))
		obj_val.total_textbox++;
}

/**
* @brief 
*
* @param touch
* @param release
*/
void set_event_handler(u32 touch, u32 release)
{
	event.on_touch = touch;
	event.on_release = release;
}

/**
* @brief 
*
* @param active_tf
*/
void set_active_textbox_layout(s32int active_tf)
{
	fill_rect(obj_val.obj_coord[active_tf][0] - 2, 
			obj_val.obj_coord[active_tf][1] - 2,
			obj_val.obj_coord[active_tf][2] + 2, 
			obj_val.obj_coord[active_tf][3] + 2,
			color(BLUE));
	fill_rect(obj_val.obj_coord[active_tf][0], obj_val.obj_coord[active_tf][1],
			obj_val.obj_coord[active_tf][2], obj_val.obj_coord[active_tf][3],
			color(WHITE));
	
	set_text(clicked_text[active_tf], obj_val.obj_id[active_tf]);
}

/**
* @brief 
*
* @param active_tf
*/
void set_inactive_textbox_layout(s32int active_tf)
{
	fill_rect(obj_val.obj_coord[active_tf][0] - 2, 
			obj_val.obj_coord[active_tf][1] - 2,
			obj_val.obj_coord[active_tf][2] + 2, 
			obj_val.obj_coord[active_tf][3] + 2,
			color(BLACK));
	fill_rect(obj_val.obj_coord[active_tf][0], obj_val.obj_coord[active_tf][1],
			obj_val.obj_coord[active_tf][2], obj_val.obj_coord[active_tf][3],
			color(WHITE));
	
	set_text(clicked_text[active_tf], obj_val.obj_id[active_tf]);
}
