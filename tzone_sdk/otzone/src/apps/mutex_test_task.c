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
 */

/* OpenVirtualization mutex test task implementation */

#include <sw_types.h>
#include <monitor.h>
#include <sw_debug.h>
#include <cpu_data.h>
#include <global.h>
#include <task.h>
#include <page_table.h>
#include <sw_string_functions.h>
#include <sw_mem_functions.h>

#include <otz_id.h>
#include <otz_common.h>
#include <cpu.h>
#include <task_control.h>
#include <mutex_test_task.h>
#include <sw_semaphores.h>
#include <otz_app_eg.h>
#include <secure_api.h>

static sw_mutex_t g_otz_mutex = OTZ_MUTEX_INIT;

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
                u32 *ret_res_buf_len)
{
    otz_mutex_test_data_t otz_mutex_test_data;
    unsigned char *out_buf;
    int offset = 0, pos = 0, mapped = 0, type, out_len=0;

    while (offset <= req_buf_len) {
        if(decode_data(req_buf, meta_data, &type, &offset, &pos, &mapped,
                                                (void**)&out_buf, &out_len)) {
            return SMC_EINVAL_ARG;
        }
        if(type != OTZ_ENC_UINT32) {
            return SMC_EINVAL_ARG;
        }
        otz_mutex_test_data.len = *((u32*)out_buf);
        if(decode_data(req_buf, meta_data, &type, &offset, &pos, &mapped,
                                                (void**)&out_buf, &out_len)) {
            return SMC_EINVAL_ARG;
        }
        if(type != OTZ_ENC_ARRAY) {
            return SMC_EINVAL_ARG;
        }
        if(out_len < DATA_BUF_LEN) {
            sw_memcpy(otz_mutex_test_data.data, out_buf, out_len);
        } else {
            return(SMC_ENOMEM);
        }
        break;
    }
#ifndef CONFIG_SW_ELF_LOADER_SUPPORT
   sw_printf("SW: Attempting to lock the variable \n");
#endif
    if(sw_mutex_lock(&g_otz_mutex) == OTZ_INVALID) {
#ifndef CONFIG_SW_ELF_LOADER_SUPPORT
        sw_printf("SW: Unable to lock mutex. It is invalid \n");
#endif
        goto handle_error;
    }
#ifndef CONFIG_SW_ELF_LOADER_SUPPORT
    sw_printf("SW: Lock successful. Trying to lock it one more time \n");
#endif
    if(sw_mutex_trylock(&g_otz_mutex) == OTZ_BUSY) {
#ifndef CONFIG_SW_ELF_LOADER_SUPPORT
        sw_printf("SW: Mutex already locked. We cannot lock it anymore !! \n");
#endif
    }
    sw_mutex_unlock(&g_otz_mutex);
#ifndef CONFIG_SW_ELF_LOADER_SUPPORT
    sw_printf("SW: Unlock successful. Trying to lock it one more time \n");
#endif
    if(sw_mutex_trylock(&g_otz_mutex) == OTZ_BUSY) {
#ifndef CONFIG_SW_ELF_LOADER_SUPPORT
        sw_printf("SW: Error while unlocking the mutex !! \n");
#endif
        goto handle_error;
    }
 
#ifndef CONFIG_SW_ELF_LOADER_SUPPORT
    sw_printf("SW: Going to sleep \n");
#endif
    usleep(10);

#ifndef CONFIG_SW_ELF_LOADER_SUPPORT
    sw_printf("SW: Second time locking successful \n");
#endif
    sw_mutex_unlock(&g_otz_mutex);
handle_error:
    offset = 0, pos = OTZ_MAX_REQ_PARAMS;
    while (offset <= res_buf_len) {
        if(decode_data(res_buf, meta_data, &type, &offset, &pos, &mapped,
                                            (void**)&out_buf, &out_len)) {
            return SMC_EINVAL_ARG;
        }
        if(type != OTZ_ENC_ARRAY) {
            return SMC_EINVAL_ARG;
        }
        if(out_len >= otz_mutex_test_data.len) {
            sw_memcpy(out_buf,otz_mutex_test_data.response, otz_mutex_test_data.len);
            if(update_response_len(meta_data, pos, otz_mutex_test_data.len)) {
                return SMC_EINVAL_ARG;
            }
        } else {
            return(SMC_ENOMEM);
        }
        break;
    }
    *ret_res_buf_len = otz_mutex_test_data.len;
    return 0;
}

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
 * SMC_SUCCESS: Mutex test data command processed successfully. \n
 * SMC_*: An implementation-defined error code for any other error.
 */
int process_otz_mutex_test_svc(u32 svc_cmd_id, void *req_buf, u32 req_buf_len, 
                    void *resp_buf, u32 res_buf_len,
                    struct otzc_encode_meta *meta_data, u32 *ret_res_buf_len)
{
    int ret_val = SMC_ERROR;
    if((req_buf_len == 0) || (res_buf_len == 0) ) {
        return(0);
    }

    switch (svc_cmd_id) {
        case OTZ_MUTEX_TEST_CMD_ID_TEST:
            ret_val = process_otz_mutex_test_cmd(req_buf,req_buf_len,
                         resp_buf,res_buf_len,meta_data,ret_res_buf_len);
            break;
        default:
            ret_val = SMC_EOPNOTSUPP;
            break;
    }

    return ret_val;
}

/**
 * @brief Mutex test task entry point
 *
 * This function implements the commands to test the mutex and 
 * semaphore operations
 *
 * @param task_id: task identifier
 * @param tls: Pointer to task local storage
 */
void mutex_test_task(int task_id, sw_tls* tls)
{
    ipc_test_echo(OTZ_SVC_MUTEX_TEST, task_id);
    tls->ret_val = process_otzapi(task_id, tls);
    handle_task_return(task_id, tls);
    while(1);
}
