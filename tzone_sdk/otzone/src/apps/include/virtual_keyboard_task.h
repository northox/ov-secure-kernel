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
 * Virtual Keybiard Tester implementation
 *
 */
#include <sw_types.h>
//#include <otz_api.h>
#include <secure_api.h>

#define NUM_ARGS 4
#define ARG_LEN  32

int process_otz_virtual_keyboard_svc(u32 svc_cmd_id);


/**
 * @brief
 * global variables for the task should be defined as a member of the global 
 * structure 
 */
typedef struct virtual_keyboard_global {
}virtual_keyboard_global;

/**
 * @brief: Mutex test task init
 *
 * This function initializes Mutex test task parameters
 *
 * @param psa_config: Configuration parameter for the task and its get called 
 * before the task creation
 *
 * @return otz_return_t:
 * OTZ_OK \n
 * OTZ_* - An implementation-defined error code for any other error.\n
 */
int virtual_keyboard_task_init(sa_config_t *psa_config);

/**
 * @brief Mutex test task exit
 *
 * This function gets called before the task deletion
 *
 * @param data: Private data which need to be freed.
 * @return otz_return_t:
 * OTZ_OK \n
 * OTZ_FAIL \n 
 */
int virtual_keyboard_task_exit(void* data);

/**
 * @brief Mutex test task entry point
 *
 * This function implements the commands to test the mutex and 
 * semaphore operations
 *
 * @param task_id: Task identifier
 * @param tls: Pointer to task local storage
 */
void virtual_keyboard_task(int task_id, sw_tls* tls);

