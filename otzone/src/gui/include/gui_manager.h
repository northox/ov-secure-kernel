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
 * Header for GUI manager implementation
 */

#ifndef __GUI_H__
#define __GUI_H__

#include <sw_types.h>

#define PORTRAIT 1
#define LANDSCAPE 0

#define PASSWORD 1
#define NO_PASSWORD 0

#define REPAINT 1
#define NO_REPAINT 0

#define TEXT_BG 1
#define NO_TEXT_BG 0

#define TITLE_BAR 1
#define NO_TITLE_BAR 0

#define FONT_SIZE 8

extern u8 fontdata_8x8[];

extern u8 clicked_text[500][200];

extern s32int gui_open;

/**
* @brief 
*/
struct gui_info {
	s32int orientation;
	s32int screen_width;
	s32int screen_height;
	s32int bits_per_pixel;
	s32int line_length;
	s32int smem_length;
};

struct gui_info gui_info;

/**
* @brief 
*/
struct obj_values {
	s32int button_width;
	s32int textfield_width;
	s32int label_width;
	s32int window_x1, window_y1, window_x2, window_y2, 
		obj_x, obj_y;
	s32int obj_width, obj_height;
	s32int max_height, curr_height; 
	s32int end_inserting_obj;
	s32int obj_coord[500][4];
	u8* obj_str[500];
	u8* obj_type[500];
	u8 obj_id[500][500];
	u32 window_color, obj_color[500];
	s32int obj_count;
	s32int space_bw_obj;
	u8 active_textbox[100];
	s32int total_textbox;
};

struct obj_values obj_val;

/**
* @brief 
*/
struct event {
	u32 on_touch;
	u32 on_release;
};

struct event event;

/**
* @brief 
*
* @param active_tf_id
*/
extern void set_active_textbox_layout(s32int active_tf_id);

/**
* @brief 
*
* @param active_tf_id
*/
extern void set_inactive_textbox_layout(s32int active_tf_id);

/**
* @brief 
*
* @param touch
* @param release
*/
extern void set_event_handler(u32 touch, u32 release);

/**
* @brief 
*
* @param orient
*/
extern void set_orientation(s32int orient);

/**
* @brief 
*
* @param x
* @param y
* @param color
*/
extern void put_pixel(s32int x, s32int y, u32 color);

/**
* @brief 
*
* @param x1
* @param y1
* @param x2
* @param y2
* @param color
*/
extern void draw_line(s32int x1, s32int y1, s32int x2, s32int y2, 
		u32 color);

/**
* @brief 
*
* @param x1
* @param y1
* @param x2
* @param y2
* @param color
*/
extern void fill_rect(s32int x1, s32int y1, s32int x2, s32int y2, 
		u32 color);

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
extern void set_window_values(s32int x1, s32int y1, s32int x2, s32int y2, s32int
		titlebar,u32 color);

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
extern void bitmap_on_given_coord(s32int x, s32int y, s32int width, 
		s32int height, u32 *bmp_array, u8* id);

/**
* @brief 
*
* @param width
* @param height
* @param bmp_array
* @param id
*/
extern void bitmap(s32int width, s32int height, u32* bmp_array, u8* id);

/**
* @brief 
*/
extern void sw_window_init(void);

/**
* @brief 
*/
extern void sw_window_exit(void);

/**
* @brief 
*
* @param offset
* @param color
*/
extern void sw_window_set_pixel(s32int offset, u32 color);

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
extern void set_window(s32int x1, s32int y1, s32int x2, s32int y2, 
		u8* title, u32 color, s32int titlebar);

/**
* @brief 
*
* @param x
* @param y
* @param str
* @param width
* @param height
* @param color
* @param id
*/
extern void button_on_given_coord(s32int x, s32int y, u8* str, s32int width, 
		s32int height, u32 color, u8* id);

/**
* @brief 
*
* @param id
* @param str
* @param width
* @param height
* @param color
*/
extern void button(u8* id, u8* str, s32int width, s32int height, u32 color);

/**
* @brief 
*
* @param x
* @param y
* @param str
* @param id
* @param width
* @param height
* @param color
*/
extern void label_on_given_coord(s32int x, s32int y, u8* str, u8* id, 
		s32int width, s32int height, u32 color);

/**
* @brief 
*
* @param id
* @param str
* @param width
* @param height
* @param color
*/
extern void label(u8* id, u8* str, s32int width, s32int height, u32 color);

/**
* @brief 
*
* @param x
* @param y
* @param str
* @param id
* @param width
* @param height
* @param color
* @param password
*/
extern void textfield_on_given_coord(s32int x, s32int y, u8* str, u8* id, 
		s32int width, s32int height, u32 color, u32 password);

/**
* @brief 
*
* @param id
* @param str
* @param width
* @param height
* @param color
* @param password
*/
extern void textfield(u8* id, u8* str, s32int width, 
		s32int height, u32 color, u32 password);

/**
* @brief 
*
* @param str
*/
extern void compute_button_width(u8* str);

/**
* @brief 
*
* @param str
*/
extern void compute_label_width(u8* str);
	
/**
* @brief 
*
* @param str
*/
extern void compute_textfield_width(u8* str);

/**
* @brief 
*
* @param str
* @param width
* @param height
*
* @return 
*/
extern s32int check_space_for_object(u8* str, s32int width, s32int height);

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
extern void set_object_values(u8* id, u8* str, s32int width, s32int height, 
		u8* type, u32 color);

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
extern void set_object_values_given_coord(u8* id, u8* str, s32int x, s32int y, 
		s32int width, s32int height, u8* type, u32 color);

/**
* @brief 
*
* @param x
* @param y
* @param c
* @param color
*/
extern void put_char(s32int x, s32int y, u8 c, u32 color);

/**
* @brief 
*
* @param x
* @param y
* @param str
* @param length
* @param color
* @param clear
*/
extern void put_string(s32int x, s32int y, u8 *str, s32int length,
		u32 color, s32int clear);

/**
* @brief 
*
* @param str
* @param id[10]
*/
extern void set_text(u8* str, u8 id[10]);

#endif
