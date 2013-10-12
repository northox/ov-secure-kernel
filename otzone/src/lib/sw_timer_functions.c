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
 * Implementation of sleep and other timer based functions.
 */

#include "sw_timer_functions.h"
#include "sw_io.h"
#include <sw_timer.h>
#include <sw_types.h>
#include <sw_debug.h>
#include <task.h>
#include <global.h>
#include "sw_board.h"

#if 0
/* the base address of the timer line */
extern u32 g_timer_addr;
/* the reading factor that we need to use */
extern u32 g_timer_ctrl;
extern u64 get_current_ticks();

s32int sw_sleep(u32 seconds)
{
	u64 current_ticks = 0,end_value = (seconds*10);
	/* we have a 1mhz clock. The current period of interrupts is one every
	 * 100ms. We need to wait till we get seconds*10 interrupts.
	 */
	current_ticks = get_current_ticks();
	while((get_current_ticks() - current_ticks) < end_value);
    return(0);
}

s32int sw_usleep(u32 seconds)
{
	u64 current_ticks = 0,end_value = seconds;
	/* we have a 1mhz clock. The current period of interrupts is one every
	 * 100ms. We need to wait till we get seconds interrupts. In effect, we
	 * do not do microseconds sleep, but milliseconds sleep
	 */
	current_ticks = get_current_ticks();
	while((get_current_ticks() - current_ticks) < end_value);
    return(0);
}

#endif

/**
 * @brief 
 *      This function wakes up the task which has called sleep
 *
 * @param tevent
 *      The timer_event structure used for putting this function to sleep
 */
void wake_up_from_sleep(struct timer_event* tevent)
{
    int task_id = *(int*)(tevent->data);

#ifdef TIMER_DBG
    sw_printf("SW: wake up from sleep\n");
    sw_printf("SW: task id %x\n",task_id);
#endif

    /* This is called from interrupt context so no locking may require */
    schedule_task_id(task_id);
    tevent->state &= ~TIMER_STATE_EXECUTING;
    timer_event_destroy(tevent);
    return;
}

/**
 * @brief 
 *      This function sleeps for given number of seconds
 * @param secs
 *      Number of secs to sleep
 */
void sw_sleep(u32 secs)
{
    timeval_t time;
    time.tval.nsec = 0;
    time.tval.sec = secs;

# ifndef CONFIG_EMULATE_FIQ
    int current_context;
    struct timer_event *tevent;

    current_context = get_current_task_id();

#ifdef TIMER_DBG
    sw_printf("SW: sleep sec 0x%08x nsec 0x%08x \n",time.tval.sec,time.tval.nsec);
#endif

    tevent = timer_event_create(&wake_up_from_sleep, &current_context);
    if(!tevent){
        sw_printf("SW: Out of memory : Cannot Perform Sleep\n");
        return;
    }

    timer_event_start(tevent, &time);

    suspend_task(current_context, TASK_STATE_WAIT);
    schedule();
# else

    u32 clockcycles = timeval_to_clockcycles(&time);
#ifdef TIMER_DBG
    sw_printf("SW: clockcycles 0x%08x \n",clockcycles);
#endif

    enable_timer();
    while(1){
        u32 curr_val = read_sleep_timer();
        if(curr_val > clockcycles){
            break;
        }
    }
    disable_timer();
# endif
    return;
}

/**
 * @brief 
 *      This function sleeps for given number of micro-seconds
 * @param usecs
 *      Number of micro-seconds to sleep
 */
void sw_usleep(u32 usecs)
{
    timeval_t time;
    time.tval.nsec = usecs * 1000;
    time.tval.sec = 0;

# ifndef CONFIG_EMULATE_FIQ
    int current_context;
    struct timer_event *tevent;

    current_context = get_current_task_id();

#ifdef TIMER_DBG
    sw_printf("SW: sleep sec 0x%08x nsec 0x%08x \n",time.tval.sec,time.tval.nsec);
#endif

    tevent = timer_event_create(&wake_up_from_sleep, &current_context);
    if(!tevent){
        sw_printf("SW: Out of memory : Cannot Perform Sleep\n");
        return;
    }

    suspend_task(current_context, TASK_STATE_WAIT);
    timer_event_start(tevent, &time);

    schedule();
# else
    u32 clockcycles = timeval_to_clockcycles(&time);

#ifdef TIMER_DBG
    sw_printf("SW: clockcycles 0x%08x \n",clockcycles);
#endif

    enable_timer();
    while(1){
        u32 curr_val = read_sleep_timer();
        if(curr_val > clockcycles){
            break;
        }
    }
    disable_timer();
# endif
    return;
}
