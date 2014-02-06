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
 * SW Timer declarations.
 */

#ifndef __TIMER_FRAME_WORK_
#define __TIMER_FRAME_WORK_

#include <sw_types.h>
#include <sw_list.h>
#include <sw_semaphores.h>

#define NSECS_PER_SEC   1000000000L
/*Seconds will be set to maximum value and the number of nanoseconds
 * will be zero */
#define TIMEVAL_MAX     (((s64)~((u64)1 << 63)) & (~((u64)0xFFFFFFFF)))
#define MAX_NUM_OF_TIMERS 2

    /*The timer event is Inactive*/
#define    TIMER_STATE_INACTIVE     0x00
    /* The timer event is active and is waiting for expiration*/
#define    TIMER_STATE_ACTIVE       0x01
    /*The timer is expired and is waiting on the callback list to be executed*/
#define    TIMER_STATE_PENDING      0x02
    /* The timer event is currently being executed */
#define    TIMER_STATE_EXECUTING    0x04


/**
 * @brief 
 *  Structure to represent the time
 *  in High resolution format( Resolution in Nano seconds)
 */
typedef union {
    s64 tval64;
    struct {
        s32 nsec, sec;
    }tval;
}timeval_t;

/**
* @brief 
*/
enum timer_callback_mode {
    /* The handler function should be run in softirq*/
    TIMER_CALLBACK_SOFTIRQ,
    /* The handler function should be run in hardirq context itself*/
    TIMER_CALLBACK_HARDIRQ,
    /* The handler function should be executed in hardirq and it should not
     * restart the timer*/
    TIMER_CALLBACK_HARDIRQ_NORESTART
};

/**
* @brief 
*/
enum timer_cbfn_return_value{
    TIMER_RESTART,
    TIMER_NORESTART
};

/**
* @brief 
*/
struct timer_clock_info {
    struct timer_cpu_info* cpu_info;
    int    clock_id;
    
    struct spinlock spin_lock;
    struct list active;    
    timeval_t   clock_period;
    timeval_t   timer_period;

    int shift;
    unsigned long mult;
};

/**
* @brief 
*/
struct timer_cpu_info {
        struct timer_clock_info clock_info[MAX_NUM_OF_TIMERS];

        timeval_t expires_next;
        struct list pending_routines;
        unsigned long num_events;
        int free_run_clock_id;
        int tick_timer_id;

};

extern struct timer_cpu_info timer_cpu_info;

/**
* @brief 
*/
struct timer_event {
    struct list node;  
    struct list callback_entry;

    timeval_t   expires;
    struct timer_clock_info* clk_info;
    int (*handler)(void* );
    int state;
    int callback_mode; 
    void* data;
};

typedef void (*sw_timer_event_handler) (struct timer_event*);

/**
* @brief 
*
* @param handler
* @param priv_data
*
* @return 
*/
struct timer_event* timer_event_create(sw_timer_event_handler handler, void* priv_data);

/**
* @brief 
*
* @param tevent
*
* @return 
*/
int timer_event_destroy(struct timer_event* tevent);

/**
* @brief 
*
* @param tevent
* @param time
*/
void timer_event_start(struct timer_event *tevent, timeval_t *time);

/**
* @brief 
*/
void __free_running_cntr_intr(void);

/**
* @brief 
*
* @return 
*/
u64 read_timestamp(void);

/**
* @brief 
*/
void timer_interrupt(void);

/**
* @brief 
*/
void init_sw_timer(void );

/**
* @brief 
*
* @param value1
* @param value2
*
* @return 
*/
int timeval_after(const timeval_t *value1, const timeval_t *value2);

/**
* @brief 
*
* @param value1
* @param value2
*
* @return 
*/
int timeval_after(const timeval_t *value1, const timeval_t *value2);

/**
* @brief 
*
* @return 
*/
u32 get_clock_period_us(void);

#endif
