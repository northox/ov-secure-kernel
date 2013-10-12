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
 * Trustzone based hypervisor scheduler functions implementation
 */

/*
 * Support functions for implementing (Trustzone based)hypervisor
 * running in monitor mode
 */

#include <system_context.h>
#include <sw_debug.h>
#include <sw_timer.h>
#include <tzhyp_global.h>

static struct timer_event* tzhypsched_event;
static timeval_t tzhypsched_time;

/**
* @brief 
*
* @param 
*/
static void tzhyp_schedevent_handler(struct timer_event *);

/**
 * @brief
 * Hypervisor scheduler does the job of scheduling between multiple guest OS
 */
void tzhyp_schedule_guest(void)
{
	static struct system_context *boundary = &ns_world[0] + GUESTS_NO;
	struct system_context *current, *next;
#ifdef SCHEDULE_HIGH_PRIORITY_GUEST
	static u32 count = 0, low_priority_guest_scheduled = 0;
#endif

#ifdef SW_OTZHYP_DEBUG    
	sw_printf("tzhyp_sched_guest\n");
#endif

	current = ns_sys_current;	
#ifdef SCHEDULE_HIGH_PRIORITY_GUEST
	if(current->guest_no != HIGH_PRIORITY_GUEST)
		low_priority_guest_scheduled = 1;

    if(low_priority_guest_scheduled && 
        is_guest_irq_active(current->guest_no)) {
        /* Schedule the low priorty guest @ 800 milli seconds interval
            40ms * 20 = 800 milli seconds */
        if(count < 20) {
            count ++;
            return current->guest_no;
        }
        else {
            count = 0;
        }
    }
#endif

	next = ns_sys_current + 1;
	if (next >= boundary) /* reset to index 0 */
		next = ns_world;
#ifdef SW_OTZHYP_DEBUG    
	sw_printf("current = %x, next = %x\n", current, next);
#endif
/**
 * CPU context switching to a new guest OS
 * For every guest OS there is a cpu register context meant for NS world.
 * Secure world is common across all guest OS. CPU register context involves 
 * both core registers and system registers(eg. cp15).
 * a. The sytem register context of the 'current' guest OS(NS world) is saved 
 * and the same is restored with that of the 'next' guest OS to be scheduled.
 * b. Saving and restoring of core registers(NS world) is handled by the 
 * monitor fiq handler entry and exit path. 
 * So we just need to adjust he context pointers so that the right core 
 * register context gets restored during exit from the exception.
 */	
	ns_sys_current = next;	
	tzhyp_sysregs_switch(&current->sysctxt_cp15, &next->sysctxt_cp15);
	tzhyp_device_switch(current, next);
}

/**
* @brief 
*
* @return 
*/
int tzhyp_schedevent_init(void)
{

#ifdef SW_OTZHYP_DEBUG    
	sw_printf("tzhyp_schedevent_init\n");
#endif

	tzhypsched_event = timer_event_create(&tzhyp_schedevent_handler, 
					      (void*)NULL);
	if(!tzhypsched_event){
		sw_printf("xxx : Cannot register Handler\n");
		return -1;
	}

	/* Time duration = 40ms */
	tzhypsched_time.tval.nsec = 40000000;
	tzhypsched_time.tval.sec = 0;

	timer_event_start(tzhypsched_event, &tzhypsched_time);

	return 0;
}

/**
* @brief 
*
* @param x
*/
void tzhyp_schedevent_handler(struct timer_event *x)
{

	if (ns_preempt_flag) {
		tzhyp_schedule_guest();
	} 

	timer_event_destroy(tzhypsched_event);
	tzhyp_schedevent_init();
}


