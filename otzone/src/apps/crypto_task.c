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
 * Crypto task implementation
 */



#include <monitor.h>
#include <sw_debug.h>
#include <cpu_data.h>
#include <global.h>
#include <task.h>
#include <page_table.h>
#include <sw_string_functions.h>
#include <sw_mem_functions.h>
#include <elf_loader.h>

#include <otz_id.h>
#include <otz_common.h>
#include <cpu.h>
#include <task_control.h>
#include <crypto_task.h>
#include <otz_app_eg.h>
#include "otz_tee_crypto_api.h"
#include "sw_types.h"

/**
* @brief 
*
* @param input_buf
* @param input_len
* @param output_buf
* @param output_len
*
* @return 
*/
extern int otzone_rc4_algorithm(char *input_buf, int input_len,
                                            char *output_buf, int *output_len);

/**
* @brief 
*/
void process_otz_crypto_load_libs()
{
#ifdef _CRYPTO_BUILD_
	OpenSSL_add_all_ciphers();
	OpenSSL_add_all_digests();
#endif
}

/**
* @brief 
*/
void process_otz_crypto_unload_libs()
{
#ifdef _CRYPTO_BUILD_
	EVP_cleanup();
	CRYPTO_cleanup_all_ex_data();
#endif
}


/**
* @brief 
*
* @param req_buf
* @param req_buf_len
* @param res_buf
* @param res_buf_len
* @param meta_data
* @param ret_res_buf_len
* @param svc_cmd_id
*
* @return 
*/
int process_otz_crypto_digest_cmd(void *req_buf, u32 req_buf_len,
                void *res_buf, u32 res_buf_len,
                struct otzc_encode_meta *meta_data,
                u32 *ret_res_buf_len,u32 svc_cmd_id)
{
#ifdef _CRYPTO_BUILD_
    crypto_data_t crypto_data;
    u8 *out_buf;
    u32 offset = 0, pos = 0, mapped = 0, type =0, out_len =0;
	u32 cmd_id = 0;

    while (offset <= req_buf_len) {
        if(decode_data(req_buf, meta_data, &type, &offset, &pos, &mapped, 
                                                (void**)&out_buf, &out_len)) {
            return SMC_EINVAL_ARG;
        }
        if((type != OTZ_ENC_ARRAY) && (type != OTZ_MEM_REF)){ 
            return SMC_EINVAL_ARG;
        }
        if(out_len < CRYPT_BUF_LEN) {
            sw_memcpy(crypto_data.data, out_buf, out_len);
        } else {
            return(SMC_ENOMEM);
        }
        break;
    }
	crypto_data.len = CRYPT_BUF_LEN;
    switch(svc_cmd_id) {
    	case OTZ_CRYPT_CMD_ID_MD5:
    		cmd_id = TEE_ALG_MD5;
    	case OTZ_CRYPT_CMD_ID_SHA1:
			if(cmd_id == 0) {
	    		cmd_id = TEE_ALG_SHA1;
			}
    	case OTZ_CRYPT_CMD_ID_SHA224:
			if(cmd_id == 0) {
				cmd_id = TEE_ALG_SHA224;
			}
    	case OTZ_CRYPT_CMD_ID_SHA256:
			if(cmd_id == 0) {
				cmd_id = TEE_ALG_SHA256;
			}
    	case OTZ_CRYPT_CMD_ID_SHA384:
			if(cmd_id == 0) {
				cmd_id = TEE_ALG_SHA384;
			}
    	case OTZ_CRYPT_CMD_ID_SHA512:
			if(cmd_id == 0) {
				cmd_id = TEE_ALG_SHA512;
			}
    		if(test_digest_api(crypto_data.data,out_len,
                    		crypto_data.response,&(crypto_data.len),
                    			cmd_id) != TEEC_SUCCESS) {
    			sw_memcpy(crypto_data.response,crypto_data.data,out_len);
    			crypto_data.len = out_len;
    		}
    		break;
    	default:
    		break;
    }
    offset = 0, pos = OTZ_MAX_REQ_PARAMS;
    while (offset <= res_buf_len) {
        if(decode_data(res_buf, meta_data, &type, &offset, &pos, &mapped, 
                                            (void**)&out_buf, &out_len)) {
            return SMC_EINVAL_ARG;
        }
        if((type != OTZ_ENC_ARRAY) && (type != OTZ_MEM_REF)){ 
            return SMC_EINVAL_ARG;
        }
        if(out_len >= crypto_data.len) {
            sw_memcpy(out_buf,crypto_data.response, crypto_data.len);
            if(update_response_len(meta_data, pos, crypto_data.len)) {
                return SMC_EINVAL_ARG;
            }
        } else {
            return(SMC_ENOMEM);
        }
        break;
    }
    *ret_res_buf_len = crypto_data.len;
#endif
    return 0;
}


/**
* @brief 
*
* @param req_buf
* @param req_buf_len
* @param res_buf
* @param res_buf_len
* @param meta_data
* @param ret_res_buf_len
* @param svc_cmd_id
*
* @return 
*/
int process_otz_crypto_hmac_cmd(void *req_buf, u32 req_buf_len,
                void *res_buf, u32 res_buf_len,
                struct otzc_encode_meta *meta_data,
                u32 *ret_res_buf_len,u32 svc_cmd_id)
{
#ifdef _CRYPTO_BUILD_
    crypto_data_t crypto_data;
    u8 *out_buf;
    u8 key_buf[HMAC_KEY_LEN];
    u16 key_len = 0,loop_cntr = 0;
    u32 offset = 0, pos = 0, mapped = 0, type =0, out_len =0;
	u32 cmd_id = 0;

    while (offset <= req_buf_len) {
        if(decode_data(req_buf, meta_data, &type, &offset, &pos, &mapped,
                                                (void**)&out_buf, &out_len)) {
            return SMC_EINVAL_ARG;
        }
        if((type != OTZ_ENC_ARRAY) && (type != OTZ_MEM_REF)){
            return SMC_EINVAL_ARG;
        }
        if(out_len <= HMAC_KEY_LEN) {
            sw_memcpy(key_buf, out_buf, out_len);
            key_len = out_len;
        } else {
            return(SMC_ENOMEM);
        }
        if(decode_data(req_buf, meta_data, &type, &offset, &pos, &mapped,
                                                (void**)&out_buf, &out_len)) {
            return SMC_EINVAL_ARG;
        }
        if((type != OTZ_ENC_ARRAY) && (type != OTZ_MEM_REF)){
            return SMC_EINVAL_ARG;
        }
        if(out_len < CRYPT_BUF_LEN) {
            sw_memcpy(crypto_data.data, out_buf, out_len);
        } else {
            return(SMC_ENOMEM);
        }
        break;
    }
	crypto_data.len = CRYPT_BUF_LEN;
    switch(svc_cmd_id) {
    	case OTZ_CRYPT_CMD_ID_HMAC_MD5:
    		cmd_id = TEE_ALG_HMAC_MD5;
    	case OTZ_CRYPT_CMD_ID_HMAC_SHA1:
    		if(cmd_id == 0) {
    			cmd_id = TEE_ALG_HMAC_SHA1;
    		}
    	case OTZ_CRYPT_CMD_ID_HMAC_SHA224:
    		if(cmd_id == 0) {
    			cmd_id = TEE_ALG_HMAC_SHA224;
    		}
    	case OTZ_CRYPT_CMD_ID_HMAC_SHA256:
    		if(cmd_id == 0) {
    			cmd_id = TEE_ALG_HMAC_SHA256;
    		}
    	case OTZ_CRYPT_CMD_ID_HMAC_SHA384:
    		if(cmd_id == 0) {
    			cmd_id = TEE_ALG_HMAC_SHA384;
    		}
    	case OTZ_CRYPT_CMD_ID_HMAC_SHA512:
    		if(cmd_id == 0) {
    			cmd_id = TEE_ALG_HMAC_SHA512;
    		}
    		if(test_hmac_api(crypto_data.data,out_len,key_buf,key_len,
                    		crypto_data.response,&(crypto_data.len),
                    			cmd_id) != TEEC_SUCCESS) {
    			sw_memcpy(crypto_data.response,crypto_data.data,out_len);
    			crypto_data.len = out_len;
    		}
    		if(compare_hmac_values(crypto_data.data,out_len,key_buf,key_len,
                    		crypto_data.response,&(crypto_data.len),
                    				cmd_id) != TEEC_SUCCESS) {
    			sw_memcpy(crypto_data.response,crypto_data.data,out_len);
    			crypto_data.len = out_len;
    		}
    		break;
    	default:
    		break;
    }
    offset = 0, pos = OTZ_MAX_REQ_PARAMS;
    while (offset <= res_buf_len) {
        if(decode_data(res_buf, meta_data, &type, &offset, &pos, &mapped,
                                            (void**)&out_buf, &out_len)) {
            return SMC_EINVAL_ARG;
        }
        if((type != OTZ_ENC_ARRAY) && (type != OTZ_MEM_REF)){
            return SMC_EINVAL_ARG;
        }
        if(out_len >= crypto_data.len) {
            sw_memcpy(out_buf,crypto_data.response, crypto_data.len);
            if(update_response_len(meta_data, pos, crypto_data.len)) {
                return SMC_EINVAL_ARG;
            }
        } else {
            return(SMC_ENOMEM);
        }
        break;
    }
    *ret_res_buf_len = crypto_data.len;
#endif
    return(0);
}


/**
* @brief 
*
* @param req_buf
* @param req_buf_len
* @param res_buf
* @param res_buf_len
* @param meta_data
* @param ret_res_buf_len
* @param svc_cmd_id
*
* @return 
*/
int process_otz_crypto_cipher_cmd(void *req_buf, u32 req_buf_len,
                void *res_buf, u32 res_buf_len,
                struct otzc_encode_meta *meta_data,
                u32 *ret_res_buf_len,u32 svc_cmd_id)
{
#ifdef _CRYPTO_BUILD_
    crypto_data_t crypto_data;
    u8 *out_buf,*local_init_vector = NULL;
    u8 key_buf[CRYPT_BUF_LEN],init_vector[CRYPT_BUF_LEN];
    u32 cipher_choice = 0;
    u16 key_len = 0,loop_cntr = 0,init_vector_len=0;
    u32 offset = 0, pos = 0, mapped = 0, type =0, out_len =0;
	u32 cmd_id = 0,init_vector_present = 0;

    while (offset <= req_buf_len) {
        if(decode_data(req_buf, meta_data, &type, &offset, &pos, &mapped,
                                                (void**)&out_buf, &out_len)) {
            return SMC_EINVAL_ARG;
        }
        if((type != OTZ_ENC_ARRAY) && (type != OTZ_MEM_REF)){
			return SMC_EINVAL_ARG;
		}
        cipher_choice = out_buf[0];
        if(decode_data(req_buf, meta_data, &type, &offset, &pos, &mapped,
                                                (void**)&out_buf, &out_len)) {
            return SMC_EINVAL_ARG;
        }
        if((type != OTZ_ENC_ARRAY) && (type != OTZ_MEM_REF)){
			return SMC_EINVAL_ARG;
		}
        init_vector_present = out_buf[0];
        if(init_vector_present == 1) {
			if(decode_data(req_buf, meta_data, &type, &offset, &pos, &mapped,
						(void**)&out_buf, &out_len)) {
				return SMC_EINVAL_ARG;
			}
			if((type != OTZ_ENC_ARRAY) && (type != OTZ_MEM_REF)){
				return SMC_EINVAL_ARG;
			}
			if(out_len <= CRYPT_BUF_LEN) {
				sw_memcpy(init_vector, out_buf, out_len);
				init_vector_len = out_len;
			} else {
				return(SMC_ENOMEM);
			}
			local_init_vector = &(init_vector[0]);
        } else {
        	init_vector_len = 0;
        	local_init_vector = NULL;
        }
        if(decode_data(req_buf, meta_data, &type, &offset, &pos, &mapped,
                                                (void**)&out_buf, &out_len)) {
            return SMC_EINVAL_ARG;
        }
        if((type != OTZ_ENC_ARRAY) && (type != OTZ_MEM_REF)){
            return SMC_EINVAL_ARG;
        }
        if(out_len <= CRYPT_BUF_LEN) {
            sw_memcpy(key_buf, out_buf, out_len);
            key_len = out_len;
        } else {
            return(SMC_ENOMEM);
        }
        if(decode_data(req_buf, meta_data, &type, &offset, &pos, &mapped,
                                                (void**)&out_buf, &out_len)) {
            return SMC_EINVAL_ARG;
        }
        if((type != OTZ_ENC_ARRAY) && (type != OTZ_MEM_REF)){
            return SMC_EINVAL_ARG;
        }
        if(out_len < CRYPT_BUF_LEN) {
            sw_memcpy(crypto_data.data, out_buf, out_len);
        } else {
            return(SMC_ENOMEM);
        }
        break;
    }
	/*sw_printf("Init vector length is 0x%x \n",init_vector_len);
	for(loop_cntr=0;loop_cntr<init_vector_len;loop_cntr++) {
		if(loop_cntr % 16 == 0) {
			sw_printf("\n");
		}
		sw_printf("0x%x ",local_init_vector[loop_cntr]);
	}
	sw_printf("\n");*/
	/*sw_printf("Input key length is 0x%x \n",key_len);
	for(loop_cntr=0;loop_cntr<key_len;loop_cntr++) {
		if(loop_cntr % 16 == 0) {
			sw_printf("\n");
		}
		sw_printf("0x%x ",key_buf[loop_cntr]);
	}
	sw_printf("\n");
	sw_printf("Input data length is 0x%x \n",out_len);
	for(loop_cntr=0;loop_cntr<out_len;loop_cntr++) {
		if(loop_cntr % 16 == 0) {
			sw_printf("\n");
		}
		sw_printf("0x%x ",crypto_data.data[loop_cntr]);
	}
	sw_printf("\n");*/
    crypto_data.len = CRYPT_BUF_LEN;
    switch(svc_cmd_id) {
    	case OTZ_CRYPT_CMD_ID_CIPHER_AES_128_CBC:
    		cmd_id = TEE_ALG_AES_CBC_NOPAD;
    	case OTZ_CRYPT_CMD_ID_CIPHER_AES_128_ECB:
    		if(cmd_id == 0) {
    			cmd_id = TEE_ALG_AES_ECB_NOPAD;
    		}
    	case OTZ_CRYPT_CMD_ID_CIPHER_AES_128_CTR:
    		if(cmd_id == 0) {
				cmd_id = TEE_ALG_AES_CTR;
			}
    	case OTZ_CRYPT_CMD_ID_CIPHER_AES_128_XTS:
    		if(cmd_id == 0) {
				cmd_id = TEE_ALG_AES_XTS;
			}
        case OTZ_CRYPT_CMD_ID_CIPHER_DES_ECB:
    		if(cmd_id == 0) {
				cmd_id = TEE_ALG_DES_ECB_NOPAD;
			}
        case OTZ_CRYPT_CMD_ID_CIPHER_DES_CBC:
    		if(cmd_id == 0) {
				cmd_id = TEE_ALG_DES_CBC_NOPAD;
			}
        case OTZ_CRYPT_CMD_ID_CIPHER_DES3_ECB:
    		if(cmd_id == 0) {
				cmd_id = TEE_ALG_DES3_ECB_NOPAD;
			}
        case OTZ_CRYPT_CMD_ID_CIPHER_DES3_CBC:
    		if(cmd_id == 0) {
				cmd_id = TEE_ALG_DES3_CBC_NOPAD;
			}
    		if(test_cipher_api(crypto_data.data,out_len,local_init_vector,
							init_vector_len,key_buf,key_len,
							crypto_data.response,&(crypto_data.len),
							cmd_id,cipher_choice) != TEEC_SUCCESS) {
    			sw_memcpy(crypto_data.response,crypto_data.data,out_len);
    			crypto_data.len = out_len;
    		}
    		break;
    	default:
    		break;
    }
    offset = 0, pos = OTZ_MAX_REQ_PARAMS;
    while (offset <= res_buf_len) {
        if(decode_data(res_buf, meta_data, &type, &offset, &pos, &mapped,
                                            (void**)&out_buf, &out_len)) {
            return SMC_EINVAL_ARG;
        }
        if((type != OTZ_ENC_ARRAY) && (type != OTZ_MEM_REF)){
            return SMC_EINVAL_ARG;
        }
        if(out_len >= crypto_data.len) {
            sw_memcpy(out_buf,crypto_data.response, crypto_data.len);
            if(update_response_len(meta_data, pos, crypto_data.len)) {
                return SMC_EINVAL_ARG;
            }
        } else {
            return(SMC_ENOMEM);
        }
        break;
    }
    *ret_res_buf_len = crypto_data.len;

#endif
	return(0);
}


/**
 * @brief Process crypto service 
 *
 * This function process the crypto service commands
 *
 * @param svc_cmd_id: Command identifer to process the crypto service command
 * @param req_buf: Virtual address of the request buffer
 * @param req_buf_len: Request buffer length
 * @param resp_buf: Virtual address of the response buffer
 * @param res_buf_len: Response buffer length
 * @param meta_data: Virtual address of the meta data of the encoded data
 * @param ret_res_buf_len: Return length of the response buffer
 *
 * @return SMC return codes:
 * SMC_SUCCESS: Crypto service command processed successfully. \n
 * SMC_*: An implementation-defined error code for any other error.
 */
int process_otz_crypto_svc(u32 svc_cmd_id, void *req_buf, u32 req_buf_len, 
                    void *resp_buf, u32 res_buf_len,
                    struct otzc_encode_meta *meta_data, u32 *ret_res_buf_len)
{
    int ret_val = SMC_ERROR;

    if((req_buf_len == 0) || (res_buf_len == 0) ) {
        return(0);
    }
    
    switch (svc_cmd_id) {
    	case OTZ_CRYPT_CMD_ID_LOAD_LIBS:
    		process_otz_crypto_load_libs();
    		break;
    	case OTZ_CRYPT_CMD_ID_UNLOAD_LIBS:
    		process_otz_crypto_unload_libs();
    		break;
        case OTZ_CRYPT_CMD_ID_ENCRYPT:
        case OTZ_CRYPT_CMD_ID_DECRYPT:
        	break;
        case OTZ_CRYPT_CMD_ID_MD5:
        case OTZ_CRYPT_CMD_ID_SHA1:
        case OTZ_CRYPT_CMD_ID_SHA224:
        case OTZ_CRYPT_CMD_ID_SHA256:
        case OTZ_CRYPT_CMD_ID_SHA384:
        case OTZ_CRYPT_CMD_ID_SHA512:
            ret_val = process_otz_crypto_digest_cmd(req_buf,req_buf_len,
                     	 	 	 	 	resp_buf,res_buf_len,meta_data,
                     	 	 	 	 	ret_res_buf_len,svc_cmd_id);
            break;
        case OTZ_CRYPT_CMD_ID_HMAC_MD5:
        case OTZ_CRYPT_CMD_ID_HMAC_SHA1:
        case OTZ_CRYPT_CMD_ID_HMAC_SHA224:
        case OTZ_CRYPT_CMD_ID_HMAC_SHA256:
        case OTZ_CRYPT_CMD_ID_HMAC_SHA384:
        case OTZ_CRYPT_CMD_ID_HMAC_SHA512:
        	ret_val = process_otz_crypto_hmac_cmd(req_buf,req_buf_len,
					resp_buf,res_buf_len,meta_data,
					ret_res_buf_len,svc_cmd_id);
			break;
        case OTZ_CRYPT_CMD_ID_CIPHER_AES_128_CBC:
        case OTZ_CRYPT_CMD_ID_CIPHER_AES_128_ECB:
        case OTZ_CRYPT_CMD_ID_CIPHER_AES_128_CTR:
        case OTZ_CRYPT_CMD_ID_CIPHER_AES_128_XTS:
        case OTZ_CRYPT_CMD_ID_CIPHER_DES_ECB:
        case OTZ_CRYPT_CMD_ID_CIPHER_DES_CBC:
        case OTZ_CRYPT_CMD_ID_CIPHER_DES3_ECB:
        case OTZ_CRYPT_CMD_ID_CIPHER_DES3_CBC:
        	ret_val = process_otz_crypto_cipher_cmd(req_buf,req_buf_len,
					resp_buf,res_buf_len,meta_data,
					ret_res_buf_len,svc_cmd_id);
        	break;
        default:
            ret_val = SMC_EOPNOTSUPP;
            break;
    }
    return ret_val;
}

/**
 * @brief Crypto task entry point
 *
 * This function implements the processing of crypto commands.
 *
 * @param task_id: task identifier
 * @param tls: Pointer to task local storage
 */
void crypto_task(int task_id, sw_tls* tls)
{
    tls->ret_val = process_otzapi(task_id, tls);
    handle_task_return(task_id, tls);
    while(1);
}

/**
 * @brief: Crypto task init
 *
 * This function initializes crypto task parameters
 *
 * @param psa_config: Configuration parameter for the task and its get called 
 * before the task creation
 *
 * @return otz_return_t:
 * OTZ_OK \n
 * OTZ_* - An implementation-defined error code for any other error.\n
 */
int crypto_task_init(sa_config_t *psa_config)
{
    sw_memset(psa_config, 0x0, sizeof(sa_config_t));
    psa_config->service_uuid = OTZ_SVC_CRYPT;
    sw_strcpy(psa_config->service_name, "crypto");
    psa_config->stack_size = TASK_STACK_SIZE;
    psa_config->entry_point = (u32)&crypto_task;
	psa_config->process = (u32)&process_otz_crypto_svc;


    psa_config->data = (void *)sw_malloc(sizeof(struct crypto_global));
    if(!psa_config->data) {
        sw_printf("SW: crypto task init: allocation of local storage data failed\n");
        return OTZ_ENOMEM;
    }
    return OTZ_OK;
}


/**
 * @brief Crypto task exit
 *
 * This function gets called before the task deletion
 *
 * @param data: Private data which need to be freed.
 * @return otz_return_t:
 * OTZ_OK \n
 * OTZ_FAIL \n 
 */
int crypto_task_exit(void* data)
{
    if(data)
        sw_free(data);
    return OTZ_OK;
}

