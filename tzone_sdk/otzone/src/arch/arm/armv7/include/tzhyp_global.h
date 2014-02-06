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
 * Header for Trustzone based hypervisor global implementation
 */

#ifndef TZHYP_GLOBAL_H
#define TZHYP_GLOBAL_H

#include <sw_types.h>
#include <system_context.h>
#include <tzhyp_config.h>
#include <nsk_boot.h>
#include <sw_board.h>
#include <sw_mcore.h>

/**
* @brief Non secure task contexts
*/
extern struct system_context ns_world[MAX_CORES * GUESTS_NO];

/**
* @brief Common Secure context 
*/
extern struct system_context s_world[1];

/**
* @brief Non secure and Secure context pointers
*/
extern struct system_context *ns_sys_current;

/**
* @brief 
*/
extern struct system_context *s_sys_current; 

/**
* @brief Non secure preempt flag
*/
extern u32 ns_preempt_flag;

/**
* @brief NS switch flag to indicate TLB flush 
*/
extern u32 ns_switch_flag;

#if 0
extern u8 _NSADMIN_SECTION_START;
extern u8 _NSADMIN_SECTION_END;
#endif
extern u32 tzhyp_nsadmin_start, tzhyp_nsadmin_end;

/**
* @brief 
*/
extern u32 kernel_start, kernel_end;

/**
* @brief 
*/
extern u32 kernel_2_start, kernel_2_end;

/**
* @brief 
*/
extern u32 initrd_image_start, initrd_image_end;

/**
* @brief 
*
* @return 
*/
extern  int tzhyp_guest_init(void);

/**
* @brief 
*
* @return 
*/
extern  int tzhyp_schedevent_init(void);

/**
* @brief 
*/
extern void tzhyp_schedule_guest(void);

/**
* @brief 
*/
extern void tzhyp_device_context_init(void);

extern void tzhyp_device_switch(struct system_context *, 
				struct system_context *);


extern int nsk_load(struct nsk_boot_info *);

#ifdef CONFIG_MULTI_GUESTS_SUPPORT
/**
 * @brief 
 *
 * @return 
 */
int nsk_initrd_load(void);
#endif

#endif
