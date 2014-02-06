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
 * trustzone based hypervisor boot implementation
 */

#include <nsk_boot.h>
#include <sw_board.h>
#include <sw_debug.h>
#include <sw_timer.h>
#include <mem_mng.h>
#include <sw_mem_functions.h>
#include <cpu_asm.h>
#include <monitor.h>
#include <tzhyp_global.h>
#include <tzhyp.h>

static struct nsk_boot_info ns_world_binfo[GUESTS_NO];
static struct timer_event* tzhypboot_event;
static timeval_t tzhypboot_time;

/**
* @brief 
*/
static void tzhyp_load_guests(void);

/**
* @brief 
*/
static void tzhyp_guest_context_init(void);

/**
* @brief 
*
* @param 
*/
static void tzhyp_bootevent_handler(struct timer_event *);

/**
* @brief 
*/
static void tzhyp_bootevent(void);

/**
* @brief 
*
* @return 
*/
int tzhyp_guest_init(void)
{
	int guest;

	/* Initialize Guest 1 */
	guest = 0;
	/* Assuming both start and load address are 1mb aligned*/
	ns_world_binfo[guest].nskbi_srcaddr = (u32)&kernel_start; 
	ns_world_binfo[guest].nskbi_loadaddr = NSK_LOAD_ADDRESS; 
	ns_world_binfo[guest].nskbi_size  = 
		((u32)&kernel_end) - ((u32)&kernel_start);
	ns_world_binfo[guest].nskbi_initrd_flag = 0;
	

#ifdef CONFIG_MULTI_GUESTS_SUPPORT
        /* Initialize Guest 2 */
	guest = 1;
	/* Assuming both start and load address are 1mb aligned*/
	ns_world_binfo[guest].nskbi_srcaddr = (u32)&kernel_2_start; 
	ns_world_binfo[guest].nskbi_loadaddr = 
		NSK_LOAD_ADDRESS + GUEST_MEM_SIZE; 
	ns_world_binfo[guest].nskbi_size  = 
		((u32)&kernel_2_end) - ((u32)&kernel_2_start);

#ifndef CONFIG_ZYNQ7_BOARD	
	/* Use initrd for the second guest */
	ns_world_binfo[guest].nskbi_initrd_flag = 1;
	ns_world_binfo[guest].nskbi_initrd_sa = &initrd_image_start;
	ns_world_binfo[guest].nskbi_initrd_la = LINUX_INITRD_ADDR;
	ns_world_binfo[guest].nskbi_initrd_size = 
		((u32)&initrd_image_end) - ((u32)&initrd_image_start);
#endif


#endif

	/* Load all the guests to memory */
	tzhyp_load_guests();

	/* Initialize the register context for all the guests */
	tzhyp_guest_context_init();

	return 0;
}

/**
* @brief 
*/
static void tzhyp_load_guests(void)
{
	int guest;

	for (guest = 0; guest < GUESTS_NO; guest++)
		nsk_load(&ns_world_binfo[guest]);

#ifdef LINUX_ATAG_BOOT_EN
	nsk_atag_init();
#endif
}

/**
* @brief 
*/
static void tzhyp_guest_context_init(void)
{
	int guest, core;
	struct system_context *core_ns_world, *reference;

	sw_printf("tzhyp_guest_context_init:");

	/* Initialize the guest 0 context to a sane state */
	smc_nscpu_context_init();

	tzhyp_device_context_init();

	/* Context base for guest 0 of primary core */
	core_ns_world = reference = 
		(struct system_context *)GET_CORE_CONTEXT_BYID(ns_world, 0); 
	
	/* Copy the guest 0 context to remaining guests */
	for (guest = 1; guest < GUESTS_NO; guest++)
		sw_memcpy(&core_ns_world[guest], reference, 
			  sizeof(struct system_context));

	/* Initialize guest specific parameters */
	for (guest = 0; guest < GUESTS_NO; guest++) {
		core_ns_world[guest].sysctxt_core.lr_mon = 
			ns_world_binfo[guest].nskbi_loadaddr;
		core_ns_world[guest].guest_no = guest;
	}

	/* Initialize the secondary core contexts as well */ 
	for (core = 1; core < MAX_CORES; core++) {
		sw_printf("context init for sec core %x\n", core);
		sw_printf("size of system context = %x\n", 
			  sizeof(struct system_context));
		core_ns_world = 
			(struct system_context *)GET_CORE_CONTEXT_BYID(ns_world, 
								       core);
		for (guest = 0; guest < GUESTS_NO; guest++) {
			sw_memcpy(&core_ns_world[guest], reference, 
				  sizeof(struct system_context));
			core_ns_world[guest].guest_no = guest;
		}
	}
}

/**
* @brief 
*/
static void tzhyp_bootevent(void)
{

	tzhypboot_event = timer_event_create(&tzhyp_bootevent_handler, 
					      (void*)NULL);
	if(!tzhypboot_event){
		sw_printf("xxx : Cannot register Handler\n");
		return;
	}

	/* Time duration = 1s */
	tzhypboot_time.tval.nsec = 0;
	tzhypboot_time.tval.sec = 1;

	timer_event_start(tzhypboot_event, &tzhypboot_time);
}

/**
* @brief 
*
* @param x
*/
static void tzhyp_bootevent_handler(struct timer_event *x)
{

	sw_printf("boot event handler\n");
	if (ns_preempt_flag) {
		tzhyp_schedule_guest();
	}  else {
		timer_event_start(tzhypboot_event, &tzhypboot_time);	
	}
}

/**
* @brief 
*/
void mon_nscpu_context_init()
{
	struct core_context *core_ctxt;
	struct cp15_context *cp15_ctxt;
#ifdef CONFIG_NEON_SUPPORT
	struct vfp_context *vfp_ctxt;
#endif
	struct system_context *primary_ns_world;
	
	primary_ns_world = (struct system_context *)
		GET_CORE_CONTEXT_BYID(ns_world, 0);
	core_ctxt = &primary_ns_world->sysctxt_core;
	cp15_ctxt = &primary_ns_world->sysctxt_cp15;
#ifdef CONFIG_NEON_SUPPORT
        vfp_ctxt  = &primary_ns_world->sysctxt_vfp;
#endif
	sw_memset(core_ctxt, 0, sizeof(struct core_context));
#ifdef CONFIG_NEON_SUPPORT
	sw_memset(vfp_ctxt, 0, sizeof(struct vfp_context));
#endif
#ifdef LINUX_ATAG_BOOT_EN
	core_ctxt->r0 = 0;
	core_ctxt->r1 = LINUX_MACHINE_ID;
	core_ctxt->r2 = (u32)NORMAL_WORLD_RAM_START + 0x100;
#endif
	
	core_ctxt->spsr_mon = CPSR_RESET_VAL;

	/* 
	 * Save cp15 reset state
	 */
	tzhyp_sysregs_save(cp15_ctxt);
}

/**
* @brief 
*/
void tzhyp_boot_ack_event(void)
{
	static int guest_booted = 0;

	sw_printf("tzhyp_boot ack event\n");

	guest_booted++;
	if (guest_booted < GUESTS_NO)
		tzhyp_bootevent();
	else if (guest_booted == GUESTS_NO)
		tzhyp_schedevent_init();
	else 
		sw_printf("tzhyp: Warning! Ignoring wrong ack event\n");
}

