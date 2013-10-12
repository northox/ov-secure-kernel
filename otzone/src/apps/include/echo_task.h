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
 * Heade for Echo task implementation
 */

#ifndef __OTZ_APP_ECHO_TASK_H__
#define __OTZ_APP_ECHO_TASK_H__

#include <sw_types.h>
#include <secure_api.h>

/**
 * @brief 
 * global variables for the task should be defined as a member of the global 
 * structure 
 */
typedef struct echo_global
{
#ifdef OTZONE_ASYNC_NOTIFY_SUPPORT
    int data_available;
#endif
}echo_global;


/**
 * @brief: Echo task init
 *
 * This function initializes echo task parameters
 *
 * @param psa_config: Configuration parameter for the task and its get called 
 * before the task creation
 *
 * @return otz_return_t:
 * OTZ_OK \n
 * OTZ_* - An implementation-defined error code for any other error.\n
 */
int echo_task_init(sa_config_t *psa_config);

/**
 * @brief Echo task exit
 *
 * This function gets called before the task deletion
 *
 * @param data: Private data which need to be freed.
 * @return otz_return_t:
 * OTZ_OK \n
 * OTZ_FAIL \n 
 */
int echo_task_exit(void* data);

/**
 * @brief Echo task entry point
 *
 * This function implements the commands to echo the input buffer using copying 
 * the buffer and using shared memory
 *
 * @param task_id: task identifier
 * @param tls: Pointer to task local storage
 */
void echo_task(int task_id, sw_tls* tls);


#ifdef OTZONE_ASYNC_NOTIFY_SUPPORT

/**
 * @brief Interrupt handler for echo task
 *
 * This function set the variable to implement the functionality of notification
 * meachanism.
 *
 * @param interrupt: Interrupt ID
 * @param data: TLS data
 */
void echo_task_handler(struct timer_event* tevent);

/**
 * @brief Echo the data for the user supplied buffer with async support
 *
 * This function copies the request buffer to response buffer to show the 
 * non-zero copy functionality and to show the async support by wait for the 
 * flag and it got set in interrupt handler
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
int process_otz_echo_async_send_cmd(void *req_buf, u32 req_buf_len, 
                             void *res_buf, u32 res_buf_len, 
                             struct otzc_encode_meta *meta_data,
                             u32 *ret_res_buf_len);
#endif

/**
 * @brief Echo the data for the user supplied buffer
 *
 * This function copies the request buffer to response buffer to show the 
 * non-zero copy functionality
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
int process_otz_echo_send_cmd(void *req_buf, u32 req_buf_len, 
                             void *res_buf, u32 res_buf_len, 
                             struct otzc_encode_meta *meta_data,
                             u32 *ret_res_buf_len);

/**
 * @brief Echo the data for the shared buffer
 *
 * This function copies the request buffer to response buffer to show the 
 * zero copy functionality
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
int process_otz_echo_send_cmd_shared_buf(void *req_buf, u32 req_buf_len, 
                             void *res_buf, u32 res_buf_len, 
                             struct otzc_encode_meta *meta_data,
                             u32 *ret_res_buf_len);

/**
 * @brief Echo the data for IPI testing
 *
 * This function copies the request buffer to response buffer to show the 
 * IPI functionality
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
int process_otz_echo_ipi_send_cmd(void *req_buf, u32 req_buf_len, 
                             void *res_buf, u32 res_buf_len, 
                             struct otzc_encode_meta *meta_data,
                             u32 *ret_res_buf_len);

/**
 * @brief Process echo service 
 *
 * This function process the echo service commands
 *
 * @param svc_cmd_id: Command identifer to process the echo service command
 * @param req_buf: Virtual address of the request buffer
 * @param req_buf_len: Request buffer length
 * @param resp_buf: Virtual address of the response buffer
 * @param res_buf_len: Response buffer length
 * @param meta_data: Virtual address of the meta data of the encoded data
 * @param ret_res_buf_len: Return length of the response buffer
 *
 * @return SMC return codes:
 * SMC_SUCCESS: Echo service command processed successfully. \n
 * SMC_*: An implementation-defined error code for any other error.
 */
int process_otz_echo_svc(u32 svc_cmd_id, 
                    void *req_buf, u32 req_buf_len, 
                    void *resp_buf, u32 res_buf_len,
                    struct otzc_encode_meta *meta_data, u32 *ret_res_buf_len);
#endif /* __OTZ_APP_TASK2_H__ */
