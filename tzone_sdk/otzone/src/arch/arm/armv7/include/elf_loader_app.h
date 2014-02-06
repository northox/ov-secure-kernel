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
 * Dynamic module init and exit function implementations
 *
 */

/**
 * @brief initializes the sa_config_t function for the virtual_keyboard_task
 *
 * @param psa_config: Configuration parameter for the task
 *
 * @return OTZ_OK on success, -1 on failure
 */
int virtual_keyboard_task_init(sa_config_t * psa_config);


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
int virtual_keyboard_task_exit(void* data);

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
int mutex_test_task_init(sa_config_t *psa_config);


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
int mutex_test_task_exit(void *data);

