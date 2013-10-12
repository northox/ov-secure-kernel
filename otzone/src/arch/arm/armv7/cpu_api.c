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
 * Timer routines
 */

#include <sw_types.h>
#include <sw_board.h>
#include <sw_debug.h>
#include <global.h>
#include <cpu.h>
#include <gic.h>
#include <secure_timer.h>
#if 0
#include <sp804_sleep_timer.h>
#endif
/**
 * @brief Init secure kernel timer
 * 
 * This function initialize the secure kernel timer
 */
void timer_init(void)
{
    secure_timer_init(FREE_RUNNING_TIMER_BASE);
    secure_timer_init(TICK_TIMER_BASE);
/*  sp804_sleep_timer_init(SECURE_TIMER_BASE); */
}

/**
 * @brief Enable secure kernel timer
 * 
 * This function enables the secure kernel timer
 */
void enable_timer(void)
{
    secure_timer_init(FREE_RUNNING_TIMER_BASE);
    secure_set_timer(FREE_RUNNING_TIMER_BASE,0,
                      MODE_FREE_RUNNING, TIMER_COUNT_MAX);
    secure_timer_enable(FREE_RUNNING_TIMER_BASE, 0);
}

/**
 * @brief Disable secure kernel timer
 * 
 * This function disables the secure kernel timer
 */
void disable_timer(void)
{
    secure_timer_disable(FREE_RUNNING_TIMER_BASE, 0);
}


# ifdef CONFIG_EMULATE_FIQ
/**
 * @brief 
 *
 * @return 
 */
u32 read_sleep_timer(void)
{
    return ~read_timer_value(FREE_RUNNING_TIMER_BASE);

}
#endif

/**
 * @brief 
 *
 * @param usecs
 */
void trigger_tick(u64 usecs)
{
    if(usecs > TIMER_COUNT_MAX)
        usecs = TIMER_COUNT_MAX;
#ifdef CPU_API_DBG
    sw_printf("SW: new oneshot tick value 0x%x\n", (u32)usecs);
#endif

    if(usecs == 0) {
        sw_printf("SW: one shot time minimum value should be 1 us\n");
        usecs = 1;
    }

    secure_set_timer(TICK_TIMER_BASE, 0, MODE_ONESHOT, (u32)usecs);
    secure_timer_enable(TICK_TIMER_BASE, 0);
}

/**
 * @brief 
 *
 * @param clockcycles
 *
 * @return 
 */
u64 clockcycles_to_timeval(u32 clockcycles)
{
    timeval_t time;
    u64 tmp_nsecs;
    u64 tmp_secs;
    u64 tmp_usecs;
    u64 tmp_calc;
    u64 usecs;


    /* Divide by 1000
     * x/1000 = x>>10 + 3*x>>17 +9*x>>24;
     *   or
     *   y = x>>10;
     * ==> x/1000 =  y + (3*y)>>7 + (9*y)>>14
     */

    usecs  =  clockcycles * get_clock_period_us();
    
    /* Find number of seconds */
    tmp_secs = usecs;
    tmp_secs = (tmp_secs >> 10);
    tmp_secs = tmp_secs + ((3 * tmp_secs)>>7) + ((9 * tmp_secs)>>14);

    tmp_secs = (tmp_secs >> 10);
    tmp_secs = tmp_secs + ((3 * tmp_secs)>>7) + ((9 * tmp_secs)>>14);
 
    /* Multiply number of seconds with 10^6 to convert it to number of
     * clockcycles and then subtract it from the total number of
     * clockcycles to obtain the number of microseconds
     */
    /* Multiply by thousand
     * 1000 = 1024 + 8 - 32
     *      = 2^10 + 2^3 - 2^5
     */
    tmp_calc = (tmp_secs << 10) + (tmp_secs << 3) - (tmp_secs << 5);
    tmp_calc = (tmp_calc << 10) + (tmp_calc << 3) - (tmp_calc << 5);
    tmp_usecs = usecs - tmp_calc;

    tmp_nsecs = (tmp_usecs<<10) + (tmp_usecs <<3) - (tmp_usecs<<5);

    time.tval.nsec = tmp_nsecs;
    time.tval.sec = tmp_secs;

    return time.tval64;
}

/**
 * @brief 
 *
 * @param time
 *
 * @return 
 */
u64 timeval_to_clockcycles(timeval_t *time)
{
    u64 useconds = 0;
    u64 clockcycles;
    u64 tmp_sec , tmp_nsec;

    if((time->tval.sec < 0) || (time->tval.nsec < 0))
        return 0;

    tmp_sec = time->tval.sec;

    if(tmp_sec > 0) {
        /* multiply by 1000 * 1000 */
        tmp_sec = (tmp_sec<<10) + (tmp_sec <<3) - (tmp_sec<<5);
        tmp_sec = (tmp_sec<<10) + (tmp_sec <<3) - (tmp_sec<<5);
    }

    /* Divide by 1000
     * x/1000 = x>>10 + 3*x>>17 +9*x>>24;
     *   or
     *   y = x>>10;
     * ==> x/1000 =  y + (3*y)>>7 + (9*y)>>14
     */
    tmp_nsec = time->tval.nsec;
    tmp_nsec = tmp_nsec >> 10;
    tmp_nsec = tmp_nsec + ((3 * tmp_nsec)>>7) + ((9*tmp_nsec)>>14);

    useconds = tmp_nsec;
    useconds += tmp_sec;

    clockcycles = (u32)useconds / (u32)get_clock_period_us();

/* Minimum clock cycles to be 1 clock period */
    if(clockcycles == 0)
	    clockcycles = get_clock_period_us(); 
    return clockcycles;
}


/**
 * @brief Emulate timer IRQ functionality
 */
void emulate_timer_irq(void)
{
#ifdef CONFIG_EMULATE_FIQ
/*      asm volatile("swi #0xaaaa"); */
     emulate_swi_handler(0xbbbb);
#endif
}

/**
* @brief Invoke scheduler
*
* This function invokes the scheduler to schedule the next ready task
*/
void schedule(void)
{
#ifdef CONFIG_EMULATE_FIQ
    emulate_timer_irq();
#else
    asm volatile("swi #0xbbbb");    
#endif
}


#ifdef OTZONE_ASYNC_NOTIFY_SUPPORT
/**
 * @brief Invoke non-secure kernel callback handler
 *
 * @param callback_fn: Callback function
 * @param svc_id: Service ID
 * @param session_id: Session ID
 * @param enc_id: Encoded context ID
 * @param client_pid: Client process ID
 */
void invoke_ns_callback(u32 svc_id, u32 session_id, u32 enc_id, u32 client_pid,
                                                        u32 dev_file_id)
{
    set_notify_data(svc_id, session_id, enc_id, client_pid, dev_file_id);
    generate_soft_int_to_core0(NS_SGI_NOTIFY_INTERRUPT);

    return;
}
#endif
