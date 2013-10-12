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
 * virtual keyboard application
 */

#include <virtual_keyboard.h>
#include <gui_manager.h>
#include <color_manager.h>
#include <touch_manager.h>
#include <keyboard_keys.h>
#include <sw_syscall.h>
#include <elf_loader.h>
//#include <sw_info.h>


s32int active_tf = 0;
s32int close = 0;

#ifdef TOUCH_SUPPORT
/**
* @brief 
*/
void on_touch_event(void)
{
#ifdef DEBUG_VK
	sw_printf("FUNC: on_touch_event\n");
#endif
	if(sw_strcmp(touch_info.last_touched, "pay\0") == 0) {
		calc_bmp_array_on_given_coord(touch_info.x, touch_info.y, 200, 
				51, keyon_pay, "pay\0");
#ifdef CONFIG_KSPACE_PROTECTION	
		__sw_fb_flush();
#else	
		sw_flush_cache();
#endif
	}
}

/**
* @brief 
*/
void on_release_event(void)
{
#ifdef DEBUG_VK
	sw_printf("FUNC: on_release_event\n");
#endif
	if(sw_strcmp(touch_info.last_touched, "pay\0") == 0) {
		close = 1;
		gui_open = 0;
	}
	else if((sw_strcmp(touch_info.type, "textfield\0") == 0) ||
			(sw_strcmp(touch_info.type, "password\0") == 0)) {
		virtual_keyboard(touch_info.active_textbox);
	}
	else
		sw_printf("Not PAY Button and TextField\n");
}
#endif

/**
* @brief 
*
* @return 
*/
s32int vk_app(void)
{
	close = 0;
#ifdef DEBUG_VK
	sw_printf("FUNC: vk_app\n");
#endif
	s32int w = 80;
	s32int h = 60;
	s32int i = -1;
	sw_window_init();
	set_orientation(PORTRAIT);

	//calc_bmp_array_on_given_coord(388, 6, KEY_WIDTH, KEY_HEIGHT, key_0, "0\0");
	calc_bmp_array_on_given_coord(170, 134, 271, 168, creditcard, "credit\0");
	calc_bmp_array_on_given_coord(68, 350, 115, 28, amount, "amount\0");
	calc_bmp_array_on_given_coord(68, 440, 128, 28, passcode, "passcode\0");
	calc_bmp_array_on_given_coord(388, 6, 194, 59, logo, "logo\0");
	calc_bmp_array_on_given_coord(54, 21, 289, 40, secure_payment, "logo\0");

	u8 text1[500], text2[500];
	sw_strcpy(text1, "Enter Text...\0");
	sw_strcpy(text2, "Enter Passcode...\0");

	textfield_on_given_coord(200, 340, text1, "TextField\0", 205, 
			45, 0xffffffff, NO_PASSWORD);

	active_tf = obj_val.obj_count;
	textfield_on_given_coord(200, 430, text2, "Password\0", 205,
			45, 0xffffffff, PASSWORD);
	
	calc_bmp_array_on_given_coord(205, 500, 200, 51, key_pay, "pay\0");

#ifdef CONFIG_KSPACE_PROTECTION	
		__sw_fb_flush();
#else	
		sw_flush_cache();
#endif

	virtual_keyboard(active_tf);
#ifdef TOUCH_SUPPORT
	while((close == 0) && (gui_open == 1)) {
		set_event_handler((u32)&on_touch_event, (u32)&on_release_event);
		(*activate_touch())();
	}
#ifdef DEBUG_VK
	sw_printf("vk_app_exit\n");
#endif

#endif
	sw_window_exit();
	return 0;
}
EXPORT_SYMBOL(vk_app);
