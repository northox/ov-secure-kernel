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
 * Dynamic module init and exit function implementation
 *
 */

#include <task.h>
#include <secure_api.h>
#include <sw_debug.h>
#include <otz_id.h>
#include <fat32.h>
#include <page_table.h>
#include <dispatcher_task.h>
#include <sw_string_functions.h>
#include <sw_mem_functions.h>
#include <mem_mng.h>
#include <sw_list.h>
#include <elf_loader.h>
#include <elf_loader_app.h>
#include <mutex_test_task.h>
#include <virtual_keyboard_task.h>

#ifdef CONFIG_GUI_SUPPORT
/**
 * @brief initializes the sa_config_t function for the virtual_keyboard_task
 *
 * @param psa_config: Configuration parameter for the task
 *
 * @return OTZ_OK on success, -1 on failure
 */
int virtual_keyboard_task_init(sa_config_t * psa_config) 
{ 
	sw_memset(psa_config,0x0,sizeof(sa_config_t));
	psa_config->service_uuid = OTZ_SVC_VIRTUAL_KEYBOARD; 
	sw_strcpy(psa_config->service_name,"virtual_keyboard"); 
	psa_config->stack_size = TASK_STACK_SIZE; 
	psa_config->data = NULL;
#ifdef CONFIG_SW_ELF_LOADER_SUPPORT
	psa_config->elf_flag = ELF_FLAG;
	psa_config->process = 0x0;
	psa_config->entry_point = 0x0;
#endif
#ifndef CONFIG_SW_ELF_LOADER_SUPPORT
	psa_config->entry_point = &virtual_keyboard_task;
	psa_config->process = &process_otz_virtual_keyboard_svc;
#endif
	sw_strcpy(psa_config->file_path,"/apps/virtual_keyboard_task.o"); 
	sw_strcpy(psa_config->entry_func,"virtual_keyboard_task");
	sw_strcpy(psa_config->process_name,"process_otz_virtual_keyboard_svc");
	return OTZ_OK; 
}  

/**
 * @brief Virtual Keyboard task exit
 *
 * This function gets called before the task deletion
 *
 * @param data: Private data which need to be freed.
 * @return otz_return_t:
 * OTZ_OK \n
 * OTZ_FAIL \n 
 */
int virtual_keyboard_task_exit(void* data)
{
	if(data)
		sw_free(data);
	return OTZ_OK;
}

#endif

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
int mutex_test_task_init(sa_config_t *psa_config)
{
	sw_memset(psa_config,0x0,sizeof(sa_config_t));
	psa_config->service_uuid = OTZ_SVC_MUTEX_TEST;
	sw_strcpy(psa_config->service_name, "otz_mutex_test");
	psa_config->stack_size = TASK_STACK_SIZE ;
	sw_strcpy(psa_config->entry_func,"mutex_test_task");
	psa_config->data = NULL;	

#ifdef CONFIG_SW_ELF_LOADER_SUPPORT
	psa_config->elf_flag = ELF_FLAG;
	psa_config->process = 0x0;
	psa_config->entry_point = 0x0;
#endif
#ifndef CONFIG_SW_ELF_LOADER_SUPPORT
	psa_config->entry_point = &mutex_test_task;
	psa_config->process = &process_otz_mutex_test_svc;
#endif
	sw_strcpy(psa_config->process_name,"process_otz_mutex_test_svc");
	sw_strcpy(psa_config->file_path, "/apps/mutex_test_task.o");
	return OTZ_OK;
}


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
int mutex_test_task_exit(void *data)
{
    if(data)
        sw_free(data);
    return OTZ_OK;
}

