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
 * Source file for global platform TEE crypto API
 *
 */

#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/modes.h>
#include <modes_lcl.h>
#include <openssl/rsa.h>
#include <openssl/dsa.h>
#include <otz_tee_crypto_api.h>


/**
* @brief 
*
* @param panicCode
*/
void TEE_Panic(TEE_Result panicCode)
{
	/* A suitable panic function needs to be implemented, which will not
	 * return to the caller. For now, we use just return();
	 */
	return;
}

/**
* @brief 
*
* @param algorithm
*
* @return 
*/
static int check_valid_algorithm(uint32_t algorithm)
{
	switch(algorithm) {
		case TEE_ALG_AES_ECB_NOPAD:
		case TEE_ALG_AES_CBC_NOPAD:
		case TEE_ALG_AES_CTR:
		case TEE_ALG_AES_CTS:
		case TEE_ALG_AES_XTS:
		case TEE_ALG_AES_CBC_MAC_NOPAD:
		case TEE_ALG_AES_CBC_MAC_PKCS5:
		case TEE_ALG_AES_CMAC:
		case TEE_ALG_AES_CCM:
		case TEE_ALG_AES_GCM:
		case TEE_ALG_DES_ECB_NOPAD:
		case TEE_ALG_DES_CBC_NOPAD:
		case TEE_ALG_DES_CBC_MAC_NOPAD:
		case TEE_ALG_DES_CBC_MAC_PKCS5:
		case TEE_ALG_DES3_ECB_NOPAD:
		case TEE_ALG_DES3_CBC_NOPAD:
		case TEE_ALG_DES3_CBC_MAC_NOPAD:
		case TEE_ALG_DES3_CBC_MAC_PKCS5:
		case TEE_ALG_RSASSA_PKCS1_V1_5_MD5:
		case TEE_ALG_RSASSA_PKCS1_V1_5_SHA1:
		case TEE_ALG_RSASSA_PKCS1_V1_5_SHA224:
		case TEE_ALG_RSASSA_PKCS1_V1_5_SHA256:
		case TEE_ALG_RSASSA_PKCS1_V1_5_SHA384:
		case TEE_ALG_RSASSA_PKCS1_V1_5_SHA512:
		case TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA1:
		case TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA224:
		case TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA256:
		case TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA384:
		case TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA512:
		case TEE_ALG_RSAES_PKCS1_V1_5:
		case TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA1:
		case TEE_ALG_RSA_NOPAD:
		case TEE_ALG_DSA_SHA1:
		case TEE_ALG_DH_DERIVE_SHARED_SECRET:
		case TEE_ALG_MD5:
		case TEE_ALG_SHA1:
		case TEE_ALG_SHA224:
		case TEE_ALG_SHA256:
		case TEE_ALG_SHA384:
		case TEE_ALG_SHA512:
		case TEE_ALG_HMAC_MD5:
		case TEE_ALG_HMAC_SHA1:
		case TEE_ALG_HMAC_SHA224:
		case TEE_ALG_HMAC_SHA256:
		case TEE_ALG_HMAC_SHA384:
		case TEE_ALG_HMAC_SHA512:
			break;
		default:
			return(-1);
	}
	return(0);
}

/**
* @brief 
*
* @param mode
*
* @return 
*/
static int check_valid_mode(uint32_t mode)
{
	switch(mode) {
		case TEE_MODE_ENCRYPT:
		case TEE_MODE_DECRYPT:
		case TEE_MODE_SIGN:
		case TEE_MODE_VERIFY:
		case TEE_MODE_MAC:
		case TEE_MODE_DIGEST:
		case TEE_MODE_DERIVE:
			break;
		default:
			return(-1);
	}
	return(0);
}

/**
* @brief 
*
* @param mode
* @param algorithm
*
* @return 
*/
static int check_valid_algorithm_for_mode(uint32_t mode,uint32_t algorithm)
{
	switch(mode) {
		case TEE_MODE_ENCRYPT:
		case TEE_MODE_DECRYPT:
			switch(algorithm) {
				case TEE_ALG_AES_ECB_NOPAD:
				case TEE_ALG_AES_CBC_NOPAD:
				case TEE_ALG_AES_CTR:
				case TEE_ALG_AES_CTS:
				case TEE_ALG_AES_XTS:
				case TEE_ALG_AES_CCM:
				case TEE_ALG_AES_GCM:
				case TEE_ALG_DES_ECB_NOPAD:
				case TEE_ALG_DES_CBC_NOPAD:
				case TEE_ALG_DES3_ECB_NOPAD:
				case TEE_ALG_DES3_CBC_NOPAD:
				case TEE_ALG_RSAES_PKCS1_V1_5:
				case TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA1:
				case TEE_ALG_RSA_NOPAD:
					break;
				default:
					return(-1);
			}
			break;
		case TEE_MODE_SIGN:
		case TEE_MODE_VERIFY:
			switch(algorithm) {
				case TEE_ALG_RSASSA_PKCS1_V1_5_MD5:
				case TEE_ALG_RSASSA_PKCS1_V1_5_SHA1:
				case TEE_ALG_RSASSA_PKCS1_V1_5_SHA224:
				case TEE_ALG_RSASSA_PKCS1_V1_5_SHA256:
				case TEE_ALG_RSASSA_PKCS1_V1_5_SHA384:
				case TEE_ALG_RSASSA_PKCS1_V1_5_SHA512:
				case TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA1:
				case TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA224:
				case TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA256:
				case TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA384:
				case TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA512:
				case TEE_ALG_DSA_SHA1:
					break;
				default:
					return(-1);
			}
			break;
		case TEE_MODE_MAC:
			switch(algorithm) {
				case TEE_ALG_DES_CBC_MAC_NOPAD:
				case TEE_ALG_AES_CBC_MAC_NOPAD:
				case TEE_ALG_AES_CBC_MAC_PKCS5:
				case TEE_ALG_AES_CMAC:
				case TEE_ALG_DES_CBC_MAC_PKCS5:
				case TEE_ALG_DES3_CBC_MAC_NOPAD:
				case TEE_ALG_DES3_CBC_MAC_PKCS5:
				case TEE_ALG_HMAC_MD5:
				case TEE_ALG_HMAC_SHA1:
				case TEE_ALG_HMAC_SHA224:
				case TEE_ALG_HMAC_SHA256:
				case TEE_ALG_HMAC_SHA384:
				case TEE_ALG_HMAC_SHA512:
					break;
				default:
					return(-1);
			}
			break;
		case TEE_MODE_DIGEST:
			switch(algorithm) {
				case TEE_ALG_MD5:
				case TEE_ALG_SHA1:
				case TEE_ALG_SHA224:
				case TEE_ALG_SHA256:
				case TEE_ALG_SHA384:
				case TEE_ALG_SHA512:
					break;
				default:
					return(-1);
			}
			break;
		case TEE_MODE_DERIVE:
			switch(algorithm) {
				case TEE_ALG_DH_DERIVE_SHARED_SECRET:
					break;
				default:
					return(-1);
			}
			break;
		default:
			return(-1);
	}
	return(0);
}


/**
* @brief 
*
* @param localOperationHandle
*
* @return 
*/
static TEEC_Result handleDigestAlloc(TEE_OperationHandle localOperationHandle)
{
	if(localOperationHandle->algorithm == TEE_ALG_MD5) {
		if((localOperationHandle->crypto_ctxt =
				(MD5_CTX*)sw_malloc(sizeof(MD5_CTX))) == NULL) {
			return(TEEC_ERROR_OUT_OF_MEMORY);
		}
		if(!MD5_Init((MD5_CTX*)(localOperationHandle->crypto_ctxt))) {
			return(TEEC_ERROR_OUT_OF_MEMORY);
		}
	}
	if(localOperationHandle->algorithm == TEE_ALG_SHA1) {
		if((localOperationHandle->crypto_ctxt =
			(SHA_CTX*)sw_malloc(sizeof(SHA_CTX))) == NULL) {
			return(TEEC_ERROR_OUT_OF_MEMORY);
		}
		if(!SHA_Init(localOperationHandle->crypto_ctxt)) {
			return(TEEC_ERROR_OUT_OF_MEMORY);
		}
	}
	if(localOperationHandle->algorithm == TEE_ALG_SHA224) {
		if((localOperationHandle->crypto_ctxt =
			(SHA256_CTX*)sw_malloc(sizeof(SHA256_CTX))) == NULL) {
			return(TEEC_ERROR_OUT_OF_MEMORY);
		}
		if(!SHA224_Init(localOperationHandle->crypto_ctxt)) {
			return(TEEC_ERROR_OUT_OF_MEMORY);
		}
	}
	if(localOperationHandle->algorithm == TEE_ALG_SHA256) {
		if((localOperationHandle->crypto_ctxt =
			(SHA256_CTX*)sw_malloc(sizeof(SHA256_CTX))) == NULL) {
			return(TEEC_ERROR_OUT_OF_MEMORY);
		}
		if(!SHA256_Init(localOperationHandle->crypto_ctxt)) {
			return(TEEC_ERROR_OUT_OF_MEMORY);
		}
	}
	if(localOperationHandle->algorithm == TEE_ALG_SHA384) {
		if((localOperationHandle->crypto_ctxt =
			(SHA512_CTX*)sw_malloc(sizeof(SHA512_CTX))) == NULL) {
			return(TEEC_ERROR_OUT_OF_MEMORY);
		}
		if(!SHA384_Init(localOperationHandle->crypto_ctxt)) {
			return(TEEC_ERROR_OUT_OF_MEMORY);
		}
	}
	if(localOperationHandle->algorithm == TEE_ALG_SHA512) {
		if((localOperationHandle->crypto_ctxt =
			(SHA512_CTX*)sw_malloc(sizeof(SHA512_CTX))) == NULL) {
			return(TEEC_ERROR_OUT_OF_MEMORY);
		}
		if(!SHA512_Init(localOperationHandle->crypto_ctxt)) {
			return(TEEC_ERROR_OUT_OF_MEMORY);
		}
	}
	return(TEEC_SUCCESS);
}


/**
* @brief 
*
* @param localOperationHandle
*
* @return 
*/
static TEEC_Result handleMacAlloc(TEE_OperationHandle localOperationHandle)
{
	switch(localOperationHandle->algorithm){
		case TEE_ALG_HMAC_MD5:
		case TEE_ALG_HMAC_SHA1:
		case TEE_ALG_HMAC_SHA224:
		case TEE_ALG_HMAC_SHA256:
		case TEE_ALG_HMAC_SHA384:
		case TEE_ALG_HMAC_SHA512:
			if((localOperationHandle->crypto_ctxt =
				(HMAC_CTX*)sw_malloc(sizeof(HMAC_CTX))) == NULL) {
				return(TEEC_ERROR_OUT_OF_MEMORY);
			}
			HMAC_CTX_init(localOperationHandle->crypto_ctxt);
			break;
		default:
			break;
	}
	return(TEEC_SUCCESS);
}

/**
* @brief 
*
* @param localOperationHandle
*
* @return 
*/
static TEEC_Result handleCipherAlloc(TEE_OperationHandle localOperationHandle)
{
	u8 load_cipher = 0;

	switch(localOperationHandle->algorithm){
		case TEE_ALG_AES_ECB_NOPAD:
			if(load_cipher == 0) {
				load_cipher = 1;
				localOperationHandle->crypto_cipher = EVP_aes_128_ecb();
			}
		case TEE_ALG_AES_CBC_NOPAD:
			if(load_cipher == 0) {
				load_cipher = 1;
				localOperationHandle->crypto_cipher = EVP_aes_128_cbc();
			}
		case TEE_ALG_AES_CTR:
			if(load_cipher == 0) {
				load_cipher = 1;
				localOperationHandle->crypto_cipher = EVP_aes_128_ctr();
			}
		/*case TEE_ALG_AES_CTS: This case alone merits special treatment*/
		case TEE_ALG_AES_XTS:
			if(load_cipher == 0) {
				load_cipher = 1;
				localOperationHandle->crypto_cipher = EVP_aes_128_xts();
			}
		case TEE_ALG_DES_ECB_NOPAD:
			if(load_cipher == 0) {
				load_cipher = 1;
				localOperationHandle->crypto_cipher = EVP_des_ecb();
			}
		case TEE_ALG_DES_CBC_NOPAD:
			if(load_cipher == 0) {
				load_cipher = 1;
				localOperationHandle->crypto_cipher = EVP_des_cbc();
			}
		case TEE_ALG_DES3_ECB_NOPAD:
			if(load_cipher == 0) {
				load_cipher = 1;
				localOperationHandle->crypto_cipher = EVP_des_ede3();
			}
		case TEE_ALG_DES3_CBC_NOPAD:
			if(load_cipher == 0) {
				load_cipher = 1;
				localOperationHandle->crypto_cipher = EVP_des_ede3_cbc();
			}
			if((localOperationHandle->crypto_ctxt =
				(EVP_CIPHER_CTX*)sw_malloc(sizeof(EVP_CIPHER_CTX))) == NULL) {
				return(TEEC_ERROR_OUT_OF_MEMORY);
			}
			EVP_CIPHER_CTX_init((EVP_CIPHER_CTX*)(localOperationHandle->crypto_ctxt));
			localOperationHandle->keyValue = NULL;
			break;
		case TEE_ALG_AES_CTS:
			localOperationHandle->crypto_ctxt = NULL;
			localOperationHandle->crypto_cipher = NULL;
			break;
		case TEE_ALG_AES_CCM:
			if((localOperationHandle->crypto_ctxt =
				(CCM128_CONTEXT*)sw_malloc(sizeof(CCM128_CONTEXT))) == NULL) {
				return(TEEC_ERROR_OUT_OF_MEMORY);
			}
			localOperationHandle->keyValue = NULL;
			localOperationHandle->crypto_cipher = NULL;
			break;
		case TEE_ALG_AES_GCM:
			if((localOperationHandle->crypto_ctxt =
				(GCM128_CONTEXT*)sw_malloc(sizeof(GCM128_CONTEXT))) == NULL) {
				return(TEEC_ERROR_OUT_OF_MEMORY);
			}
			localOperationHandle->keyValue = NULL;
			localOperationHandle->crypto_cipher = NULL;
			break;
		default:
			break;
	}
	return(TEEC_SUCCESS);
}

/**
* @brief 
*
* @param operation
* @param algorithm
* @param mode
* @param maxKeySize
*
* @return 
*/
TEEC_Result TEE_AllocateOperation(TEE_OperationHandle *operation,
			uint32_t algorithm, uint32_t mode,uint32_t maxKeySize)
{
	TEE_OperationHandle localOperationHandle;
	TEEC_Result retVal;

	/*if(init_done == 0){
		init_done = 1;
		OpenSSL_add_all_ciphers();
		OpenSSL_add_all_digests();
		ENGINE_load_builtin_engines();
	}*/
	if(check_valid_algorithm(algorithm) == -1) {
		return(TEEC_ERROR_NOT_SUPPORTED);
	}
	if(check_valid_mode(mode) == -1) {
		return(TEEC_ERROR_NOT_SUPPORTED);
	}
	if(check_valid_algorithm_for_mode(mode,algorithm) == -1) {
		return(TEEC_ERROR_NOT_SUPPORTED);
	}
	if((localOperationHandle =
		(TEE_OperationHandle)sw_malloc(sizeof(TEE_OperationHandleVar))) == NULL) {
		return(TEEC_ERROR_OUT_OF_MEMORY);
	}
	localOperationHandle->algorithm = algorithm;
	switch(mode) {
		case TEE_MODE_DIGEST:
			if((retVal = handleDigestAlloc(localOperationHandle)) != TEEC_SUCCESS) {
				return(retVal);
			}
			localOperationHandle->operationClass = TEE_OPERATION_DIGEST;
			break;
		case TEE_MODE_MAC:
			if((retVal = handleMacAlloc(localOperationHandle))
															!= TEEC_SUCCESS) {
				return(retVal);
			}
			localOperationHandle->operationClass = TEE_OPERATION_MAC;
			break;
		case TEE_MODE_DECRYPT:
		case TEE_MODE_ENCRYPT:
			if((retVal = handleCipherAlloc(localOperationHandle))
															!= TEEC_SUCCESS) {
				return(retVal);
			}
			if((algorithm == TEE_ALG_AES_CCM) || (algorithm == TEE_ALG_AES_GCM)) {
				localOperationHandle->operationClass = TEE_OPERATION_AE;
			} else if((algorithm == TEE_ALG_RSAES_PKCS1_V1_5) ||
					(algorithm == TEE_ALG_RSA_NOPAD)) {
				localOperationHandle->operationClass =
					TEE_OPERATION_ASYMMETRIC_CIPHER;
				localOperationHandle->crypto_ctxt = NULL;
			} else {
				localOperationHandle->operationClass = TEE_OPERATION_CIPHER;
			}
			break;
		case TEE_MODE_SIGN:
		case TEE_MODE_VERIFY:
			localOperationHandle->crypto_ctxt = NULL;
			localOperationHandle->operationClass =
				TEE_OPERATION_ASYMMETRIC_SIGNATURE;
			break;
		default:
			break;
	}
	localOperationHandle->mode = mode;
	localOperationHandle->maxKeySize = maxKeySize;
	*operation = localOperationHandle;
	return(TEEC_SUCCESS);
}

/**
* @brief 
*
* @param operation
*/
static void handleDigestFree(TEE_OperationHandle operation)
{
	switch(operation->algorithm) {
		case TEE_ALG_MD5:
			OPENSSL_cleanse((MD5_CTX*)(operation->crypto_ctxt),sizeof(MD5_CTX));
		break;
		case TEE_ALG_SHA1:
			OPENSSL_cleanse((SHA_CTX*)(operation->crypto_ctxt),sizeof(SHA_CTX));
			break;
		case TEE_ALG_SHA224:
		case TEE_ALG_SHA256:
			OPENSSL_cleanse((SHA256_CTX*)(operation->crypto_ctxt),sizeof(SHA256_CTX));
			break;
		case TEE_ALG_SHA384:
		case TEE_ALG_SHA512:
			OPENSSL_cleanse((SHA512_CTX*)(operation->crypto_ctxt),sizeof(SHA512_CTX));
			break;
		default:
			break;
	}
	return;
}

/**
* @brief 
*
* @param operation
*/
static void handleMacFree(TEE_OperationHandle operation)
{
	switch(operation->algorithm) {
		case TEE_ALG_HMAC_MD5:
		case TEE_ALG_HMAC_SHA1:
		case TEE_ALG_HMAC_SHA224:
		case TEE_ALG_HMAC_SHA256:
		case TEE_ALG_HMAC_SHA384:
		case TEE_ALG_HMAC_SHA512:
			HMAC_CTX_cleanup((HMAC_CTX*)(operation->crypto_ctxt));
			break;
		default:
			break;
	}
	return;
}

/**
* @brief 
*
* @param operation
*/
static void handleCipherFree(TEE_OperationHandle operation)
{

	switch(operation->algorithm) {
		case TEE_ALG_AES_ECB_NOPAD:
		case TEE_ALG_AES_CBC_NOPAD:
		case TEE_ALG_AES_CTR:
		case TEE_ALG_AES_XTS:
		case TEE_ALG_DES_ECB_NOPAD:
		case TEE_ALG_DES_CBC_NOPAD:
		case TEE_ALG_DES3_ECB_NOPAD:
		case TEE_ALG_DES3_CBC_NOPAD:
			if(operation->keyValue != NULL) {
				sw_free(operation->keyValue);
			}
			EVP_CIPHER_CTX_cleanup(operation->crypto_ctxt);
			break;
		case TEE_ALG_AES_CTS:
		case TEE_ALG_AES_CCM:
		case TEE_ALG_AES_GCM:
			if(operation->IV != NULL) {
				sw_free(operation->IV);
			}
			if(operation->keyValue != NULL) {
				sw_free(operation->keyValue);
			}
			if(operation->crypto_ctxt != NULL) {
				OPENSSL_cleanse(operation->crypto_ctxt,
				sizeof(operation->crypto_ctxt));
			}
			break;
		case TEE_ALG_RSAES_PKCS1_V1_5:
		case TEE_ALG_RSA_NOPAD:
			if(operation->keyValue != NULL) {
				RSA_free(operation->keyValue);
				sw_free(operation->keyValue);
			}
			break;
		default:
			break;
	}
	return;
}


/**
* @brief 
*
* @param operation
*/
void TEE_FreeOperation(TEE_OperationHandle operation)
{
	if(operation == NULL) {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return;
	}
	switch(operation->operationClass) {
		case TEE_OPERATION_DIGEST:
			handleDigestFree(operation);
			break;
		case TEE_OPERATION_MAC:
			handleMacFree(operation);
			break;
		case TEE_OPERATION_CIPHER:
		case TEE_OPERATION_AE:
		case TEE_OPERATION_ASYMMETRIC_CIPHER:
			handleCipherFree(operation);
			break;
		case TEE_OPERATION_ASYMMETRIC_SIGNATURE:
			switch(operation->algorithm) {
				case TEE_ALG_RSASSA_PKCS1_V1_5_MD5:
				case TEE_ALG_RSASSA_PKCS1_V1_5_SHA1:
					if(operation->keyValue != NULL) {
						RSA_free(operation->keyValue);
						sw_free(operation->keyValue);
					}
					break;
				case TEE_ALG_DSA_SHA1:
					if(operation->keyValue != NULL) {
						DSA_free(operation->keyValue);
						sw_free(operation->keyValue);
					}
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
	sw_free(operation->crypto_ctxt);
	sw_free(operation);
	return;
}

/**
* @brief 
*
* @param operation
* @param operationInfo
*/
void TEE_GetOperationInfo(TEE_OperationHandle operation,
					TEE_OperationInfo* operationInfo)
{
	if(operation == NULL) {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return;
	}
	if(operationInfo == NULL) {
		return;
	}
	operationInfo->algorithm = operation->algorithm;
	operationInfo->mode = operation->mode;
	operationInfo->mode = operation->mode;
	operationInfo->digestLength = operation->digestLength;
	if((operation->algorithm == TEE_ALG_RSAES_PKCS1_V1_5) ||
			(operation->algorithm == TEE_ALG_RSA_NOPAD)) {
		if((operationInfo->keyValue =
				(u8*)sw_malloc(sizeof(u8)*SW_RSA_KEYLEN)) != NULL) {
			operationInfo->keySize =
			i2d_RSAPublicKey((RSA*)(operation->keyValue),
				(u8**)(&(operationInfo->keyValue)));
			operationInfo->keySize +=
				i2d_RSAPrivateKey((RSA*)(operation->keyValue),
				(u8**)(&(operationInfo->keyValue)));
			operationInfo->maxKeySize = operationInfo->keySize;
		}
	} else {
		operationInfo->maxKeySize = operation->maxKeySize;
		operationInfo->keySize = operation->keySize;
		if(operation->keyValue != NULL) {
			sw_memcpy(operationInfo->keyValue,operation->keyValue,
														operation->keySize);
		}
	}
	operationInfo->requiredKeyUsage = operation->requiredKeyUsage;
	operationInfo->handleState = operation->handleState;
	return;
}

/**
* @brief 
*
* @param operation
*/
void TEE_ResetOperation(TEE_OperationHandle operation)
{
	if(operation == NULL) {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return;
	}
	switch(operation->mode) {
		case TEE_OPERATION_ASYMMETRIC_CIPHER:
		case TEE_OPERATION_ASYMMETRIC_SIGNATURE:
		case TEE_OPERATION_KEY_DERIVATION:
			return;
		break;
	}
	return;
}


/**
* @brief 
*
* @param operation
* @param key
*
* @return 
*/
static TEEC_Result fillRSAKeys(TEE_OperationHandle operation,
														TEE_ObjectHandle* key)
{

	operation->keySize = SW_RSA_KEYLEN;
	operation->maxKeySize = SW_RSA_KEYLEN;
	if(key == NULL) {
		if((operation->keyValue = RSA_generate_key(SW_RSA_KEYLEN,
			RSA_F4,NULL,NULL)) == NULL) {
			operation->maxKeySize = operation->keySize = 0;
			return(TEEC_ERROR_GENERIC);
		}
		operation->keySize = SW_RSA_KEYLEN;
		operation->maxKeySize = SW_RSA_KEYLEN;
	} else {
		if((operation->keyValue = (RSA*)sw_malloc(sizeof(RSA))) == NULL) {
			return(TEEC_ERROR_OUT_OF_MEMORY);
		}
		if(key->dataLen != SW_RSA_KEYLEN) {
			sw_free(operation->keyValue);
			operation->maxKeySize = operation->keySize = 0;
			return(TEEC_ERROR_BAD_PARAMETERS);
		}
		u8 *localKeyPtr = key->dataPtr;
		uint32_t localKeyLen = key->dataLen;
		if(d2i_RSAPublicKey((RSA**)(&(operation->keyValue)),
			(const u8**)(&localKeyPtr),localKeyLen) == NULL){
			sw_free(operation->keyValue);
			operation->maxKeySize = operation->keySize = 0;
			return(TEEC_ERROR_BAD_PARAMETERS);
		} else {
			localKeyLen -= (localKeyPtr - (u8*)key->dataPtr);
			if(d2i_RSAPrivateKey((RSA**)(&(operation->keyValue)),
			(const u8**)(&localKeyPtr),localKeyLen) == NULL) {
				sw_free(operation->keyValue);
				operation->maxKeySize = operation->keySize = 0;
				return(TEEC_ERROR_BAD_PARAMETERS);
			}
		}
	}
	return(TEEC_SUCCESS);
}


/**
* @brief 
*
* @param operation
* @param key
*
* @return 
*/
TEEC_Result TEE_SetOperationKey(TEE_OperationHandle operation,
														TEE_ObjectHandle* key)
{
	TEEC_Result retVal;

	if(operation == NULL) {
		return(TEEC_ERROR_BAD_PARAMETERS);
	}
	if(operation->mode == TEE_MODE_DIGEST) {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return(TEEC_ERROR_BAD_PARAMETERS);
	}
	/*if(operation->algorithm == TEE_ALG_AES_XTS) {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return(TEEC_ERROR_BAD_PARAMETERS);
	}*/
	if(operation->operationClass == TEE_OPERATION_CIPHER) {
		if((operation->crypto_cipher != NULL) &&
			(key->dataLen != ((EVP_CIPHER*)(operation->crypto_cipher))->key_len)) {
			return(TEEC_ERROR_BAD_PARAMETERS);
		}
		if((operation->keyValue = (u8*)sw_malloc(key->dataLen)) == NULL) {
			return(TEEC_ERROR_OUT_OF_MEMORY);
		}
		memcpy(operation->keyValue,key->dataPtr,key->dataLen);
		operation->keySize = key->dataLen;
	}
	if((operation->operationClass == TEE_OPERATION_AE) ||
			(operation->algorithm == TEE_ALG_AES_CTS))  {
		if((operation->keyValue = (AES_KEY*)sw_malloc(sizeof(AES_KEY))) == NULL) {
			return(TEEC_ERROR_OUT_OF_MEMORY);
		}
		if(operation->mode == TEE_MODE_DECRYPT) {
			AES_set_decrypt_key(key->dataPtr,key->dataLen,operation->keyValue);
		} else if(operation->mode == TEE_MODE_ENCRYPT) {
			AES_set_encrypt_key(key->dataPtr,key->dataLen,operation->keyValue);
		}
		operation->keySize = key->dataLen;
	}
	if((operation->operationClass == TEE_OPERATION_ASYMMETRIC_CIPHER) &&
		((operation->algorithm == TEE_ALG_RSAES_PKCS1_V1_5) ||
			(operation->algorithm == TEE_ALG_RSA_NOPAD))) {
		if((retVal = fillRSAKeys(operation,key)) != TEEC_SUCCESS) {
			return(retVal);
		}
	}
	if((operation->operationClass == TEE_OPERATION_ASYMMETRIC_SIGNATURE)) {
		if((operation->algorithm == TEE_ALG_RSASSA_PKCS1_V1_5_MD5) ||
			(operation->algorithm == TEE_ALG_RSASSA_PKCS1_V1_5_SHA1)) {
			if((retVal = fillRSAKeys(operation,key)) != TEEC_SUCCESS) {
				return(retVal);
			}
		} else if(operation->algorithm == TEE_ALG_DSA_SHA1) {
			if((operation->keyValue = DSA_generate_parameters(SW_DSA_KEYLEN,
			NULL,0,NULL,NULL,NULL,NULL)) == NULL) {
				operation->keySize = operation->maxKeySize = 0;
				return(TEEC_ERROR_BAD_PARAMETERS);
			}
			operation->keySize = operation->maxKeySize = SW_DSA_KEYLEN;
		}
	}
	return(TEEC_SUCCESS);
}


/**
* @brief 
*
* @param operation
* @param key1
* @param key2
*
* @return 
*/
TEEC_Result TEE_SetOperationKey2(TEE_OperationHandle operation,
				TEE_ObjectHandle* key1,TEE_ObjectHandle* key2)
{
	if(operation == NULL) {
		return(TEEC_ERROR_BAD_PARAMETERS);
	}
	if(key1 == NULL || key2 == NULL) {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return(TEEC_ERROR_BAD_PARAMETERS);
	}
	if(operation->mode == TEE_MODE_DIGEST) {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return(TEEC_ERROR_BAD_PARAMETERS);
	}
	if(operation->algorithm == TEE_ALG_AES_XTS) {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return(TEEC_ERROR_BAD_PARAMETERS);
	}
	return(TEEC_SUCCESS);
}


/**
* @brief 
*
* @param dstOperation
* @param srcOperation
*/
void TEE_CopyOperation(TEE_OperationHandle dstOperation,
			TEE_OperationHandle srcOperation)
{
	if((dstOperation == NULL) || (srcOperation == NULL)) {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return;
	}
	if((dstOperation->mode != srcOperation->mode) ||
		(dstOperation->algorithm != srcOperation->algorithm)) {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return;
	}
	if((srcOperation->keyValue != NULL) &&
		(srcOperation->keySize > dstOperation->keySize)) {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return;
	}
	sw_memcpy(dstOperation,srcOperation,sizeof(srcOperation));
	sw_memcpy(dstOperation->crypto_ctxt,srcOperation->crypto_ctxt,
		sizeof(srcOperation->crypto_ctxt));
	if(srcOperation->keyValue != NULL) {
		if(srcOperation->keySize > dstOperation->keySize) {
			TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
			return;
		}
		sw_memcpy(dstOperation->keyValue,srcOperation->keyValue,
		sizeof(srcOperation->keySize));
	}
	return;
}


/**
* @brief 
*
* @param operation
* @param chunk
* @param chunkSize
*/
void TEE_DigestUpdate(TEE_OperationHandle operation,
			void* chunk, size_t chunkSize)
{
	if(operation == NULL) {
		return;
	}
	if(operation->operationClass != TEE_OPERATION_DIGEST) {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return;
	}
	if(operation->algorithm == TEE_ALG_MD5) {
		/*if(!MD5_Init((MD5_CTX*)(operation->crypto_ctxt))) {
			return;
		}*/
		MD5_Update((MD5_CTX*)(operation->crypto_ctxt),chunk,chunkSize);
	}
	if(operation->algorithm == TEE_ALG_SHA1) {
		/*if(!SHA_Init((SHA_CTX*)(operation->crypto_ctxt))) {
			return;
		}*/
		SHA_Update((SHA_CTX*)(operation->crypto_ctxt),chunk,chunkSize);
	}
	if((operation->algorithm == TEE_ALG_SHA224) ||
		(operation->algorithm == TEE_ALG_SHA256)) {
		/*if(operation->algorithm == TEE_ALG_SHA224) {
			if(!SHA224_Init((SHA256_CTX*)(operation->crypto_ctxt))) {
				return;
			}
		}
		if(operation->algorithm == TEE_ALG_SHA256) {
			if(!SHA256_Init((SHA256_CTX*)(operation->crypto_ctxt))) {
				return;
			}
		}*/
		SHA256_Update((SHA256_CTX*)(operation->crypto_ctxt),chunk,chunkSize);
	}
	if((operation->algorithm == TEE_ALG_SHA384) ||
		(operation->algorithm == TEE_ALG_SHA512)) {
		/*if(operation->algorithm == TEE_ALG_SHA384) {
			if(!SHA384_Init((SHA512_CTX*)(operation->crypto_ctxt))) {
				return;
			}
		}
		if(operation->algorithm == TEE_ALG_SHA512) {
			if(!SHA512_Init((SHA512_CTX*)(operation->crypto_ctxt))) {
				return;
			}
		}*/
		SHA512_Update((SHA512_CTX*)(operation->crypto_ctxt),chunk,chunkSize);
	}
	return;
}


/**
* @brief 
*
* @param operation
* @param chunk
* @param chunkLen
* @param hash
* @param hashLen
*
* @return 
*/
TEEC_Result TEE_DigestDoFinal(TEE_OperationHandle operation,
		void* chunk, size_t chunkLen,void* hash, size_t *hashLen)
{
	if((operation == NULL) || (hash == NULL)) {
		return(TEEC_ERROR_BAD_PARAMETERS);
	}
	if(operation->operationClass != TEE_OPERATION_DIGEST) {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return(TEEC_ERROR_BAD_PARAMETERS);
	}
	switch(operation->algorithm) {
		case TEE_ALG_MD5:
			if(*hashLen < MD5_DIGEST_LENGTH) {
				return(TEEC_ERROR_SHORT_BUFFER);
			}
			/*if(!MD5_Init((MD5_CTX*)(operation->crypto_ctxt))) {
				return(TEEC_ERROR_GENERIC);
			}*/
			if(chunkLen != 0) {
				MD5_Update((MD5_CTX*)(operation->crypto_ctxt),chunk,chunkLen);
			}
			static unsigned char md5_output[MD5_DIGEST_LENGTH];
			MD5_Final(md5_output,(MD5_CTX*)(operation->crypto_ctxt));
			sw_memcpy(hash,md5_output,MD5_DIGEST_LENGTH);
			*hashLen = MD5_DIGEST_LENGTH;
			break;
		case TEE_ALG_SHA1:
			if(*hashLen < SHA_DIGEST_LENGTH) {
				return(TEEC_ERROR_SHORT_BUFFER);
			}
			/*if(!SHA_Init((SHA_CTX*)(operation->crypto_ctxt))) {
				return(TEEC_ERROR_GENERIC);
			}*/
			if(chunkLen != 0) {
				SHA_Update((SHA_CTX*)(operation->crypto_ctxt),chunk,chunkLen);
			}
			static unsigned char sha1_output[SHA_DIGEST_LENGTH];
			SHA_Final(sha1_output,(SHA_CTX*)(operation->crypto_ctxt));
			sw_memcpy(hash,sha1_output,SHA_DIGEST_LENGTH);
			*hashLen = SHA_DIGEST_LENGTH;
			break;
		case TEE_ALG_SHA224:
		case TEE_ALG_SHA256:
			if(*hashLen < SHA_DIGEST_LENGTH) {
				return(TEEC_ERROR_SHORT_BUFFER);
			}
			/*if(!SHA_Init((SHA_CTX*)(operation->crypto_ctxt))) {
				return(TEEC_ERROR_GENERIC);
			}*/
			if(chunkLen != 0) {
				SHA256_Update((SHA256_CTX*)(operation->crypto_ctxt),chunk,chunkLen);
			}
			static unsigned char sha256_output[SHA256_DIGEST_LENGTH];
			SHA256_Final(sha256_output,(SHA256_CTX*)(operation->crypto_ctxt));
			if(operation->algorithm == TEE_ALG_SHA224) {
				*hashLen = SHA224_DIGEST_LENGTH;
			} else {
				*hashLen = SHA256_DIGEST_LENGTH;
			}
			sw_memcpy(hash,sha256_output,*hashLen);
			break;
		case TEE_ALG_SHA384:
		case TEE_ALG_SHA512:
			if(*hashLen < SHA_DIGEST_LENGTH) {
				return(TEEC_ERROR_SHORT_BUFFER);
			}
			/*if(!SHA_Init((SHA_CTX*)(operation->crypto_ctxt))) {
				return(TEEC_ERROR_GENERIC);
			}*/
			if(chunkLen != 0) {
				SHA512_Update((SHA512_CTX*)(operation->crypto_ctxt),chunk,chunkLen);
			}
			static unsigned char sha512_output[SHA512_DIGEST_LENGTH];
			SHA512_Final(sha512_output,(SHA512_CTX*)(operation->crypto_ctxt));
			if(operation->algorithm == TEE_ALG_SHA384) {
				*hashLen = SHA384_DIGEST_LENGTH;
			} else {
				*hashLen = SHA512_DIGEST_LENGTH;
			}
			sw_memcpy(hash,sha512_output,*hashLen);
			break;
		default:
			break;
	}
		return(TEEC_SUCCESS);
}


/**
* @brief 
*
* @param operation
* @param IV
* @param IVLen
*/
void TEE_CipherInit(TEE_OperationHandle operation,void* IV, size_t IVLen)
{
	if(operation == NULL) {
		return;
	}
	if(operation->operationClass != TEE_OPERATION_CIPHER)  {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return;
	}
	if(operation->keyValue == NULL) {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return;
	}
	if(operation->algorithm == TEE_ALG_AES_CTS) {
		if((operation->IV = sw_malloc(IVLen)) == NULL) {
			return;
		}
		memcpy(operation->IV,IV,IVLen);
		operation->IVLen = IVLen;
		return;
	}
	switch(operation->algorithm) {
		case TEE_ALG_AES_ECB_NOPAD:
		case TEE_ALG_AES_CBC_NOPAD:
		case TEE_ALG_AES_CTR:
		case TEE_ALG_AES_XTS:
		case TEE_ALG_DES_ECB_NOPAD:
		case TEE_ALG_DES_CBC_NOPAD:
		case TEE_ALG_DES3_ECB_NOPAD:
		case TEE_ALG_DES3_CBC_NOPAD:
			if(operation->crypto_cipher == NULL) {
				return;
			}
			(operation->mode == TEE_MODE_ENCRYPT) ?
				EVP_EncryptInit_ex((EVP_CIPHER_CTX*)(operation->crypto_ctxt),
				(EVP_CIPHER*)(operation->crypto_cipher),
				NULL,operation->keyValue,IV) :
				EVP_DecryptInit_ex((EVP_CIPHER_CTX*)(operation->crypto_ctxt),
				(EVP_CIPHER*)(operation->crypto_cipher),
				NULL,operation->keyValue,IV);
			EVP_CIPHER_CTX_set_padding((EVP_CIPHER_CTX*)(operation->crypto_ctxt),0);
			break;
		default:
			break;
	}
	return;
}


/**
* @brief 
*
* @param operation
* @param srcData
* @param srcLen
* @param destData
* @param destLen
*
* @return 
*/
TEEC_Result TEE_CipherUpdate(TEE_OperationHandle operation,
		void* srcData, size_t srcLen,void* destData, size_t *destLen)
{
	u8 retVal = 0;

	if(operation == NULL) {
		return(TEEC_ERROR_BAD_PARAMETERS);
	}
	if(operation->operationClass != TEE_OPERATION_CIPHER) {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return(TEEC_ERROR_BAD_PARAMETERS);
	}
	switch(operation->algorithm) {
		case TEE_ALG_AES_ECB_NOPAD:
		case TEE_ALG_AES_CBC_NOPAD:
		case TEE_ALG_AES_CTR:
		case TEE_ALG_AES_XTS:
		case TEE_ALG_DES_ECB_NOPAD:
		case TEE_ALG_DES_CBC_NOPAD:
		case TEE_ALG_DES3_ECB_NOPAD:
		case TEE_ALG_DES3_CBC_NOPAD:
			retVal = ((operation->mode == TEE_MODE_ENCRYPT) ?
			EVP_EncryptUpdate((EVP_CIPHER_CTX*)(operation->crypto_ctxt),
			destData,(int*)destLen,srcData,srcLen):
			EVP_DecryptUpdate((EVP_CIPHER_CTX*)(operation->crypto_ctxt),
			destData,(int*)destLen,srcData,srcLen));
			if(!retVal) {
				return(TEEC_ERROR_GENERIC);
			}
			break;
		case TEE_ALG_AES_CTS:
			(operation->mode == TEE_MODE_ENCRYPT) ?
			CRYPTO_cts128_encrypt_block(srcData,destData,srcLen,
			operation->keyValue,operation->IV,
			(block128_f)AES_encrypt):
			CRYPTO_cts128_decrypt_block(srcData,destData,srcLen,
			operation->keyValue,operation->IV,
			(block128_f)AES_encrypt);
			*destLen = srcLen;
		break;
		default:
			break;
	}
	return(TEEC_SUCCESS);
}


/**
* @brief 
*
* @param operation
* @param srcData
* @param srcLen
* @param destData
* @param destLen
*
* @return 
*/
TEEC_Result TEE_CipherDoFinal(TEE_OperationHandle operation,
		void* srcData, size_t srcLen,void* destData, size_t *destLen)
{
	u8 retVal = 0;

	if(operation == NULL) {
		return(TEEC_ERROR_BAD_PARAMETERS);
	}
	if(operation->operationClass != TEE_OPERATION_CIPHER) {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return(TEEC_ERROR_BAD_PARAMETERS);
	}
	switch(operation->algorithm) {
		case TEE_ALG_AES_ECB_NOPAD:
		case TEE_ALG_AES_CBC_NOPAD:
		case TEE_ALG_AES_CTR:
		case TEE_ALG_AES_XTS:
		case TEE_ALG_DES_ECB_NOPAD:
		case TEE_ALG_DES_CBC_NOPAD:
		case TEE_ALG_DES3_ECB_NOPAD:
		case TEE_ALG_DES3_CBC_NOPAD:
			if(operation->mode == TEE_MODE_ENCRYPT) {
				if(srcLen != 0) {
					if(!EVP_EncryptUpdate((EVP_CIPHER_CTX*)(operation->crypto_ctxt),
						destData,(int*)destLen,srcData,srcLen)) {
						return(TEEC_ERROR_GENERIC);
					}
				}
				if(!EVP_EncryptFinal_ex((EVP_CIPHER_CTX*)(operation->crypto_ctxt),
						destData,(int*)destLen)) {
					return(TEEC_ERROR_GENERIC);
				}
			} else if(operation->mode == TEE_MODE_DECRYPT) {
				if(srcLen != 0) {
					if(!EVP_DecryptUpdate((EVP_CIPHER_CTX*)(operation->crypto_ctxt),
					destData,(int*)destLen,srcData,srcLen)) {
						return(TEEC_ERROR_GENERIC);
					}
				}
				if(!EVP_DecryptFinal_ex((EVP_CIPHER_CTX*)(operation->crypto_ctxt),							destData,(int*)destLen)) {
					return(TEEC_ERROR_GENERIC);
				}
			}
			break;
		case TEE_ALG_AES_CTS:
			if(srcLen != 0) {
				if(operation->mode == TEE_MODE_ENCRYPT) {
					CRYPTO_cts128_encrypt_block(srcData,destData,srcLen,
					operation->keyValue,operation->IV,
					(block128_f)AES_encrypt);
				} else if(operation->mode == TEE_MODE_DECRYPT) {
					CRYPTO_cts128_decrypt_block(srcData,destData,srcLen,
					operation->keyValue,operation->IV,
					(block128_f)AES_encrypt);
				}
				*destLen = srcLen;
			}
			break;
		default:
			break;
	}
	return(TEEC_SUCCESS);
}


/**
* @brief 
*
* @param operation
* @param IV
* @param IVLen
*/
void TEE_MACInit(TEE_OperationHandle operation,void* IV, size_t IVLen)
{
	if(operation == NULL) {
		return;
	}
	if(operation->operationClass != TEE_OPERATION_MAC) {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return;
	}
	if(IV == NULL) {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return;
	}
	switch(operation->algorithm) {
		case TEE_ALG_HMAC_MD5:
			if(!HMAC_Init((HMAC_CTX*)(operation->crypto_ctxt),IV,IVLen,EVP_md5())) {
				return;
			}
			break;
		case TEE_ALG_HMAC_SHA1:
			if(!HMAC_Init((HMAC_CTX*)(operation->crypto_ctxt),IV,IVLen,EVP_sha1())) {
				return;
			}
			break;
		case TEE_ALG_HMAC_SHA224:
			if(!HMAC_Init((HMAC_CTX*)(operation->crypto_ctxt),IV,IVLen,EVP_sha224())) {
				return;
			}
			break;
		case TEE_ALG_HMAC_SHA256:
			if(!HMAC_Init((HMAC_CTX*)(operation->crypto_ctxt),IV,IVLen,EVP_sha256())) {
				return;
			}
			break;
		case TEE_ALG_HMAC_SHA384:
			if(!HMAC_Init((HMAC_CTX*)(operation->crypto_ctxt),IV,IVLen,EVP_sha384())) {
				return;
			}
			break;
		case TEE_ALG_HMAC_SHA512:
			if(!HMAC_Init((HMAC_CTX*)(operation->crypto_ctxt),IV,IVLen,EVP_sha512())) {
				return;
			}
			break;
		default:
			break;
	}
	return;
}


/**
* @brief 
*
* @param operation
* @param chunk
* @param chunkSize
*/
void TEE_MACUpdate(TEE_OperationHandle operation,void* chunk, size_t chunkSize)
{
	if(operation == NULL) {
		return;
	}
	if(operation->operationClass != TEE_OPERATION_MAC) {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return;
	}
	switch(operation->algorithm) {
		case TEE_ALG_HMAC_MD5:
		case TEE_ALG_HMAC_SHA1:
		case TEE_ALG_HMAC_SHA224:
		case TEE_ALG_HMAC_SHA256:
		case TEE_ALG_HMAC_SHA384:
		case TEE_ALG_HMAC_SHA512:
			if(!HMAC_Update((HMAC_CTX*)operation->crypto_ctxt,chunk,chunkSize)) {
				return;
			}
			break;
		default:
			break;
	}
	return;
}


/**
* @brief 
*
* @param operation
* @param message
* @param messageLen
* @param mac
* @param macLen
*
* @return 
*/
TEEC_Result TEE_MACComputeFinal(TEE_OperationHandle operation,
		void* message, size_t messageLen,void* mac, size_t *macLen)
{
	if(operation == NULL) {
		return(TEEC_ERROR_BAD_PARAMETERS);
	}
	if(operation->operationClass != TEE_OPERATION_MAC) {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return(TEEC_ERROR_BAD_PARAMETERS);
	}
	switch(operation->algorithm) {
		case TEE_ALG_HMAC_MD5:
		case TEE_ALG_HMAC_SHA1:
		case TEE_ALG_HMAC_SHA224:
		case TEE_ALG_HMAC_SHA256:
		case TEE_ALG_HMAC_SHA384:
		case TEE_ALG_HMAC_SHA512:
			if(*macLen < EVP_MAX_MD_SIZE) {
				return(TEEC_ERROR_SHORT_BUFFER);
			}
			if(messageLen > 0) {
				if(!HMAC_Update((HMAC_CTX*)operation->crypto_ctxt,message,
																messageLen)) {
					return(TEEC_ERROR_BAD_PARAMETERS);
				}
			}
			if(!HMAC_Final((HMAC_CTX*)operation->crypto_ctxt,mac,macLen)) {
				return(TEEC_ERROR_BAD_PARAMETERS);
			}
			break;
		default:
			break;
	}
	return(TEEC_SUCCESS);
}

/**
* @brief 
*
* @param operation
* @param message
* @param messageLen
* @param mac
* @param macLen
*
* @return 
*/
TEEC_Result TEE_MACCompareFinal(TEE_OperationHandle operation,
		void* message, size_t messageLen,void* mac, size_t *macLen)
{
	u8 tempOutput[EVP_MAX_MD_SIZE+1];
	size_t tempOutputLen = EVP_MAX_MD_SIZE;

	if(operation == NULL) {
		return(TEEC_ERROR_BAD_PARAMETERS);
	}
	if(operation->operationClass != TEE_OPERATION_MAC) {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return(TEEC_ERROR_BAD_PARAMETERS);
	}
	switch(operation->algorithm) {
		case TEE_ALG_HMAC_MD5:
		case TEE_ALG_HMAC_SHA1:
		case TEE_ALG_HMAC_SHA224:
		case TEE_ALG_HMAC_SHA256:
		case TEE_ALG_HMAC_SHA384:
		case TEE_ALG_HMAC_SHA512:
			if(messageLen > 0) {
				if(!HMAC_Update((HMAC_CTX*)operation->crypto_ctxt,
					message,messageLen)) {
					return(TEEC_ERROR_BAD_PARAMETERS);
				}
			}
			if(!HMAC_Final((HMAC_CTX*)operation->crypto_ctxt,
				(void*)tempOutput,&tempOutputLen)) {
				return(TEEC_ERROR_BAD_PARAMETERS);
			}
			if(*macLen != tempOutputLen) {
				return(TEEC_ERROR_MAC_INVALID);
			}
			if(sw_memcmp(tempOutput,mac,*macLen) != 0) {
				return(TEEC_ERROR_MAC_INVALID);
			}
			break;
		default:
			break;
	}
	return(TEEC_SUCCESS);
}


/**
* @brief 
*
* @param operation
* @param nonce
* @param nonceLen
* @param tagLen
* @param AADLen
* @param payloadLen
*
* @return 
*/
TEEC_Result TEE_AEInit(TEE_OperationHandle operation,
				void* nonce, size_t nonceLen, uint32_t tagLen,
				uint32_t AADLen, uint32_t payloadLen)
{
	if(operation == NULL) {
		return(TEEC_ERROR_BAD_PARAMETERS);
	}
	if(operation->operationClass != TEE_OPERATION_AE) {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return(TEEC_ERROR_BAD_PARAMETERS);
	}
	if(operation->keyValue == NULL) {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return(TEEC_ERROR_BAD_PARAMETERS);
	}
	if(operation->algorithm == TEE_ALG_AES_GCM) {
		switch(tagLen) {
			case TEE_TAG_LEN_96:
			case TEE_TAG_LEN_104:
			case TEE_TAG_LEN_112:
			case TEE_TAG_LEN_120:
			case TEE_TAG_LEN_128:
				break;
			default:
				return(TEEC_ERROR_NOT_SUPPORTED);
		}
		CRYPTO_gcm128_init(operation->crypto_ctxt,operation->keyValue,
				(block128_f)AES_encrypt);
		CRYPTO_gcm128_setiv(operation->crypto_ctxt,nonce,nonceLen);
	}
	if(operation->algorithm == TEE_ALG_AES_CCM) {
		switch(tagLen) {
			case TEE_TAG_LEN_32:
			case TEE_TAG_LEN_48:
			case TEE_TAG_LEN_64:
			case TEE_TAG_LEN_96:
			case TEE_TAG_LEN_112:
			case TEE_TAG_LEN_128:
				break;
			default:
				return(TEEC_ERROR_NOT_SUPPORTED);
		}
		CRYPTO_ccm128_init(operation->crypto_ctxt,nonceLen,
					payloadLen,operation->keyValue,
					(block128_f)AES_encrypt);
		CRYPTO_ccm128_setiv(operation->crypto_ctxt,nonce,nonceLen,payloadLen);
	}
	operation->aadLen = AADLen;
	return(TEEC_SUCCESS);
}

/**
* @brief 
*
* @param operation
* @param AADdata
* @param AADdataLen
*/
void TEE_AEUpdateAAD(TEE_OperationHandle operation,
				void* AADdata, size_t AADdataLen)
{
	if(operation == NULL) {
		return;
	}
	if(operation->operationClass != TEE_OPERATION_AE) {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return;
	}
	/*if((operation->keySize + AADdataLen) > operation->maxKeySize) {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return;
	}*/
	if(operation->algorithm == TEE_ALG_AES_GCM) {
		CRYPTO_gcm128_aad(operation->crypto_ctxt,AADdata,AADdataLen);
	}
	if(operation->algorithm == TEE_ALG_AES_CCM) {
		CRYPTO_ccm128_aad(operation->crypto_ctxt,AADdata,AADdataLen);
	}
	return;
}

/**
* @brief 
*
* @param operation
* @param srcData
* @param srcLen
* @param destData
* @param destLen
*
* @return 
*/
TEEC_Result TEE_AEUpdate(TEE_OperationHandle operation,void* srcData,
			size_t srcLen,void* destData, size_t *destLen)
{
	if(operation == NULL) {
		return(TEEC_ERROR_BAD_PARAMETERS);
	}
	if(operation->operationClass != TEE_OPERATION_AE) {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return(TEEC_ERROR_BAD_PARAMETERS);
	}
	if(*destLen < srcLen) {
		return(TEEC_ERROR_SHORT_BUFFER);
	}
	if(operation->algorithm == TEE_ALG_AES_CCM) {
		if(operation->mode == TEE_MODE_DECRYPT) {
			CRYPTO_ccm128_decrypt(operation->crypto_ctxt,srcData,destData,*destLen);
		}
		if(operation->mode == TEE_MODE_ENCRYPT) {
			CRYPTO_ccm128_encrypt(operation->crypto_ctxt,srcData,destData,*destLen);
		}
	}
	if(operation->algorithm == TEE_ALG_AES_GCM) {
		if(operation->mode == TEE_MODE_DECRYPT) {
			CRYPTO_gcm128_decrypt(operation->crypto_ctxt,srcData,destData,*destLen);
		}
		if(operation->mode == TEE_MODE_ENCRYPT) {
			CRYPTO_gcm128_encrypt(operation->crypto_ctxt,srcData,destData,*destLen);
		}
	}
	return(TEEC_SUCCESS);
}

/**
* @brief 
*
* @param operation
* @param srcData
* @param srcLen
* @param destData
* @param destLen
* @param tag
* @param tagLen
*
* @return 
*/
TEEC_Result TEE_AEEncryptFinal(TEE_OperationHandle operation,
			void* srcData, size_t srcLen,void* destData, size_t* destLen,
				void* tag,size_t* tagLen)
{
	if(operation == NULL) {
		return(TEEC_ERROR_BAD_PARAMETERS);
	}
	if(operation->operationClass != TEE_OPERATION_AE) {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return(TEEC_ERROR_BAD_PARAMETERS);
	}
	if((srcLen > 0) && (*destLen < srcLen)) {
		return(TEEC_ERROR_SHORT_BUFFER);
	}
	if((operation->algorithm == TEE_ALG_AES_CCM) && (srcLen != 0)) {
		CRYPTO_ccm128_encrypt(operation->crypto_ctxt,srcData,destData,*destLen);
	}
	if(operation->algorithm == TEE_ALG_AES_GCM) {
		if(srcLen != 0) {
			CRYPTO_gcm128_encrypt(operation->crypto_ctxt,srcData,destData,*destLen);
		}
		CRYPTO_gcm128_finish(operation->crypto_ctxt,tag,*tagLen);
	}
	return(TEEC_SUCCESS);
}


/**
* @brief 
*
* @param operation
* @param srcData
* @param srcLen
* @param destData
* @param destLen
* @param tag
* @param tagLen
*
* @return 
*/
TEEC_Result TEE_AEDecryptFinal(TEE_OperationHandle operation, void* srcData,
				size_t srcLen, void* destData, size_t *destLen,
						void* tag, size_t tagLen)
{
	if(operation == NULL) {
		return(TEEC_ERROR_BAD_PARAMETERS);
	}
	if(operation->operationClass != TEE_OPERATION_AE) {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return(TEEC_ERROR_BAD_PARAMETERS);
	}
	if((srcLen > 0) && (*destLen < srcLen)) {
		return(TEEC_ERROR_SHORT_BUFFER);
	}
	if((operation->algorithm == TEE_ALG_AES_CCM) && (srcLen != 0)) {
		CRYPTO_ccm128_decrypt(operation->crypto_ctxt,srcData,destData,*destLen);
	}
	if(operation->algorithm == TEE_ALG_AES_GCM) {
		if(srcLen != 0) {
			CRYPTO_gcm128_decrypt(operation->crypto_ctxt,srcData,destData,*destLen);
		}
		CRYPTO_gcm128_finish((GCM128_CONTEXT*)operation->crypto_ctxt,tag,tagLen);
	}
	return(TEEC_SUCCESS);
}


/**
* @brief 
*
* @param operation
* @param params
* @param paramCount
* @param srcData
* @param srcLen
* @param destData
* @param destLen
*
* @return 
*/
TEEC_Result TEE_AsymmetricEncrypt(TEE_OperationHandle operation,
		TEE_Attribute* params, uint32_t paramCount, void* srcData,
		size_t srcLen, void* destData, size_t *destLen)
{
	if(operation == NULL) {
		return(TEEC_ERROR_BAD_PARAMETERS);
	}
	if(operation->operationClass != TEE_OPERATION_ASYMMETRIC_CIPHER) {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return(TEEC_ERROR_BAD_PARAMETERS);
	}
	switch(operation->algorithm) {
		case TEE_ALG_RSAES_PKCS1_V1_5:
			if(params->attributeID ==  SW_RSA_USE_PRIVATE) {
				*destLen = RSA_private_encrypt(srcLen,srcData,destData,
				operation->keyValue,RSA_PKCS1_PADDING);
				if(*destLen == 0) {
					return(TEEC_ERROR_NO_DATA);
				}
			} else {
				*destLen = RSA_public_encrypt(srcLen,srcData,destData,
				operation->keyValue,RSA_PKCS1_PADDING);
				if(*destLen == 0) {
					return(TEEC_ERROR_NO_DATA);
				}
			}
			break;
		case TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA1:
			break;
		case TEE_ALG_RSA_NOPAD:
			if(params->attributeID == SW_RSA_USE_PRIVATE) {
				*destLen = RSA_private_encrypt(srcLen,srcData,destData,
				operation->keyValue,RSA_NO_PADDING);
				if(*destLen == 0) {
					return(TEEC_ERROR_NO_DATA);
				}
			} else {
				*destLen = RSA_public_encrypt(srcLen,srcData,destData,
				operation->keyValue,RSA_NO_PADDING);
				if(*destLen == 0) {
					return(TEEC_ERROR_NO_DATA);
				}
			}
			break;
		default:
			return(TEEC_ERROR_BAD_PARAMETERS);
	}
	return(TEEC_SUCCESS);
}


/**
* @brief 
*
* @param operation
* @param params
* @param paramCount
* @param srcData
* @param srcLen
* @param destData
* @param destLen
*
* @return 
*/
TEEC_Result TEE_AsymmetricDecrypt(TEE_OperationHandle operation,
		TEE_Attribute* params, uint32_t paramCount, void* srcData,
			size_t srcLen, void* destData, size_t *destLen)
{
	if(operation == NULL) {
		return(TEEC_ERROR_BAD_PARAMETERS);
	}
	if(operation->operationClass != TEE_OPERATION_ASYMMETRIC_CIPHER) {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return(TEEC_ERROR_BAD_PARAMETERS);
	}
	switch(operation->algorithm) {
		case TEE_ALG_RSAES_PKCS1_V1_5:
			if(params->attributeID == SW_RSA_USE_PUBLIC) {
				*destLen = RSA_public_decrypt(srcLen,srcData,destData,
				operation->keyValue,RSA_PKCS1_PADDING);
				if(*destLen == 0) {
					return(TEEC_ERROR_NO_DATA);
				}
			} else {
				*destLen = RSA_private_decrypt(srcLen,srcData,destData,
				operation->keyValue,RSA_PKCS1_PADDING);
				if(*destLen == 0) {
					return(TEEC_ERROR_NO_DATA);
				}
			}
			break;
		case TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA1:
			break;
		case TEE_ALG_RSA_NOPAD:
			if(params->attributeID == SW_RSA_USE_PUBLIC) {
				*destLen = RSA_public_decrypt(srcLen,srcData,destData,
				operation->keyValue,RSA_NO_PADDING);
				if(*destLen == 0) {
					return(TEEC_ERROR_NO_DATA);
				}
			} else {
				*destLen = RSA_private_decrypt(srcLen,srcData,destData,
				operation->keyValue,RSA_NO_PADDING);
				if(*destLen == 0) {
					return(TEEC_ERROR_NO_DATA);
				}
			}
			break;
		default:
			return(TEEC_ERROR_BAD_PARAMETERS);
	}
	return(TEEC_SUCCESS);
}


/**
* @brief 
*
* @param operation
* @param params
* @param paramCount
* @param digest
* @param digestLen
* @param signature
* @param signatureLen
*
* @return 
*/
TEEC_Result TEE_AsymmetricSignDigest(TEE_OperationHandle operation,
		TEE_Attribute* params, uint32_t paramCount, void* digest,
		size_t digestLen, void* signature, size_t *signatureLen)
{

	if(operation == NULL) {
		return(TEEC_ERROR_BAD_PARAMETERS);
	}
	if(operation->operationClass != TEE_OPERATION_ASYMMETRIC_SIGNATURE) {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return(TEEC_ERROR_BAD_PARAMETERS);
	}
	switch(operation->algorithm) {
		case TEE_ALG_RSASSA_PKCS1_V1_5_MD5:
			if(RSA_sign(NID_md5,digest,digestLen,signature,
				signatureLen,operation->keyValue) == 0) {
				return(TEEC_ERROR_BAD_STATE);
			}
			break;
		case TEE_ALG_RSASSA_PKCS1_V1_5_SHA1:
			if(RSA_sign(NID_sha1,digest,digestLen,signature,
				signatureLen,operation->keyValue) == 0) {
				return(TEEC_ERROR_BAD_STATE);
			}
			break;
		case TEE_ALG_RSASSA_PKCS1_V1_5_SHA224:
		case TEE_ALG_RSASSA_PKCS1_V1_5_SHA256:
		case TEE_ALG_RSASSA_PKCS1_V1_5_SHA384:
		case TEE_ALG_RSASSA_PKCS1_V1_5_SHA512:
		case TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA1:
		case TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA224:
		case TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA256:
		case TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA384:
		case TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA512:
			break;
		case TEE_ALG_DSA_SHA1:
			if(DSA_sign(NID_sha1,digest,digestLen,signature,
				signatureLen,operation->keyValue) == 0) {
				return(TEEC_ERROR_BAD_STATE);
			}
			break;
			break;
		default:
			return(TEEC_ERROR_BAD_PARAMETERS);
	}
	return(TEEC_SUCCESS);
}

/**
* @brief 
*
* @param operation
* @param params
* @param paramCount
* @param digest
* @param digestLen
* @param signature
* @param signatureLen
*
* @return 
*/
TEEC_Result TEE_AsymmetricVerifyDigest(TEE_OperationHandle operation,
		TEE_Attribute* params, uint32_t paramCount, void* digest,
		size_t digestLen, void* signature, size_t signatureLen)
{

	if(operation == NULL) {
		return(TEEC_ERROR_BAD_PARAMETERS);
	}
	if(operation->operationClass != TEE_OPERATION_ASYMMETRIC_SIGNATURE) {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return(TEEC_ERROR_BAD_PARAMETERS);
	}
	switch(operation->algorithm) {
		case TEE_ALG_RSASSA_PKCS1_V1_5_MD5:
			if(RSA_verify(NID_md5,digest,digestLen,signature,
			signatureLen,operation->keyValue) == 0) {
				return(TEEC_ERROR_BAD_STATE);
			}
			break;
		case TEE_ALG_RSASSA_PKCS1_V1_5_SHA1:
			if(RSA_verify(NID_sha1,digest,digestLen,signature,
			signatureLen,operation->keyValue) == 0) {
				return(TEEC_ERROR_BAD_STATE);
			}
			break;
		case TEE_ALG_RSASSA_PKCS1_V1_5_SHA224:
		case TEE_ALG_RSASSA_PKCS1_V1_5_SHA256:
		case TEE_ALG_RSASSA_PKCS1_V1_5_SHA384:
		case TEE_ALG_RSASSA_PKCS1_V1_5_SHA512:
		case TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA1:
		case TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA224:
		case TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA256:
		case TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA384:
		case TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA512:
			break;
		case TEE_ALG_DSA_SHA1:
			if(DSA_verify(NID_sha1,digest,digestLen,signature,
			signatureLen,operation->keyValue) == 0) {
				return(TEEC_ERROR_BAD_STATE);
			}
			break;
		default:
			return(TEEC_ERROR_BAD_PARAMETERS);
	}
	return(TEEC_SUCCESS);
}

/**
* @brief 
*
* @param operation
* @param params
* @param paramCount
* @param derivedKey
*/
void TEE_DeriveKey(TEE_OperationHandle operation, TEE_Attribute* params,
			uint32_t paramCount, TEE_ObjectHandle* derivedKey)
{
	if(operation == NULL) {
		return;
	}
	if(operation->operationClass != TEE_OPERATION_KEY_DERIVATION) {
		TEE_Panic(TEEC_ERROR_BAD_PARAMETERS);
		return;
	}
	if(operation->algorithm != TEE_ALG_DH_DERIVE_SHARED_SECRET) {
		return;
	}
}

/**
* @brief 
*
* @param randomBuffer
* @param randomBufferLen
*/
void TEE_GenerateRandom(void* randomBuffer, size_t randomBufferLen)
{
	return;
}

