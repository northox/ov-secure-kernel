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
 *  Helper functions for Task management.
 */

#include <sw_types.h>
#include <task.h>
#include <sw_mem_functions.h>
#include <sw_string_functions.h>
#include <sw_debug.h>
#include <cpu_data.h>
#include <global.h>
#include <mem_mng.h>
#include <sw_user_mgmt.h>

/**
 * @brief Create a task
 *
 * This function helps in task creation. It allocates the task structure, 
 * task local storage, task stack. It puts the task in suspend state and 
 * adds to global task list.
 *
 * @param psa_config: Configuration for task creation
 * @param task_id: Output parameter for the task ID.
 *
 * @return otz_return_t:
 * OTZ_OK - Session established successfully\n
 * OTZ_* - An implementation-defined error code for any other error.\n
 */
int create_task(sa_config_t *psa_config, int *task_id)
{
    int ret = OTZ_OK;
    struct sw_global *pglobal = &global_val;
    struct sw_task  *new_task = NULL;
    int tmp_task_id;

    *task_id = -1;

    if(pglobal->exec_mode == 1 && getPermission(psa_config->service_uuid) == -1){
        sw_printf("SW: IPC or Task creation denied.\n");
        ret = OTZ_ENOMEM;
        goto ret_func;
    }

    tmp_task_id = pglobal->next_task_id++;
    if(alloc_private_heap(tmp_task_id) == -1){
        sw_printf("SW: Private heap allocation failed \n");
        ret = OTZ_ENOMEM;
        goto ret_func;
    }

    new_task = (struct sw_task  *)sw_malloc_private(tmp_task_id, sizeof(struct sw_task));


    if(!new_task) {
        sw_printf("SW: task creation failed: malloc failed\n");
        ret = OTZ_ENOMEM;
        goto ret_func;
    }

    new_task->task_id = tmp_task_id;
    new_task->service_id = psa_config->service_uuid;
    new_task->entry_addr = psa_config->entry_point;
    sw_strcpy(new_task->name, psa_config->service_name);

    new_task->tls = (struct sw_tls  *)sw_malloc_private(new_task->task_id, sizeof(struct sw_tls));
    if(!new_task->tls) {
        sw_printf("SW: task creation failed: malloc failed for local storage\n");
        ret = OTZ_ENOMEM;
        goto handle_err1;
    }
    ((struct sw_tls*)new_task->tls)->private_data = psa_config->data;
    ((struct sw_tls*)new_task->tls)->elf_flag = psa_config->elf_flag;
    ((struct sw_tls*)new_task->tls)->process = psa_config->process;
    new_task->task_sp = (void *)sw_malloc_private(new_task->task_id,psa_config->stack_size); 
/*  task_sp = &task_stack[(pglobal->next_task_id -1) - TASK_ID_START]; */

    if(!new_task->task_sp) {
        sw_printf("SW: task creation failed: malloc failed for stack allocation\n");
        ret = OTZ_ENOMEM;
        goto handle_err1;
    }
    
    new_task->task_sp_size = psa_config->stack_size;

    INIT_LIST_HEAD(&new_task->head);
    INIT_LIST_HEAD(&new_task->ready_head);
    INIT_LIST_HEAD(&new_task->pending_head);
    list_add_tail(&pglobal->task_list, &new_task->head);

    new_task->state = TASK_STATE_SUSPEND;
    *task_id = new_task->task_id;

    if(setPermission(new_task) == -1){
        sw_printf("SW: No Pre-defined permission set for %s task. Assigning NULL value\n", psa_config->service_name);
    }
    
    /* Changing the execution mode from SecureOS to Task */
    if(psa_config->service_uuid == 1){
        pglobal->exec_mode = 1;
    }

    goto ret_func;

handle_err1:
    if(psa_config->data){
        if((sw_strcmp(psa_config->service_name ,"dispatcher")) == 0)
            sw_free_private(COMMON_HEAP_ID, psa_config->data);
        else
            sw_free(psa_config->data);    
    }

    if(new_task->tls)
        sw_free_private(tmp_task_id, new_task->tls);

    if(new_task->task_sp)
        sw_free_private(tmp_task_id, new_task->task_sp);

    if(new_task)
        sw_free_private(tmp_task_id, new_task);

ret_func:
    return ret;
}

/**
 * @brief Destroy the created task
 *
 * This function cleans up the created task. It frees the resources which 
 * got allocated in task creation. It removes the task for global task list
 * 
 * @param task_id: Task ID 
 *
 * @return otz_return_t:
 * OTZ_OK - Session established successfully\n
 * OTZ_* - An implementation-defined error code for any other error.\n
 */
int destroy_task(int task_id)
{
    struct list *l;
    bool found;
    struct sw_task  *task, *ready_task;
    struct sw_global *pglobal = &global_val;

    task= NULL;
    found = FALSE;

    if (list_empty(&pglobal->task_list)) {
        return OTZ_EFAIL; /* Fix the return code */
    }
    task= NULL;
    found = FALSE;
    list_for_each(l, &pglobal->task_list) {
        task = list_entry(l, struct sw_task, head);
        if (task->task_id == task_id) {
            found = TRUE;
            break;
        }
    }

    if (!found) {
        return OTZ_EFAIL; /* Fix the return code */
    }
    
    list_del(&task->head);

    if (!list_empty(&pglobal->ready_to_run_list)) {
        ready_task= NULL;
        list_for_each(l, &pglobal->ready_to_run_list) {
            ready_task = list_entry(l, struct sw_task, ready_head);
            if (ready_task->task_id == task_id) {
                list_del(&ready_task->ready_head);
                break;
            }
        }
    }

    cpu_task_exit(task);

    if(task->tls)
        sw_free_private(task_id, task->tls);    
    sw_free_private(task_id, task->task_sp);
    sw_free_private(task_id, task);
    if(free_private_heap(task_id) == -1){
        sw_printf("SW: Cannot free this heap\n");
    }
    return OTZ_OK;
}

/**
 * @brief 
 *      This functions wakes up a task from sleep.
 *      It is used for async tasks
 * @param task_id
 *      The task to be resumed
 * @return 
 */
void resume_async_task(int task_id)
{
    struct sw_task* task = get_task(task_id);
    sw_wakeup(&task->wq_head, WAKE_UP_IMMEDIATE);
}

/**
 * @brief Start the task
 *
 * This function gets called on command invocation and init the task context 
 * the schedule the task.
 *
 * @param task_id: Task ID
 * @param params: Command parameters
 *
 * @return otz_return_t:
 * OTZ_OK - Session established successfully\n
 * OTZ_* - An implementation-defined error code for any other error.\n
 */
int start_task(int task_id, u32* params)
{
    int ret;
    struct list *l;
    struct sw_task  *task;
    struct sw_global *pglobal = &global_val;
    irq_flags_t irqflags;

    bool found;
    if (list_empty(&pglobal->task_list)) {
        return OTZ_EFAIL; /* Fix the return code */
    }

    task= NULL;
    found = FALSE;
    list_for_each(l, &pglobal->task_list) {
        task = list_entry(l, struct sw_task, head);
        if (task->task_id == task_id) {
            found = TRUE;
            break;
        }
    }

    if (!found) {
        return OTZ_EFAIL; /* Fix the return code */
    }

    ret = cpu_task_init((void*)task);
    if(OTZ_OK != ret) 
        return ret;

    if(params) {
        task->tls->params[0] = params[0];
        task->tls->params[1] = params[1];
        task->tls->params[2] = params[2];
        task->tls->params[3] = params[3];
    }
    else {
        sw_memset(task->tls->params, 0, sizeof(task->tls->params));
    }

    spin_lock_irqsave(&global_val.ready_to_run_lock, &irqflags);
    schedule_task(task);
    spin_unlock_irqrestore(&global_val.ready_to_run_lock,irqflags);
    return OTZ_OK;
}

/**
 * @brief Task context switch function
 *
 * This function switches the context between two tasks
 *
 * @param new_task: Task for which context need to be restored
 * @param old_task: Task for which context need to be saved
 */
void task_context_switch(struct sw_task  *new_task, struct sw_task  *old_task)
{
    if(old_task)
        save_task_context_regs(&old_task->regs);
    if(new_task)
        restore_task_context_regs(&new_task->regs);
}

/**
 * @brief Get task state
 *
 * @param task_id: Task ID
 *
 * @return Returns the task state: 
 * TASK_STATE_SUSPEND - Task in suspend state\n
 * TASK_STATE_WAIT - Task in wait state \n
 * TASK_STATE_READY_TO_RUN - Task is in ready to run state \n
 * TASK_STATE_RUNNING - Task is in running state \n
 */
int get_task_state(int task_id)
{
    struct list *l;
    struct sw_task  *task;
    bool found;
    struct sw_global *pglobal = &global_val;

    if (list_empty(&pglobal->task_list)) {
        return TASK_STATE_SUSPEND; /* Fix the return code */
    }

    task= NULL;
    found = FALSE;
    list_for_each(l, &pglobal->task_list) {
        task = list_entry(l, struct sw_task, head);
        if (task->task_id == task_id) {
            found = TRUE;
            break;
        }
    }

    if (!found) {
        return TASK_STATE_SUSPEND; /* Fix the return code */
    }

    return task->state;
}

/**
 * @brief Helper function to print the task context
 *
 * @param task_id: Task ID
 *
 * @return otz_return_t:
 * OTZ_OK - Session established successfully\n
 * OTZ_* - An implementation-defined error code for any other error.\n
 */
int print_task(int task_id)
{
    struct list *l;
    struct sw_task  *task;
    struct sw_global *pglobal = &global_val;

    bool found;
    if (list_empty(&pglobal->task_list)) {
        return OTZ_EFAIL; /* Fix the return code */
    }

    task= NULL;
    found = FALSE;
    list_for_each(l, &pglobal->task_list) {
        task = list_entry(l, struct sw_task, head);
        if (task->task_id == task_id) {
            found = TRUE;
            break;
        }
    }

    if (!found) {
        return OTZ_EFAIL; /* Fix the return code */
    }
    print_cpu_task_regs(task);
    return OTZ_OK;
}


/**
 * @brief Get task local storage
 *
 * This helper function returns the task local storage
 *
 * @param task_id: Task ID
 *
 * @return Returns the task local storage pointer or NULL.
 */
sw_tls* get_task_tls(int task_id)
{
    struct list *l;
    struct sw_task  *task;
    struct sw_global *pglobal = &global_val;

    bool found;
    if (list_empty(&pglobal->task_list)) {
        return NULL;
    }

    task= NULL;
    found = FALSE;
    list_for_each(l, &pglobal->task_list) {
        task = list_entry(l, struct sw_task, head);
        if (task->task_id == task_id) {
            found = TRUE;
            break;
        }
    }

    if (!found) {
        return NULL;
    }
    
    return task->tls;
}

/**
 * @brief Helper function to return task structure
 *
 * This helper function returns the task structure for the given task ID
 * 
 * @param task_id: Task ID
 *
 * @return Returns the pointer to task structure or NULL
 */
struct sw_task* get_task(int task_id)
{
    struct list *l;
    struct sw_task  *task;
    struct sw_global *pglobal = &global_val;

    bool found;
    if (list_empty(&pglobal->task_list)) {
        return NULL;
    }

    task= NULL;
    found = FALSE;
    list_for_each(l, &pglobal->task_list) {
        task = list_entry(l, struct sw_task, head);
        if (task->task_id == task_id) {
            found = TRUE;
            break;
        }
    }

    if (!found) {
        return NULL;
    }
    
    return task;
}
