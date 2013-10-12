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
 * DRM task implementation
 */


#include <sw_types.h>
#include <monitor.h>
#include <sw_debug.h>
#include <cpu_data.h>
#include <global.h>
#include <task.h>
#include <dispatcher_task.h>
#include <page_table.h>
#include <sw_string_functions.h>
#include <sw_mem_functions.h>
#include <sw_wait.h>
#include <sw_semaphores.h>
#include <elf_loader.h>
#include <otz_common.h>
#include <otz_id.h>
#include <cpu.h>
#include <task_control.h>
#include <drm_task.h>

#include <otz_app_eg.h>

#include <sw_io.h>

/**
 * @brief Dummy DRM data echo for the user supplied buffer
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
int process_otz_drm_send_cmd(void *req_buf, u32 req_buf_len, 
                             void *res_buf, u32 res_buf_len, 
                             struct otzc_encode_meta *meta_data,
                             u32 *ret_res_buf_len)
{
    drm_data_t drm_data;
    char *out_buf;
    int offset = 0, pos = 0, mapped = 0, type, out_len; 
    
    if(req_buf_len > 0) {
        while (offset <= req_buf_len) {
            if(decode_data(req_buf, meta_data, 
                &type, &offset, &pos, &mapped, (void**)&out_buf, &out_len)) {
                    return SMC_EINVAL_ARG;
                }
            else {
                if(type != OTZ_ENC_UINT32)
                    return SMC_EINVAL_ARG;

                drm_data.len = *((u32*)out_buf);
            }

            if(decode_data(req_buf, meta_data, 
                &type, &offset, &pos, &mapped, (void**)&out_buf, &out_len)) {
                    return SMC_EINVAL_ARG;
            }
            else {
                if(type != OTZ_ENC_ARRAY)
                    return SMC_EINVAL_ARG;

                sw_memcpy(drm_data.data, out_buf, drm_data.len);
            }

            break;
        }
    }

    offset = 0, pos = OTZ_MAX_REQ_PARAMS;
    if(res_buf_len > 0) {
        while (offset <= res_buf_len) {
            if(decode_data(res_buf, meta_data, 
                &type, &offset, &pos, &mapped, (void**)&out_buf, &out_len)) {
                    return SMC_EINVAL_ARG;
            }
            else {
                if(type != OTZ_ENC_ARRAY)
                    return SMC_EINVAL_ARG;
            }
            sw_memcpy(out_buf, drm_data.data, drm_data.len);
            if(update_response_len(meta_data, pos, drm_data.len))
                return SMC_EINVAL_ARG;

            break;
        }
        *ret_res_buf_len = drm_data.len;
    }
    sw_printf("SW: drm task data: %s\n", drm_data.data);
    return 0;
}

/**
 * @brief Dummy DRM data echo for the shared buffer
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
int process_otz_drm_send_cmd_shared_buf(void *req_buf, u32 req_buf_len, 
                             void *res_buf, u32 res_buf_len, 
                             struct otzc_encode_meta *meta_data,
                             u32 *ret_res_buf_len)
{
    drm_data_t drm_data;
    char *out_buf;
    int offset = 0, pos = 0, mapped = 0, type, out_len; 

    if(req_buf_len > 0) {
        while (offset <= req_buf_len) {
            if(decode_data(req_buf, meta_data, 
                &type, &offset, &pos, &mapped, (void**)&out_buf, &out_len)) {
                    return SMC_EINVAL_ARG;
                }
            else {
                if(type != OTZ_ENC_UINT32)
                    return SMC_EINVAL_ARG;

                drm_data.len = *((u32*)out_buf);
            }

            if(decode_data(req_buf, meta_data, 
                &type, &offset, &pos, &mapped, (void**)&out_buf, &out_len)) {
                    return SMC_EINVAL_ARG;
            }
            else {
                if(type != OTZ_MEM_REF)
                    return SMC_EINVAL_ARG;
                sw_memcpy(drm_data.data, out_buf, drm_data.len);

                if(mapped)
                    unmap_from_ns((va_t)out_buf);
            }

            break;
        }
    }
    
    offset = 0, pos = OTZ_MAX_REQ_PARAMS;
    if (res_buf_len > 0) {
        while (offset <= res_buf_len) {
            if(decode_data(res_buf, meta_data, 
                &type, &offset, &pos, &mapped, (void**)&out_buf, &out_len)) {
                    return SMC_EINVAL_ARG;
            }
            else {
                if(type != OTZ_MEM_REF)
                    return SMC_EINVAL_ARG;
            }
            sw_memcpy(out_buf, drm_data.data, drm_data.len);
            if(update_response_len(meta_data, pos, drm_data.len))
                return SMC_EINVAL_ARG;

            if(mapped)
                unmap_from_ns((va_t)out_buf);

            break;
        }
        *ret_res_buf_len = drm_data.len;
    }

    return 0;
}

/**
 * @brief Process DRM service 
 *
 * This function process the DRM service commands
 *
 * @param svc_cmd_id: Command identifer to process the drm service command
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
int process_otz_drm_svc(u32 svc_cmd_id, 
                    void *req_buf, u32 req_buf_len, 
                    void *resp_buf, u32 res_buf_len,
                    struct otzc_encode_meta *meta_data, u32 *ret_res_buf_len) 
{
    int ret_val = SMC_ERROR;

    switch (svc_cmd_id) {
        case OTZ_DRM_CMD_ID_SEND_CMD:
            ret_val = process_otz_drm_send_cmd(
            req_buf,
            req_buf_len,
            resp_buf,
            res_buf_len,
            meta_data,
            ret_res_buf_len);
        break;
        case OTZ_DRM_CMD_ID_SEND_CMD_SHARED_BUF:
            ret_val = process_otz_drm_send_cmd_shared_buf(
            req_buf,
            req_buf_len,
            resp_buf,
            res_buf_len,
            meta_data,
            ret_res_buf_len);
	break;
    default:
        ret_val = SMC_EOPNOTSUPP;
        break;
    }

    return ret_val;
}

/**
 * @brief DRM task entry point
 *
 * This function implements the commands to drm the input buffer using copying 
 * the buffer and using shared memory
 *
 * @param task_id: task identifier
 * @param tls: Pointer to task local storage
 */
void drm_task(int task_id, sw_tls* tls)
{
    tls->ret_val = 0;
#ifdef CONFIG_NS_JIFFIES_WORKAROUND
	disable_ns_jiffies_update();
#endif
    tls->ret_val = process_otzapi(task_id, tls);

#ifdef CONFIG_NS_JIFFIES_WORKAROUND
	enable_ns_jiffies_update();
#endif
    handle_task_return(task_id, tls);

    DIE_NOW(0, "drm task - hangs\n");
}

/**
 * @brief: DRM task init
 *
 * This function initializes drm task parameters
 *
 * @param psa_config: Configuration parameter for the task and its get called 
 * before the task creation
 *
 * @return otz_return_t:
 * OTZ_OK \n
 * OTZ_* - An implementation-defined error code for any other error.\n
 */

int drm_task_init(sa_config_t *psa_config)
{
	
	sw_memset(psa_config, 0x0, sizeof(sa_config_t));
    psa_config->service_uuid = OTZ_SVC_DRM;
    sw_strcpy(psa_config->service_name, "drm");
    psa_config->stack_size = TASK_STACK_SIZE;
    psa_config->entry_point = (u32)&drm_task;
    psa_config->process = (u32)&process_otz_drm_svc;
    psa_config->data = (void*)sw_malloc(sizeof(struct drm_global));

    if(!psa_config->data) {
        sw_printf("SW: drm task init: allocation of local storage data failed\n");
        return OTZ_ENOMEM;
    }

    sw_memset(psa_config->data, 0, sizeof(struct drm_global));
    return OTZ_OK;
}


/**
 * @brief DRM task exit
 *
 * This function gets called before the task deletion
 * @return otz_return_t:
 * OTZ_OK \n
 * OTZ_FAIL \n 
 */
int drm_task_exit(void *data)
{
    if(data)
        sw_free(data);
    return OTZ_OK;
}
