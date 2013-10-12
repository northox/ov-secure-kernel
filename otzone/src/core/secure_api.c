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
 * Helper API's for Secure kernel
 */
#include <elf_loader.h>
#include <sw_types.h>
#include <sw_debug.h>
#include <sw_mem_functions.h>
#include <sw_string_functions.h>
#include <cpu_data.h>
#include <page_table.h>
#include <secure_api.h>
#include <otz_common.h>
#include <otz_id.h>
#include <task.h>
#include <elf_loader_app.h>
#include <dispatcher_task.h>
#include <echo_task.h>
#include <drm_task.h>
#include <crypto_task.h>
#include <mutex_test_task.h>
#include <cpu_mmu.h>
#include <cpu.h>
#include <virtual_keyboard_task.h>
#include <sw_syscall.h>



/**
 * @brief Dispatcher command function to handle the Open session request from
 * non-secure world
 *
 * This function invokes the open session API and its get called from dispatcher
 * task. 
 *
 * @param param: pointer to struct otz_smc_cmd
 *
 * @return otz_return_t:
 * OTZ_OK - Session established successfully\n
 * OTZ_* - An implementation-defined error code for any other error.\n
 */
int open_session_from_ns(void *param)
{

	int ret_val;
	int *svc_id = NULL;
	sa_config_t sa_config;
	pa_t cmd_phy;
	struct otz_smc_cmd *cmd = NULL;
	void *session_context = NULL;

	cmd_phy = (pa_t) param;


	if(map_to_ns(cmd_phy, (va_t*) &cmd)) {
		ret_val = OTZ_ENOMEM;
		goto ret_func;
	}


	if(cmd->resp_buf_len == 0) {
		ret_val = OTZ_EFAIL;
		goto ret_func;
	}

	if(map_to_ns(cmd->resp_buf_phys, (va_t*)&session_context) != 0) {
		ret_val = OTZ_ENOMEM;
		goto ret_func;
	}

	if(map_to_ns(cmd->req_buf_phys, (va_t*)&svc_id) != 0) {
		ret_val = OTZ_ENOMEM;
		goto ret_func;
	}

	if(*svc_id == OTZ_SVC_INVALID) {
		ret_val = OTZ_EFAIL;
		goto ret_func;
	}

	ret_val = sa_create_entry_point(*svc_id, &sa_config);
	if(ret_val == OTZ_OK) {
		ret_val = sa_open_session(&sa_config, session_context);
	}

	if(ret_val != OTZ_OK) {
		sa_destroy_entry_point(*svc_id,
				sa_config.data,sa_config.elf_flag);   
		cmd->cmd_status = OTZ_STATUS_INCOMPLETE;
	}
	else {
		sw_printf("SW: session context %x\n", *((int*)session_context));
		cmd->ret_resp_buf_len = sizeof(int);
		cmd->cmd_status = OTZ_STATUS_COMPLETE;
	}

ret_func:
	if(svc_id)
		unmap_from_ns((va_t)svc_id);

	if(session_context)
		unmap_from_ns((va_t)session_context);

	if(cmd)    
		unmap_from_ns((va_t)cmd);    

	return ret_val;
}

/**
 * @brief Dispatcher command function to handle the Close session request from
 * non-secure world
 *
 * This function invokes the close session API and its get called from dispatcher
 * task. 
 * @param param: pointer to struct otz_smc_cmd
 *
 * @return otz_return_t:
 * OTZ_OK - Session established successfully\n
 * OTZ_* - An implementation-defined error code for any other error.\n
 */
int close_session_from_ns(void *param)
{
	int ret_val;
	int *svc_id = NULL;
	pa_t cmd_phy;
	struct otz_smc_cmd *cmd = NULL;
	void *session_context = NULL;
	sw_tls *tls;

	cmd_phy = (pa_t) param;

	if(map_to_ns(cmd_phy, (va_t*) &cmd)) {
		ret_val = OTZ_ENOMEM;
		goto ret_func;
	}

	if(cmd->req_buf_len == 0) {
		ret_val = OTZ_EFAIL;
		goto ret_func;
	}

	if(map_to_ns(cmd->req_buf_phys, (va_t*)&svc_id) != 0) {
		ret_val = OTZ_ENOMEM;
		goto ret_func;
	}

	if(map_to_ns(cmd->resp_buf_phys, (va_t*)&session_context) != 0) {
		ret_val = OTZ_ENOMEM;
		goto ret_func;
	}


	tls = get_task_tls(*((u32*)session_context));

	sa_destroy_entry_point(*svc_id, tls->private_data,tls->elf_flag);
	sa_close_session(session_context);


	cmd->cmd_status = OTZ_STATUS_COMPLETE;

ret_func:
	if(svc_id)
		unmap_from_ns((va_t)svc_id);

	if(session_context)
		unmap_from_ns((va_t)session_context);

	if(cmd)
		unmap_from_ns((va_t)cmd);    

	return ret_val;
}

/**
 * @brief Open session request from secure application task
 *
 * This function invokes the open session API and its get called from secure
 * application task. This is to initiate the request from secure application,
 * either for IPI or secure to non-secure world communication.
 *
 * @param svc_id: Service ID
 * @param session_id: Pointer to session ID as an output parameter
 *
 * @return otz_return_t:
 * OTZ_OK - Session established successfully\n
 * OTZ_* - An implementation-defined error code for any other error.\n
 */
int open_session_from_secure(int svc_id, int *session_id)
{
	int ret_val;
	sa_config_t sa_config;

	if(svc_id == OTZ_SVC_INVALID) {
		ret_val = OTZ_EFAIL;
		goto ret_func;
	}

	ret_val = sa_create_entry_point(svc_id, &sa_config);
	if(ret_val == OTZ_OK) {
		ret_val = sa_open_session(&sa_config, session_id);
	}

	if(ret_val == OTZ_OK) {
		sw_printf("SW: session context %x\n", *((int*)session_id));
	}
	else {
		sa_destroy_entry_point(svc_id, sa_config.data,
				sa_config.elf_flag);   
	}
ret_func:
	return ret_val;
}

/**
 * @brief Close session request which is got initiated from secure task
 *
 * This function invokes the close session API and its get called from 
 * secure application task. This is to close the request from secure application,
 * either for IPI or secure to non-secure world communication.
 *
 * @param svc_id: Service ID
 * @param session_id: Session ID
 *
 * @return otz_return_t:
 * OTZ_OK - Session established successfully\n
 * OTZ_* - An implementation-defined error code for any other error.\n
 */
int close_session_from_secure(int svc_id, int session_id)
{
	sw_tls *tls;
	tls = get_task_tls(session_id);

	sa_destroy_entry_point(svc_id, tls->private_data,tls->elf_flag);
	sa_close_session((void*)&session_id);
	return OTZ_OK;
}

/**
 * @brief Invokes the init function of the service task
 *
 * This function invokes the init function of the corresponding service task
 *
 * @param svc_id: Service identifier of the task
 * @param psa_config: Configuration details for the task
 *
 * @return otz_return_t:
 * OTZ_OK - Session established successfully\n
 * OTZ_* - An implementation-defined error code for any other error.\n
 */
int sa_create_entry_point(int svc_id, sa_config_t *psa_config)
{
	int ret=OTZ_OK;
	if(svc_id == OTZ_SVC_ECHO) {
		 if(echo_task_init(psa_config) != OTZ_OK)
			 return -1;
	} 
	else if(svc_id == OTZ_SVC_DRM) {
		if( drm_task_init(psa_config) != OTZ_OK)
			return -1;
	}
	else if(svc_id == OTZ_SVC_CRYPT) {
		if( crypto_task_init(psa_config) != OTZ_OK)
			return -1;
	}
	else if(svc_id == OTZ_SVC_MUTEX_TEST) {
		if(mutex_test_task_init(psa_config) != OTZ_OK)
			return -1;
	}
	else if(svc_id == OTZ_SVC_GLOBAL) {
		return dispatch_task_init(psa_config);
	}

#ifdef CONFIG_GUI_SUPPORT
	else if(svc_id == OTZ_SVC_VIRTUAL_KEYBOARD) {
		if(virtual_keyboard_task_init(psa_config) != OTZ_OK)
			return -1;		
	}
	else
		return -1;

#endif
#ifdef CONFIG_SW_ELF_LOADER_SUPPORT
	if(psa_config->elf_flag == ELF_FLAG)
		ret = __elf_load(psa_config);
	return ret;
#endif

}

/**
 * @brief Invokes the exit function of the service task
 *
 *
 * This function invokes the exit function of the corresponding service task
 *
 * @param svc_id: Service identifier of the task
 * @param data: Private data which need to be freed
 *
 * @return otz_return_t:
 * OTZ_OK - Session closed successfully\n
 * OTZ_* - An implementation-defined error code for any other error.\n
 */
int sa_destroy_entry_point(int svc_id, void *data, int elf_flag)
{
#if defined (CONFIG_SW_ELF_LOADER_SUPPORT)
	/* unmap the region of secure memory */
	if (elf_flag == ELF_FLAG_MAP){
		elf_loader_cleanup();
	}
#endif

	if(svc_id == OTZ_SVC_ECHO) {
		return echo_task_exit(data);
	}
	if(svc_id == OTZ_SVC_DRM) {
		return drm_task_exit(data);
	}
	if(svc_id == OTZ_SVC_CRYPT) {
		return crypto_task_exit(data);
	}
	if(svc_id == OTZ_SVC_MUTEX_TEST) {
		return mutex_test_task_exit(data);
	}
	if(svc_id == OTZ_SVC_GLOBAL) {
		return dispatch_task_exit(data);
	}
#ifdef CONFIG_GUI_SUPPORT
	if(svc_id == OTZ_SVC_VIRTUAL_KEYBOARD) {

		return virtual_keyboard_task_exit(data);
	}
#endif

	return OTZ_EFAIL;
}


/**
 * @brief Open Session
 *
 * This function establish the session between secure and 
 * non-secure application. This function creates the task correspond to the 
 * service ID.
 * 
 * @param psa_config: Pointer to task init configuration structure. 
 * @param session_context: Session ID as output parameter. 
 *
 * @return otz_return_t:
 * OTZ_OK - Session established successfully\n
 * OTZ_* - An implementation-defined error code for any other error.\n
 */
int sa_open_session(sa_config_t *psa_config, void *session_context)
{
	int ret_val;
	/* session context = task id */
	ret_val = create_task(psa_config, (int*)session_context);

	return ret_val;
}

/**
 * @brief Close Session
 *
 * This function close the session which got established between secure and 
 * non-secure application. This function destroys the corresponding service 
 * task.
 *
 * @param session_context: Session ID
 *
 * @return otz_return_t:
 * OTZ_OK - Session closed successfully\n
 * OTZ_* - An implementation-defined error code for any other error.\n
 */
int sa_close_session(void *session_context)
{
	int ret_val;
	ret_val = destroy_task(*((int*)session_context));
	return ret_val;
}


/**
 * @brief Helper function to return the service id, session id and command id 
 * from the smc command parameter
 *
 * @param svc_id: service identifier as output parameter
 * @param task_id: session context as output parameter
 * @param cmd_id: command identifier as output parameter
 * @param cmd_type: command type as output parameter
 */
void get_api_context(int *svc_id, int *task_id, int *cmd_id, int *cmd_type)
{
	pa_t cmd_phy;
	struct otz_smc_cmd *cmd = NULL;

	*svc_id = OTZ_SVC_INVALID;
	*task_id = 0;
	*cmd_id = OTZ_GLOBAL_CMD_ID_INVALID;
	*cmd_type = params_stack[2];

	if(*cmd_type == OTZ_CMD_TYPE_NS_TO_SECURE) {
		cmd_phy = (pa_t) params_stack[1];
		if(!cmd) {
			if(map_to_ns(cmd_phy, (va_t*) &cmd))
				return;
		}
	}
	else {
		cmd = (struct otz_smc_cmd *)params_stack[1];
	}

#ifdef SECURE_API_DBG
	sw_printf("SW: get_api_context: cmd va 0x%x cmd pa 0x%x\n", 
			cmd, va_to_pa_ns(cmd));

	sw_printf("SW: get_api_context: cmd va 0x%x cmd pa 0x%x\n", 
			cmd, va_to_pa(cmd));
#endif

	*svc_id = ((cmd->id >> 10) & (0x3ff));
	*cmd_id = (cmd->id & 0x3ff);
	*task_id = cmd->context;

	if(*cmd_type == OTZ_CMD_TYPE_NS_TO_SECURE) {
		unmap_from_ns((va_t)cmd);
	}

	return;
}

/**
 * @brief Helper function to decode the data which got passed from 
 * non-secure world
 *
 * This helper function decodes the data which got encoded from non-secure or
 * secure application.
 *
 * @param data: Encoded data
 * @param meta_data: Meta data helps to identify the encoded data
 * @param type: Data type of the decoded data as output parameter
 * @param offset: Current offset as input parameter and 
 * Next offset as output parameter
 * @param pos: Current position as input parameter and 
 * Next position as output parameter
 * @param mapped: Decoded data is shared memory or not as output parameter
 * @param out_data: Decoded data as output parameter
 * @param out_len: Decoded data length as output parameter
 *
 * @return otz_return_t:
 * OTZ_OK - Decoded data successfully\n
 * OTZ_* - An implementation-defined error code for any other error.\n
 */
int decode_data(void *data, 
		struct otzc_encode_meta *meta_data,
		int *type, int *offset, int *pos, 
		int *mapped, void **out_data, int *out_len)
{
	int ret = 0;
	int temp_offset = *offset, temp_pos = *pos;
	*mapped = 0;

	switch(meta_data[temp_pos].type) {
		case OTZ_ENC_UINT32: {
					     *offset = temp_offset + sizeof(u32);
					     *pos = temp_pos + 1;
					     *type = OTZ_ENC_UINT32;
					     *out_data =  data + temp_offset;
					     *out_len =  meta_data[temp_pos].len;
					     break;
				     }
		case OTZ_ENC_ARRAY: {
					    *offset = temp_offset + meta_data[temp_pos].len;
					    *pos = temp_pos + 1;
					    *type = OTZ_ENC_ARRAY;
					    *out_data =  data + temp_offset;
					    *out_len =  meta_data[temp_pos].len;
					    break;
				    }
		case OTZ_MEM_REF: {
					  if(map_to_ns((pa_t)(*((u32*)data + temp_offset)), (va_t*)out_data) != 0)
						  return -1;
					  *offset = temp_offset + sizeof(u32);
					  *type = OTZ_MEM_REF;
					  *pos = temp_pos + 1;
					  *mapped = 1;
					  *out_len =  meta_data[temp_pos].len;
					  break;
				  }
		case OTZ_SECURE_MEM_REF: {
						 *out_data = (void*)(*((u32*)data + temp_offset));
						 *offset = temp_offset + sizeof(u32);
						 *type = OTZ_MEM_REF;
						 *pos = temp_pos + 1;
						 *mapped = 0;
						 *out_len =  meta_data[temp_pos].len;
						 break;
					 }
		default:
					 ret = -1;
					 break;
	}
	return ret;
}

EXPORT_SYMBOL(decode_data);
/**
 * @brief Helper function to decode the response data for IPI 
 *
 * This helper function decodes the data which got returned from target secure 
 * application.
 *
 * @param data: Encoded data
 * @param meta_data: Meta data helps to identify the encoded data
 * @param type: Data type of the decoded data as output parameter
 * @param offset: Current offset as input parameter and 
 * Next offset as output parameter
 * @param pos: Current position as input parameter and 
 * Next position as output parameter
 * @param out_data: Decoded data as output parameter
 * @param out_len: Decoded data length as output parameter
 *
 * @return otz_return_t:
 * OTZ_OK - Decoded data successfully\n
 * OTZ_* - An implementation-defined error code for any other error.\n
 */
int decode_ipi_out_data(void *data, 
		struct otzc_encode_meta *meta_data,
		int *type, int *offset, int *pos, 
		void **out_data, int *out_len)
{
	int ret = 0;
	int temp_offset = *offset, temp_pos = *pos;

	switch(meta_data[temp_pos].type) {
		case OTZ_ENC_UINT32: {
					     *offset = temp_offset + sizeof(u32);
					     *pos = temp_pos + 1;
					     *type = OTZ_ENC_UINT32;
					     *out_data =  data + temp_offset;
					     *out_len =  meta_data[temp_pos].len;
					     break;
				     }
		case OTZ_SECURE_MEM_REF: {
						 *out_data = (void*)(*((u32*)data + temp_offset));
						 *offset = temp_offset + sizeof(u32);
						 *type = OTZ_MEM_REF;
						 *pos = temp_pos + 1;
						 *out_len =  meta_data[temp_pos].ret_len;
						 break;
					 }
		default:
					 ret = -1;
					 break;
	}
	return ret;
}

/**
 * @brief Helper function to update the response length
 *
 * @param meta_data: Meta data helps to identify the encoded data
 * @param pos: Current position 
 * @param len: new response length
 *
 * @return otz_return_t:
 * OTZ_OK - Updated responsed length successfully\n
 * OTZ_* - An implementation-defined error code for any other error.\n
 */
int update_response_len(struct otzc_encode_meta *meta_data, int pos, int len)
{
	if(pos <= OTZ_MAX_REQ_PARAMS + OTZ_MAX_RES_PARAMS) {
		meta_data[pos -1].ret_len = len;
		return 0;
	}
	else
		return -1;
}

EXPORT_SYMBOL(update_response_len);
/**
 * @brief Helper function to encode the data which originate from secure
 * application
 *
 * This function encodes the data into a encoder stream and this function gets
 * called from secure application. This will be used for IPI or secure to
 * non-secure communication.
 *
 * @param enc: Encode command structure
 * @param pmeta_data: Meta data which get populated based on the encoding
 * @param penc_context: Encoder context which contains the encoded stream for
 * request and response buffer and its details.
 * @param encode_type: Encoding data type - UINT32 or Shared memory
 *
 * @return otz_return_t:
 * OTZ_OK - Encoded data successfully\n
 * OTZ_* - An implementation-defined error code for any other error.\n
 */
int otz_encode_data(struct otz_secure_encode_cmd *enc, 
		struct otzc_encode_meta **pmeta_data,
		struct otz_secure_encode_ctx **penc_context,
		int encode_type)
{
	int ret_val = OTZ_OK;
	struct otzc_encode_meta *meta_data = NULL;
	struct otz_secure_encode_ctx *enc_context = NULL;

	meta_data = *pmeta_data;
	enc_context = *penc_context;

	if(enc == NULL) {
		ret_val = OTZ_ILLEGAL_ARGUMENT;
		goto ret_func;
	}

	if(encode_type != OTZ_ENC_UINT32 && encode_type != OTZ_SECURE_MEM_REF) {
		ret_val = OTZ_ILLEGAL_ARGUMENT;
		goto ret_func;
	}

	if(enc_context == NULL) {
		enc_context = (struct otz_secure_encode_ctx *)sw_malloc(
				sizeof(struct otz_secure_encode_ctx));

		if(!enc_context) {
			ret_val = OTZ_ENOMEM;
			goto ret_free_mem;
		}
		sw_memset(enc_context, 0, sizeof(struct otz_secure_encode_ctx));
	} 

	if(meta_data == NULL) {
		meta_data = (struct otzc_encode_meta *)sw_malloc(
				sizeof(struct otzc_encode_meta) *
				(OTZ_MAX_RES_PARAMS + OTZ_MAX_REQ_PARAMS));
		if(!meta_data) {
			ret_val = OTZ_ENOMEM;
			goto ret_free_mem;
		}
		sw_memset(meta_data, 0, sizeof(struct otzc_encode_meta) *
				(OTZ_MAX_RES_PARAMS + OTZ_MAX_REQ_PARAMS));
	} 

	if(enc->param_type == OTZC_PARAM_IN) {  
		if(!enc_context->req_addr) {
			enc_context->req_addr = (void *)sw_malloc(OTZ_1K_SIZE);
			if(!enc_context->req_addr) {
				sw_printf("SW: otz_client_encode: request addr malloc failed\n");
				ret_val = OTZ_ENOMEM;
				goto ret_free_mem;
			}
			enc_context->enc_req_offset = 0;
			enc_context->enc_req_pos = 0;
		}
		if((enc_context->enc_req_offset + sizeof(u32) <= 
					OTZ_1K_SIZE) &&
				(enc_context->enc_req_pos < OTZ_MAX_REQ_PARAMS)) {
			*((u32*)enc_context->req_addr + enc_context->enc_req_offset) =
				(u32)enc->data;
			enc_context->enc_req_offset += sizeof(u32);

			meta_data[enc_context->enc_req_pos].type = encode_type;
			meta_data[enc_context->enc_req_pos].len = enc->len;
			enc_context->enc_req_pos++;
		}
		else {
			ret_val = OTZ_ENOMEM;
			goto ret_free_mem;
		}
	}
	else if(enc->param_type == OTZC_PARAM_OUT) {   
		if(!enc_context->res_addr) {
			enc_context->res_addr = (void *)sw_malloc(OTZ_1K_SIZE);
			if(!enc_context->res_addr) {
				sw_printf("SW: otz_client_encode: response addr malloc failed\n");
				ret_val = OTZ_ENOMEM;
				goto ret_free_mem;
			}

			enc_context->enc_res_pos = OTZ_MAX_REQ_PARAMS;
			enc_context->enc_res_offset = 0;
		}

		if((enc_context->enc_res_offset + sizeof(u32)
					<= OTZ_1K_SIZE) &&
				(enc_context->enc_res_pos < 
				 (OTZ_MAX_RES_PARAMS + OTZ_MAX_REQ_PARAMS ))) {
			*((u32*)enc_context->res_addr + 
					enc_context->enc_res_offset) 
				= (u32)enc->data;
			enc_context->enc_res_offset += sizeof(u32);
			meta_data[enc_context->enc_res_pos].type 
				=  encode_type;
			meta_data[enc_context->enc_res_pos].len = enc->len;
			enc_context->enc_res_pos++;
		}
		else {
			ret_val = OTZ_ENOMEM;
			goto ret_free_mem;
		}   
	}
	else {
		ret_val = OTZ_ILLEGAL_ARGUMENT;
		goto ret_func;
	}
	goto ret_func;

ret_free_mem:
	if(enc_context) {
		if(enc_context->req_addr) {
			sw_free(enc_context->req_addr);
			enc_context->req_addr = NULL;
		}
		if(enc_context->res_addr) {
			sw_free(enc_context->res_addr);
			enc_context->res_addr = NULL;
		}
		sw_free(enc_context);
	} 
	if(meta_data) {
		sw_free(meta_data);
	} 
ret_func:
	if(! (*pmeta_data))
		*pmeta_data = meta_data;
	if(! (*penc_context))
		*penc_context = enc_context;

	return ret_val;
}

/**
 * @brief Release the encoded data
 *
 * This function releases the resources which got allocated in encoding function.
 *
 * @param enc_context: Encode context
 * @param meta_data: Meta data which got populated in encoding
 *
 */
void otz_release_data(struct otz_secure_encode_ctx *enc_context, 
		struct otzc_encode_meta *meta_data)
{
	if(enc_context) {
		if(enc_context->req_addr) {
			sw_free(enc_context->req_addr);
			enc_context->req_addr = NULL;
		}
		if(enc_context->res_addr) {
			sw_free(enc_context->res_addr);
			enc_context->res_addr = NULL;
		}
		sw_free(enc_context);
	} 
	if(meta_data) {
		sw_free(meta_data);
	} 
}

/**
 * @brief Establish session between two tasks.
 *
 * This function used in IPC functionality to establish session between two
 * tasks.
 *
 * @param svc_id: Target service ID
 * @param session_id: Session ID as output parameter
 *
 * @return otz_return_t:
 * OTZ_OK - Session established successfully\n
 * OTZ_* - An implementation-defined error code for any other error.\n
 */
int ipc_connect(int svc_id, int *session_id)
{
	return open_session_from_secure(svc_id, session_id);
}

/**
 * @brief Disconnect the previously established session between two tasks.
 *
 * This function used in IPC functionality to close the session which got
 * established between two tasks.
 *
 * @param svc_id: Target service ID
 * @param session_id: Session ID need to be closed
 *
 * @return otz_return_t:
 * OTZ_OK - Session closed successfully\n
 * OTZ_* - An implementation-defined error code for any other error.\n
 */
int ipc_disconnect(int svc_id, int session_id)
{
	return close_session_from_secure(svc_id, session_id);
}


/**
 * @brief Send the data between two tasks
 *
 * This function used to send the data between two tasks. We need to call this
 * function after the session establishment.
 *
 * @param src_svc_id: Source service ID
 * @param src_context: Source session ID
 * @param svc_id: Target service ID
 * @param session_id: Target session ID
 * @param cmd_id: Target command ID
 * @param enc_ctx: Encode context
 * @param meta_data: Meta data 
 *
 * @return otz_return_t:
 * OTZ_OK - Data sent successfully\n
 * OTZ_* - An implementation-defined error code for any other error.\n
 *
 */
int ipc_send(int src_svc_id, int src_context, int svc_id, int session_id, int cmd_id, 
		struct otz_secure_encode_ctx *enc_ctx, 
		struct otzc_encode_meta *meta_data)
{
	struct otz_smc_cmd smc_cmd;

	smc_cmd.src_id = (src_svc_id << 10);
	smc_cmd.src_context = src_context;
	smc_cmd.id = (svc_id << 10) | cmd_id;
	smc_cmd.context = session_id;
	smc_cmd.req_buf_len = enc_ctx->enc_req_offset;
	smc_cmd.resp_buf_len = enc_ctx->enc_res_offset;
	smc_cmd.ret_resp_buf_len = 0;

	if(enc_ctx->req_addr)
		smc_cmd.req_buf_phys = (pa_t)enc_ctx->req_addr;
	else
		smc_cmd.req_buf_phys = 0;

	if(enc_ctx->res_addr)
		smc_cmd.resp_buf_phys = (pa_t)enc_ctx->res_addr;
	else
		smc_cmd.resp_buf_phys = 0;

	if(meta_data)
		smc_cmd.meta_data_phys = (pa_t)meta_data;
	else
		smc_cmd.meta_data_phys = 0;       

	otz_ipi(&smc_cmd);

	return OTZ_OK;
}

/**
 * @brief Helper function to send the data between two tasks for single request
 * and single response buffer 
 *
 * This function helps to send the data between two task of single request and
 * single response. This takes care of session establishment, encoding, sending
 * and decoding of data.
 *
 * @param src_svc_id: Source service ID
 * @param src_session_id: Source session ID
 * @param target_svc_id: Target service ID
 * @param target_cmd_id: Target command ID
 * @param req_buf: Request buffer
 * @param req_buf_len: Request buffer length
 * @param res_buf: Response buffer
 * @param res_buf_len: Response buffer length
 * @param ret_res_buf_len: Return response buffer length
 *
 * @return otz_return_t:
 * OTZ_OK -  API success\n
 * OTZ_* - An implementation-defined error code for any other error.\n
 */
int ipc_api(int src_svc_id, int src_session_id, int target_svc_id, 
		int target_cmd_id, void *req_buf, int req_buf_len, 
		void* res_buf, int res_buf_len, int *ret_res_buf_len)
{
	int ret_val, session_id;
	struct otz_secure_encode_cmd enc;
	struct otzc_encode_meta *meta_data = NULL;
	struct otz_secure_encode_ctx *enc_ctx = NULL;
	void *decode_out;
	int offset = 0, pos = OTZ_MAX_REQ_PARAMS, type;

	ret_val = ipc_connect(target_svc_id, &session_id);

	if(ret_val != OTZ_OK) {
		sw_printf("SW: ipc api: connect failed\n");
		goto ret_func;
	}

	enc.len = req_buf_len;
	enc.data = req_buf;
	enc.param_type = OTZC_PARAM_IN;

	ret_val = otz_encode_data(&enc, &meta_data, &enc_ctx, OTZ_SECURE_MEM_REF);
	if(ret_val != OTZ_OK) {
		sw_printf("SW: ipc api: encode data failed\n");
		goto handler_err1;
	}

	enc.len = res_buf_len;
	enc.data = res_buf;
	enc.param_type = OTZC_PARAM_OUT;

	ret_val = otz_encode_data(&enc, &meta_data, &enc_ctx, OTZ_SECURE_MEM_REF);
	if(ret_val != OTZ_OK) {
		sw_printf("SW: ipc api: encode data failed\n");
		goto handler_err1;
	}

	ret_val = ipc_send(src_svc_id, src_session_id, target_svc_id, session_id, 
			target_cmd_id, enc_ctx, meta_data);

	if(ret_val != OTZ_OK) {
		sw_printf("SW: ipc api: send failed\n");
		goto handler_err1;
	}

	if(get_task_tls(src_session_id)->ipi_ret_val == SMC_SUCCESS) {

		if(decode_ipi_out_data(enc_ctx->res_addr, meta_data, 
					&type, &offset, &pos,(void**)&decode_out, ret_res_buf_len)) {

			otz_release_data(enc_ctx, meta_data);
			sw_printf("SW: ipc api: decode data failed\n");
			goto handler_err1;
		}

		if(decode_out != res_buf) {
			otz_release_data(enc_ctx, meta_data);
			sw_printf("SW: ipc api: wrong response buffer\n");
			goto handler_err1;
		}
	}
	else {
		otz_release_data(enc_ctx, meta_data);
		sw_printf("SW: ipc api: target service returns error 0x%x\n", 
				get_task_tls(src_session_id)->ipi_ret_val);
		goto handler_err1;
	}    
	otz_release_data(enc_ctx, meta_data);

handler_err1:
	ipc_disconnect(OTZ_SVC_CRYPT, session_id);

ret_func:
	return ret_val;

}

/**
 * @brief: Test IPC command for crypto operation 
 *
 * This function helps to test the IPC functionality by invoking crypto
 * operation.
 *
 * @param src_svc_id: Source service ID
 * @param src_session_id: Source session ID
 */
void ipc_test_crypto(int src_svc_id, int src_session_id)
{
	int ret_val, session_id, loop_cntr;
	struct otz_secure_encode_cmd enc;
	struct otzc_encode_meta *meta_data = NULL;
	struct otz_secure_encode_ctx *enc_ctx = NULL;
	char *input_string = "This is a test for IPC encryption/decryption";

	char output_buf[256], *decode_out;
	int output_buf_len = 256;

	char decrypt_output_buf[256];
	int decrypt_output_buf_len = 256;

	int offset = 0, pos = OTZ_MAX_REQ_PARAMS, type;

	u32 input_buf_len = sw_strlen(input_string)+1;

	ret_val = ipc_connect(OTZ_SVC_CRYPT, &session_id);

	if(ret_val != OTZ_OK) {
		sw_printf("SW: ipc test cmd: connect failed\n");
		goto ret_func;
	}

	enc.len = sizeof(u32);
	enc.data = (void*)input_buf_len;
	enc.param_type = OTZC_PARAM_IN;

	ret_val = otz_encode_data(&enc, &meta_data, &enc_ctx, OTZ_ENC_UINT32);
	if(ret_val != OTZ_OK) {
		sw_printf("SW: ipc test cmd: encode data failed\n");
		goto handler_err1;
	}

	enc.len = input_buf_len;
	enc.data = input_string;
	enc.param_type = OTZC_PARAM_IN;

	ret_val = otz_encode_data(&enc, &meta_data, &enc_ctx, OTZ_SECURE_MEM_REF);
	if(ret_val != OTZ_OK) {
		sw_printf("SW: ipc test cmd: encode data failed\n");
		goto handler_err1;
	}

	enc.len = output_buf_len;
	enc.data = output_buf;
	enc.param_type = OTZC_PARAM_OUT;

	ret_val = otz_encode_data(&enc, &meta_data, &enc_ctx, OTZ_SECURE_MEM_REF);
	if(ret_val != OTZ_OK) {
		sw_printf("SW: ipc test cmd: encode data failed\n");
		goto handler_err1;
	}

	ret_val = ipc_send(src_svc_id, src_session_id, OTZ_SVC_CRYPT, 
			session_id, OTZ_CRYPT_CMD_ID_ENCRYPT, 
			enc_ctx, meta_data);

	if(ret_val != OTZ_OK) {
		sw_printf("SW: ipc test cmd: send failed\n");
		goto handler_err1;
	}

	if(get_task_tls(src_session_id)->ipi_ret_val == SMC_SUCCESS) {

		if(decode_ipi_out_data(enc_ctx->res_addr, meta_data, 
					&type, &offset, &pos,(void**)&decode_out, &output_buf_len)) {

			otz_release_data(enc_ctx, meta_data);
			sw_printf("SW: ipc test cmd: decode data failed\n");
			goto handler_err1;
		}

		if(decode_out != output_buf) {
			sw_printf("SW: ipc test cmd: wrong out buffer\n");
		}
		sw_printf("SW: IPC Encrypted string is ");
		for(loop_cntr=0;loop_cntr<output_buf_len;loop_cntr++) {
			sw_printf("SW: 0x%x ",output_buf[loop_cntr]);
		}

	}
	else {
		otz_release_data(enc_ctx, meta_data);
		sw_printf("SW: ipc test cmd: service returns error 0x%x failed\n", 
				get_task_tls(src_session_id)->ipi_ret_val);
		goto handler_err1;
	}    
	otz_release_data(enc_ctx, meta_data);


	meta_data = NULL;
	enc_ctx = NULL;

	enc.len = sizeof(u32);
	enc.data = (void*)output_buf_len;
	enc.param_type = OTZC_PARAM_IN;

	ret_val = otz_encode_data(&enc, &meta_data, &enc_ctx, OTZ_ENC_UINT32);
	if(ret_val != OTZ_OK) {
		sw_printf("SW: ipc test cmd: encode data failed\n");
		goto handler_err1;
	}

	enc.len = output_buf_len;
	enc.data = output_buf;
	enc.param_type = OTZC_PARAM_IN;

	ret_val = otz_encode_data(&enc, &meta_data, &enc_ctx, OTZ_SECURE_MEM_REF);
	if(ret_val != OTZ_OK) {
		sw_printf("SW: ipc test cmd: encode data failed\n");
		goto handler_err1;
	}

	enc.len = decrypt_output_buf_len;
	enc.data = decrypt_output_buf;
	enc.param_type = OTZC_PARAM_OUT;

	ret_val = otz_encode_data(&enc, &meta_data, &enc_ctx, OTZ_SECURE_MEM_REF);
	if(ret_val != OTZ_OK) {
		sw_printf("SW: ipc test cmd: encode data failed\n");
		goto handler_err1;
	}

	ret_val = ipc_send(src_svc_id, src_session_id, OTZ_SVC_CRYPT, session_id, 
			OTZ_CRYPT_CMD_ID_DECRYPT, 
			enc_ctx, meta_data);

	if(ret_val != OTZ_OK) {
		sw_printf("SW: ipc test cmd: send failed\n");
		goto handler_err1;
	}

	if(get_task_tls(src_session_id)->ipi_ret_val == SMC_SUCCESS) {
		if(decode_ipi_out_data(enc_ctx->res_addr, meta_data, 
					&type, &offset, &pos, (void**)&decode_out, 
					&decrypt_output_buf_len)) {

			otz_release_data(enc_ctx, meta_data);
			sw_printf("SW: ipc test cmd: decode data failed\n");
			goto handler_err1;
		}

		if(decode_out != decrypt_output_buf) {
			sw_printf("SW: ipc test cmd: wrong out buffer\n");
		}
		sw_printf("SW: IPC: decrypted output %s\n", decrypt_output_buf);
	}
	else {
		otz_release_data(enc_ctx, meta_data);
		sw_printf("SW: ipc test cmd: service returns error 0x%x failed\n", 
				get_task_tls(src_session_id)->ipi_ret_val);
		goto handler_err1;
	}

	otz_release_data(enc_ctx, meta_data);

handler_err1:
	ipc_disconnect(OTZ_SVC_CRYPT, session_id);

ret_func:
	return;
}

/**
 * @brief Test IPC command for echo operation
 *
 * This function helps to test IPC command of echo service.
 *
 * @param src_svc_id: Source service ID
 * @param src_session_id: Source session ID
 */
void ipc_test_echo(int src_svc_id, int src_session_id)
{
	char* req_buf = "Test IPI echo cmd";
	int   req_buf_len = sw_strlen(req_buf) + 1;

	char res_buf[256];
	int  res_buf_len = 256;
	int  ret_val;

	ret_val = ipc_api(src_svc_id, src_session_id, OTZ_SVC_ECHO,  
			OTZ_ECHO_CMD_ID_IPI_SEND_CMD, req_buf, req_buf_len, 
			res_buf, res_buf_len, &res_buf_len);

	if(ret_val == OTZ_OK) {
		sw_printf("SW: ipc echo test: %s\n", res_buf);
	}
}
EXPORT_SYMBOL(ipc_test_echo);
/**
 * @brief Helper function to handle the task return
 *
 * This function helps to handle the return functionality of task. This function
 * puts the task in to wait or suspend state based on the return value and also
 * helps to set the return value of secure call or IPC call.
 *
 * @param task_id: Task ID
 * @param tls: Task local storage 
 */
void handle_task_return(int task_id, sw_tls* tls)
{
	suspend_task(task_id, TASK_STATE_SUSPEND);

	if(tls->params[2] == OTZ_CMD_TYPE_NS_TO_SECURE){
		set_secure_api_ret(tls->ret_val);
	}
	else if(tls->params[2] == OTZ_CMD_TYPE_SECURE_TO_SECURE)
		otz_ipi_return((struct otz_smc_cmd *)tls->params[1], tls->ret_val);

	schedule();
}

EXPORT_SYMBOL(handle_task_return);

#ifdef OTZONE_ASYNC_NOTIFY_SUPPORT
/**
 * @brief Send notification to non-secure world
 *
 * This function sends the notification message to Non-secure world.
 *
 * @param task_id: Task ID
 */
void notify_ns(int task_id)
{
	sw_tls *tls;
	pa_t cmd_phy;
	u32 *params;
	struct otz_smc_cmd *cmd = NULL;


	tls = get_task_tls(task_id);
	if(!tls)
		return;

	params = tls->params;
	if(!params){
		return;
	}
	if(params[2] == OTZ_CMD_TYPE_NS_TO_SECURE) {
		cmd_phy = (pa_t) params[1];
		if(!cmd) {
			if(map_to_ns(cmd_phy, (va_t*) &cmd)) {
				return;
			}
		}
	}
	else {
		cmd = (struct otz_smc_cmd *)params[1];
	}


	invoke_ns_callback(((cmd->id >> 10) & (0x3ff)), 
			cmd->context, cmd->enc_id, cmd->src_context,
			cmd->dev_file_id);

	return;   
}

/**
 * @brief Dispatcher command function to register the shared memory
 * for notification
 *
 * This function registers the shared memory which will be used for 
 * notification from secure world to normal world.
 *
 * @param param: pointer to struct otz_smc_cmd
 *
 * @return otz_return_t:
 * OTZ_OK - Shared memory registration success\n
 * OTZ_* - An implementation-defined error code for any other error.\n
 */
int register_notify_data_api(void *param)
{
	int ret_val = OTZ_OK;
	pa_t cmd_phy;
	struct otz_smc_cmd *cmd = NULL;

	cmd_phy = (pa_t) param;


	if(map_to_ns(cmd_phy, (va_t*) &cmd)) {
		ret_val = OTZ_ENOMEM;
		goto ret_func;
	}

	ret_val = register_notify_data(cmd->req_buf_phys);

ret_func:

	if(cmd)    
		unmap_from_ns((va_t)cmd);    

	return ret_val;
}

/**
 * @brief Dispatcher command function to un-register the shared memory
 * of notification
 *
 * This function un-registers the shared memory which was used for 
 * notification from secure world to normal world.
 *
 *
 * @return otz_return_t:
 * OTZ_OK - Shared memory un-registration success\n
 * OTZ_* - An implementation-defined error code for any other error.\n
 */
int unregister_notify_data_api(void)
{
	unregister_notify_data();
	return OTZ_OK;
}
#endif
