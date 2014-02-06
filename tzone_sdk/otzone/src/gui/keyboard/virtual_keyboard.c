/* 
 * OpenVirtualization: 
 * For additional details and support contact developer@sierraware.com.
 * Additional documentation can be found at www.openvirtualization.org
 * 
 * Copyright (C) 2011 SierraWare
 *
 * This file is part of the Sierraware OpenTrustPlatform.
 *
 * Sierraware OpenTrustPlatform is free software: you can redistribute it 
 * and/or modify it under the terms of the GNU Lesser General Public License 
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Sierraware OpenTrustPlatform is distributed in the hope that it 
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Sierraware OpenTrustPlatform.  If not, 
 * see <http://www.gnu.org/licenses/>.
 * 
 * virtual keyboard implementation
 */

#include <virtual_keyboard.h>
#include <gui_manager.h>
#include <color_manager.h>
#include <touch_manager.h>
#include <keyboard_keys.h>
#include <sw_string_functions.h>
#include <sw_syscall.h>
//#include <sw_info.h>

s32int count = 0;
s32int vk_close = 0;
s32int vk_active = 0;

/**
* @brief 
*
* @param vk_active
*
* @return 
*/
s32int virtual_keyboard(s32 vk_active)
{
#ifdef DEBUG_VK
	sw_printf("FUNC: virtual_keyboard\n");
#endif
	vk_close = 0;
	set_window(0, 672, 600, 1024, "VIR KEYBOARD\0", color(BLACK), NO_TITLE_BAR);

	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_1, "1\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_2, "2\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_3, "3\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_4, "4\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_5, "5\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_6, "6\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_7, "7\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_8, "8\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_9, "9\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_0, "0\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_backspace, "backspace\0");

	label("label1\0", "", KEY_WIDTH/2, KEY_HEIGHT/2, color(BLACK));
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_q, "q\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_w, "w\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_e, "e\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_r, "r\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_t, "t\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_y, "y\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_u, "u\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_i, "i\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_o, "o\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_p, "p\0");

	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_period, ".\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_a, "a\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_s, "s\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_d, "d\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_f, "f\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_g, "g\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_h, "h\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_j, "j\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_k, "k\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_l, "l\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_enter, "enter\0");

	calc_bmp_array(71, KEY_HEIGHT, key_plus, "+\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_z, "z\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_x, "x\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_c, "c\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_v, "v\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_b, "b\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_n, "n\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_m, "m\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_equals, "=\0");
	calc_bmp_array(71, KEY_HEIGHT, key_ampersand, "&\0");

	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_comma, ",\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_question, "?\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_exclamation, "!\0");
	calc_bmp_array(209, KEY_HEIGHT, key_spacewide, "space\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_hyphen, "-\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_asterisk, "*\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_at, "@\0");
	calc_bmp_array(KEY_WIDTH, KEY_HEIGHT, key_underscore, "_\0");

#ifdef CONFIG_KSPACE_PROTECTION	
		__sw_fb_flush();
#else	
		sw_flush_cache();
#endif

#ifdef TOUCH_SUPPORT
	while((vk_close == 0) && (gui_open == 1)) {
		set_event_handler((u32)&on_touch, (u32)&on_release);
		(*activate_touch())();
	}
#ifdef DEBUG_VK
	sw_printf("vk_exit\n");
#endif
#endif
	/* sw_window_exit(); */
	return 0;
}
/**
* @brief 
*/
void on_touch(void)
{
#ifdef DEBUG_VK
	sw_printf("FUNC: on_touch\n");
#endif
	if(sw_strcmp(touch_info.last_touched, "0\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_0, "0\0");
	else if(sw_strcmp(touch_info.last_touched, "1\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_1, "1\0");
	else if(sw_strcmp(touch_info.last_touched, "2\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_2, "2\0");
	else if(sw_strcmp(touch_info.last_touched, "3\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_3, "3\0");
	else if(sw_strcmp(touch_info.last_touched, "4\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_4, "4\0");
	else if(sw_strcmp(touch_info.last_touched, "5\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_5, "5\0");
	else if(sw_strcmp(touch_info.last_touched, "6\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_6, "6\0");
	else if(sw_strcmp(touch_info.last_touched, "7\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_7, "7\0");
	else if(sw_strcmp(touch_info.last_touched, "8\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_8, "8\0");
	else if(sw_strcmp(touch_info.last_touched, "9\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_9, "9\0");
	else if(sw_strcmp(touch_info.last_touched, "backspace\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_backspace, "backspace\0");
	else if(sw_strcmp(touch_info.last_touched, "q\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_q, "q\0");
	else if(sw_strcmp(touch_info.last_touched, "w\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_w, "w\0");
	else if(sw_strcmp(touch_info.last_touched, "e\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_e, "e\0");
	else if(sw_strcmp(touch_info.last_touched, "r\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_r, "r\0");
	else if(sw_strcmp(touch_info.last_touched, "t\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_t, "t\0");
	else if(sw_strcmp(touch_info.last_touched, "y\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_y, "y\0");
	else if(sw_strcmp(touch_info.last_touched, "u\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_u, "u\0");
	else if(sw_strcmp(touch_info.last_touched, "i\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_i, "i\0");
	else if(sw_strcmp(touch_info.last_touched, "o\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_o, "o\0");
	else if(sw_strcmp(touch_info.last_touched, "p\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_p, "p\0");
	else if(sw_strcmp(touch_info.last_touched, ".\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_period, ".\0");
	else if(sw_strcmp(touch_info.last_touched, "a\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_a, "a\0");
	else if(sw_strcmp(touch_info.last_touched, "s\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_s, "s\0");
	else if(sw_strcmp(touch_info.last_touched, "d\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_d, "d\0");
	else if(sw_strcmp(touch_info.last_touched, "f\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_f, "f\0");
	else if(sw_strcmp(touch_info.last_touched, "g\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_g, "g\0");
	else if(sw_strcmp(touch_info.last_touched, "h\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_h, "h\0");
	else if(sw_strcmp(touch_info.last_touched, "j\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_j, "j\0");
	else if(sw_strcmp(touch_info.last_touched, "k\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_k, "k\0");
	else if(sw_strcmp(touch_info.last_touched, "l\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_l, "l\0");
	else if(sw_strcmp(touch_info.last_touched, "enter\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_enter, "enter\0");
	else if(sw_strcmp(touch_info.last_touched, "+\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, 71, 
				KEY_HEIGHT, keyon_plus, "+\0");
	else if(sw_strcmp(touch_info.last_touched, "z\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_z, "z\0");
	else if(sw_strcmp(touch_info.last_touched, "x\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_x, "x\0");
	else if(sw_strcmp(touch_info.last_touched, "c\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_c, "c\0");
	else if(sw_strcmp(touch_info.last_touched, "v\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_v, "v\0");
	else if(sw_strcmp(touch_info.last_touched, "b\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_b, "b\0");
	else if(sw_strcmp(touch_info.last_touched, "n\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_n, "n\0");
	else if(sw_strcmp(touch_info.last_touched, "m\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_m, "m\0");
	else if(sw_strcmp(touch_info.last_touched, "=\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_equals, "=\0");
	else if(sw_strcmp(touch_info.last_touched, "&\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, 71, 
				KEY_HEIGHT, keyon_ampersand, "&\0");
	else if(sw_strcmp(touch_info.last_touched, ",\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_comma, ",\0");
	else if(sw_strcmp(touch_info.last_touched, "?\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_question, "?\0");
	else if(sw_strcmp(touch_info.last_touched, "!\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_exclamation, "!\0");
	else if(sw_strcmp(touch_info.last_touched, "space\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, 209, 
				KEY_HEIGHT, keyon_spacewide, "space\0");
	else if(sw_strcmp(touch_info.last_touched, "-\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_hyphen, "-\0");
	else if(sw_strcmp(touch_info.last_touched, "*\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_asterisk, "*\0");
	else if(sw_strcmp(touch_info.last_touched, "@\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_at, "@\0");
	else if(sw_strcmp(touch_info.last_touched, "_\0") == 0)
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, keyon_underscore, "_\0");
	else if(sw_strcmp(touch_info.last_touched, "pay\0") == 0) {
		sw_printf("PAID\n");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, 200, 
				51, keyon_pay, "pay\0");
	}
#ifdef CONFIG_KSPACE_PROTECTION	
		__sw_fb_flush();
#else	
		sw_flush_cache();
#endif
}

/**
* @brief 
*/
void on_release(void)
{
#ifdef DEBUG_VK
	sw_printf("FUNC: on_release\n");
#endif
	/*if(sw_strcmp(obj_val.active_textbox, "TextField\0") == 0) {
	  vk_active = 0;
	  }
	  else if(sw_strcmp(obj_val.active_textbox, "Password\0") == 0) {
	  vk_active = 1;
	  }*/
	vk_active = touch_info.active_textbox;
	if(sw_strcmp(touch_info.last_touched, "0\0") == 0) {
		sw_strcat(clicked_text[vk_active], "0\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_0, "0\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "1\0") == 0) {
		sw_strcat(clicked_text[vk_active], "1\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_1, "1\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "2\0") == 0) {
		sw_strcat(clicked_text[vk_active], "2\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_2, "2\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "3\0") == 0) {
		sw_strcat(clicked_text[vk_active], "3\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_3, "3\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "4\0") == 0) {
		sw_strcat(clicked_text[vk_active], "4\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_4, "4\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "5\0") == 0) {
		sw_strcat(clicked_text[vk_active], "5\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_5, "5\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "6\0") == 0) {
		sw_strcat(clicked_text[vk_active], "6\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_6, "6\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "7\0") == 0) {
		sw_strcat(clicked_text[vk_active], "7\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_7, "7\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "8\0") == 0) {
		sw_strcat(clicked_text[vk_active], "8\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_8, "8\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "9\0") == 0) {
		sw_strcat(clicked_text[vk_active], "9\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_9, "9\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "backspace\0") == 0) {
		u8 temp_text[200];
		sw_strcpy(temp_text, clicked_text[vk_active]);
		sw_strcpy(clicked_text[vk_active], "\0");
		sw_strncat(clicked_text[vk_active], temp_text, sw_strlen(temp_text) -1);
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_backspace, "backspace\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "q\0") == 0) {
		sw_strcat(clicked_text[vk_active], "q\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_q, "q\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "w\0") == 0) {
		sw_strcat(clicked_text[vk_active], "w\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_w, "w\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "e\0") == 0) {
		sw_strcat(clicked_text[vk_active], "e\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_e, "e\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "r\0") == 0) {
		sw_strcat(clicked_text[vk_active], "r\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_r, "r\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "t\0") == 0) {
		sw_strcat(clicked_text[vk_active], "t\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_t, "t\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "y\0") == 0) {
		sw_strcat(clicked_text[vk_active], "y\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_y, "y\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "u\0") == 0) {
		sw_strcat(clicked_text[vk_active], "u\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_u, "u\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "i\0") == 0) {
		sw_strcat(clicked_text[vk_active], "i\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_i, "i\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "o\0") == 0) {
		sw_strcat(clicked_text[vk_active], "o\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_o, "o\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "p\0") == 0) {
		sw_strcat(clicked_text[vk_active], "p\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_p, "p\0");
	}
	else if(sw_strcmp(touch_info.last_touched, ".\0") == 0) {
		sw_strcat(clicked_text[vk_active], ".\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_period, ".\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "a\0") == 0) {
		sw_strcat(clicked_text[vk_active], "a\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_a, "a\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "s\0") == 0) {
		sw_strcat(clicked_text[vk_active], "s\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_s, "s\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "d\0") == 0) {
		sw_strcat(clicked_text[vk_active], "d\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_d, "d\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "f\0") == 0) {
		sw_strcat(clicked_text[vk_active], "f\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_f, "f\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "g\0") == 0) {
		sw_strcat(clicked_text[vk_active], "g\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_g, "g\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "h\0") == 0) {
		sw_strcat(clicked_text[vk_active], "h\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_h, "h\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "j\0") == 0) {
		sw_strcat(clicked_text[vk_active], "j\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_j, "j\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "k\0") == 0) {
		sw_strcat(clicked_text[vk_active], "k\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_k, "k\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "l\0") == 0) {
		sw_strcat(clicked_text[vk_active], "l\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_l, "l\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "enter\0") == 0) {
		fill_rect(0, 672, 600, 1024, color(BLACK));
#ifdef CONFIG_KSPACE_PROTECTION	
		__sw_fb_flush();
#else	
		sw_flush_cache();
#endif

		vk_close = 1;
		gui_open = 0;
	}
	else if(sw_strcmp(touch_info.last_touched, "+\0") == 0) {
		sw_strcat(clicked_text[vk_active], "+\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, 71, 
				KEY_HEIGHT, key_plus, "+\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "z\0") == 0) {
		sw_strcat(clicked_text[vk_active], "z\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_z, "z\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "x\0") == 0) {
		sw_strcat(clicked_text[vk_active], "x\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_x, "x\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "c\0") == 0) {
		sw_strcat(clicked_text[vk_active], "c\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_c, "c\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "v\0") == 0) {
		sw_strcat(clicked_text[vk_active], "v\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_v, "v\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "b\0") == 0) {
		sw_strcat(clicked_text[vk_active], "b\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_b, "b\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "n\0") == 0) {
		sw_strcat(clicked_text[vk_active], "n\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_n, "n\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "m\0") == 0) {
		sw_strcat(clicked_text[vk_active], "m\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_m, "m\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "=\0") == 0) {
		sw_strcat(clicked_text[vk_active], "=\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_equals, "=\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "&\0") == 0) {
		sw_strcat(clicked_text[vk_active], "&\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, 71, 
				KEY_HEIGHT, key_ampersand, "&\0");
	}
	else if(sw_strcmp(touch_info.last_touched, ",\0") == 0) {
		sw_strcat(clicked_text[vk_active], ",\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_comma, ",\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "?\0") == 0) {
		sw_strcat(clicked_text[vk_active], "?\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_question, "?\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "!\0") == 0) {
		sw_strcat(clicked_text[vk_active], "!\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_exclamation, "!\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "space\0") == 0) {
		sw_strcat(clicked_text[vk_active], " \0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, 209, 
				KEY_HEIGHT, key_spacewide, "space\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "-\0") == 0) {
		sw_strcat(clicked_text[vk_active], "-\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_hyphen, "-\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "*\0") == 0) {
		sw_strcat(clicked_text[vk_active], "*\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_asterisk, "*\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "@\0") == 0) {
		sw_strcat(clicked_text[vk_active], "@\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_at, "@\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "_\0") == 0) {
		sw_strcat(clicked_text[vk_active], "_\0");
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, KEY_WIDTH, 
				KEY_HEIGHT, key_underscore, "_\0");
	}
	else if(sw_strcmp(touch_info.last_touched, "pay\0") == 0) {
		gui_open = 0;
	}

	if((sw_strcmp(touch_info.type, "textfield\0") != 0) &&
			(sw_strcmp(touch_info.type, "password\0") !=0)) {

		set_text(clicked_text[vk_active], obj_val.obj_id[vk_active]);
	}
#ifdef CONFIG_KSPACE_PROTECTION	
		__sw_fb_flush();
#else	
		sw_flush_cache();
#endif

}
