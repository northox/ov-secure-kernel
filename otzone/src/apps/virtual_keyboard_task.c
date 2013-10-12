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
 * Virtual Keyboard implementation
 *
 */
#include <virtual_keyboard_task.h>
#include <virtual_keyboard.h>
#include <otz_id.h>
#include <dispatcher_task.h>
#include <sw_string_functions.h>
#include <sw_mem_functions.h>
#include <sw_board.h>
#include <sw_io.h>
#include <sw_fb.h>
#include <elf_loader.h>
/**
 * @brief 
 *
 * @param svc_cmd_id
 *
 * @return 
 */
int process_otz_virtual_keyboard_svc(u32 svc_cmd_id) {
	switch (svc_cmd_id) {
		case OTZ_VIRTUAL_KEYBOARD_CMD_ID_SHOW:
			vk_app();
			break;
		case OTZ_VIRTUAL_CMD_ID_PLAY_MEDIA:
			write_media_player_data();
			break;
	}
	return 0;
}


/**
 * @brief Virtual keyboard task entry point
 *
 * This function implements the processing of virtual keyboard commands.
 *
 * @param task_id: task identifier
 * @param tls: Pointer to task local storage
 */
void virtual_keyboard_task(int task_id,sw_tls* tls)
{
/*	sw_printf("inside virtual_keyboard_task\n"); */
#ifdef CONFIG_NS_JIFFIES_WORKAROUND
	disable_ns_jiffies_update();
#endif
    tls->ret_val = process_otzapi(task_id, tls);

#ifdef CONFIG_NS_JIFFIES_WORKAROUND
	enable_ns_jiffies_update();
#endif
	handle_task_return(task_id, tls);
	while(1);
}


