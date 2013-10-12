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
 * Global variables handling functions
 */

#include <sw_types.h>
#include <task.h>
#include <sw_debug.h>
#include <cpu_data.h>
#include <cpu.h>
#include <sw_mem_functions.h>
#include <sw_timer.h>
#include <mem_mng.h>

struct sw_global global_val;

/**
 * @brief Global initialization
 * 
 * This function initializes the global variables structure
 */
void global_init(void)
{
    INIT_LIST_HEAD(&global_val.task_list);
    INIT_LIST_HEAD(&global_val.ready_to_run_list);
    INIT_LIST_HEAD(&global_val.page_ref_list);

    INIT_SPIN_LOCK(&global_val.ready_to_run_lock);
    INIT_LIST_HEAD(&global_val.device_acl_list);

    global_val.current_task_id = 0;
    global_val.exec_mode = 0;
    global_val.next_task_id = TASK_ID_START;
    sw_memset(params_stack, 0, sizeof(params_stack));
    if(alloc_private_heap(COMMON_HEAP_ID) == -1){
        DIE_NOW(0,"heap allocation failed\n");
    }
#ifdef OTZONE_ASYNC_NOTIFY_SUPPORT
    global_val.notify_data = NULL;
#endif
}

/**
 * @brief Schedules the task 
 *
 * This function implements the functionality of scheduling the given task
 *
 * @param task: Pointer to task structure
 */
void schedule_task(struct sw_task* task)
{
    list_add_tail(&global_val.ready_to_run_list, &task->ready_head);
}

/**
 * @brief 
 *      This function adds the task to the ready to run list as the first
 *      element so that the next task scheduled will be this
 * @param task
 *      Pointer to the Next structure
 */
void schedule_task_next(struct sw_task* task)
{
    list_add(&global_val.ready_to_run_list, &task->ready_head);
}

/**
 * @brief 
 *
 * @param task_id
 */
void schedule_task_id(int task_id)
{
    struct sw_task* task;
    irq_flags_t irqflags;

    task = get_task(task_id);
    if(task){

        spin_lock_irqsave(&global_val.ready_to_run_lock, &irqflags);
        task->state = TASK_STATE_READY_TO_RUN;
        schedule_task(task);
        spin_unlock_irqrestore(&global_val.ready_to_run_lock,irqflags);

    }
    return;
}

/**
 * @brief Suspends the task 
 *
 * This function suspend the given task
 *
 * @param task_id: Task ID
 * @param state: State of the task
 */
void suspend_task(int task_id, int state)
{
    struct list *l;
    struct sw_task  *task = NULL;
    bool found;

    if (list_empty(&global_val.task_list)) {
        return; 
    }

    found = FALSE;
    list_for_each(l, &global_val.task_list) {
        task = list_entry(l, struct sw_task, head);
        if(task->task_id == task_id) {
            found = TRUE;
            break;
        }
            
    }
    if(found) {
        task->state = state;

        if(global_val.current_task_id == task_id && 
            task->state == TASK_STATE_SUSPEND)
            global_val.current_task_id = -1;
    }
}

/**
 * @brief Update the current task ID
 *
 * @param task: Pointer to the task structure
 */
void update_current_task(struct sw_task* task)
{
    if(task != NULL)
        global_val.current_task_id = task->task_id;
}

/**
 * @brief 
 *      Returns the id of the current running task
 *
 * @return
 *      task id
 */
int get_current_task_id(void)
{
    return global_val.current_task_id;
}

/**
 * @brief Get the current task
 *
 * This function returns the current task which is running
 *
 * @return : Pointer to the task structure or NULL
 */
struct sw_task* get_current_task(void)
{
    struct list *l;
    struct sw_task  *task = NULL;
    bool found;

    if (list_empty(&global_val.task_list)) {
        return task; 
    }

    found = FALSE;
    list_for_each(l, &global_val.task_list) {
        task = list_entry(l, struct sw_task, head);
        if(task->task_id == global_val.current_task_id) {
            found = TRUE;
            break;
        }
            
    }
    if(found)
        return task;
    else
        return NULL;
}

/**
 * @brief Get the next ready to run task
 *
 * This function returns the next task which is ready to run
 *
 * @return : Pointer to the task structure or NULL
 */
struct sw_task* get_next_task(void)
{
    struct list *l;
    struct sw_task  *current_task = get_current_task();
    struct sw_task  *next_task;
    struct sw_task  *temp_task;
    irq_flags_t irqflags;

    spin_lock_irqsave(&global_val.ready_to_run_lock, &irqflags);

    if (list_empty(&global_val.ready_to_run_list)) {
        if(current_task != NULL && current_task->state != TASK_STATE_WAIT && 
            current_task->state != TASK_STATE_SUSPEND){
            next_task = current_task;
            goto func_return;
        }
        else{
            next_task = NULL;
            goto func_return;
        }
    }

    l = list_pop(&global_val.ready_to_run_list);
    next_task = list_entry(l, struct sw_task, ready_head);

    list_for_each_entry(temp_task, &global_val.ready_to_run_list ,ready_head){
        if(temp_task == current_task)
            goto func_return;
    }

    if(current_task != NULL && current_task->state != TASK_STATE_WAIT && 
        current_task->state != TASK_STATE_SUSPEND) {
        schedule_task(current_task);        
    }

func_return:
    spin_unlock_irqrestore(&global_val.ready_to_run_lock,irqflags);
    return next_task;
}

/**
 * @brief Prints the all task names
 */
void print_task_list(void)
{
    struct list *l;
    struct sw_task  *task;

    if (list_empty(&global_val.task_list)) {
        return;
    }

    list_for_each(l, &global_val.task_list) {
        task = list_entry(l, struct sw_task, head);
        sw_printf("SW: task name %s\n", task->name);
    }
    return;
}


/**
* @brief Register IRQ handler for the specificed interrupt ID 
*
* This function register the IRQ handler for the specified interrupt ID. This
* could be a function from any task.
*
* @param interrupt: Interrupt ID
* @param handler: Function pointer to IRQ handler
* @param data: IRQ handler data
*/
void register_irq(u32 interrupt, irq_handler handler, void *data)
{
    if (interrupt > NO_OF_INTERRUPTS_IMPLEMENTED) {
        return;
    }
    global_val.task_irq_handler[interrupt] = handler;
    global_val.task_irq_handler_data[interrupt] = data;
}

/**
* @brief Invoke the registered IRQ handler of specified interrupt number
*
* This function invokes the corresponding registered IRQ handler of the
* specified interrupt ID.
*
* @param interrupt: Interrupt ID
* @param regs: Context registers
*/
void invoke_irq_handler(u32 interrupt, struct swi_temp_regs *regs)
{
    if(global_val.task_irq_handler[interrupt]) {
        global_val.task_irq_handler[interrupt](interrupt, 
            global_val.task_irq_handler_data[interrupt]);
    }
}

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
int register_notify_data(pa_t notify_data_phys)
{
    int ret_val = OTZ_OK;
    struct otzc_notify_data *data;
    if(map_to_ns(notify_data_phys, (va_t*)&data) != 0) {
        ret_val = OTZ_ENOMEM;
        goto ret_func;
    }

    global_val.notify_data = data;
ret_func:
    return ret_val;
}

/**
* @brief Un-register the shared memory used for the notification
*
* This function removed the page table entry created for sharing the memory data
* of notification
*/
void unregister_notify_data(void)
{ 
    if(global_val.notify_data)
        unmap_from_ns((va_t)global_val.notify_data);

    global_val.notify_data = NULL;
    
}

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
        int client_pid, int dev_file_id)
{
    if(global_val.notify_data) {
        global_val.notify_data->service_id = service_id;
        global_val.notify_data->session_id = session_id;
        global_val.notify_data->enc_id = enc_id;
        global_val.notify_data->client_pid = client_pid;
        global_val.notify_data->dev_file_id = dev_file_id;
    }
}
#endif
