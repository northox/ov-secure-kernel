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
 * Helper function to process the service commands
 */

#ifndef __OTZ_APP_TASK_CONTROL_H__
#define __OTZ_APP_TASK_CONTROL_H__

#include <sw_types.h>

/**
 * @brief Process service
 *
 * This function calls the apporpriate service handlers
 *
 * @param svc_id: Service identifer used to identify the service
 * @param svc_cmd_id: Command identifier of the service
 * @param req_buf: Virtual address of the request buffer
 * @param req_buf_len: Request buffer length
 * @param resp_buf: Virtual address of the response buffer
 * @param res_buf_len: Response buffer length
 * @param meta_data: Virtual address of the meta data of the encoded data
 * @param ret_res_buf_len: Return length of the response buffer
 *
 * @return SMC return codes:
 * SMC_SUCCESS: API processed successfully. \n
 * SMC_*: An implementation-defined error code for any other error.
 */
int process_service(u32 svc_id, u32 svc_cmd_id, 
                    void *req_buf, u32 req_buf_len, 
                    void *resp_buf, u32 res_buf_len,
                    struct otzc_encode_meta *meta_data, u32 *ret_res_buf_len);

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
int process_otzapi(int session_id, sw_tls* tls);

#endif /* __OTZ_APP_TASK_CONTROL_H__ */
