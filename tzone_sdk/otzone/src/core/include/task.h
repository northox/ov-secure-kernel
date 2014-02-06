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
 *  Helper function declarations of task management
 */

#ifndef __OTZ_TASK_H__
#define __OTZ_TASK_H__

#include <sw_types.h>
#include <sw_list.h>
#include <global.h>
#include <cpu_task.h>
#include <sw_wait.h>

/**
 * @brief
 Secure API configuration details for task
 */
typedef struct sa_config_t
{
/*! Service UUID */
    int       service_uuid;
/*! Service Name */
    char      service_name[32];
/*! Stack size of the task */
    u32       stack_size;
/*  Name of the entry function */
    char entry_func[32];
/*! Entry point for the task */

    u32       entry_point;
/*! flag to indicate cleanup and the presence of loader support */
    u32 elf_flag;
/*! file path*/
    char file_path[255];
/*! process name */
    char process_name[32];
/* service func pointer */
    int (*process)(u32 svc_cmd_id,void *req_buf, u32 req_buf_len,
		    void *resp_buf, u32 resp_buf_len, struct otzc_encode_meta *
		    meta_data, u32 *ret_res_buf_len);    
/*! Task data */
    void*     data;
}sa_config_t;


/**
 * @brief Task state constants
 */
enum task_state_e {
    TASK_STATE_SUSPEND = 0,
    TASK_STATE_WAIT,
    TASK_STATE_READY_TO_RUN,
    TASK_STATE_RUNNING
};

/**
 * @brief Task local storage
 */
typedef struct sw_tls
{
/*! Command parameters */
    u32 params[4];
/*! Return value of the secure API */
    u32 ret_val;
/*! Return value of the IPC */
    u32 ipi_ret_val;
/*!  elf flag */
    int elf_flag;
/*! process pointer */
    int (*process)(u32 svc_cmd_id,void *req_buf, u32 req_buf_len,
		    void *resp_buf, u32 resp_buf_len, struct otzc_encode_meta *
		    meta_data, u32 *ret_res_buf_len);   
/*! Private data */
    void *private_data;
}sw_tls;

/**
*User Access Control
**/
typedef struct user_access_control{
    char *username; //task name;
    u32 uid; //user id
    u32 gid; //group id
}acl_t;


/**
 * @brief Task structure
 */
struct sw_task {
    struct list head;
    struct list ready_head;
    struct list pending_head;
/*! Task ID */
    u32 task_id;
/*! Service ID */           
    u32 service_id;         
/*! Task entry address */
    u32 entry_addr;     
/*! Task name */    
    char name[32];          
/*! Task state */
    u32 state;
/*! Task stack pointer */
    void* task_sp;
/*! Task stack size */
    u32 task_sp_size;
/*! task local storage */
    sw_tls* tls;
/*! Registers of the task */
    struct sw_task_cpu_regs regs;
/*! Wait queue */
    struct wait_queue_head wq_head;
/*! User Access Control List */
    acl_t acl;
};


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
int create_task(sa_config_t *psa_config, int *task_id);

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
int destroy_task(int task_id);

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
int start_task(int task_id, u32* params);

/**
 * @brief Task context switch function
 *
 * This function switches the context between two tasks
 *
 * @param new_task: Task for which context need to be restored
 * @param old_task: Task for which context need to be saved
 */
void task_context_switch(struct sw_task *new_task, struct sw_task *old_task);

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
int get_task_state(int task_id);


/**
 * @brief Helper function to print the task context
 *
 * @param task_id: Task ID
 *
 * @return otz_return_t:
 * OTZ_OK - Session established successfully\n
 * OTZ_* - An implementation-defined error code for any other error.\n
 */
int print_task(int task_id);

/**
 * @brief 
 *      This functions wakes up a task from sleep.
 *      It is used for async tasks
 * @param task_id
 *      The task to be resumed
 * @return 
 */
void resume_async_task(int task_id);

/**
 * @brief Get task local storage
 *
 * This helper function returns the task local storage
 *
 * @param task_id: Task ID
 *
 * @return Returns the task local storage pointer or NULL.
 */
sw_tls* get_task_tls(int task_id);

/**
 * @brief Helper function to return task structure
 *
 * This helper function returns the task structure for the given task ID
 * 
 * @param task_id: Task ID
 *
 * @return Returns the pointer to task structure or NULL
 */
struct sw_task* get_task(int task_id);
#endif /* __OTZ_TASK_H__ */
