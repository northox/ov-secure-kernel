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
 * STORAGE Tester implementation
 *
 */
#include <gp_internal_api_test.h>
#include <otz_tee_crypto_api.h>
#include <otz_tee_internal_storage_api.h>
#include <otz_api.h>
#include <otz_tee_api.h>
#include <sw_buddy.h>
#include <sw_debug.h>
#include <sw_string_functions.h>
#include <otz_tee_mem_mgmt.h>

/**
 * @brief 
 */
void test_storage_api()
{
    uint32_t storageID=TEE_OBJECT_STORAGE_PRIVATE,
             r_flags=TEE_DATA_FLAG_ACCESS_READ,
             w_flags=TEE_DATA_FLAG_ACCESS_WRITE,
             rw_flags=(TEE_DATA_FLAG_ACCESS_READ | TEE_DATA_FLAG_ACCESS_WRITE),
             a_attribute_val=0x00000005,b_attribute_val=0x00000007,
			 pop_ret_val,attribute_cnt=0x00000003,seek_ret_val,open_seek_retval,
			 crt_ret_val,write_ret_val,open_write_retval,read_ret_val,
			 open_read_retval,open_ret_val,open_delete_retval,allocate1_ret_val,
			 allocate2_ret_val,rd_trunc_cnt=0x00000000,open_truncate_retval,
			 trunc_size=0x0000000A,truncate_ret_val,rdtest_truncated_retval,
			 optest_truncated_retval,rdtest_written_retval,
			 optest_written_retval,rd_write_cnt=0x00000000,read_cnt=0x00000000,
			 trunc_cnt=0x00000000,open_rename_retval,de_a,
			 rd_rename_cnt=0x00000000,optest_renamed_retval,rename_ret_val,
			 rdtest_renamed_retval,optest_deleted_retval;

    typedef signed int int32_t;
    int32_t offset=0x00000003;
    size_t objectIDLen=0x00000040,read_size=0x0000000F,rd_trunc_size=0x0000000A,
	   	   rd_write_size=0x0000002C,rd_rename_size=0x0000000C;
    void* open_objectID="/test.dir/test.txt";
	void* rename_objectID="/test.dir/new.txt";
    void* initialData="This a sierraware created sample initial data\n";
    void* create_objectID="/test.dir/crt.txt";
    void* read_objectID="/test.dir/read.txt";
    void* write_objectID="/test.dir/write.txt";
    void* seek_objectID="/test.dir/seek.txt";
    void* delete_objectID="/test.dir/delete.txt";
	void* trunc_objectID="/test.dir/truncate.txt";
    char  wrie_buffer[255]={"This a sierraware created sample test string\n"};
    char  read_buffer[255],rd_trunc_buffer[255],rd_write_buffer[255],
		  rd_rename_buffer[255];
    void* attrsbuffer="This will get populated sometimes in the test fn\n";
    void* p_buffer="And finally we tested GP_INTERNAL_STORAGE APP\n";

    TEE_ObjectHandle crtattributes;
    TEE_ObjectHandle *first_object;
    TEE_ObjectHandle *second_object;

    TEE_Whence whence;
    whence=0x00000000;
	
	sw_printf("-----------Allocating Memory For Create Object--------------\n");
    first_object=(TEE_ObjectHandle*)TEE_Malloc(sizeof(TEE_ObjectHandle),0);
    sw_printf("-------Allocating Memory For Create Object members----------\n");
    allocate1_ret_val=TEE_AllocateTransientObject(TEE_TYPE_AES,0x00000800,
													 first_object);
    sw_printf("the allocate transient function returns value is %x \n", 
			   allocate1_ret_val);

    crt_ret_val=TEE_CreatePersistentObject(storageID,create_objectID,
			objectIDLen,w_flags,crtattributes,initialData,
			(size_t)(sw_strlen((char*)initialData)),first_object);

    sw_printf("The create Persistent object funtion \
returns value is  %x \n \n",crt_ret_val);

    sw_printf("------------Allocating Memory For open Object---------------\n");
    second_object=(TEE_ObjectHandle*)TEE_Malloc(sizeof(TEE_ObjectHandle),0);
	sw_printf("------------Allocating Memory For open Object members-------\n");
    allocate2_ret_val=TEE_AllocateTransientObject(TEE_TYPE_RSA_KEYPAIR,
													 0x00000800,second_object);
    sw_printf("the allocate transient function returns value is %x \n",
			   allocate2_ret_val);

    open_ret_val=TEE_OpenPersistentObject(storageID,open_objectID,objectIDLen,
                                            r_flags,second_object);
    sw_printf("The open Persistent object funtion returns value is %x \n \n",
			   open_ret_val);

    sw_printf("*****Reset the open object***** \n");
    TEE_ResetTransientObject(*second_object);

	open_read_retval=TEE_OpenPersistentObject(storageID,read_objectID,
									objectIDLen,r_flags,second_object);

    sw_printf("The open Persistent object funtion \
returns value is %x \n \n",open_read_retval);

    read_ret_val=TEE_ReadObjectData(*second_object,(void*)&read_buffer,
							read_size,&read_cnt);

    sw_printf("The Read Persistent funtion returns value is %x \n \n",
			   read_ret_val);

    sw_printf("*****Reset the read object***** \n");
    TEE_ResetTransientObject(*second_object);
	
	open_write_retval=TEE_OpenPersistentObject(storageID,write_objectID,
									  objectIDLen,w_flags,second_object);
    sw_printf("The open Persistent object funtion \
returns value is %x \n \n",open_write_retval);

    write_ret_val=TEE_WriteObjectData(*second_object,(void*)&wrie_buffer,
                         (size_t)(sw_strlen((char*)&wrie_buffer)));
    sw_printf("The write Persistent funtion returns value is %x \n \n",
			   write_ret_val);

    sw_printf("*****Reset the write object***** \n");
    TEE_ResetTransientObject(*second_object);

	optest_written_retval=TEE_OpenPersistentObject(storageID,write_objectID,
										objectIDLen,r_flags,second_object);

    sw_printf("The open Persistent object funtion \
returns value is %x \n \n",optest_written_retval);

    rdtest_written_retval=TEE_ReadObjectData(*second_object,
						  (void*)&rd_write_buffer,rd_write_size,
						  &rd_write_cnt);

    sw_printf("The Read Persistent funtion returns value is %x \n \n",
			   rdtest_written_retval);

	sw_printf("******TESTING:write persistent object*******\n");
    if(rdtest_written_retval==1) {
        sw_printf("SUCCESS \n");
    }
    else {
        sw_printf("FAILURE \n");
    }

    sw_printf("*****Reset the read object***** \n");
    TEE_ResetTransientObject(*second_object);

	open_truncate_retval=TEE_OpenPersistentObject(storageID,trunc_objectID,
							objectIDLen,w_flags,second_object);

    sw_printf("The open Persistent object funtion \
returns value is %x \n \n",open_truncate_retval);

	truncate_ret_val=TEE_TruncateObjectData(*second_object,trunc_size);
	sw_printf("The truncate Persistent funtion returns value is %x \n \n",
			   truncate_ret_val);

	sw_printf("*****Reset the truncate object***** \n");
    TEE_ResetTransientObject(*second_object);
	
	optest_truncated_retval=TEE_OpenPersistentObject(storageID,trunc_objectID,
								objectIDLen,r_flags,second_object);

    sw_printf("The open Persistent object funtion \
returns value is %x \n \n",optest_truncated_retval);

    rdtest_truncated_retval=TEE_ReadObjectData(*second_object,
							(void*)&rd_trunc_buffer,rd_trunc_size,
							&rd_trunc_cnt);

    sw_printf("The Read Persistent funtion returns value is %x \n \n",
			   rdtest_truncated_retval);

	sw_printf("******TESTING:truncate persistent object*******\n");
    if(rdtest_truncated_retval==1) {
        sw_printf("SUCCESS \n");
    }
    else {
        sw_printf("FAILS \n");
    }

    sw_printf("*****Reset the read object***** \n");
    TEE_ResetTransientObject(*second_object);

	open_rename_retval=TEE_OpenPersistentObject(storageID,open_objectID,
										objectIDLen,rw_flags,second_object);

    sw_printf("The open Persistent object funtion \
returns value is %x \n \n",open_rename_retval);

	rename_ret_val=TEE_RenamePersistentObject(*second_object,rename_objectID,
												objectIDLen);
    sw_printf("The rename Persistent funtion returns value is %x \n \n",
			   rename_ret_val);

    sw_printf("*****Reset the rename object***** \n");
    TEE_ResetTransientObject(*second_object);

	optest_renamed_retval=TEE_OpenPersistentObject(storageID,rename_objectID,
								objectIDLen,r_flags,second_object);

    sw_printf("The open Persistent object funtion \
returns value is %x \n \n",optest_renamed_retval);

    rdtest_renamed_retval=TEE_ReadObjectData(*second_object,
						   (void*)&rd_rename_buffer,rd_rename_size,
						   &rd_rename_cnt);

    sw_printf("The Read Persistent funtion returns value is %x \n \n",
			   rdtest_renamed_retval);

	sw_printf("******TESTING:rename persistent object*******\n");
    if(rdtest_renamed_retval==1) {
        sw_printf("SUCCESS \n");
    }
    else {
        sw_printf("FAILS \n");
    }

    sw_printf("*****Reset the read object***** \n");
    TEE_ResetTransientObject(*second_object);

    open_seek_retval=TEE_OpenPersistentObject(storageID,seek_objectID,
								   objectIDLen,rw_flags,second_object);

    sw_printf("The open Persistent object funtion \
returns value is %x \n \n",open_seek_retval);

    seek_ret_val=TEE_SeekObjectData(*second_object,offset,whence);
    sw_printf("The seek Persistent funtion returns value is %x \n \n",
			   						seek_ret_val);

    sw_printf("*****Reset the seek object***** \n");
    TEE_ResetTransientObject(*second_object);

    open_delete_retval=TEE_OpenPersistentObject(storageID,delete_objectID,
					   			   objectIDLen,r_flags,second_object);

    sw_printf("The open Persistent object funtion returns value is %x \n",
			   					open_delete_retval);

    TEE_CloseAndDeletePersistentObject(*second_object);
	
	sw_printf("*****Reset the close object***** \n");
    TEE_ResetTransientObject(*second_object);

	optest_deleted_retval=TEE_OpenPersistentObject(storageID,delete_objectID,
									objectIDLen,r_flags,second_object);

    sw_printf("The open Persistent object funtion \
returns value is %x \n \n",optest_deleted_retval);

	sw_printf("******TESTING:close and delete persistent object*******\n");
	if(optest_deleted_retval!=1) {
		sw_printf("SUCCESS \n");
	}
	else {
		sw_printf("FAILS\n");
	}
	
	sw_printf("*****Reset the seek object***** \n");
    TEE_ResetTransientObject(*second_object);

    TEE_Attribute* attref;
    attref=(TEE_Attribute*)TEE_Malloc(sizeof(TEE_Attribute),0);
    TEE_InitRefAttribute(attref,0x00000001,p_buffer,
						(size_t)(sw_strlen((char*)p_buffer)));
    TEE_Free((void*)attref);

	TEE_Attribute* attval;
    attval=(TEE_Attribute*)TEE_Malloc(sizeof(TEE_Attribute),0);
    TEE_InitValueAttribute(attval,0x20000000,a_attribute_val,b_attribute_val);
    TEE_Free((void*)attval);

    TEE_Attribute attributes[3];
	attributes[0].attributeID=0x20000000;
    attributes[0].content.value.a=0x0000000A;
    attributes[0].content.value.b=0x0000000B;

	attributes[1].attributeID=0x00000275;
	attributes[1].content.ref.length=(size_t)(sw_strlen((char*)attrsbuffer));
    attributes[1].content.ref.buffer=TEE_Malloc
		(attributes[1].content.ref.length,0);
    TEE_MemCpy(attributes[1].content.ref.buffer,attrsbuffer,
		(u32)(attributes[1].content.ref.length));

	attributes[2].attributeID=0x23425676;
    attributes[2].content.value.a=0x0000001E;
    attributes[2].content.value.b=0x0000001F;

    pop_ret_val=TEE_PopulateTransientObject(*second_object,attributes,
											attribute_cnt);

	sw_printf("the populate transient function returns value is %x \n",
			   pop_ret_val);
	
	sw_printf("*****Reset the populate object***** \n");
    TEE_ResetTransientObject(*second_object);

    TEE_CopyObjectAttributes(*second_object,*first_object);

	sw_printf("*****free the create object by call TEE_FreeTransientObject \
fn***** \n");
    TEE_FreeTransientObject(*first_object);

    sw_printf("*****free the common object by call TEE_FreeTransientObject \
fn***** \n");
    TEE_FreeTransientObject(*second_object);
    sw_printf("--------------Program Successfully Terminated--------------\n");	
}
