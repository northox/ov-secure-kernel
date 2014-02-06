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
 * Header file for global platform TEE client API
 */

/**
* @brief 
*/
typedef enum
{
	TEE_DATA_SEEK_SET = 0,
	TEE_DATA_SEEK_CUR,
	TEE_DATA_SEEK_END
} TEE_Whence;

typedef struct __TEE_ObjectEnumHandle* TEE_ObjectEnumHandle;

/**
 * @brief 
 */
enum Object_Storage_Constants {
	TEE_OBJECT_STORAGE_PRIVATE = 0x00000001,
};

/**
 * @brief 
 */
enum Data_Flag_Constants {
	TEE_DATA_FLAG_ACCESS_READ = 0x00000001,
	TEE_DATA_FLAG_ACCESS_WRITE = 0x00000002,
	TEE_DATA_FLAG_ACCESS_WRITE_META = 0x00000004,
	TEE_DATA_FLAG_SHARE_READ = 0x00000010,
	TEE_DATA_FLAG_SHARE_WRITE = 0x00000020,
	TEE_DATA_FLAG_CREATE = 0x00000200,
	TEE_DATA_FLAG_EXCLUSIVE = 0x00000400,
};

/**
 * @brief 
 */
enum Usage_Constants {
	TEE_USAGE_EXTRACTABLE = 0x00000001,
	TEE_USAGE_ENCRYPT = 0x00000002,
	TEE_USAGE_DECRYPT = 0x00000004,
	TEE_USAGE_MAC = 0x00000008,
	TEE_USAGE_SIGN = 0x00000010,
	TEE_USAGE_VERIFY = 0x00000020,
	TEE_USAGE_DERIVE = 0x00000040,
};

/**
 * @brief 
 */
enum Handle_Flag_Constants {
	TEE_HANDLE_FLAG_PERSISTENT = 0x00010000,
	TEE_HANDLE_FLAG_INITIALIZED = 0x00020000,
	TEE_HANDLE_FLAG_KEY_SET = 0x00040000,
	TEE_HANDLE_FLAG_EXPECT_TWO_KEYS = 0x00080000,
};

/**
 * @brief 
 */
enum Miscellaneous_Constants {
	TEE_DATA_MAX_POSITION = 0xFFFFFFFF,
	TEE_OBJECT_ID_MAX_LEN = 64,
};

/**
* @brief 
*
* @param object
* @param objectInfo
*/
void TEE_GetObjectInfo(
	TEE_ObjectHandle object,
	TEE_ObjectInfo* objectInfo);

/**
* @brief 
*
* @param object
* @param objectUsage
*/
void TEE_RestrictObjectUsage(
	TEE_ObjectHandle object,
	uint32_t objectUsage);

/**
* @brief 
*
* @param object
* @param attributeID
* @param buffer
* @param size
*
* @return 
*/
TEE_Result TEE_GetObjectBufferAttribute(
	TEE_ObjectHandle object,
	uint32_t attributeID,
	void* buffer, size_t* size);

/**
* @brief 
*
* @param object
* @param attributeID
* @param a
* @param b
*
* @return 
*/
TEE_Result TEE_GetObjectValueAttribute(
	TEE_ObjectHandle object,
	uint32_t attributeID,
	uint32_t* a,
	uint32_t* b);

/**
* @brief 
*
* @param object
*/
void TEE_CloseObject( TEE_ObjectHandle object);

/**
* @brief 
*
* @param objectType
* @param maxObjectSize
* @param object
*
* @return 
*/
TEE_Result TEE_AllocateTransientObject(
	uint32_t objectType,
	uint32_t maxObjectSize,
	TEE_ObjectHandle* object);

//TEE_AllocateTransientObject and Object Sizes
enum TEE_AllocateTransientObject {
	TEE_TYPE_AES, 			 //128, 192, or 256 bits
	TEE_TYPE_DES, 			 //Always 56 bits
	TEE_TYPE_DES3, 			 //112 or 168 bits
	TEE_TYPE_HMAC_MD5, 		 //Between 64 and 512 bits, multiple of 8 bits
	TEE_TYPE_HMAC_SHA1, 	 //Between 80 and 512 bits, multiple of 8 bits
	TEE_TYPE_HMAC_SHA224, 	 //Between 112 and 512 bits, multiple of 8 bits
	TEE_TYPE_HMAC_SHA256, 	 //Between 192 and 1024 bits, multiple of 8 bits
	TEE_TYPE_HMAC_SHA384, 	 //Between 256 and 1024 bits, multiple of 8 bits
	TEE_TYPE_HMAC_SHA512, 	 //Between 256 and 1024 bits, multiple of 8 bits
	TEE_TYPE_RSA_PUBLIC_KEY, //Object size is the number of bits in the modulus.
							 //All key size up to 2048 bits must be supported. Support for bigger key 3
      						 //sizes is implementation-dependent. Minimum key size is 256 bits.
	TEE_TYPE_RSA_KEYPAIR, 	 //Same as for RSA public key size.
	TEE_TYPE_DSA_PUBLIC_KEY,  //Between 512 and 1024 bits, multiple of 64 bits
	TEE_TYPE_DSA_KEYPAIR,
	TEE_TYPE_DH_KEYPAIR, 	 //From 256 to 2048 bits
	TEE_TYPE_GENERIC_SECRET  //Multiple of 8 bits, up to 4096 bits. This type is intended for secret data
							 //that is not directly used as a key in a cryptographic operation, but
							 //participates in a key derivation.
};

/**
* @brief 
*
* @param object
*/
void TEE_FreeTransientObject(
	TEE_ObjectHandle object);

/**
* @brief 
*
* @param object
*/
void TEE_ResetTransientObject(
	TEE_ObjectHandle object);

/**
* @brief 
*
* @param object
* @param attrs
* @param attrCount
*
* @return 
*/
TEE_Result TEE_PopulateTransientObject(
	TEE_ObjectHandle object,
	TEE_Attribute* attrs, 
	uint32_t attrCount);

/**
* @brief 
*
* @param attr
* @param attributeID
* @param buffer
* @param length
*/
void TEE_InitRefAttribute(
	TEE_Attribute* attr,
	uint32_t attributeID,
	void* buffer, 
	size_t length);

/**
* @brief 
*
* @param attr
* @param attributeID
* @param a
* @param b
*/
void TEE_InitValueAttribute(
	TEE_Attribute* attr,
	uint32_t attributeID,
	uint32_t a, uint32_t b);

/**
* @brief 
*
* @param destObject
* @param srcObject
*/
void TEE_CopyObjectAttributes(
	TEE_ObjectHandle destObject,
	TEE_ObjectHandle srcObject);

/**
* @brief 
*
* @param object
* @param keySize
* @param params
* @param paramCount
*
* @return 
*/
TEE_Result TEE_GenerateKey(
	TEE_ObjectHandle object,
	uint32_t keySize,
	TEE_Attribute* params, 
	uint32_t paramCount);

/**
* @brief 
*
* @param storageID
* @param objectID
* @param objectIDLen
* @param flags
* @param object
*
* @return 
*/
TEE_Result TEE_OpenPersistentObject(
	uint32_t storageID,
	void* objectID, 
	size_t objectIDLen,
	uint32_t flags,
	TEE_ObjectHandle* object);

/**
* @brief 
*
* @param storageID
* @param objectID
* @param objectIDLen
* @param flags
* @param attributes
* @param initialData
* @param initialDataLen
* @param object
*
* @return 
*/
TEE_Result TEE_CreatePersistentObject(
	uint32_t storageID, 
	void* objectID, 
	size_t objectIDLen,
	uint32_t flags,
	TEE_ObjectHandle attributes,
	void* initialData, 
	size_t initialDataLen,
	TEE_ObjectHandle* object);

/**
* @brief 
*
* @param object
*/
void TEE_CloseAndDeletePersistentObject( TEE_ObjectHandle object );

/**
* @brief 
*
* @param object
* @param newObjectID
* @param newObjectIDLen
*
* @return 
*/
/*TEE_Result TEE_RenamePersistentObject(
	TEE_ObjectHandle object,
	void* newObjectID, 
	size_t newObjectIDLen);*/

/**
* @brief 
*
* @param objectEnumerator
*
* @return 
*/
TEE_Result TEE_AllocatePersistentObjectEnumerator(
	TEE_ObjectEnumHandle* objectEnumerator );

/**
* @brief 
*
* @param objectEnumerator
*/
void TEE_FreePersistentObjectEnumerator(TEE_ObjectEnumHandle
	objectEnumerator );


/**
* @brief 
*
* @param objectEnumerator
*/
void TEE_ResetPersistentObjectEnumerator(TEE_ObjectEnumHandle
	objectEnumerator );

/**
* @brief 
*
* @param objectEnumerator
* @param storageID
*
* @return 
*/
TEE_Result TEE_StartPersistentObjectEnumerator(
	TEE_ObjectEnumHandle objectEnumerator,
	uint32_t storageID);


/**
* @brief 
*
* @param objectEnumerator
* @param objectInfo
* @param objectID
* @param objectIDLen
*
* @return 
*/
TEE_Result TEE_GetNextPersistentObject(
	TEE_ObjectEnumHandle objectEnumerator,
	TEE_ObjectInfo objectInfo,
	void* objectID,
	size_t* objectIDLen );

/**
* @brief 
*
* @param object
* @param buffer
* @param size
* @param count
*
* @return 
*/
TEE_Result TEE_ReadObjectData(
	TEE_ObjectHandle object,
	void* buffer,
	size_t size,
	uint32_t* count );

/**
* @brief 
*
* @param object
* @param buffer
* @param size
*
* @return 
*/
TEE_Result TEE_WriteObjectData(
	TEE_ObjectHandle object,
	void* buffer, 
	size_t size );

/**
* @brief 
*
* @param object
* @param size
*
* @return 
*/
/*TEE_Result TEE_TruncateObjectData(
	TEE_ObjectHandle object,
	uint32_t size );*/

/**
* @brief 
*
* @param object
* @param offset
* @param whence
*
* @return 
*/
typedef signed int int32_t;
TEE_Result TEE_SeekObjectData(
	TEE_ObjectHandle object,
	int32_t offset,
	TEE_Whence whence );
