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
 * SW Timer Implementation.
 */

#include <sw_types.h>
#include <sw_list.h>
#include <sw_timer.h>
#include <global.h>
#include <task.h>
#include <sw_buddy.h>
#include <sw_debug.h>
#include <sw_mem_functions.h>

/*
#define TIMER_DBG 1
*/


struct timer_cpu_info timer_cpu_info;


/**
 * @brief 
 * The elements of this structure are useful
 * for implementing the sw_timer
 */
static struct sw_tmr_info{

    timeval_t sw_timestamp;
    u64 abs_cycles_count;
    u64 cycles_count_old;
    u64 cycles_count_new;

    timeval_t timer_period;
    timeval_t clock_period;
}sw_timer_info;

/**
 * @brief
 * This function initializes the Timer structure variables
 */
void init_sw_timer(void )
{
    /* TODO :Later, this function should be made to read values
     * from Device Tree */
    int iter;

    timer_cpu_info.expires_next.tval64 = TIMEVAL_MAX;
    INIT_LIST_HEAD(&timer_cpu_info.pending_routines);
    timer_cpu_info.num_events = 0;

    for(iter = 0; iter < MAX_NUM_OF_TIMERS; iter++)
    {
        timer_cpu_info.clock_info[iter].cpu_info = &timer_cpu_info;
        timer_cpu_info.clock_info[iter].clock_id = iter;
        INIT_LIST_HEAD(&timer_cpu_info.clock_info[iter].active);
        INIT_SPIN_LOCK(&timer_cpu_info.clock_info[iter].spin_lock);
        timer_cpu_info.clock_info[iter].clock_period.tval64 = 
                                                        get_clock_period();
        timer_cpu_info.clock_info[iter].timer_period.tval64 = 
                                                        get_timer_period();
    }

    /* TODO :Later this should be made to obtain from Device Tree */
    timer_cpu_info.free_run_clock_id = 0;
    timer_cpu_info.tick_timer_id = 1;

#define SW_FREE_RUNNING_CNTR    timer_cpu_info.free_run_clock_id
#define SW_TICKTIMER            timer_cpu_info.tick_timer_id


    sw_timer_info.sw_timestamp.tval64 = 0;
    sw_timer_info.abs_cycles_count = 0;
    sw_timer_info.cycles_count_new = 0;
    sw_timer_info.cycles_count_old = 0;
    sw_timer_info.timer_period.tval64 = get_timer_period();
    sw_timer_info.clock_period.tval64 = get_clock_period();

}

/**
 * @brief 
 *      This function adds the two elements and returns the sum
 * @param value1
 *      Time value 1
 * @param value2
 *      Time value 2
 * @return
 *      The Sum of the input arguments
 */
static u64 timeval_add(const timeval_t *value1, const timeval_t *value2)
{
    timeval_t sum;
    sum.tval64 = value1->tval64 + value2->tval64;

    if(sum.tval.nsec >  (NSECS_PER_SEC- 1)){
        sum.tval.nsec -= NSECS_PER_SEC;
        sum.tval.sec += 1;
    }

    return sum.tval64;
}

/**
 * @brief 
 *      This function subtracts value 1 from value 2 and
 *      returns the difference
 * @param value1
 *      Time value 1
 * @param value2
 *      Time value 2
 * @return 
 *      The difference between the Input arguments
 */
static u64 timeval_sub(const timeval_t *value1 , const timeval_t *value2)
{
    timeval_t diff;
    diff.tval64 = value1->tval64 - value2->tval64;

    if(diff.tval.nsec < 0){
        diff.tval.nsec += NSECS_PER_SEC;
    }

    return diff.tval64;
}

/**
 * @brief 
 *      Checks whether value 1 comes after value2
 *      If yes returns 1 else returns 0
 * @param value1
 *      Time value1
 * @param value2
 *      Time value 2 
 * @return 
 *      1 - If value1 comes after value2
 *      0 - If value2 comes after value1
 */
int timeval_after(const timeval_t *value1, const timeval_t *value2)
{   
    timeval_t diff;
    diff.tval64  = timeval_sub(value1, value2);

    if(diff.tval.sec > 0 ||
            ((diff.tval.sec == 0) && (diff.tval.nsec > 1000)))
        return 1;
    else
        return 0;

}

/**
 * @brief 
 *      Checks whether value 1 comes before value2
 *      If yes returns 1 else returns 0
 * @param value1
 *          Time value 1
 * @param value2
 *          Time value 2
 * @return 
 *      1 - If value1 comes before value2
 *      0 - If value2 comes before value1
 */
int timeval_before(const timeval_t *value1, const timeval_t *value2)
{
    if(timeval_after(value1,value2))
        return 0;
    else
        return 1;
}

/**
 * @brief 
 *      This function adds the timer event to the list;
 *      The list elements are such that they are in the ascending order of time
 *      of expiry
 *
 * @param head
 *      The head of the list
 * @param new
 *      The new event to be added
 */
static void timer_queue_add(struct list* head , struct timer_event* new )
{
    struct timer_event *temp;
    int added_to_list = 0;


    if(list_empty(head)){
        list_add(head, &new->node);
        added_to_list = 1;
    }
    else {
        list_for_each_entry(temp, head, node){

            if(new->expires.tval64 < temp->expires.tval64){
                list_add_tail(&temp->node , &new->node);
                added_to_list = 1;
                break;
            }
        }
    }

    if(!added_to_list)
        list_add_tail(head, &new->node);
    
    new->state = TIMER_STATE_ACTIVE;

    return;
}

/**
 * @brief 
 *      This function deletes the timer event from the 
 *      list
 * @param tevent
 *      The event which needs to be deleted
 */
static void timer_queue_del(struct timer_event* tevent )
{

    if(!tevent)
        return;

    list_del(&tevent->node);
    tevent->state = TIMER_STATE_INACTIVE;
}

/**
 * @brief 
 *      This function returns the next element in the list
 * @param head
 *      The element whose next element need to be returned
 * @return 
 *      -The next element 
 *      -NULL : If the next element is not present 
 */
static struct list* timer_queue_getnext(struct list* head)
{
    struct list* next;

    if(list_empty(head))
        next = NULL;
    else
        next =  head->next;

    return next;
}

/**
 * @brief
 *  This function updates the timestamp
 *  This function is called only when the free running counter interrupt is
 *  generated.(When free running counter reaches zero this interrupt arises and
 *  after that the counter reload itself to the max value(mostly 0xFFFFFFFF) and
 *  continue to count down)
 *
 *  Otherwise the timestamp is updated by read_timestamp function
 */
void __free_running_cntr_intr(void)
{
    u64 tmp_cycles;
    timeval_t tmp_timeval;

    sw_timer_info.cycles_count_new = TIMER_COUNT_MAX;   
    tmp_cycles = sw_timer_info.cycles_count_new - 
                                    sw_timer_info.cycles_count_old;

    sw_timer_info.abs_cycles_count += tmp_cycles;
    sw_timer_info.cycles_count_old = 0;

    tmp_timeval.tval64 = clockcycles_to_timeval(tmp_cycles);
    sw_timer_info.sw_timestamp.tval64 = timeval_add(&sw_timer_info.sw_timestamp,
                                                                &tmp_timeval);
    return;
}

/**
 * @brief 
 *      It updates and returns the current timestamp value
 * @return 
 *      The current timestamp value
 */
u64 read_timestamp(void)
{
    u64 tmp_cycles;
    timeval_t tmp_timeval;

    sw_timer_info.cycles_count_new = read_freerunning_cntr();
    tmp_cycles = sw_timer_info.cycles_count_new - 
                                        sw_timer_info.cycles_count_old;

    sw_timer_info.abs_cycles_count += tmp_cycles;
    sw_timer_info.cycles_count_old = sw_timer_info.cycles_count_new;

    tmp_timeval.tval64 = clockcycles_to_timeval(tmp_cycles);
    sw_timer_info.sw_timestamp.tval64 = timeval_add(&sw_timer_info.sw_timestamp,
                                                                &tmp_timeval);
#ifdef TIMER_DBG
    sw_printf("SW: Timestamp :  sec 0x%08x nsec 0x%08x \n",
            sw_timer_info.sw_timestamp.tval.sec,
            sw_timer_info.sw_timestamp.tval.nsec);
#endif
    return sw_timer_info.sw_timestamp.tval64;
}


/**
 * @brief 
 *      This function is called to create a new timer event.
 *      It allocates the memory for timer event struct, initializes the
 *      elements and returns the structure.
 *      If it cannot allocate the memory then it returns NULL
 *
 * @param handler
 *      The handler which will be called on the expiration of the timer
 * @param priv_data
 *      The data which may be needed by the handler
 *
 * @return
 *      -The timer_event structure allocated and initialized
 *      -NULL if it fails to allocate the structure
 */
struct timer_event* timer_event_create(sw_timer_event_handler handler,
                                                            void* priv_data)
{
    struct timer_event* new_event;
  
    new_event = (struct timer_event*)sw_malloc_private(COMMON_HEAP_ID,
                                                    sizeof(struct timer_event));
    if(!new_event){
        sw_printf("SW: Malloc failed in Creating New Timer event \n");
        return NULL;
    }

    sw_memset( new_event, 0, sizeof(struct timer_event));
   
    new_event->clk_info = &timer_cpu_info.clock_info[SW_TICKTIMER];
    INIT_LIST_HEAD(&new_event->node);
    new_event->handler = (void*)handler;
    new_event->expires.tval64 = TIMEVAL_MAX;
    new_event->state = TIMER_STATE_INACTIVE;
    new_event->callback_mode = TIMER_CALLBACK_HARDIRQ;
    new_event->data = priv_data;

    return new_event;
}

/**
 * @brief 
 *      This function deletes the timer event structure
 *      After expiration of the timer ,the application can choose to keep
 *      and reuse the same structure again or delete it.If it decides to delete
 *      the structure then this function is called.
 *      This function frees the memory
 *
 * @param tevent
 *      The structure which needs to be deleted
 *
 * @return
 *       0 on success 
 *      -1 on failure
 */
int timer_event_destroy(struct timer_event* tevent)
{
    if(!tevent)
        return -1;

    if(tevent->state == TIMER_STATE_INACTIVE){
        sw_free_private(COMMON_HEAP_ID, tevent);
        return 0;
    }
    sw_printf("SW: Timer Event in USE :Cannot Free this Timer Event \n");
    return -1;
}

/**
 * @brief 
 *      Sets the absolute expire time to the timer event
 * @param tevent
 *      timer event structure for which absolute expire time need to be assigned
 * @param abs_time
 *      Absolute expire time
 */
static void timer_event_set_expire(struct timer_event *tevent, 
                                                    timeval_t *abs_time)
{
    tevent->expires = *abs_time;
#ifdef TIMER_DBG    
    sw_printf("SW: Set expire time: sec 0x%08x nsec 0x%08x \n",
                    tevent->expires.tval.sec,tevent->expires.tval.nsec);
#endif
}

/**
 * @brief 
 *      The expire time is obtained from the given timer event structure
 * @param tevent
 *      The timer event structure
 * @return 
 *  Absolute expire time
 */
static u64 timer_get_expire(struct timer_event *tevent)
{
    return tevent->expires.tval64;
}

/**
 * @brief 
 *      This function programs the hardware (by calling appropriate functions)
 *      to generate an interrupt at the expires time(the argument)
 *
 * @param expires
 *      The time at which an interrupt need to be generated
 */
static void program_tick_for_event(u64 expires_time)
{
    timeval_t time_till_next_tick;
    u64 clockcycles;

    timeval_t expires;
    timeval_t now;

    expires.tval64 = expires_time;
    now.tval64 = read_timestamp();
    time_till_next_tick.tval64  = timeval_sub(&expires, &now);

    if((time_till_next_tick.tval.sec < 0) ||
            (time_till_next_tick.tval.nsec < 0)) {
        clockcycles = 1;
        goto set_trigger;
    }

#ifdef TIMER_DBG   
    sw_printf("SW: Inside program_tick_for_event\n");
    sw_printf("SW: Expiration time: sec 0x%08x nsec 0x%08x \n",
                    expires.tval.sec, expires.tval.nsec);
    sw_printf("SW: time_till_next_tick : sec 0x%08x nsec 0x%08x \n",
                    time_till_next_tick.tval.sec, time_till_next_tick.tval.nsec);
#endif

    clockcycles = timeval_to_clockcycles(&time_till_next_tick);

#ifdef TIMER_DBG   
    sw_printf("SW: clockcycles %08x\n",(u32)clockcycles);
#endif

set_trigger:
    timer_cpu_info.expires_next = expires;

    trigger_tick(clockcycles);
    return;
}

/**
 * @brief 
 *      It starts the timer event by calling the appropriate functions
 *      The time interval is written to the hardwire timer and the event is
 *      added to the list of events to be handled on timer expiry
 * @param tevent
 *      The timer event which needed to be started
 * @param time
 *      The time duration after which the event need to be expire  
 */
void timer_event_start(struct timer_event *tevent, timeval_t *time)
{
    timeval_t temp_time;
    irq_flags_t irqflags;

    if(!tevent)
        return ;

    temp_time.tval64 = read_timestamp();
    time->tval64 =  timeval_add(&temp_time, time);

#ifdef TIMER_DBG
    sw_printf("SW: Timer Event start time  sec 0x%08x nsec 0x%08x \n",
            time->tval.sec,time->tval.nsec);
#endif

    timer_event_set_expire(tevent, time);

    spin_lock_irqsave(&tevent->clk_info->spin_lock, &irqflags);
    timer_queue_add(&tevent->clk_info->active, tevent); 
    spin_unlock_irqrestore(&tevent->clk_info->spin_lock, irqflags);


    if(timer_cpu_info.expires_next.tval64 > tevent->expires.tval64){
        program_tick_for_event(tevent->expires.tval64);
    }
    return;
}

/**
 * @brief 
 *      This function is called from the timer interrupt function
 *      This function calls the handler function of the expired timer
 * @param tevent
 *      The event whose handler need to be called
 */
static void __run_timer_handler(struct timer_event* tevent)
{
    sw_timer_event_handler hndl;

    timer_cpu_info.num_events++;

    /* No need for spinlock as it is called from Interrupt context */
    timer_queue_del(tevent);

    hndl = (sw_timer_event_handler)tevent->handler;

    if(hndl)
        hndl(tevent);

    return;
}

/**
 * @brief
 *      This is the function which is called on expiry of any timer event.
 *      This fn goes through the list of registered timer events. If the expiry
 *      time of the event is already in the past then the corresponding 
 *      handler function is called.
 *      As the list is already sorted according to their time of
 *      expiration, the function returns either when it encounters the first event
 *      whose expiry time is in the future or when the list becomes empty.
 */
void timer_interrupt(void)
{
    struct timer_clock_info* clock_info;
    struct list *node;    
    struct timer_event* tevent;

    timeval_t expires_next, now, temp_time, time_diff;
    expires_next.tval64 = TIMEVAL_MAX;

#ifdef TIMER_DBG
    sw_printf("SW: Inside timer interrupt\n");
#endif

    clock_info = &timer_cpu_info.clock_info[SW_TICKTIMER];

    while((node = timer_queue_getnext(&clock_info->active))){

        tevent = list_entry(node, struct timer_event, node);
        temp_time.tval64 = timer_get_expire(tevent);

        now.tval64 = read_timestamp();


        time_diff.tval64 = timeval_sub(&temp_time, &now);

#ifdef TIMER_DBG
        sw_printf("SW: current 0x%x:0x%x and expires 0x%x:0x%x and diff 0x%x:0x%x\n", 
                now.tval.sec, now.tval.nsec, 
                temp_time.tval.sec, temp_time.tval.nsec,
                time_diff.tval.sec, time_diff.tval.nsec);
#endif

        if((time_diff.tval.sec > 0) || 
            ((time_diff.tval.sec == 0) && time_diff.tval.nsec > 1000)) { 
/*        if( now.tval64 < temp_time.tval64){ */
            timeval_t expires;

            expires = temp_time;

            if( expires.tval64 < expires_next.tval64) 
                expires_next = expires;
	    
	    program_tick_for_event(expires_next.tval64);
            break;
        }

        tevent->state = TIMER_STATE_EXECUTING;
        __run_timer_handler(tevent);
    }

    timer_cpu_info.expires_next.tval64 = TIMEVAL_MAX;
    return;
}

/**
 * @brief 
 *      checks whether the timer event is active 
 * @param tevent
 *      The timer event whose state need to be checked
 * @return 
 *      1 - If active
 *      0 - If not active
 */
int is_timer_event_active(struct timer_event* tevent)
{
    if(!tevent)
        return 0;

    if(tevent->state & TIMER_STATE_ACTIVE)
        return 1;
    else
        return 0;
}

/**
 * @brief 
 *      checks whether the timer event is waiting in callback mode
 * @param tevent
 *      The timer event whose state need to be checked
 * @return 
 *      1 - If it is waiting on callback
 *      0 - If it is not waiting on callback
 */
int is_timer_event_waiting_on_callback(struct timer_event* tevent)
{
    if(!tevent)
        return 0;

    if (tevent->state & TIMER_STATE_PENDING)
        return 1;
    else 
        return 0;
}

/**
 * @brief
 *      Stops a timer_event
 *
 * @param tevent
 */
void timer_event_stop(struct timer_event* tevent)
{
    struct timer_clock_info* clock_info;
    struct timer_event* next_event;
    timeval_t expires,expires_next;
    irq_flags_t irqflags;

    if(is_timer_event_waiting_on_callback(tevent))
        return;

    if(is_timer_event_active(tevent)){

        clock_info = &timer_cpu_info.clock_info[SW_TICKTIMER];

        spin_lock_irqsave(&tevent->clk_info->spin_lock, &irqflags);

        if(&tevent->node == timer_queue_getnext(&clock_info->active)){

            spin_unlock_irqrestore(&tevent->clk_info->spin_lock, irqflags);
            expires.tval64 = timer_get_expire(tevent); 

            if(expires.tval64 == timer_cpu_info.expires_next.tval64){

               if(tevent->node.next == &clock_info->active)
                   expires_next.tval64 = TIMEVAL_MAX;
               else{
                   next_event = list_entry(tevent->node.next,struct timer_event, node);
                   expires_next = next_event->expires;
               }

               if(expires_next.tval64 != timer_cpu_info.expires_next.tval64)
                   program_tick_for_event(expires_next.tval64);
               
           }
        }
        else {
            spin_unlock_irqrestore(&tevent->clk_info->spin_lock, irqflags);
        }

        spin_lock_irqsave(&tevent->clk_info->spin_lock, &irqflags);
        timer_queue_del(tevent);
        spin_unlock_irqrestore(&tevent->clk_info->spin_lock, irqflags);

    }
    return;
}
