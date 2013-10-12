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
 * Task controller implementation
 */

#include <elf_loader.h>
#include <sw_types.h>
#include <monitor.h>
#include <sw_debug.h>
#include <cpu_data.h>
#include <global.h>
#include <task.h>
#include <page_table.h>
#include <sw_string_functions.h>
#include <sw_mem_functions.h>
#include <otz_common.h>
#include <otz_id.h>
#include <cpu.h>
#include <otz_app_eg.h>
#include <task_control.h>
#include <echo_task.h>
#include <crypto_task.h>
#include <crypto_task.h>
#include <mutex_test_task.h>
#include <virtual_keyboard_task.h>


/**
 * @brief Process API
 *
 * This function process the API and also verify the validity of session 
 * identifer
 * @param session_id: Session identifer for the API
 * @param tls: Pointer to task local storage
 *
 * @return SMC return codes:
 * SMC_SUCCESS: API processed successfully. \n
 * SMC_*: An implementation-defined error code for any other error.
 */
int process_otzapi(int session_id, sw_tls *tls)
{
    int ret_val;
    pa_t cmd_phy;
    u32 *params;
    struct otz_smc_cmd *cmd = NULL;
    void *req_buf = NULL, *res_buf = NULL;
    struct otzc_encode_meta *meta_data = NULL;

    u32 svc_id, svc_cmd_id;

    params = tls->params;
    if(!params){
        ret_val = SMC_ENOMEM;
        goto ret_func;
    }
    
    if(params[2] == OTZ_CMD_TYPE_NS_TO_SECURE) {
        cmd_phy = (pa_t) params[1];
        if(!cmd) {
            if(map_to_ns(cmd_phy, (va_t*) &cmd)) {
                ret_val = SMC_ENOMEM;
                goto ret_func;
            }
        }

        if(cmd->req_buf_len >  0) {
            if(map_to_ns(cmd->req_buf_phys, (va_t*)&req_buf) != 0) {
                ret_val = SMC_ENOMEM;
                goto handle_err1;
            }
        }

        if(cmd->resp_buf_len >  0) {
            if(map_to_ns(cmd->resp_buf_phys, (va_t*)&res_buf) != 0) {
                ret_val = SMC_ENOMEM;
                goto handle_err2;
            }
        }

        if(map_to_ns(cmd->meta_data_phys, (va_t*)&meta_data) != 0) {
            ret_val = SMC_ENOMEM;
            goto handle_err2;
        }
    }
    else {
        cmd = (struct otz_smc_cmd *)params[1];
        req_buf = (void*)cmd->req_buf_phys;
        res_buf = (void*)cmd->resp_buf_phys;
        meta_data = (void*)cmd->meta_data_phys;
    }

    if(cmd->context != session_id) {
        cmd->cmd_status = OTZ_STATUS_COMPLETE;
        return SMC_EINVAL_ARG;
    }

    svc_id = ((cmd->id >> 10) & (0x3ff));
    svc_cmd_id = (cmd->id & 0x3ff);
/*
    sw_printf("SW: service id 0x%x\t cmd id 0x%x\n", svc_id, svc_cmd_id);
*/
    if(tls->process)
   	 ret_val = tls->process( svc_cmd_id, 
                            req_buf, cmd->req_buf_len, 
                            res_buf, cmd->resp_buf_len, 
                            meta_data, &cmd->ret_resp_buf_len);

    if(!ret_val)
        cmd->cmd_status = OTZ_STATUS_COMPLETE;

    if(ret_val == SMC_PENDING) {
    }

handle_err2:
    if(params[2] == OTZ_CMD_TYPE_NS_TO_SECURE) {
        if(req_buf)
            unmap_from_ns((va_t)req_buf);
        if(res_buf)
            unmap_from_ns((va_t)res_buf);
        if(meta_data)
            unmap_from_ns((va_t)meta_data);
    }

handle_err1:
    if(params[2] == OTZ_CMD_TYPE_NS_TO_SECURE) {
        unmap_from_ns((va_t)cmd);
    }

ret_func:
    return ret_val;
}
EXPORT_SYMBOL(process_otzapi);
