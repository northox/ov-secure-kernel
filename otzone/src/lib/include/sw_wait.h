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
 * SW Wait-Queue Definition. 
 */

#ifndef __SW_WAIT_H_
#define __SW_WAIT_H_

#include <sw_types.h>
#include <sw_semaphores.h>
#include <sw_list.h>
#include <sw_timer.h>
#include <task.h>


#define TASK_INTERRUPTIBLE   0
#define TASK_UNINTERRUPTIBLE 1

#define WAKE_UP             0
#define WAKE_UP_IMMEDIATE   1    

struct wait_queue;

/**
* @brief 
*
* @param 
* @param u32
*
* @return 
*/
typedef void (*wait_queue_function)(struct wait_queue* , u32);

/**
 * @brief
 *  Wait queue list head.
 *  Wait queue elements are added to this
 */
struct wait_queue_head{
    int elements_count;
    struct spinlock spin_lock;
    struct list elements_list;
};

/**
 * @brief 
 *  Wait queue element
 */
struct wait_queue{
    struct list head;
    void* data;
    wait_queue_function func;
};


#define __sw_wait_event(wq_head, condition)                     \
do {                                                            \
                                                                \
    struct wait_queue wq;                                       \
    INIT_LIST_HEAD(&wq.head);                                   \
    wq.data = get_current_task();                                          \
    wq.func = sw_default_wake_function;                         \
                                                                \
    while(1){                                                   \
                                                                \
        if(condition)                                           \
            break;                                              \
        sw_prepare_for_wait(wq_head, &wq, TASK_STATE_WAIT);   \
        schedule();                                             \
    }                                                           \
                                                                \
    sw_complete_wait(wq_head, &wq);                                   \
                                                                \
                                                                \
}while(0)


/**
 * @brief 
 *  This is called to put the current task in a waitqueue.
 *  When the condition is satisfied the task is woken up
 *
 * @param wq_head
 *  The waitqueue head to which the current task need to be added
 *
 * @param condition
 *   The condition which needs to be satisfied to resume the task
 *
 * @return 
 *     Returns nothing
 */
#define sw_wait_event(wq_head, condition)    \
do {                                        \
    if(condition)                           \
        break;                              \
    __sw_wait_event(wq_head, condition);     \
}while(0)

#ifdef OTZONE_ASYNC_NOTIFY_SUPPORT

#define __sw_wait_event_async(wq_head, condition, retval)                     \
do {                                                            \
                                                                \
    struct wait_queue wq;                                       \
    INIT_LIST_HEAD(&wq.head);                                   \
    wq.data = get_current_task();                                          \
    wq.func = sw_default_wake_function;                         \
    int call_smc = 1;   \
                                                                \
    while(1){                                                   \
                                                                \
        if(condition)                                           \
            break;                                              \
        sw_prepare_for_wait(wq_head, &wq, TASK_STATE_WAIT);   \
        if(call_smc){                                          \
            call_smc = 0;                   \
            set_secure_api_ret(retval); \
        }                                   \
        schedule();                                             \
    }                                                           \
                                                                \
    sw_complete_wait(wq_head, &wq);                                   \
                                                                \
                                                                \
}while(0)

/**
 * @brief 
 *      This function is called to put the current task to sleep
 *      and notify the non-secure world about this
 *      Task pending signal is sent to non-secure world
 *      
 * @param wq_head
 *      The wait queue head on which the task is put to sleep
 * @param condition
 *      The condition to wake up
 * @param retval
 *      The flag to be sent to non-secure world
 * @return 
 */
#define sw_wait_event_async(wq_head, condition, retval)    \
do {                                        \
    if(condition)                           \
        break;                              \
    __sw_wait_event_async(wq_head, condition, retval);     \
}while(0)

#endif
/**
 * @brief 
 *      This function wakes up an element from the given waitqueue 
 *
 * @param wq_head
 *      The waitqueue head from which all the elements need to be woken up
 * @param wakeup_flag
 *      It says whether the element need to be woken up immediately or can be
 *      woken up later
 */

void sw_wakeup(struct wait_queue_head * wq_head, u32 wakeup_flag);

/**
 * @brief 
 *      This function wakes up all the elements attached to the given 
 *      waitqueue head
 * @param wq_head
 *      The waitqueue head from which all the elements need to be woken up
 *      
 * @param wakeup_flag
 *      It says whether the element need to be woken up immediately or can be
 *      woken up later
 */

void sw_wakeup_all(struct wait_queue_head * wq_head, u32 wakeup_flag);

/**
 * @brief 
 *     This function wakes up a task by changing the state of the task and
 *     adding it to the ready to run list 
 * @param wq
 *      The waitqueue element which need to be waken up
 * @param wakeup_flag
 *      It says whether the element need to be woken up immediately or can be
 *      woken up later
 */
void sw_default_wake_function(struct wait_queue* wq, u32 flag);

/**
 * @brief 
 *      Initializes the wait queue element
 *
 * @param wq
 *      The wait queue element 
 * @param func
 *      The function to be called while waking up the element fom waitqueue
 */
void sw_init_waitqueue_entry_func(struct wait_queue* wq,
        wait_queue_function func);
/**
 * @brief 
 *  Initializes the wait queue element
 *
 * @param wq
 *  The wait queue element
 */
void sw_init_waitqueue_entry(struct wait_queue* wq);

/**
 * @brief 
 *      This function is called to add the wait queue element to the wait queue
 *      head
 * @param wq_head
 *      Wait queue list head to which the element need to be added
 * @param wq
 *      The waitqueue element
 */
void sw_add_to_waitqueue(struct wait_queue_head* wq_head, 
                            struct wait_queue* wq);

/**
 * @brief 
 *      This function is called to remove the wait queue element from 
 *      the wait queue head 
 * @param wq_head
 *      Wait queue list head from which the element needs to be deleted
 * @param wq
 *      The waitqueue element
 */
void sw_remove_from_waitqueue(struct wait_queue_head* wq_head,
                                            struct wait_queue* wq);

/**
 * @brief 
 *      This function sets the state of the TASK 
 * @param state
 *      The state to which the current task need to be moved
 */
void sw_set_task_state(int state);

/**
 * @brief
 *      This function sets the state of the TASK to the given state and adds the
 *      element to the waitqueue
 *
 * @param wq_head
 *          The list head to which the element need to be added
 * @param wq
 *          The waitqueue element
 * @param state
 *          The state to which the current TASK need to be set
 */
void sw_prepare_for_wait(struct wait_queue_head* wq_head, struct wait_queue* wq,
                                u32 state);

/**
 * @brief    
 *      This function is called when the task is woken up and this function
 *      removes the waitqueue element from the waitqueue
 * @param wq_head
 *      The waitqueue head to which this element is attached
 * @param wq
 *      The wait queue element
 */
void sw_complete_wait(struct wait_queue_head *wq_head, struct wait_queue *wq);

#endif
