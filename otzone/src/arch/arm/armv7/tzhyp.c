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
 * trustzone based hypervisor implementation
 */


#include <sw_types.h>
#include <sw_debug.h>
#include <system_context.h>
#include <mem_mng.h>
#include <sw_mem_functions.h>
#include <page_table.h>
#include <tzhyp_global.h>
#include <tzhyp_config.h>
#include <sw_board.h>

/**
* @brief Non secure contexts
*/
struct system_context ns_world[MAX_CORES * GUESTS_NO];

/**
* @brief Common Secure context;  no secure context for secondary cores 
*/
struct system_context s_world[1];

/**
* @brief Non secure and Secure context pointers
*/
struct system_context *ns_sys_current = (struct system_context *)ns_world;

/*
 * The system context for secure wolrd is static, so the word current might be a
 * misnomer. 
 */
struct system_context *s_sys_current = (struct system_context *)s_world; 

/**
* @brief Non secure preempt flag
*/
u32 ns_preempt_flag;

/**
* @brief NS switch flag to indicate TLB flush 
*/
u32 ns_switch_flag;

static int tzhyp_nsadmin_init(void);

/**
* @brief 
*
* @return 
*/
static int tzhyp_nsadmin_init(void)
{
	int error; 
	void *loadaddr = (u32 *)NSADMIN_LOAD;
#if 0 
	void *startaddr = (u32 *)&_NSADMIN_SECTION_START;
	int size = &_NSADMIN_SECTION_END -  &_NSADMIN_SECTION_START;
#endif
	void *startaddr = (u32 *)&tzhyp_nsadmin_start;
	int size = (u32)&tzhyp_nsadmin_end - (u32)&tzhyp_nsadmin_start;

#if 0
	sw_printf("nsadmin:\n");
	sw_printf("loadaddr = %x\n", loadaddr);
	sw_printf("start_addr = %x\n", startaddr);
	sw_printf("*startaddr = %x\n", *(u32 *)startaddr);
	sw_printf("size = %x\n", size);
#endif
	error = map_nsmemsect_normal((va_t)loadaddr, (pa_t)loadaddr, 
				     SECTION_SIZE);
	if (error < 0)
		return error;

	sw_memcpy(loadaddr, startaddr, size);

	unmap_nsmemsect_normal((va_t)loadaddr, SECTION_SIZE);
	
	return 0;
}

/**
* @brief 
*
* @return 
*/
int tzhyp_init(void)
{
	int error;

	error = tzhyp_nsadmin_init();
	if (error < 0) {
		sw_printf("tzhyp: nsadmin init failed\n");
		return error;
	}
	error =  tzhyp_guest_init();
	if (error < 0) {
		sw_printf("tzhyp: guest init failed\n");
		return error;
	}

	return 0;
}

