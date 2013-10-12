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
 * SW Wait-Queue Implementation.
 */

#include <task.h>
#include <sw_wait.h>
#include <sw_debug.h>
#include <sw_timer.h>
#include <global.h>

/**
 * @brief 
 *      This function is called to add the waitqueue element waitqueue list.
 *
 * @param wq_head
 *      Wait queue list head to which the element need to be added
 * @param wq
 *      The waitqueue element
 */
static void __add_to_waitqueue(struct wait_queue_head* wq_head,
                                            struct wait_queue* wq)
{
    list_add(&wq_head->elements_list, &wq->head);
    wq_head->elements_count++;
    return;
}

/**
 * @brief 
 *      This function is called to add the wait queue element to the wait queue
 *      head
 * @param wq_head
 *      Wait queue list head to which the element need to be added
 * @param wq
 *      The waitqueue element
 */
void sw_add_to_waitqueue(struct wait_queue_head* wq_head, struct wait_queue* wq)
{
    u32 irqflags;
    spin_lock_irqsave(&wq_head->spin_lock, &irqflags);
    __add_to_waitqueue(wq_head, wq);
    spin_unlock_irqrestore(&wq_head->spin_lock, irqflags);
}

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
                                                struct wait_queue* wq)
{
    u32 irqflags;
    spin_lock_irqsave(&wq_head->spin_lock, &irqflags);
    list_del(&wq->head);
    wq_head->elements_count--;
    spin_unlock_irqrestore(&wq_head->spin_lock, irqflags);
}

/**
 * @brief 
 *  Initializes the wait queue element
 *
 * @param wq
 *  The wait queue element
 */
void sw_init_waitqueue_entry(struct wait_queue* wq)
{
    if(wq){
        wq->data = get_current_task();
        wq->func = sw_default_wake_function;
    }
    return;
}

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
        wait_queue_function func)
{
    if(wq){
        wq->data = get_current_task();
        wq->func = func;
    }
    return;
}

/**
 * @brief 
 *      This function sets the state of the TASK 
 * @param state
 *      The state to which the current task need to be moved
 */
void sw_set_task_state(int state)
{
    int task_id = get_current_task_id();
    suspend_task(task_id, state);
}

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
                        u32 state)
{
    u32 irqflags;
    int task_id;

    spin_lock_irqsave(&wq_head->spin_lock, &irqflags);
    if(list_empty(&wq->head)){
        __add_to_waitqueue(wq_head, wq);
    }

    task_id = get_current_task_id();
    suspend_task(task_id, state); 
    spin_unlock_irqrestore(&wq_head->spin_lock,irqflags);
    return;
}

/**
 * @brief    
 *      This function is called when the task is woken up and this function
 *      removes the waitqueue element from the waitqueue
 * @param wq_head
 *      The waitqueue head to which this element is attached
 * @param wq
 *      The wait queue element
 */
void sw_complete_wait(struct wait_queue_head *wq_head, struct wait_queue *wq)
{
    u32 irqflags;

    if(!list_empty(&wq->head)){
        spin_lock_irqsave(&wq_head->spin_lock, &irqflags);
        list_del(&wq->head);
        INIT_LIST_HEAD(&wq->head);
        wq_head->elements_count--;
        spin_unlock_irqrestore(&wq_head->spin_lock, irqflags);
    }

    return;
}

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
void sw_wakeup_all(struct wait_queue_head *wq_head, u32 wakeup_flag)
{
    u32 irqflags;
    struct wait_queue *wq;

    spin_lock_irqsave(&wq_head->spin_lock, &irqflags);

    list_for_each_entry(wq, &wq_head->elements_list, head){

        spin_unlock_irqrestore(&wq_head->spin_lock, irqflags);
        wq->func(wq, wakeup_flag);
        spin_lock_irqsave(&wq_head->spin_lock, &irqflags);
    }

    spin_unlock_irqrestore(&wq_head->spin_lock, irqflags);

}

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
void sw_wakeup(struct wait_queue_head * wq_head, u32 wakeup_flag)
{
    u32 irqflags;
    struct wait_queue *wq;
#ifdef WQ_DBG
    sw_printf("SW: Inside Wake up function\n");
#endif

    spin_lock_irqsave(&wq_head->spin_lock, &irqflags);

    list_for_each_entry(wq, &wq_head->elements_list, head){

        spin_unlock_irqrestore(&wq_head->spin_lock, irqflags);
        wq->func(wq, wakeup_flag);
        return;
    }
        spin_unlock_irqrestore(&wq_head->spin_lock, irqflags);

}

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
void sw_default_wake_function(struct wait_queue* wq, u32 wakeup_flag)
{
    u32 irqflags;
    struct sw_task* task = wq->data;

#ifdef WQ_DBG
    sw_printf("SW: Inside Default Wake up function\n");
#endif
    spin_lock_irqsave(&global_val.ready_to_run_lock, &irqflags);

    task->state = TASK_STATE_READY_TO_RUN;
    if(wakeup_flag == WAKE_UP_IMMEDIATE)
        schedule_task_next(task);
    else
        schedule_task(task);

    spin_unlock_irqrestore(&global_val.ready_to_run_lock, irqflags);

    if(wakeup_flag == WAKE_UP_IMMEDIATE)
        schedule();

    return;
}
