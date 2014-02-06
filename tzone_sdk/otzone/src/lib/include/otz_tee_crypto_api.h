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
 * Header file for global platform TEE crypto API
 */

#ifndef OTZ_TEE_CRYPTO_API_H
#define OTZ_TEE_CRYPTO_API_H


#include "otz_api.h"
#include "otz_tee_api.h"


/**
 * @brief This is the size of the key that would be used for RSA
 */
#define SW_RSA_KEYLEN 1024

#define ALGORITHM_NAME_LENGTH 128

/**
 * @brief This is the size of the key that would be used for DSA
 */
#define SW_DSA_KEYLEN 1024

/**
 * @brief Definition of some truth values.
 */
#define SW_CRYPTO_TRUE 1
#define SW_CRYPTO_FALSE 0

/*
 * @brief Definitions for using public or private key encryption for RSA
 */
#define SW_RSA_USE_PUBLIC 1
#define SW_RSA_USE_PRIVATE 2

/*
 * @brief defines for maximum key length values of various algorithms
 */
#define AES_128_CBC_MAX_KEY_LEN 16
/**
 * @brief Typedefs for various crypto operation constants
 */
enum __TEE_Operation_Constants {
	TEE_OPERATION_CIPHER = 0x1,
	TEE_OPERATION_MAC,
	TEE_OPERATION_AE,
	TEE_OPERATION_DIGEST,
	TEE_OPERATION_ASYMMETRIC_CIPHER,
	TEE_OPERATION_ASYMMETRIC_SIGNATURE,
	TEE_OPERATION_KEY_DERIVATION
};

typedef enum __TEE_Operation_Constants TEE_Operation_Constants;

/**
 * @brief Valid tag lengths for Asymmetric cipher encryption
 */
enum __TEE_Valid_Tag_Lengths {
	TEE_TAG_LEN_32 = 32,
	TEE_TAG_LEN_48 = 48,
	TEE_TAG_LEN_64 = 64,
	TEE_TAG_LEN_96 = 96,
	TEE_TAG_LEN_104 = 104,
	TEE_TAG_LEN_112 = 112,
	TEE_TAG_LEN_120 = 120,
	TEE_TAG_LEN_128 = 128,
};

typedef enum __TEE_Valid_Tag_Lengths TEE_Valid_Tag_Lengths;

/**
 * @brief Typedefs for various crypto operation modes
 */
enum __TEE_OperationMode {
	TEE_MODE_ENCRYPT = 0x0,
	TEE_MODE_DECRYPT,
	TEE_MODE_SIGN,
	TEE_MODE_VERIFY,
	TEE_MODE_MAC,
	TEE_MODE_DIGEST,
	TEE_MODE_DERIVE
} ;

typedef enum __TEE_OperationMode TEE_OperationMode;

/*
 * @brief A structure that gives information regarding the
 *        cryptographic operation
 */
struct __TEE_OperationInfo {
	uint32_t algorithm;
	uint32_t operationClass;
	uint32_t mode;
	uint32_t digestLength;
	uint32_t maxKeySize;
	uint32_t keySize;
	uint32_t requiredKeyUsage;
	uint32_t handleState;
	void *keyValue;
} ;

typedef struct __TEE_OperationInfo TEE_OperationInfo;

/**
 * @brief opaque structure definition for an operation handle.
 * It contains almost all the elements of operation info structure
 * and if possible few other elements for book-keeping purposes.
 */
struct __TEE_OperationHandle {
	uint32_t algorithm;
	uint32_t operationClass;
	uint32_t mode;
	uint32_t digestLength;
	uint32_t maxKeySize;
	uint32_t keySize;
	uint32_t requiredKeyUsage;
	uint32_t handleState;
	void *keyValue;
	void *crypto_ctxt;
	void *IV;
	void *crypto_cipher;
	uint32_t IVLen;
	uint32_t aadLen;
};

typedef struct __TEE_OperationHandle* TEE_OperationHandle;
typedef struct __TEE_OperationHandle TEE_OperationHandleVar;

typedef struct __TEE_ObjectHandle TEE_ObjectHandleVar;


/*
 * @brief An opaque structure that provides information regarding the current
 * 		  cryptographic operation
 */
/*typedef struct {
	uint32_t algorithm;
} __TEE_OperationHandle;

__TEE_OperationHandle* TEEOperationHandle;
*/

/*typedef struct {
	uint32_t algorithm;
} __TEE_OperationHandle* TEE_OperationHandle;*/


/* The value 0x60210230 is being used by all these algorithm types.
 * Hence we introduce an #define for these so that we can use them in
 * programs
 */
#define TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA224 TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA1
#define TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA256 TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA1
#define TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA384 TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA1
#define TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA512 TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA1

/*
 * @brief An enumeration of all possible ID's for algorithm values.
 */
enum __TEE_CRYPTO_ALGORITHM_ID {
	TEE_ALG_AES_ECB_NOPAD =  0x10000010,
	TEE_ALG_AES_CBC_NOPAD = 0x10000110,
	TEE_ALG_AES_CTR = 0x10000210,
	TEE_ALG_AES_CTS = 0x10000310,
	TEE_ALG_AES_XTS = 0x10000410,
	TEE_ALG_AES_CBC_MAC_NOPAD = 0x30000110,
	TEE_ALG_AES_CBC_MAC_PKCS5 = 0x30000510,
	TEE_ALG_AES_CMAC = 0x30000610,
	TEE_ALG_AES_CCM = 0x40000710,
	TEE_ALG_AES_GCM = 0x40000810,
	TEE_ALG_DES_ECB_NOPAD = 0x10000011,
	TEE_ALG_DES_CBC_NOPAD = 0x10000111,
	TEE_ALG_DES_CBC_MAC_NOPAD = 0x30000111,
	TEE_ALG_DES_CBC_MAC_PKCS5 = 0x30000511,
	TEE_ALG_DES3_ECB_NOPAD = 0x10000013,
	TEE_ALG_DES3_CBC_NOPAD = 0x10000113,
	TEE_ALG_DES3_CBC_MAC_NOPAD = 0x30000113,
	TEE_ALG_DES3_CBC_MAC_PKCS5 = 0x30000513,
	TEE_ALG_RSASSA_PKCS1_V1_5_MD5 = 0x70001830,
	TEE_ALG_RSASSA_PKCS1_V1_5_SHA1 = 0x70002830,
	TEE_ALG_RSASSA_PKCS1_V1_5_SHA224 = 0x70003830,
	TEE_ALG_RSASSA_PKCS1_V1_5_SHA256 = 0x70004830,
	TEE_ALG_RSASSA_PKCS1_V1_5_SHA384 = 0x70005830,
	TEE_ALG_RSASSA_PKCS1_V1_5_SHA512 = 0x70006830,
	TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA1 = 0x70212930,
	TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA224 = 0x70313930,
	TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA256 = 0x70414930,
	TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA384 = 0x70515930,
	TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA512 = 0x70616930,
	TEE_ALG_RSAES_PKCS1_V1_5 = 0x60000130,
	TEE_ALG_RSAES_PKCS1_OAEP_MGF1_SHA1 = 0x60210230,
	TEE_ALG_RSA_NOPAD = 0x60000030,
	TEE_ALG_DSA_SHA1 = 0x70002131,
	TEE_ALG_DH_DERIVE_SHARED_SECRET = 0x80000032,
	TEE_ALG_MD5 = 0x50000001,
	TEE_ALG_SHA1 = 0x50000002,
	TEE_ALG_SHA224 = 0x50000003,
	TEE_ALG_SHA256 = 0x50000004,
	TEE_ALG_SHA384 = 0x50000005,
	TEE_ALG_SHA512 = 0x50000006,
	TEE_ALG_HMAC_MD5 = 0x30000001,
	TEE_ALG_HMAC_SHA1 = 0x30000002,
	TEE_ALG_HMAC_SHA224 = 0x30000003,
	TEE_ALG_HMAC_SHA256 = 0x30000004,
	TEE_ALG_HMAC_SHA384 = 0x30000005,
	TEE_ALG_HMAC_SHA512 = 0x30000006
} ;

typedef enum __TEE_CRYPTO_ALGORITHM_ID TEE_CRYPTO_ALGORITHM_ID;

/**
* @brief 
*
* @param algorithm
*
* @return 
*/
static int check_valid_algorithm(uint32_t algorithm);

/**
* @brief 
*
* @param mode
*
* @return 
*/
static int check_valid_mode(uint32_t mode);

/**
* @brief 
*
* @param mode
* @param algorithm
*
* @return 
*/
static int check_valid_algorithm_for_mode(uint32_t mode, uint32_t algorithm);

/**
* @brief 
*
* @param localOperationHandle
*
* @return 
*/
static TEEC_Result handleDigestAlloc(TEE_OperationHandle localOperationHandle);

/**
* @brief 
*
* @param localOperationHandle
*
* @return 
*/
static TEEC_Result handleMacAlloc(TEE_OperationHandle localOperationHandle);

/**
* @brief 
*
* @param localOperationHandle
*
* @return 
*/
static TEEC_Result handleCipherAlloc(TEE_OperationHandle localOperationHandle);

/**
* @brief 
*
* @param operation
*/
static void handleDigestFree(TEE_OperationHandle operation);

/**
* @brief 
*
* @param operation
*/
static void handleMacFree(TEE_OperationHandle operation);

/**
* @brief 
*
* @param operation
*/
static void handleCipherFree(TEE_OperationHandle operation);

/**
* @brief 
*
* @param operation
* @param key
*
* @return 
*/
static TEEC_Result fillRSAKeys(TEE_OperationHandle operation,
														TEE_ObjectHandle* key);

/*
 * @brief This function allocates a handle for a new cryptographic operation
 *
 * @param TEE_OperationHandle - which has the details about the current
 * 			cryptographic operation
 * @param algorithm - an integer that represents the algorithm, specified in the
 * 		  enumeration, TEE_CRYPTO_ALGORITHM_ID
 * @param mode - The mode for the current operation, as specified by the
 * 		  enumeration TEE_OperationMode
 * @param maxKeySize - The maximum keysize that is in use by the algorithm.
 *
 * @return TEEC_Result which signifies the errors, if any, associated with
 * 			this operation
 */
TEEC_Result TEE_AllocateOperation(TEE_OperationHandle *operation,
					uint32_t algorithm, uint32_t mode,uint32_t maxKeySize);

/*
 * @brief This function frees the resources associated with the handle.
 *
 * @param A pointer to the sturcture operation handle.
 */
void TEE_FreeOperation(TEE_OperationHandle operation);

/*
 * @brief This function fills in the operationinfo structure associated with
 * an operation.
 *
 * @param operation - A variable of type operationhandle.
 *
 * @param operationInfo - A pointer to a variable of type operationinfo, into
 * 			which the data is filled and returned back.
 */
void TEE_GetOperationInfo(TEE_OperationHandle operation,
				TEE_OperationInfo* operationInfo);

/*
 * @brief This function resets the operation state, and is meaningful only
 * 		  multi-stage crypto operations like symmetric ciphers.
 *
 *  @param operation - A variable of type operation handle.
 *
 */
void TEE_ResetOperation(TEE_OperationHandle operation);


/*
 * @brief This function associates an operation with a specific key
 *
 * @param  operation - A variable for type operation handle
 * @param	key    - A variable of type object handle
 *
 * @return TEEC_Result - A variable that must return TEEC_SUCCESS for now.
 * 						Future versions may return a different variable.
 */

TEEC_Result TEE_SetOperationKey(TEE_OperationHandle operation,
														TEE_ObjectHandle* key);

/*
 *@brief This function associates an operation with a specific key, and is used
 *		 only with the algorithm  TEE_ALG_AES_XTS.
 *
 * @param  operation - A variable for type operation handle
 * @param	key    - A variable of type object handle
 *
 * @return TEEC_Result - A variable that must return TEEC_SUCCESS for now.
 * 						Future versions may return a different variable.
 */
TEEC_Result TEE_SetOperationKey2(TEE_OperationHandle operation,
			TEE_ObjectHandle* key1,TEE_ObjectHandle* key2);


/*
 * @brief This function copies the operation handle details from one variable
 * 			to another.
 *
 * 	@param dstOperation - A variable of type operation handle, into which the
 * 						source variable needs to be copied.
 *
 * 	@param srcOperation - A variable of type operation handle, from which the
 * 						data is copied into the destination variable.
 */
void TEE_CopyOperation( TEE_OperationHandle dstOperation,
			TEE_OperationHandle srcOperation);

/*
 * @brief This function accumulates message data for hashing into the operation
 * 		  handle variable.
 *
 * @param operation - A variable of type operation handle into which the message
 * 					  data that needs to hashed, is stored.
 * @param chunk	- The buffer that contains the data
 * @param chunkSize - The size of the incoming buffer.
 */
void TEE_DigestUpdate(TEE_OperationHandle operation,
			void* chunk, size_t chunkSize);


/* @brief This function finalizes the message digest operation and produces
 * 		  the hash of the message. The digest cannot be updated after invoking
 * 		  this function.
 *
 * @param operation - A variable of type operation handle
 * @param chunk - The final buffer that needs to be used for producing the hash
 * @param chunkLen - The size of the final buffer.
 * @param hash - The output buffer into which the hash is stored.
 * @param hashlen - The size of the output buffer.
 * @return TEEC_SUCCESS is the operation succeeded, or  TEEC_ERROR_SHORT_BUFFER
 * 			if the hash cannot be stored into the buffer provided
 */
TEEC_Result TEE_DigestDoFinal(TEE_OperationHandle operation,
		void* chunk, size_t chunkLen,void* hash, size_t *hashLen);

/*
 * @brief This function initializes a symmetric cipher operation
 *
 * @param operation - A variable of type operation handle
 * @param IV - The initialization vector
 * @param IVLen - The length of the initialization vector.
 */
void TEE_CipherInit(TEE_OperationHandle operation,void* IV, size_t IVLen);

/*
 * @brief This function encrypts or decrypts the input data.
 *
 * @param operation - A variable of type operation handle, which stores
 * 					details of the current running cipher operation.
 * @param srcData - The input buffer that is to be encrypted.
 * @param srcLen - The length of the input buffer that needs to be encrypted.
 * @param destData - The output buffer into which the data needs to be stored.
 * @param destLen - The length of the output buffer
 * @return TEEC_SUCCESS is the operation succeeded, or TEEC_ERROR_SHORT_BUFFER
 * 			if the data cannot be stored into the buffer provided
 */
TEEC_Result TEE_CipherUpdate(TEE_OperationHandle operation,
			void* srcData, size_t srcLen,void* destData, size_t *destLen);

/*
 * @brief This function finalizes the cipher operation and encrypts/decrypts
 * 			any remaining data, and the data that is supplied in the source
 * 			buffer
 *
 * @param operation - A variable of type operation handle, which stores
 * 					details of the current running cipher operation.
 * @param srcData - The input buffer that is to be encrypted.
 * @param srcLen - The length of the input buffer that needs to be encrypted.
 * @param destData - The output buffer into which the data needs to be stored.
 * @param destLen - The length of the output buffer
 * @return TEEC_SUCCESS is the operation succeeded, or TEEC_ERROR_SHORT_BUFFER
 * 			if the data cannot be stored into the buffer provided
 */
TEEC_Result TEE_CipherDoFinal(TEE_OperationHandle operation,
		void* srcData, size_t srcLen,void* destData, size_t *destLen);

/*
 * @brief This function initializes a MAC(Message Authentication code) operation
 *
 * @param operation - A variable of type operation handle
 * @param IV - Initialization vector
 * @param IVLen - The length of the initialization vector
 *
 */
void TEE_MACInit(TEE_OperationHandle operation,void* IV, size_t IVLen);

/*
 * @brief This function accumulates data for a MAC calculation
 *
 * @param operation - A variable of type operation handle
 * @param chunk -input message on which MAC calculation needs to be done
 * @param chunkSize - length of the input message
 */
void TEE_MACUpdate(TEE_OperationHandle operation,void* chunk, size_t chunkSize);


/*
 * @brief This function finalizes the computation of MAC. It used any previous
 * 		  existing buffers supplied, and the current input buffer.
 *
 * @param operation - A variable of type operation handle.
 * @param message - The input buffer on which the mac needs to be computed
 * @param messageLen - The length of the input buffer
 * @param mac - The output buffer into which the mac needs to be stored.
 * @param macLen - The length of the output buffer
 * @return TEEC_SUCCESS is the operation succeeded, or TEEC_ERROR_SHORT_BUFFER
 * 			if the data cannot be stored into the buffer provided
 */
TEEC_Result TEE_MACComputeFinal(TEE_OperationHandle operation,
		void* message, size_t messageLen,void* mac, size_t *macLen);

/*
 * @brief This function finalizes the computation of the MAC operation and
 * 		  compares it with the buffer that has been provided.
 *
 * @param operation - A variable of type operation handle.
 * @param message - The input buffer on which the mac needs to be computed
 * @param messageLen - The length of the input buffer
 * @param mac - The input buffer against which the mac needs to be checked
 * @param macLen - The length of the input buffer that has the mac value
 * @return TEEC_SUCCESS is the operation succeeded, or TEEC_ERROR_MAC_INVALID
 * 			if the mac computed is different from the value provided.
 */
TEEC_Result TEE_MACCompareFinal(TEE_OperationHandle operation,
		void* message, size_t messageLen,void* mac, size_t *macLen);

/*
 * @brief This function initializes an Authentication Encryption operation
 *
 * @param operation - A variable of type operation handle.
 * @param nonce - The initialization vector.
 * @param nonceLen - The length of the initialization vector.
 * @param tagLen - The size (in bits) of the tag.
 * @param AADLen - The length of AAD (in bytes)
 * @param payloadLen - Length of the payload (in bytes)
 * @return TEEC_SUCCESS is the operation succeeded, or TEEC_ERROR_NOT_SUPPORTED
 * 			if the tag length is not supported by the algorithm
 */
TEEC_Result TEE_AEInit(TEE_OperationHandle operation,
		void* nonce, size_t nonceLen, uint32_t tagLen,
			uint32_t AADLen, uint32_t payloadLen);

/*
 * @brief This function adds a new chunk of Additional Authentication Data (AAD)
 * 			to the Authentication Encryption operation.
 *
 * @param operation - A variable of type operation handle.
 * @param AAD - The input buffer that contains the AAD
 * @param AADdataLen - The length of the input buffer.
 */
void TEE_AEUpdateAAD(TEE_OperationHandle operation,
			void* AADdata, size_t AADdataLen);

/*
 * @brief This function accumulates data for AE operation
 *
 * @param srcData - The input buffer on which the AE operation needs to be
 *                  performed
 * @param srcLen - The length of the input buffer
 * @param destData - The output buffer into which the result of the
 * 				AE operation needs to be stored.
 * @param destLen - The length of the output buffer
 * @return TEEC_SUCCESS is the operation succeeded, or TEEC_ERROR_SHORT_BUFFER
 * 			if the data cannot be stored into the buffer provided
 */
TEEC_Result TEE_AEUpdate(TEE_OperationHandle operation,void* srcData,
			size_t srcLen,void* destData, size_t *destLen);


/*
 * @brief This function finalizes the computation of the AE encryption
 *
 * @param operation - A variable of type operation handle.
 * @param srcData - The input buffer on which the AE encryption needs to be
 * 					performed
 * @param srcLen - The length of the input buffer
 * @param destData - The output buffer against which the the result of the
 * 			operation needs to be stored.
 * @param destLen - The length of the output buffer
 * @param tag - The buffer into which the computed tag needs to be stored.
 * @param tagLen - The length of the buffer.
 * @return TEEC_SUCCESS is the operation succeeded, or TEEC_ERROR_SHORT_BUFFER
 * 			if the mac computed is different from the value provided.
 */
TEEC_Result TEE_AEEncryptFinal(TEE_OperationHandle operation,
		void* srcData, size_t srcLen,void* destData, size_t* destLen,
		void* tag,size_t* tagLen);

/*
 * @brief This function finalizes the computation of the AE encryption and
 * 			compares the tag that has been computed, with the one provided in
 * 			the parameter.
 *
 * @param operation - A variable of type operation handle.
 * @param srcData - The input buffer on which the AE encryption needs to be
 * 					performed
 * @param srcLen - The length of the input buffer
 * @param destData - The output buffer against which the the result of the
 * 			operation needs to be stored.
 * @param destLen - The length of the output buffer
 * @param tag - The buffer against which the computed tag needs to be compared.
 * @param tagLen - The length of the buffer.
 * @return TEEC_SUCCESS is the operation succeeded, or TEEC_ERROR_SHORT_BUFFER
 * 		if the buffer cannot hold the data computed, or  TEE_ERROR_MAC_INVALID
 * 			if the mac computed is different from the value provided.
 */
TEEC_Result TEE_AEDecryptFinal(TEE_OperationHandle operation, void* srcData,
			size_t srcLen, void* destData, size_t *destLen,
				void* tag, size_t tagLen);

/*
 * @brief This functions performs an encryption on a message within an
 * 			asymmetric operation. Note that this operation is valid only
 * 			on a specific subset of algorithms.
 *
 * 	@param operation - A variable of type operation handle.
 * 	@param params - A variable of type attribute
 * 	@param paramCount - The number of such attributes that have been provided.
 * 	@param srcData - Input buffer that has data to be encrypted.
 * 	@param srcLen - Length of the input buffer
 * 	@param destData - Output buffer into which the data needs to be stored.
 * 	@param destLen - Length of the output buffer.
 * 	@return TEEC_SUCCESS if the operation succeeded, or TEEC_ERROR_SHORT_BUFFER
 * 			if output buffer is not large enough to hold the result.
 */
TEEC_Result TEE_AsymmetricEncrypt(TEE_OperationHandle operation,
			TEE_Attribute* params, uint32_t paramCount, void* srcData,
				size_t srcLen, void* destData, size_t *destLen);

/*
 * @brief This functions performs a decryption on a message within an
 * 			asymmetric operation. Note that this operation is valid only
 * 			on a specific subset of algorithms.
 *
 * 	@param operation - A variable of type operation handle.
 * 	@param params - A variable of type attribute
 * 	@param paramCount - The number of such attributes that have been provided.
 * 	@param srcData - Input buffer that has data to be encrypted.
 * 	@param srcLen - Length of the input buffer
 * 	@param destData - Output buffer into which the data needs to be stored.
 * 	@param destLen - Length of the output buffer.
 * 	@return TEEC_SUCCESS if the operation succeeded, or TEEC_ERROR_SHORT_BUFFER
 * 			if output buffer is not large enough to hold the result.
 */
TEEC_Result TEE_AsymmetricDecrypt(TEE_OperationHandle operation,
		TEE_Attribute* params, uint32_t paramCount, void* srcData,
		size_t srcLen, void* destData, size_t *destLen);

/*
 * @brief This functions signs a message digest within an asymmetric operation.
 *  Note that this operation is valid only on a specific subset of algorithms.
 *
 *
 *  @param operation - A variable of type operation handle.
 *  @param params - A variable of type attribute
 * 	@param paramCount - The number of such attributes that have been provided.
 *	@param diget - Input buffer containing the input digest
 *	@param digestLen - Length of the input digest
 *	@param signature - Output buffer that has the signature of the digest
 *	@param signatureLen - Length of the output buffer provided.
 * 	@return TEEC_SUCCESS if the operation succeeded, or TEEC_ERROR_SHORT_BUFFER
 * 			if output buffer is not large enough to hold the result.
 */
TEEC_Result TEE_AsymmetricSignDigest(TEE_OperationHandle operation,
		TEE_Attribute* params, uint32_t paramCount, void* digest,
		size_t digestLen, void* signature, size_t *signatureLen);

/*
 * @brief This functions signs and verifies message digest within an
 * 		   asymmetric operation. Note that this operation is valid
 * 		   only on a specific subset of algorithms.
 *
 *
 *  @param operation - A variable of type operation handle.
 *  @param params - A variable of type attribute
 * 	@param paramCount - The number of such attributes that have been provided.
 *	@param diget - Input buffer containing the input digest
 *	@param digestLen - Length of the input digest
 *	@param signature - Buffer that has the signature against which the
 *					   verification needs to be done.
 *	@param signatureLen - Length of the signature buffer provided.
 * 	@return TEEC_SUCCESS if the operation succeeded, or TEE_ERROR_SIGNATURE_INVALID
 * 			if the signature verification failed.
 */
TEEC_Result TEE_AsymmetricVerifyDigest(TEE_OperationHandle operation,
		TEE_Attribute* params, uint32_t paramCount, void* digest,
		size_t digestLen, void* signature, size_t signatureLen);

/*
 * @brief This functions derives a key and stores it in the object handle. This
 * 		  function can be used only with the algorithm
 * 		  TEE_ALG_DH_DERIVE_SHARED_SECRET.
 *
 *  @param operation - A variable of type operation handle.
 *  @param params - A variable of type attribute
 * 	@param paramCount - The number of such attributes that have been provided.
 */
void TEE_DeriveKey(TEE_OperationHandle operation, TEE_Attribute* params,
			uint32_t paramCount, TEE_ObjectHandle* derivedKey);

/*
 * @brief This function generated random data
 *
 * @param randomBuffer - The input buffer to be used for generation for
 *        random data
 * @param randomBufferLen - The length of the input buffer
 */
void TEE_GenerateRandom(void* randomBuffer, size_t randomBufferLen);

/*
 * @brief This is a panic function, which indicates that an un-recoverable
 * 		  error has occurred. This function may not return control to the
 * 		  caller function.
 *
 * @param panicCode A variable that may be used to indicate error status.
  */
void TEE_Panic(TEE_Result panicCode);

#endif

