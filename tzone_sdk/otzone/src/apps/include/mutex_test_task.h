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
 * Header for mutex test task implementation 
 */


#ifndef __TZ_APP_MUTEX_TEST_TASK_H__
#define __TZ_APP_MUTEX_TEST_TASK_H__

#include <sw_types.h>
#include <otz_api.h>


/**
 * @brief 
 * global variables for the task should be defined as a member of the global 
 * structure 
 */
typedef struct otz_mutex_test_global {
}otz_mutex_test_global;

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
int mutex_test_task_exit(void* data);

/**
 * @brief Mutex test task entry point
 *
 * This function implements the commands to test the mutex and 
 * semaphore operations
 *
 * @param task_id: Task identifier
 * @param tls: Pointer to task local storage
 */
void mutex_test_task(int task_id, sw_tls* tls);

/**
 * @brief Process mutex test data commands 
 *
 * This function process the mutex test data commands
 *
 * @param svc_cmd_id: Command identifer to process the mutex test data
 * command
 * @param req_buf: Virtual address of the request buffer
 * @param req_buf_len: Request buffer length
 * @param resp_buf: Virtual address of the response buffer
 * @param res_buf_len: Response buffer length
 * @param meta_data: Virtual address of the meta data of the encoded data
 * @param ret_res_buf_len: Return length of the response buffer
 *
 * @return SMC return codes:
 * SMC_SUCCESS: Mutex test service command processed successfully. \n
 * SMC_*: An implementation-defined error code for any other error.
 */
int process_otz_mutex_test_svc(u32 svc_cmd_id, void *req_buf, u32 req_buf_len, 
                    void *resp_buf, u32 res_buf_len,
                    struct otzc_encode_meta *meta_data, u32 *ret_res_buf_len);

/**
 * @brief Test the mutex operations
 *
 * This function tests the functionality of mutex and semaphores
 *
 * @param req_buf: Virtual address of the request buffer
 * @param req_buf_len: Request buffer length
 * @param res_buf: Virtual address of the response buffer
 * @param res_buf_len: Response buffer length
 * @param meta_data: Virtual address of the meta data of the encoded data
 * @param ret_res_buf_len: Return length of the response buffer
 *
 * @return SMC return codes:
 * SMC_SUCCESS: API processed successfully. \n
 * SMC_*: An implementation-defined error code for any other error.
 */
int process_otz_mutex_test_cmd(void *req_buf, u32 req_buf_len,
                void *res_buf, u32 res_buf_len,
                struct otzc_encode_meta *meta_data,
                u32 *ret_res_buf_len);

#endif

