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
 * Crypto tests implementation
 */

#ifdef _CRYPTO_BUILD_

#include "crypto_tests.h"
#include "otz_tee_crypto_api.h"
#include "sw_mem_functions.h"

/**
* @brief 
*
* @param input_buf
* @param input_len
* @param output_buf
* @param output_len
* @param svc_cmd_id
*
* @return 
*/
int test_digest_api(u8 *input_buf,u32 input_len,u8* output_buf,
				u32 *output_len,u32 svc_cmd_id)
{
	TEE_OperationHandle digest_test_ops;
	TEEC_Result retVal = TEEC_SUCCESS;

	retVal = TEE_AllocateOperation(&digest_test_ops,svc_cmd_id,TEE_MODE_DIGEST,0);
	if( retVal != TEEC_SUCCESS) {
		TEE_FreeOperation(digest_test_ops);
		return(retVal);
	}
	TEE_DigestUpdate(digest_test_ops,input_buf,input_len);
	retVal = TEE_DigestDoFinal(digest_test_ops,NULL,0,output_buf,output_len);
	if(retVal != TEEC_SUCCESS) {
		TEE_FreeOperation(digest_test_ops);
		return(retVal);
	}
	TEE_FreeOperation(digest_test_ops);
	return(retVal);
}


/**
* @brief 
*
* @param input_buf
* @param input_len
* @param key_buf
* @param key_len
* @param output_buf
* @param output_len
* @param svc_cmd_id
*
* @return 
*/
int test_hmac_api(u8 *input_buf,u32 input_len,u8* key_buf, u32 key_len,
				u8* output_buf,u32 *output_len,u32 svc_cmd_id)
{
	TEE_OperationHandle hmac_test_ops;
	TEEC_Result retVal = TEEC_SUCCESS;

	retVal = TEE_AllocateOperation(&hmac_test_ops,svc_cmd_id,TEE_MODE_MAC,
																HMAC_KEY_LEN);
	if( retVal != TEEC_SUCCESS) {
		TEE_FreeOperation(hmac_test_ops);
		return(retVal);
	}
	TEE_MACInit(hmac_test_ops,key_buf,key_len);
	TEE_MACUpdate(hmac_test_ops,input_buf,input_len);
	retVal = TEE_MACComputeFinal(hmac_test_ops,NULL,0,output_buf,output_len);
	if(retVal != TEEC_SUCCESS) {
		TEE_FreeOperation(hmac_test_ops);
		return(retVal);
	}
	TEE_FreeOperation(hmac_test_ops);
	return(retVal);
}

/**
* @brief 
*
* @param input_buf
* @param input_len
* @param key_buf
* @param key_len
* @param output_buf
* @param output_len
* @param svc_cmd_id
*
* @return 
*/
int compare_hmac_values(u8 *input_buf,u32 input_len,u8* key_buf, u32 key_len,
			   u8* output_buf,u32 *output_len,u32 svc_cmd_id)
{
	TEE_OperationHandle hmac_test_ops;
	TEEC_Result retVal = TEEC_SUCCESS;

	retVal = TEE_AllocateOperation(&hmac_test_ops,svc_cmd_id,TEE_MODE_MAC,
																HMAC_KEY_LEN);
	if( retVal != TEEC_SUCCESS) {
		TEE_FreeOperation(hmac_test_ops);
		return(retVal);
	}
	TEE_MACInit(hmac_test_ops,key_buf,key_len);
	TEE_MACUpdate(hmac_test_ops,input_buf,input_len);
	retVal = TEE_MACCompareFinal(hmac_test_ops,NULL,0,output_buf,output_len);
	if(retVal != TEEC_SUCCESS) {
		TEE_FreeOperation(hmac_test_ops);
		return(retVal);
	}
	TEE_FreeOperation(hmac_test_ops);
	return(retVal);
}


/**
* @brief 
*
* @param input_buf
* @param input_len
* @param init_vector
* @param init_vector_len
* @param key_buf
* @param key_len
* @param output_buf
* @param output_len
* @param svc_cmd_id
* @param cipher_choice
*
* @return 
*/
int test_cipher_api(u8 *input_buf,u32 input_len,u8 *init_vector,
			u32 init_vector_len,u8 *key_buf, u32 key_len,
			u8* output_buf,u32 *output_len,
				u32 svc_cmd_id,u32 cipher_choice)
{
	TEE_OperationHandle cipher_test_ops;
	TEEC_Result retVal = TEEC_SUCCESS;
	uint32_t modeVal = 0,local_buf_len = 0,current_len=0;
	TEE_ObjectHandleVar keyObj;
	u16 loop_cntr = 0;
	u8* local_buf;

	if((local_buf = sw_malloc((sizeof(u8))*(*output_len))) == NULL) {
		process_otz_crypto_unload_libs();
		return(TEEC_ERROR_OUT_OF_MEMORY);
	}
	(cipher_choice == CIPHER_ENCRYPT) ? (modeVal = TEE_MODE_ENCRYPT) :
					(modeVal = TEE_MODE_DECRYPT);
	retVal = TEE_AllocateOperation(&cipher_test_ops,svc_cmd_id,modeVal,
															AES_128_CBC_LEN);
	if( retVal != TEEC_SUCCESS) {
		TEE_FreeOperation(cipher_test_ops);
		sw_free(local_buf);
		return(retVal);
	}
	keyObj.dataLen = key_len;
	if((keyObj.dataPtr = sw_malloc((sizeof(u8))*key_len)) == NULL) {
		TEE_FreeOperation(cipher_test_ops);
		sw_free(local_buf);
		retVal = TEEC_ERROR_OUT_OF_MEMORY;
		return(retVal);
	}
	sw_memcpy(keyObj.dataPtr,key_buf,key_len);
	retVal = TEE_SetOperationKey(cipher_test_ops,&keyObj);
	if(retVal != TEEC_SUCCESS) {
		TEE_FreeOperation(cipher_test_ops);
		sw_free(keyObj.dataPtr);
		sw_free(local_buf);
		return(retVal);
	}
	sw_free(keyObj.dataPtr);
	TEE_CipherInit(cipher_test_ops,init_vector,init_vector_len);
	current_len = *output_len;
	retVal = TEE_CipherUpdate(cipher_test_ops,input_buf,input_len,
					local_buf+local_buf_len,&current_len);
	if(retVal != TEEC_SUCCESS) {
		TEE_FreeOperation(cipher_test_ops);
		sw_free(local_buf);
		return(retVal);
	}
	local_buf_len += current_len;
	current_len = *output_len - local_buf_len;
	retVal = TEE_CipherDoFinal(cipher_test_ops,NULL,0,
			local_buf+local_buf_len,&current_len);
	if(retVal != TEEC_SUCCESS) {
		TEE_FreeOperation(cipher_test_ops);
		sw_free(local_buf);
		return(retVal);
	}
	local_buf_len += current_len;
	TEE_FreeOperation(cipher_test_ops);
	sw_memcpy(output_buf,local_buf,local_buf_len);
	*output_len = local_buf_len;
	sw_free(local_buf);
	return(retVal);
}

#endif

