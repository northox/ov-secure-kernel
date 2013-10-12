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
 * global variables defintions
 */

#ifndef __OTZ_GLOBAL_H__
#define __OTZ_GLOBAL_H__

#define TASK_ID_START 0x111
#include <sw_list.h>
#include <sw_board.h>
#include <cpu_data.h>
#include <otz_common.h>
#include <sw_types.h>
#include <sw_timer.h>
#include <sw_semaphores.h>
/**
 * @brief Task IRQ handler 
 * 
 * @param interrupt: Interrupt number
 * @param data: IRQ handler parameter
 */
typedef void (irq_handler)(int interrupt, void *data);

/**
 * @brief Global variables structure
 */
struct sw_global {
/*! next ready to run task ID */
    u32 next_task_id;
    /* Fix it */
/* TODO : Add a lock to this list too */
/*! Global task list */ 
    struct list task_list;
/*! Global ready to run task list */
    struct list ready_to_run_list;
/*! Spinlock to lock the ready to run list*/
    struct spinlock ready_to_run_lock;
/*! Page reference list for shared memory tracking */
    struct list page_ref_list;
/*! Current task ID */
    u32 current_task_id; 
/*! Task IRQ handler */
    irq_handler *task_irq_handler[NO_OF_INTERRUPTS_IMPLEMENTED];
/*! Task IRQ handler data */
    void *task_irq_handler_data[NO_OF_INTERRUPTS_IMPLEMENTED];
/*! Pointer to device permission list */
    struct list device_acl_list;
/*! Execution mode flag */
    u32 exec_mode;
#ifdef OTZONE_ASYNC_NOTIFY_SUPPORT
/*! Shared memory for notification */
    struct otzc_notify_data *notify_data;
#endif
};

/**
 * @brief 
 */
typedef struct acl_device{
    struct list head;
    u32 did;
    struct list group_head;
}acl_device;

/**
 * @brief 
 */
typedef struct acl_group{
    struct list head;
    u32 gid;
    struct list user_head;
}acl_group;

/**
 * @brief 
 */
typedef struct acl_user{
    struct list head;
    u32 uid;
}acl_user;


/**
 * @brief Global variable structure
 */
extern struct sw_global global_val;

/**
 * @brief Global initialization
 * 
 * This function initializes the global variables structure
 */
void global_init(void);

/**
 * @brief Get the current task
 *
 * This function returns the current task which is running
 *
 * @return : Pointer to the task structure or NULL
 */
struct sw_task* get_current_task(void);

/**
 * @brief Get the next ready to run task
 *
 * This function returns the next task which is ready to run
 *
 * @return : Pointer to the task structure or NULL
 */
struct sw_task* get_next_task(void);

/**
 * @brief Update the current task ID
 *
 * @param task: Pointer to the task structure
 */
void update_current_task(struct sw_task*);

/**
 * @brief 
 *  Returnd the ID of the current task
 * @return 
 */
int get_current_task_id(void);

/**
 * @brief Schedules the task 
 *
 * This function implements the functionality of scheduling the given task
 *
 * @param task: Pointer to task structure
 */
void schedule_task(struct sw_task* task);

/**
 * @brief 
 *      This function adds the task to the ready to run list as the first
 *      element so that the next task scheduled will be this
 * @param task
 *      Pointer to the Next structure
 */

void schedule_task_next(struct sw_task* task);

/**
 * @brief Schedules the task 
 *
 * This function implements the functionality of scheduling the given task
 *
 * @param task_id: ID of the task
 */
void schedule_task_id(int task_id);

/**
 * @brief Suspends the task 
 *
 * This function suspend the given task
 *
 * @param task_id: Task ID
 * @param state: State of the task
 */
void suspend_task(int task_id, int state);

/**
 * @brief Prints the all task names
 */
void print_task_list(void);


/**
* @brief Register IRQ handler for the specificed interrupt ID 
*
* This function register the IRQ handler for the specified interrupt ID. This
* could be a function from any task.
*
* @param interrupt : Interrupt ID
* @param handler: Function pointer to IRQ handler
* @param data: IRQ handler data
*/
void register_irq(u32 interrupt ,irq_handler handler, void *data);

/**
* @brief Invoke the registered IRQ handler of specified interrupt number
*
* This function invokes the corresponding registered IRQ handler of the
* specified interrupt ID.
*
* @param interrupt: Interrupt ID
* @param regs: Context registers
*/
void invoke_irq_handler(u32 interrupt, struct swi_temp_regs *regs);

#ifdef OTZONE_ASYNC_NOTIFY_SUPPORT
/**
* @brief Register the notification shared memory data for notification.
*
* This function creates the page table entry for sharing the memory data for
* notification
*
* @param notify_data_phys: Physical address of the shared memory used for
* Notification data
*
* @return otz_return_t:
* OTZ_OK - Shared memory registration success\n
* OTZ_* - An implementation-defined error code for any other error.\n
*/
int register_notify_data(pa_t data_phys);

/**
* @brief Un-register the shared memory used for the notification
*
* This function removed the page table entry created for sharing the memory data
* of notification
*/
void unregister_notify_data(void);

/**
* @brief Sets the values of Notification
*
* This function sets the notification data which is used by non-secure
* application upon notification from secure world.
*
* @param service_id: Service ID
* @param session_id: Session ID
* @param enc_id: Encoded context ID
* @param client_pid: Non-secure application PID
*/
void set_notify_data(int service_id, int session_id, int enc_id, 
            int client_pid, int dev_file_id);
#endif

/* scheduler.c */
/**
 * @brief Scheduler function
 *
 * This function implements the round-robin scheduler
 */
void scheduler(void);


/* cpu_timer.c */
/**
 * @brief Enable secure kernel timer
 * 
 * This function enables the secure kernel timer
 */
void enable_timer(void);

/**
 * @brief Disable secure kernel timer
 * 
 * This function disables the secure kernel timer
 */
void disable_timer(void);


/**
 * @brief Init secure kernel timer
 * 
 * This function initialize the secure kernel timer
 */
void timer_init(void);

/**
 * @brief 
 *  Returnd the ID of the current task
 * @return Returns the current task ID
 */
int get_current_task_id(void);
/**
* @brief Invoke scheduler
*
* This function invokes the scheduler to schedule the next ready task
*/
void schedule(void);

/**
 * @brief 
 *   This function converts the clockcycles to time in seconds and
 *   nanoseconds
 *   This function definition depends on clock used
 * @param clockcycles
 *  Number of clockcycles
 * @return 
 *  The converted time in seconds and nanoseconds
 */
u64 clockcycles_to_timeval(u32 clockcycles);

/**
 * @brief 
 *      It converts the time (seconds and nanoseconds)
 *      to the number of clockcycles
 * @param time
 *
 * @return 
 */
u64 timeval_to_clockcycles(timeval_t *time);

/**
 * @brief 
 *  It reads and returns the value of the timer
 *  which is used as the free running counter
 * @return 
 */
u32 read_freerunning_cntr(void);

# ifdef CONFIG_EMULATE_FIQ
/**
 * @brief 
 *
 * @return 
 */
u32 read_sleep_timer(void);
#endif

/**
 * @brief
 *  This function writes the number of clockcycles to be expired before the
 *  next tick to the tick timer and enables the tick timer
 *
 * @param usecs
 */
void trigger_tick(u64 usecs);

/**
 * @brief 
 *  This function returns the maximum time that can be kept track of before it
 *  gets expired.
 *  (Eg : The time taken for running from  0xFFFFFFFF to 0x00000000)
 * @return 
 */
u64 get_timer_period(void);

/**
 * @brief
 *  This function returns the resolution which can be obtained with the given
 *  clock
 *
 * @return 
 */
u64 get_clock_period(void);

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
                                                               u32 dev_file_id);
#endif

#endif /* __OTZ_GLOBAL_H__ */
