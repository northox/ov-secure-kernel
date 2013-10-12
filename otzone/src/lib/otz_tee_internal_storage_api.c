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
#include <otz_tee_crypto_api.h>
#include <otz_tee_internal_storage_api.h>
#include <otz_api.h>
#include <otz_tee_api.h>
#include <sw_buddy.h>
#include <sw_debug.h>
#include <sw_types.h>
#include <sw_filelib.h>
//#include <stdio.h>
//#include <errno.h>
#include <otz_tee_common.h>
#include <otz_tee_mem_mgmt.h>
#include <sw_string_functions.h>
#include <sw_mem_functions.h>
#include <libc_sos.h>

/**
 * @brief 
 *
 * @param object
 * @param objectInfo
 */
void TEE_GetObjectInfo(
	TEE_ObjectHandle object,
	TEE_ObjectInfo* objectInfo)
{
}

/**
* @brief 
*
* @param object
* @param objectUsage
*/
void TEE_RestrictObjectUsage(
	TEE_ObjectHandle object,
	uint32_t objectUsage)
{
}

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
	void* buffer, size_t* size)
{
	return TEE_SUCCESS;		
}

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
	uint32_t* b)
{
	return TEE_SUCCESS;
}

/**
* @brief 
*
* @param object
*/
void TEE_CloseObject( TEE_ObjectHandle object)
{
}

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
	TEE_ObjectHandle* object)
{
#ifdef FILE_LIB_DBG
	sw_printf("*****TEE_AllocateTransientObject***** \n");
#endif
		if(maxObjectSize <= 0x00000800){
	    switch(objectType) {
        case TEE_TYPE_AES:
#ifdef FILE_LIB_DBG
            	sw_printf("128, 192, or 256 bits is set \n");
#endif
				break;
        case TEE_TYPE_DES:
#ifdef FILE_LIB_DBG
                sw_printf("Always 56 bits is set \n");
#endif
				break;
        case TEE_TYPE_DES3:
#ifdef FILE_LIB_DBG
                sw_printf("112 or 168 bits is set \n");
#endif
				break;
        case TEE_TYPE_HMAC_MD5:
#ifdef FILE_LIB_DBG
				sw_printf("Between 64 and 512 bits, multiple of 8 bits \n") ;
#endif
				break;
		case TEE_TYPE_HMAC_SHA1:
#ifdef FILE_LIB_DBG
				sw_printf("Between 80 and 512 bits, multiple of 8 bits \n");
#endif
				break;
		case TEE_TYPE_HMAC_SHA224:
#ifdef FILE_LIB_DBG
				sw_printf("Between 112 and 512 bits, multiple of 8 bits \n");
#endif
				break;
		case TEE_TYPE_HMAC_SHA256:
#ifdef FILE_LIB_DBG
				sw_printf("Between 192 and 1024 bits, multiple of 8 bits \n");
#endif
				break;
		case TEE_TYPE_HMAC_SHA384:
#ifdef FILE_LIB_DBG
				sw_printf("Between 256 and 1024 bits, multiple of 8 bits \n");
#endif
				break;
		case TEE_TYPE_HMAC_SHA512:
#ifdef FILE_LIB_DBG
				sw_printf("Between 256 and 1024 bits, multiple of 8 bits \n");
#endif
				break;
		case TEE_TYPE_RSA_PUBLIC_KEY:
#ifdef FILE_LIB_DBG
				sw_printf("All key size up to 2048 bits must be supported \n");
#endif
				break;
		case TEE_TYPE_RSA_KEYPAIR:
#ifdef FILE_LIB_DBG
				sw_printf("Same as for RSA public key size \n");
#endif
				break;
		case TEE_TYPE_DSA_PUBLIC_KEY:
#ifdef FILE_LIB_DBG
				sw_printf("Between 512 and 1024 bits, multiple of 64 bits \n");
#endif
				break;
		case TEE_TYPE_DSA_KEYPAIR:
#ifdef FILE_LIB_DBG
				sw_printf("Between 512 and 1024 bits, multiple of 64 bits \n");
#endif
				break;
		case TEE_TYPE_DH_KEYPAIR:
#ifdef FILE_LIB_DBG
				sw_printf("From 256 to 2048 bits \n");
#endif
				break;
		case TEE_TYPE_GENERIC_SECRET:
#ifdef FILE_LIB_DBG
				sw_printf("Multiple of 8 bits, up to 4096 bits \n");
#endif
            	break;
        default:
			sw_printf("object size is not supported \n");
			return TEE_ERROR_NOT_SUPPORTED;
            break;
    }
	((TEE_ObjectHandle*)object)->ObjectInfo=(TEE_ObjectInfo*)TEE_Malloc
													(sizeof(TEE_ObjectInfo),0);
	((TEE_ObjectHandle*)object)->Attribute=(TEE_Attribute*)TEE_Malloc
													(sizeof(TEE_Attribute),0);
	if(((TEE_ObjectHandle*)object)->ObjectInfo==NULL) {
		sw_printf("resources are not available to \
allocate the object handle \n");
		return TEE_ERROR_OUT_OF_MEMORY;
	}
	
	if(((TEE_ObjectHandle*)object)->Attribute==NULL) {
        sw_printf("resources are not available to \
allocate the object handle \n");
        return TEE_ERROR_OUT_OF_MEMORY;
    }
	object->ObjectInfo->objectType=objectType;
	object->ObjectInfo->maxObjectSize=maxObjectSize;
#ifdef FILE_LIB_DBG
	sw_printf("the objectType is %x \n",object->ObjectInfo->objectType);
	sw_printf("the maxObjectsize is %x \n",object->ObjectInfo->maxObjectSize);
	sw_printf("Transient Object is successfully allocated \n ");
#endif
	return TEE_SUCCESS;
		}
		else {
			sw_printf("Error Out Of Memory \n");
			return TEE_ERROR_OUT_OF_MEMORY;
		}
}

/**
* @brief 
*
* @param object
*/
void TEE_FreeTransientObject(
	TEE_ObjectHandle object)
{	
#ifdef FILE_LIB_DBG
	sw_printf("*****Free the Transient Object***** \n");
#endif
	TEE_Free((void*)&object);
#ifdef FILE_LIB_DBG
	sw_printf("Allocated objects are freed \n");
#endif
}


/**
* @brief 
*
* @param object
*/
void TEE_ResetTransientObject(
	TEE_ObjectHandle object)
{
#ifdef FILE_LIB_DBG
	sw_printf("*****Reset the Transient Object***** \n");
#endif
	TEE_Realloc((void*)&object,sizeof(object));
#ifdef FILE_LIB_DBG
	sw_printf("object values are reseted \n");
#endif
}

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
	uint32_t attrCount)
{
	int32_t i;
	u8 pop_buff[255];
#ifdef FILE_LIB_DBG
	sw_printf("*****Populate the Transient Object***** \n");
#endif
	if(attrs==NULL) {
		sw_printf("attribute values are NULL \n");
		return TEE_ERROR_BAD_PARAMETERS;
	}
	object.Attribute=(TEE_Attribute*)TEE_Malloc(attrCount*
											sizeof(TEE_Attribute),0);
	for(i=0;i<attrCount;i++) {
		object.Attribute[i].attributeID=attrs[i]
            .attributeID;
		if(((object.Attribute[i].attributeID<<2)>>31)==0) {
		object.Attribute[i].content.ref.length=attrs[i]
			            .content.ref.length;
		object.Attribute[i].content.ref.buffer=TEE_Malloc
			(object.Attribute[i].content.ref.length,0);
        TEE_MemCpy(object.Attribute[i].content.ref.buffer,
			attrs[i].content.ref.buffer,
			(u32)(object.Attribute[i].content.ref.length));
		TEE_MemCpy((void *)pop_buff,object.Attribute[i].content.ref.buffer,
            (u32)(object.Attribute[i].content.ref.length));
		pop_buff[object.Attribute[i].content.ref.length]='\0';	
#ifdef FILE_LIB_DBG
		sw_printf("attribute ID is %x \n",object.Attribute[i].attributeID);
        sw_printf("buffer of attribute is %s \n",
			pop_buff);
		sw_printf("length of attribute is %x \n",
			object.Attribute[i].content.ref.length);
#endif
		TEE_Free(object.Attribute->content.ref.buffer);
		}
		if(((object.Attribute[i].attributeID<<2)>>31)==1) {
        object.Attribute[i].content.value.a=attrs[i]
            .content.value.a;
        object.Attribute[i].content.value.b=attrs[i]
            .content.value.b;	
#ifdef FILE_LIB_DBG
		sw_printf("attribute ID is %x \n",object.Attribute[i].attributeID);
		sw_printf("value of a in attribute is %x \n",
									object.Attribute[i].content.value.a);
        sw_printf("value of b in attribute is %x \n",
									object.Attribute[i].content.value.b);
#endif

		}
	}
		TEE_Free((void *)(object.Attribute));
#ifdef FILE_LIB_DBG
		sw_printf("PopulateTransientObject is successfully performed \n");
#endif
		return TEE_SUCCESS;
}

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
	size_t length)
{	
	u8 ref_buff[255];
#ifdef FILE_LIB_DBG
	sw_printf("*****InitRefAttributes the Transient Object***** \n");
#endif
	if(buffer==NULL) {
		sw_printf("buffer is NULL \n");
	}
	attr->attributeID=attributeID;
#ifdef FILE_LIB_DBG
	sw_printf("attribute ID is %x \n",attr->attributeID);
#endif
	if(((attr->attributeID<<2)>>31)==0) {
#ifdef FILE_LIB_DBG
		sw_printf("*****Initial Reference attributes ******\n");
#endif
		attr->content.ref.length=length;
		attr->content.ref.buffer=TEE_Malloc(attr->content.ref.length,0);
		TEE_MemCpy(attr->content.ref.buffer,buffer,
			(u32)(attr->content.ref.length));
		TEE_MemCpy((void*)ref_buff,attr->content.ref.buffer,
			(u32)(attr->content.ref.length));
		ref_buff[attr->content.ref.length]='\0';
#ifdef FILE_LIB_DBG
		sw_printf("buffer of attribute is %s \n",ref_buff);
		sw_printf("length of attribute is %x \n",attr->content.ref.length);
		sw_printf("Initial Reference attributes successfully assigned \n");
#endif
	}
}

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
	uint32_t a, uint32_t b)
{
#ifdef FILE_LIB_DBG
	sw_printf("*****Initial value Attributes *****\n");
#endif
   	attr->attributeID=attributeID;
#ifdef FILE_LIB_DBG
	sw_printf("attribute ID is %x \n",attr->attributeID);
#endif
	if(((attr->attributeID<<2)>>31)==1) {
   	attr->content.value.a=a;
   	attr->content.value.b=b;   
#ifdef FILE_LIB_DBG
	sw_printf("value of a in attribute is %x \n",attr->content.value.a);
    sw_printf("value of b in attribute is %x \n",attr->content.value.b);	
	sw_printf("Initial value Attributes successfully assigned\n");
#endif
	}
}

/**
* @brief 
*
* @param destObject
* @param srcObject
*/
void TEE_CopyObjectAttributes(
	TEE_ObjectHandle destObject,
	TEE_ObjectHandle srcObject)
{
	u8 cop_buff[255];
	destObject.Attribute=(TEE_Attribute*)TEE_Malloc(sizeof(TEE_Attribute),0);
	srcObject.Attribute=(TEE_Attribute*)TEE_Malloc(sizeof(TEE_Attribute),0);
#ifdef FILE_LIB_DBG
	sw_printf("*****Copy Object Attributes *****\n");
#endif
	if(sizeof(destObject)!=sizeof(srcObject)) {
		sw_printf("error in copy the object attributes \n");
	}
	else {
		if(srcObject.dataPtr!=NULL) {
		destObject.dataPtr=srcObject.dataPtr;
        destObject.dataLen=srcObject.dataLen;
        sw_strcpy((void *)destObject.dataName,(void *)srcObject.dataName);
#ifdef FILE_LIB_DBG
		sw_printf("file name is %s \n",destObject.dataName);
		sw_printf("file length is %x \n",destObject.dataLen);
#endif
		}
		if(srcObject.Attribute!=NULL) {
			destObject.Attribute->attributeID=srcObject.Attribute->attributeID;
		if(((destObject.Attribute->attributeID<<2)>>31)==0) {
        destObject.Attribute->content.ref.length=
			srcObject.Attribute->content.ref.length;
		destObject.Attribute->content.ref.buffer=TEE_Malloc
			(destObject.Attribute->content.ref.length,0);
		TEE_MemCpy(destObject.Attribute->content.ref.buffer,
        	srcObject.Attribute->content.ref.buffer,
			(u32)(destObject.Attribute->content.ref.length));
		TEE_MemCpy((void *)cop_buff,destObject.Attribute->content.ref.buffer,
            (u32)(destObject.Attribute->content.ref.length));
		cop_buff[destObject.Attribute->content.ref.length]='\0';
#ifdef FILE_LIB_DBG
		sw_printf("attribute ID is %x \n",destObject.Attribute->attributeID);
        sw_printf("buffer of attribute is %s \n",cop_buff);
        sw_printf("length of attribute is %x \n",
									destObject.Attribute->content.ref.length);
#endif
		}
		if(((destObject.Attribute->attributeID<<2)>>31)==1) {
        destObject.Attribute->content.value.a=srcObject.Attribute->content
            .value.a;
        destObject.Attribute->content.value.b=srcObject.Attribute->content
            .value.b;
#ifdef FILE_LIB_DBG
		sw_printf("attribute ID is %x \n",destObject.Attribute->attributeID);
		sw_printf("value of a in attribute is %x \n",
									destObject.Attribute->content.value.a);
		sw_printf("value of b in attribute is %x \n",
									destObject.Attribute->content.value.b);
#endif
		}
	}
		TEE_Free((void*)(destObject.Attribute->content.ref.buffer));
		TEE_Free((void*)(srcObject.Attribute));
		TEE_Free((void*)(destObject.Attribute));
#ifdef FILE_LIB_DBG
		sw_printf("copy the object attributes is successfully performed \n");
#endif
	}
}

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
	uint32_t paramCount)
{
	return TEE_SUCCESS;
}

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
	TEE_ObjectHandle* object)
{
	FILE* pFile=NULL;
    u8 mode[3];
    mode[0] = '\0';
#ifdef FILE_LIB_DBG
	sw_printf("***** open persistent object *****\n");
#endif
	if(objectID==NULL && objectIDLen>TEE_OBJECT_ID_MAX_LEN) {
		sw_printf("error in passing values \n");
		return TEE_ERROR_ITEM_NOT_FOUND;
	}
	switch(flags) {
		case TEE_DATA_FLAG_ACCESS_WRITE:
			mode[0] = 'w';
			mode[1] = '\0';
#ifdef FILE_LIB_DBG
			sw_printf("write mode is set \n");
#endif
		case TEE_DATA_FLAG_ACCESS_READ:
			if(mode[0] == '\0'){
				mode[0] = 'r';
				mode[1] = '\0';
#ifdef FILE_LIB_DBG
				sw_printf("read mode is set \n");
#endif
			}
		case (TEE_DATA_FLAG_ACCESS_WRITE | TEE_DATA_FLAG_ACCESS_READ):		
			if(mode[0] == '\0'){
				mode[0] = 'r';
				mode[1] = '+';
				mode[2] = '\0';
#ifdef FILE_LIB_DBG
				sw_printf("read+ mode is set \n");
#endif
			}   
			pFile=fopen((const unsigned char*)objectID,
						(const unsigned char*)mode);
			break;
		default:
			break;
	}
  	if(pFile==NULL) {
	if(errno == ENOENT) {
    	sw_printf("Error Not Item Found \n");
        return TEE_ERROR_ITEM_NOT_FOUND;
    }
    if(errno == EACCES) {
    	sw_printf("Error Access Confilct \n");
        return TEE_ERROR_ACCESS_CONFLICT;
    }
    if(errno == ENOMEM) {
    	sw_printf("Error Out Of Memory \n");
        return TEE_ERROR_OUT_OF_MEMORY;
	}
	sw_printf("Item not found in the directory \n");
	return TEE_ERROR_OUT_OF_MEMORY;
	}
	object->dataPtr=(void*)pFile;
	TEE_MemCpy((void *)object->dataName,objectID,(u32)objectIDLen);
	object->dataLen=objectIDLen;
	object->dataName[object->dataLen] = '\0';
#ifdef FILE_LIB_DBG
	sw_printf("current opened file name is %s \n",object->dataName);
	sw_printf("current opened file length is %x \n",object->dataLen);
	sw_printf("open successfully performed \n");
#endif
	return TEE_SUCCESS;
}

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
	TEE_ObjectHandle attributes, /*need to handle atributes*/
	void* initialData, 
	size_t initialDataLen,
	TEE_ObjectHandle* object)
{
	int32_t w_value;
	FILE* pFile=NULL;
	u8 mode[3];
	mode[0] = '\0';
#ifdef FILE_LIB_DBG
	sw_printf("***** create persistent object ***** \n");
#endif
	if(objectID==NULL &&  initialData==NULL 
			&& objectIDLen>TEE_OBJECT_ID_MAX_LEN 
			&& initialDataLen>TEE_OBJECT_ID_MAX_LEN) {
		sw_printf("error in passing function values \n");
		return TEE_ERROR_ITEM_NOT_FOUND;
	}
    switch(flags) {
        case TEE_DATA_FLAG_ACCESS_WRITE:
            mode[0] = 'w';
            mode[1] = '\0';
#ifdef FILE_LIB_DBG
			sw_printf("write mode is set \n");
#endif
        case TEE_DATA_FLAG_ACCESS_READ:
          	if(mode[0] == '\0'){
                mode[0] = 'r';
                mode[1] = '\0';
#ifdef FILE_LIB_DBG
				sw_printf("read mode is set \n");
#endif
            }
		case (TEE_DATA_FLAG_ACCESS_WRITE | TEE_DATA_FLAG_ACCESS_READ):
            if(mode[0] == '\0'){
                mode[0] = 'r';
                mode[1] = '+';
                mode[2] = '\0';
#ifdef FILE_LIB_DBG
				sw_printf("read+ mode is set \n");
#endif
            }
            pFile=fopen((const unsigned char*)objectID,
						(const unsigned char*)mode);
            break;
        default:
            break;
    }
	if(pFile==NULL) {
	if(errno == ENOENT) {
    	sw_printf("Error Item Not FOund \n");
        return TEE_ERROR_ITEM_NOT_FOUND;
     }
     if(errno == EACCES) {
        sw_printf("Error Access COnflict \n");
        return TEE_ERROR_ACCESS_CONFLICT;
     }
     if(errno == ENOMEM) {
        sw_printf("Error Out Of Memory \n");
        return TEE_ERROR_OUT_OF_MEMORY;
     }
     if(errno == ENOSPC) {
        sw_printf("Error No Storage Space \n");
        return TEE_ERROR_STORAGE_NO_SPACE;
      }
	 sw_printf("Error in create persistent object \n");
	 return TEE_ERROR_OUT_OF_MEMORY;
	}
	object->dataPtr=(void*)pFile;
	TEE_MemCpy((void *)object->dataName,objectID,(u32)objectIDLen);
	object->dataLen=objectIDLen;
	object->dataName[object->dataLen] = '\0';
	if(flags==TEE_DATA_FLAG_ACCESS_READ) {
#ifdef FILE_LIB_DBG
		sw_printf("storage_api:create successfully performed in read case \n");
		sw_printf("created file name is %s \n",objectID);
		sw_printf("created file length is %x \n",objectIDLen);
#endif
		return TEE_SUCCESS;
	}
	else {
		w_value=(int32_t)TEE_WriteObjectData(*object,initialData,
											 initialDataLen);
	if(w_value==TEE_SUCCESS) {
#ifdef FILE_LIB_DBG
		sw_printf("create successfully performed in write case \n");
		sw_printf("created file name is %s \n",object->dataName);
        sw_printf("created file length is %x \n",object->dataLen);
#endif
		return TEE_SUCCESS;
	}
	else {
		sw_printf("error in write of create \n");
		return TEE_ERROR_STORAGE_NO_SPACE;
	}
	}
}
/**
* @brief 
*
* @param object
*/
void TEE_CloseAndDeletePersistentObject( TEE_ObjectHandle object )
{
	s32int rem_val;
	u8 File_name[255];
	TEE_MemCpy(File_name,(void *)object.dataName,(u32)(object.dataLen));
	File_name[object.dataLen] = '\0';
	rem_val=(s32int)(remove((const char *)File_name));	
	if(rem_val!=0) {
    sw_printf("error in Remove The Object \n");
     }
	else {
#ifdef FILE_LIB_DBG
	sw_printf("Deleted file name is %s \n",File_name);
	sw_printf("delete the file successfully performed \n");
#endif
	}
}

/**
* @brief 
*
* @param object
* @param newObjectID
* @param newObjectIDLen
*
* @return 
*/
TEE_Result TEE_RenamePersistentObject(
	TEE_ObjectHandle object,
	void* newObjectID, 
	size_t newObjectIDLen)
{
	int32_t	c_val,re_value;
	u8 File_name[255];
	FILE* ofp=NULL;
#ifdef FILE_LIB_DBG
	sw_printf("***** rename persistent object ***** \n");
#endif
	if(newObjectID==NULL && newObjectIDLen>TEE_OBJECT_ID_MAX_LEN) {
		sw_printf("error in new file name \n");
		return TEE_ERROR_ACCESS_CONFLICT;
	}
	ofp=(FILE *)(object.dataPtr);
	sw_strcpy((void *)File_name,(void *)object.dataName);
	if(ofp == NULL && File_name==NULL) {
		sw_printf("error in file pointer \n");
		return TEE_ERROR_OUT_OF_MEMORY;
	}
#ifdef FILE_LIB_DBG
	sw_printf("the old file name is %s \n",File_name);
	sw_printf("the old file name length is %x \n",object.dataLen);
	sw_printf("the new file name is %s \n",newObjectID);
    sw_printf("the new file name length is %x \n",newObjectIDLen);
#endif
	re_value=(int32_t)(rename((const char *)File_name,(const char *)newObjectID));
	if(re_value != 0) {
		sw_printf("Error Access COnflict \n");
        return TEE_ERROR_ACCESS_CONFLICT;
	}
	c_val=(int32_t)(fclose(ofp));
	if(c_val==-1) {
	sw_printf("old file pointer is not closed \n");
	return TEE_ERROR_OUT_OF_MEMORY;
	}
#ifdef FILE_LIB_DBG
	sw_printf("old file pointer is successfully closed \n");
#endif
    TEE_MemCpy((void *)object.dataName,newObjectID,(u32)newObjectIDLen);
	object.dataLen=newObjectIDLen;
	object.dataName[object.dataLen] = '\0';
#ifdef FILE_LIB_DBG
	sw_printf("rename successfully performed \n");
#endif
    return TEE_SUCCESS;
}

/**
* @brief 
*
* @param objectEnumerator
*
* @return 
*/
TEE_Result TEE_AllocatePersistentObjectEnumerator(
	TEE_ObjectEnumHandle* objectEnumerator )
{
		return TEE_SUCCESS;
}

/**
* @brief 
*
* @param objectEnumerator
*/
void TEE_FreePersistentObjectEnumerator(TEE_ObjectEnumHandle
	objectEnumerator )
{

}

/**
* @brief 
*
* @param objectEnumerator
*/
void TEE_ResetPersistentObjectEnumerator(TEE_ObjectEnumHandle
	objectEnumerator )
{

}

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
	uint32_t storageID)
{
	return TEE_SUCCESS;
}


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
	size_t* objectIDLen )
{
	return TEE_SUCCESS;
}

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
	uint32_t* count )
{
	size_t robj_value,read_element=1;
	int32_t c_val;
	u8 File_name[255];
    FILE* fp=NULL;
	u8 buff[255];
#ifdef FILE_LIB_DBG
	sw_printf("***** read persistent object ***** \n");
#endif
	fp=(FILE *)(object.dataPtr);
	sw_strcpy((void *)File_name,(void *)object.dataName);
    if(fp==NULL) {
		sw_printf("error in file pointer \n");
		return TEE_ERROR_OUT_OF_MEMORY;
    }
	if(buff==NULL && count==NULL) {
		sw_printf("error in buffer \n");
		return TEE_ERROR_ITEM_NOT_FOUND;
	}
	robj_value=(size_t)(fread(buffer,(unsigned int)read_element,
				(unsigned int)size,fp));
	if(robj_value==-1) {
		sw_printf("error in read \n");
    	return TEE_ERROR_STORAGE_NO_SPACE;
	}
	*count=robj_value;
	TEE_MemCpy((void*)buff,buffer,(u32)(robj_value));
	c_val=(int32_t)(fclose(fp));
	if(c_val==-1) {
		sw_printf("ReadObjectData:file pointer is not closed \n");
		return TEE_ERROR_OUT_OF_MEMORY;
	}
#ifdef FILE_LIB_DBG
	sw_printf("ReadObjectData:file pointer is successfully closed \n");
#endif
	buff[*count] = '\0';
#ifdef FILE_LIB_DBG
	sw_printf("read file name is %s \n",File_name);
	sw_printf("value of count is %x \n",*count);
	sw_printf("buffer content is %s \n",buff);
	sw_printf("read successfully performed \n");
#endif
    return TEE_SUCCESS;

}

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
	size_t size )
{
	size_t wobj_value,write_element=1;
	int32_t c_val;
	u8 File_name[255],w_buff[255];
    FILE* fp=NULL;
#ifdef FILE_LIB_DBG
	sw_printf("***** write persistent object ***** \n");
#endif
	if(buffer==NULL) {
		sw_printf("error in buffer \n");
		return TEE_ERROR_STORAGE_NO_SPACE;
	}
	fp=(FILE *)(object.dataPtr);
	sw_strcpy((void *)File_name,(void *)object.dataName);
	if(fp==NULL) {
		sw_printf("error in file pointer \n");
		return TEE_ERROR_OUT_OF_MEMORY;
    }
	wobj_value=(size_t)(fwrite(buffer,(unsigned int)write_element,
						(unsigned int)size,fp));
	if(wobj_value==0) {
		sw_printf("storage api:file opened in read only mode \n");
        sw_printf("--------------------------------------------------------\n");
        return TEE_ERROR_STORAGE_NO_SPACE;
    }
	if(wobj_value==-1 || errno==ENOSPC) {
		sw_printf("error in write \n");
		return TEE_ERROR_STORAGE_NO_SPACE;
	}
	object.dataPtr=(void*)fp;
    c_val=(int32_t)(fclose(fp));
    if(c_val==-1) {
        sw_printf("WriteObjectData:file pointer is not closed \n");
        return TEE_ERROR_OUT_OF_MEMORY;
    }
	TEE_MemCpy((void*)w_buff,buffer,(u32)(wobj_value));
    w_buff[wobj_value]='\0';
#ifdef FILE_LIB_DBG
    sw_printf("WriteObjectData:file pointer is successfully closed \n");
	sw_printf("the write file name is %s \n",File_name);
	sw_printf("no of bytes to write %x \n",wobj_value);
	sw_printf("the write contents is %s \n",w_buff);
	sw_printf("write successfully performed \n");
#endif
	return TEE_SUCCESS;	
}

/**
* @brief 
*
* @param object
* @param size
*
*
* @return 
*/
TEE_Result TEE_TruncateObjectData(
	TEE_ObjectHandle object,
	uint32_t size )
{
	s32int tr_value;
    u8 File_name[255];
#ifdef FILE_LIB_DBG
    sw_printf("***** truncate persistent object ***** \n");
#endif
    sw_strcpy((void *)File_name,(void *)object.dataName);
    tr_value=(s32int)(truncate((const char*)File_name,(long)size));
    if(tr_value!=0) {
        sw_printf("error in truncate \n");
        return TEE_ERROR_STORAGE_NO_SPACE;
     }
#ifdef FILE_LIB_DBG
    sw_printf("truncate successfully performed \n");
#endif
    return TEE_SUCCESS;
}

/**
* @brief 
*
* @param object
* @param offset
* @param whence
*
* @return 
*/
TEE_Result TEE_SeekObjectData(
	TEE_ObjectHandle object,
	int32_t offset,
	TEE_Whence whence )
{
	int32_t sk_value,c_val, seek_set;
	u8 File_name[255];
	FILE* fp=NULL;
#ifdef FILE_LIB_DBG
	sw_printf("***** seek persistent object ***** \n");
#endif
	fp=(FILE *)(object.dataPtr);
	sw_strcpy((void *)File_name,(void *)object.dataName);
	if(fp==NULL) {
		sw_printf("the file pointer is NULL \n");
		return TEE_ERROR_OUT_OF_MEMORY;
    }
	seek_set=(int32_t)whence;
	sk_value=(int32_t)(fseek(fp,(long)offset,(int)seek_set));
	if(sk_value==-1 || sk_value>TEE_DATA_MAX_POSITION) {
		sw_printf("error in seek \n");
    	return TEE_ERROR_OVERFLOW;
	}
	c_val=(int32_t)(fclose(fp));
	if(c_val==-1) {
        sw_printf("SeekObjectData:file pointer is not closed \n");
        return TEE_ERROR_OUT_OF_MEMORY;
    }
#ifdef FILE_LIB_DBG
    sw_printf("SeekObjectData:file pointer is successfully closed \n");
	sw_printf("the write file name is %s \n",File_name);
	sw_printf("starting bytes value %x \n",seek_set);
    sw_printf("seek byte value %x \n",offset);   
	sw_printf("seek successfully performed \n");
#endif
	return TEE_SUCCESS;
}
