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
 * Linux trustzone example application.
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "otz_id.h"
#include "otz_api.h"
#include <otz_app_eg.h>


static const char* otz_service_errorlist[] =
{       
    "Service Success",
    "Service Pending",
    "Service Interrupted",
    "Service Error",
    "Service - Invalid Argument",
    "Service -Invalid Address",
    "Service No Support",
    "Service No Memory",
};

char* otz_strerror(int index) 
{                           
    return (char*)otz_service_errorlist[index]; 
}

/**
 * @brief 
 *
 * @return 
 */
int perform_echo()
{
    echo_data_t echo_data;
    otz_device_t device_otz;
    otz_session_t session_otz;
    otz_operation_t operation_otz;
    otz_return_t ret=0, service_ret;
    otz_shared_mem_t shared_mem;
    void *alloc_buf, *alloc_out_buf;
    unsigned int out_data_len;
    char *out_data;

#if 0
    static const otz_uuid_t defUUID =
    {
        0x79B77788, 0x9789, 0x4A7A,
        {
            0xA2, 0xBE, 0xB6, 0x01, 0x55, 0xEE, 0xF5, 0xF3
        }
    };
#endif

    device_otz.ui_state = OTZ_STATE_UNDEFINED;
    ret = otz_device_open("/dev/otz_client", (void*)O_RDWR, &device_otz);   
    if (ret != OTZ_SUCCESS){
        perror("device open failed\n");
        return 0;        
    }

    printf("fd :%d\n",device_otz.fd);
    
    session_otz.ui_state = OTZ_STATE_UNDEFINED;
    operation_otz.ui_state = OTZ_STATE_UNDEFINED;

    ret = otz_operation_prepare_open(&device_otz, OTZ_SVC_ECHO, NULL, NULL, 
                                        &session_otz, &operation_otz);
    if (ret == OTZ_SUCCESS){
        /*Implement encode fn using OTZ_API for encoding messages*/
    }
    else {
        perror("session open prepare failed\n");
        goto close_device;
    }

    /* Call otz_operation_perform to open session */
    ret = otz_operation_perform(&operation_otz ,&service_ret);
    if(ret != OTZ_SUCCESS){
        if(ret == OTZ_ERROR_SERVICE)
            printf("%s \n",otz_strerror(service_ret));
        else
            perror("session open  failed\n");
        session_otz.ui_state = OTZ_STATE_UNDEFINED;
        operation_otz.ui_state = OTZ_STATE_INVALID;
    }
    otz_operation_release(&operation_otz);
    if(ret != OTZ_SUCCESS){
        goto close_device;
    }

    operation_otz.ui_state = OTZ_STATE_UNDEFINED; 
    ret = otz_operation_prepare_invoke(&session_otz, OTZ_ECHO_CMD_ID_SEND_CMD, 
                                                        NULL, &operation_otz);
    if (ret != OTZ_SUCCESS) {
        perror("command  prepare failed\n");
        goto close_session;
    }
    strcpy(echo_data.data, "test data abcdefgh");
    echo_data.len = strlen(echo_data.data) + 1;

    printf("input data %s\n", echo_data.data);

    otz_encode_uint32(&operation_otz, (void*)&echo_data.len, OTZ_PARAM_IN); 
    if(operation_otz.enc_dec.enc_error_state != OTZ_SUCCESS) {
        perror("encode failed \n");
        otz_operation_release(&operation_otz);
        goto close_session;
    }

    otz_encode_array(&operation_otz, echo_data.data, echo_data.len,
                                                            OTZ_PARAM_IN);
    if(operation_otz.enc_dec.enc_error_state != OTZ_SUCCESS) {
        perror("encode failed \n");
        otz_operation_release(&operation_otz);
        goto close_session;
    }
    otz_encode_array(&operation_otz, echo_data.response, echo_data.len,
                                                            OTZ_PARAM_OUT);

    if(operation_otz.enc_dec.enc_error_state != OTZ_SUCCESS) {
        perror("encode failed \n");
        otz_operation_release(&operation_otz);
        goto close_session;
    }

    ret = otz_operation_perform(&operation_otz ,&service_ret);
    if(ret != OTZ_SUCCESS) {
        if(ret == OTZ_ERROR_SERVICE)
            printf("%s \n",otz_strerror(service_ret));
        else
            perror("operation perform failed \n");

        otz_operation_release(&operation_otz);
        goto close_session;
    }
    out_data = otz_decode_array_space(&operation_otz, &out_data_len);
    if(operation_otz.enc_dec.enc_error_state == OTZ_SUCCESS) {
        printf("out data =  %s and out data len 0x%x\n",
                                    echo_data.response , 
                                    out_data_len);
    }
    else {
        perror("decode failed \n");
        otz_operation_release(&operation_otz);
        goto close_session;
    }
    otz_operation_release(&operation_otz);

    shared_mem.ui_length  = 1024;
    shared_mem.ui_flags  = OTZ_MEM_SERVICE_RW;
    ret = otz_shared_memory_allocate(&session_otz, &shared_mem);
    if(ret != OTZ_SUCCESS) {
        perror("shared memory allocation failed\n");
        goto close_session;
    }

    operation_otz.ui_state = OTZ_STATE_UNDEFINED; 

    ret = otz_operation_prepare_invoke(&session_otz, 
                                OTZ_ECHO_CMD_ID_SEND_CMD_SHARED_BUF, 
                                NULL, &operation_otz);
    if (ret != OTZ_SUCCESS) {
        perror("command  prepare failed\n");
        goto close_session;
    }

    strcpy(shared_mem.p_block, "test_shared_buffer");
    echo_data.len = strlen("test_shared_buffer") + 1;
    
    otz_encode_uint32(&operation_otz, (void*)&echo_data.len, OTZ_PARAM_IN); 
    if(operation_otz.enc_dec.enc_error_state != OTZ_SUCCESS) {
        perror("encode failed \n");
        otz_operation_release(&operation_otz);
        goto close_session;
    }

    otz_encode_memory_reference(&operation_otz, &shared_mem, 
                0, echo_data.len, OTZ_MEM_SERVICE_RO, OTZ_PARAM_IN);
    if(operation_otz.enc_dec.enc_error_state != OTZ_SUCCESS) {
        perror("encode failed \n");
        otz_operation_release(&operation_otz);
        goto close_session;
    }

    otz_encode_memory_reference(&operation_otz, &shared_mem, 
                512, echo_data.len, OTZ_MEM_SERVICE_WO, OTZ_PARAM_OUT); 

    if(operation_otz.enc_dec.enc_error_state != OTZ_SUCCESS) {
        perror("encode failed \n");
        otz_operation_release(&operation_otz);
        goto close_session;
    }
    ret = otz_operation_perform(&operation_otz ,&service_ret);
    if(ret != OTZ_SUCCESS) {
        if(ret == OTZ_ERROR_SERVICE)
            printf("%s \n", otz_strerror(service_ret));
        else
            perror("operation perform failed \n");
        otz_operation_release(&operation_otz);
        goto close_session;
    }

    out_data = otz_decode_array_space(&operation_otz, &out_data_len);
    if(operation_otz.enc_dec.enc_error_state == OTZ_SUCCESS) {
      printf("shared res buf addr %p, out data addr %p and value  \
%s and out data len 0x%x\n", (shared_mem.p_block + 512), 
            out_data, (char*)shared_mem.p_block, out_data_len);
    }
    else {
        perror("decode failed \n");
        otz_operation_release(&operation_otz);
        goto close_session;
    }

    otz_operation_release(&operation_otz);

    operation_otz.ui_state = OTZ_STATE_UNDEFINED; 

    ret = otz_operation_prepare_invoke(&session_otz, 
                                OTZ_ECHO_CMD_ID_SEND_CMD_ARRAY_SPACE, 
                                NULL, &operation_otz);
    if (ret != OTZ_SUCCESS) {
        perror("command  prepare failed\n");
        goto close_session;
    }

    echo_data.len = strlen("test_array_space_buffer") + 1;

    otz_encode_uint32(&operation_otz, (void*)&echo_data.len, OTZ_PARAM_IN); 
    if(operation_otz.enc_dec.enc_error_state != OTZ_SUCCESS) {
        perror("encode failed \n");
        otz_operation_release(&operation_otz);
        goto close_session;
    }

    alloc_buf = otz_encode_array_space(&operation_otz, 
                    echo_data.len, OTZ_PARAM_IN);
    if(operation_otz.enc_dec.enc_error_state != OTZ_SUCCESS) {
        perror("encode failed \n");
        otz_operation_release(&operation_otz);
        goto close_session;
    }

    alloc_out_buf = otz_encode_array_space(&operation_otz, 
                    echo_data.len, OTZ_PARAM_OUT);
    if(operation_otz.enc_dec.enc_error_state != OTZ_SUCCESS) {
        perror("encode failed \n");
        otz_operation_release(&operation_otz);
        goto close_session;
    }

    strcpy(alloc_buf, "test_array_space_buffer");
    ret = otz_operation_perform(&operation_otz ,&service_ret);
    if(ret != OTZ_SUCCESS) {
        if(ret == OTZ_ERROR_SERVICE)
            printf("%s \n",otz_strerror(service_ret));
        else
            perror("operation perform failed \n");

        otz_operation_release(&operation_otz);
        goto close_session;
    }


    printf("next decoder data type 0x%x\n", 
                  otz_decode_get_type(&operation_otz));
    out_data = otz_decode_array_space(&operation_otz, &out_data_len);
    if(operation_otz.enc_dec.enc_error_state == OTZ_SUCCESS) {
      printf("res buf addr %p, out data addr %p and value  \
%s and out data len 0x%x\n", alloc_out_buf, 
                  out_data, (char*)alloc_out_buf, out_data_len);

      printf("next decoder data type 0x%x\n", 
        otz_decode_get_type(&operation_otz));
    }
    else {
        perror("decode failed \n");
        otz_operation_release(&operation_otz);
        goto close_session;
    }
    otz_operation_release(&operation_otz);
    otz_shared_memory_release(&shared_mem);


close_session:    
    operation_otz.ui_state = OTZ_STATE_UNDEFINED;
    ret = otz_operation_prepare_close(&session_otz, &operation_otz);
    if(ret != OTZ_SUCCESS)
    {
        perror("operation prepare close failed \n");
    }

    ret = otz_operation_perform(&operation_otz, &service_ret);
    if(ret != OTZ_SUCCESS)
    {
        if(ret == OTZ_ERROR_SERVICE)
            printf("%s \n",otz_strerror(service_ret));
        else
            perror("operation close failed \n");
        operation_otz.ui_state = OTZ_STATE_INVALID;
    }
    otz_operation_release(&operation_otz);
    /* Cannot perform decode after close operation */  
close_device:
    ret = otz_device_close(&device_otz);
    if (ret != OTZ_SUCCESS){
        printf("device close failed\n");
        return 0;        
    }
    else{
        printf("device close successful\n");
    }

    return 0;
}

/**
 * @brief The function to test mutexes. We do not need any parameters or return
 * values here.
 *
 **/
int test_otz_mutex()
{
    otz_mutex_test_data_t otz_mutex_test_data;
    unsigned char *out_data, input_buf_len = 10;
    char *input_buf = "1023456789";
    unsigned char output_buf[10];
    int output_buf_len = 10;
    otz_device_t device_otz;
    otz_session_t session_otz;
    otz_operation_t operation_otz;
    otz_return_t ret=0, service_ret;

    device_otz.ui_state = OTZ_STATE_UNDEFINED;
    ret = otz_device_open("/dev/otz_client", (void*)O_RDWR, &device_otz);
    if (ret != OTZ_SUCCESS){
        perror("device open failed\n");
        return 0;
    }
    session_otz.ui_state = OTZ_STATE_UNDEFINED;
    operation_otz.ui_state = OTZ_STATE_UNDEFINED;
    ret = otz_operation_prepare_open(&device_otz,OTZ_SVC_MUTEX_TEST, NULL, NULL,
                                        &session_otz, &operation_otz);
    if(ret != OTZ_SUCCESS) {
        goto end_func;
    }
    /* Call tz_operation_perform to open session */
    ret = otz_operation_perform(&operation_otz ,&service_ret);
    if(ret != OTZ_SUCCESS){
        if(ret == OTZ_ERROR_SERVICE)
            printf("%s \n",otz_strerror(service_ret));
        else
            perror("session open  failed\n");
        session_otz.ui_state = OTZ_STATE_UNDEFINED;
    }
    otz_operation_release(&operation_otz);
    if(ret != OTZ_SUCCESS){
        goto end_func;
    }

    operation_otz.ui_state = OTZ_STATE_UNDEFINED;
    ret = otz_operation_prepare_invoke(&session_otz,
                            OTZ_MUTEX_TEST_CMD_ID_TEST,NULL,&operation_otz);
    if (ret != OTZ_SUCCESS) {
        goto handle_error_2;
    }
    memcpy(otz_mutex_test_data.data,input_buf,input_buf_len);
    otz_mutex_test_data.len = input_buf_len;
    otz_encode_uint32(&operation_otz, (void*)&otz_mutex_test_data.len, 
                      OTZ_PARAM_IN);

    if(operation_otz.enc_dec.enc_error_state != OTZ_SUCCESS) {
        perror("encode failed \n");
        goto handle_error_1;
    }

    otz_encode_array(&operation_otz, otz_mutex_test_data.data,
                                        otz_mutex_test_data.len, OTZ_PARAM_IN);
    if(operation_otz.enc_dec.enc_error_state != OTZ_SUCCESS) {
        perror("encode failed \n");
        goto handle_error_1;
    }

    otz_encode_array(&operation_otz, otz_mutex_test_data.response,
                                        otz_mutex_test_data.len, OTZ_PARAM_OUT);
    if(operation_otz.enc_dec.enc_error_state != OTZ_SUCCESS) {
        goto handle_error_1;
    }
    ret = otz_operation_perform(&operation_otz ,&service_ret);
    if(ret != OTZ_SUCCESS) {
        if(ret == OTZ_ERROR_SERVICE)
            printf("%s \n",otz_strerror(service_ret));
        else
            perror("session open  failed\n");
        goto handle_error_1;
    }
    out_data = otz_decode_array_space(&operation_otz,(uint32_t *)&output_buf_len);
    if(operation_otz.enc_dec.enc_error_state != OTZ_SUCCESS) {
        perror("decode error\n");
        goto handle_error_1;
    } else {
        memcpy(output_buf,out_data,output_buf_len);
    }
handle_error_1:
    otz_operation_release(&operation_otz);
handle_error_2:
    operation_otz.ui_state = OTZ_STATE_UNDEFINED;
    ret = otz_operation_prepare_close(&session_otz, &operation_otz);
    if(ret != OTZ_SUCCESS) {
        perror("operation prepare close failed \n");
    }
    ret = otz_operation_perform(&operation_otz, &service_ret);
    if(ret != OTZ_SUCCESS) {
        if(ret == OTZ_ERROR_SERVICE)
            printf("%s \n",otz_strerror(service_ret));
        else
            perror("operation close failed \n");
        operation_otz.ui_state = OTZ_STATE_INVALID;
    }
    otz_operation_release(&operation_otz);
end_func:
    ret = otz_device_close(&device_otz);
    if (ret != OTZ_SUCCESS){
        printf("device close failed\n");
    } else{
        printf("device close successful\n");
    }
    return(0);
}


/**
* @brief 
*
* @param input_buf
* @param input_buf_len
* @param init_vector
* @param init_vector_len
* @param key_buf
* @param key_len
* @param cmd_type
* @param output_buf
* @param output_buf_len
* @param cipher_action
*
* @return 
*/
int perform_crypto(unsigned char *input_buf,int input_buf_len,
					unsigned char *init_vector,int init_vector_len,
						unsigned char *key_buf,int key_len,int cmd_type,
							unsigned char *output_buf, int *output_buf_len,
                                					unsigned char cipher_action)
{
    crypto_data_t crypt_data;
    uint8_t  *out_data,in_data[1];
    otz_device_t device_otz;
    otz_session_t session_otz;
    otz_operation_t operation_otz;
    otz_return_t ret=0, service_ret;

    device_otz.ui_state = OTZ_STATE_UNDEFINED;
    ret = otz_device_open("/dev/otz_client", (void*)O_RDWR, &device_otz);   
    if (ret != OTZ_SUCCESS){
        perror("device open failed\n");
        return 0;        
    }
    session_otz.ui_state = OTZ_STATE_UNDEFINED;
    operation_otz.ui_state = OTZ_STATE_UNDEFINED;
    ret = otz_operation_prepare_open(&device_otz,OTZ_SVC_CRYPT, NULL, NULL, 
                                        &session_otz, &operation_otz);
    if(ret != OTZ_SUCCESS) {
        goto end_func;
    }
    /* Call otz_operation_perform to open session */
    ret = otz_operation_perform(&operation_otz ,&service_ret);
    if(ret != OTZ_SUCCESS){
        if(ret == OTZ_ERROR_SERVICE)
            printf("%s \n",otz_strerror(ret));
        else
            perror("session open  failed\n");
        session_otz.ui_state = OTZ_STATE_UNDEFINED;
    }
    otz_operation_release(&operation_otz);
    if(ret != OTZ_SUCCESS){
        goto end_func;
    }
    operation_otz.ui_state = OTZ_STATE_UNDEFINED; 
    ret = otz_operation_prepare_invoke(&session_otz,cmd_type,NULL,&operation_otz);
    if (ret != OTZ_SUCCESS) {
        goto handle_error_2;
    }
    if(cipher_action != IGNORE_PARAM) {
    	in_data[0] = cipher_action;
		otz_encode_array(&operation_otz, in_data,1,OTZ_PARAM_IN);
		if(operation_otz.enc_dec.enc_error_state != OTZ_SUCCESS) {
			perror("encode failed \n");
			goto handle_error_1;
		}
    }
    if(init_vector != NULL) {
    	in_data[0] = 1;
		otz_encode_array(&operation_otz, in_data,1,OTZ_PARAM_IN);
		if(operation_otz.enc_dec.enc_error_state != OTZ_SUCCESS) {
			perror("encode failed \n");
			goto handle_error_1;
		}
		/*printf("Init vector length is 0x%x \n",init_vector_len);
		for(loop_cntr=0;loop_cntr<init_vector_len;loop_cntr++) {
			if(loop_cntr % 16 == 0) {
				printf("\n");
			}
			printf("0x%x ",init_vector[loop_cntr]);
		}
		printf("\n");*/
		otz_encode_array(&operation_otz, init_vector, init_vector_len,
																OTZ_PARAM_IN);
		if(operation_otz.enc_dec.enc_error_state != OTZ_SUCCESS) {
			perror("encode failed \n");
			goto handle_error_1;
		}
	} else if((init_vector == NULL) && (cipher_action != IGNORE_PARAM)){
    	in_data[0] = 0;
		otz_encode_array(&operation_otz, in_data,1,OTZ_PARAM_IN);
		if(operation_otz.enc_dec.enc_error_state != OTZ_SUCCESS) {
			perror("encode failed \n");
			goto handle_error_1;
		}
	}
    if(key_buf != NULL) {
		/*printf("Input key length is 0x%x \n",key_len);
		for(loop_cntr=0;loop_cntr<key_len;loop_cntr++) {
			if(loop_cntr % 16 == 0) {
				printf("\n");
			}
			printf("0x%x ",key_buf[loop_cntr]);
		}
		printf("\n");*/
		otz_encode_array(&operation_otz, key_buf, key_len,OTZ_PARAM_IN);
		if(operation_otz.enc_dec.enc_error_state != OTZ_SUCCESS) {
			perror("encode failed \n");
			goto handle_error_1;
		}
    }
	/*printf("Input data length is 0x%x \n",input_buf_len);
	for(loop_cntr=0;loop_cntr<input_buf_len;loop_cntr++) {
		if(loop_cntr % 16 == 0) {
			printf("\n");
		}
		printf("0x%x ",input_buf[loop_cntr]);
	}
	printf("\n");*/
    otz_encode_array(&operation_otz,input_buf,input_buf_len,OTZ_PARAM_IN);
    if(operation_otz.enc_dec.enc_error_state != OTZ_SUCCESS) {
        perror("encode failed \n");
        goto handle_error_1;
    }
    otz_encode_array(&operation_otz,crypt_data.response,
    					CRYPT_BUF_LEN,OTZ_PARAM_OUT);
    if(operation_otz.enc_dec.enc_error_state != OTZ_SUCCESS) {
        perror("encode failed \n");
        goto handle_error_1;
    }
    ret = otz_operation_perform(&operation_otz ,&service_ret);
    if(ret != OTZ_SUCCESS) {
            if(ret == OTZ_ERROR_SERVICE)
                printf("%s \n",otz_strerror(service_ret));
            else
                perror("operation perform failed \n");

        goto handle_error_1;
    }
    out_data = otz_decode_array_space(&operation_otz,
                                            (unsigned int*)output_buf_len);
    if(operation_otz.enc_dec.enc_error_state != OTZ_SUCCESS) {
        perror("decode error\n");
        goto handle_error_1;
    } else {
        memcpy(output_buf,out_data,*output_buf_len);
    }
	/*printf("Output data is \n");
	for(loop_cntr=0;loop_cntr<*output_buf_len;loop_cntr++) {
		if(loop_cntr % 16 == 0) {
			printf("\n");
		}
		printf("0x%x ",output_buf[loop_cntr]);
	}
	printf("\n");*/
handle_error_1:
    otz_operation_release(&operation_otz);
handle_error_2:
    operation_otz.ui_state = OTZ_STATE_UNDEFINED;
    ret = otz_operation_prepare_close(&session_otz, &operation_otz);
    if(ret != OTZ_SUCCESS) {
        perror("operation prepare close failed \n");
    }
    ret = otz_operation_perform(&operation_otz, &service_ret);
    if(ret != OTZ_SUCCESS) {
        if(ret == OTZ_ERROR_SERVICE)
            printf("%s \n",otz_strerror(service_ret));
        else
            perror("operation close failed \n");
        operation_otz.ui_state = OTZ_STATE_INVALID;
    }
    otz_operation_release(&operation_otz);
end_func:
    ret =otz_device_close(&device_otz);
    if (ret != OTZ_SUCCESS){
        printf("device close failed\n");
    }
    return(0);

}


#ifdef _CRYPTO_BUILD_
/**
 * @brief
 */
void verify_digest_crypto_libs()
{
	unsigned char md5_input[] = "message digest";
	unsigned char md5_output[MD5_OUTPUT_LEN] = \
								{0xf9,0x6b,0x69,0x7d,0x7c,0xb7,0x93,0x8d, \
								 0x52,0x5a,0x2f,0x31,0xaa,0xf1,0x61,0xd0};
	unsigned char sha1_input[] = "abc";
	unsigned char sha1_output[SHA1_OUTPUT_LEN] = \
								  {0x01,0x64,0xb8,0xa9,0x14,0xcd,0x2a,0x5e, \
								   0x74,0xc4,0xf7,0xff,0x08,0x2c,0x4d,0x97, \
								   0xf1,0xed,0xf8,0x80};
	unsigned char sha224_input[] = "abc";
	unsigned char sha224_output[SHA224_OUTPUT_LEN] = \
								  {0x23,0x09,0x7d,0x22,0x34,0x05,0xd8,0x22,\
								  0x86,0x42,0xa4,0x77,0xbd,0xa2,0x55,0xb3,\
								  0x2a,0xad,0xbc,0xe4,0xbd,0xa0,0xb3,0xf7, \
								  0xe3,0x6c,0x9d,0xa7};
	unsigned char sha256_input[] = "abc";
	unsigned char sha256_output[SHA256_OUTPUT_LEN] = \
									{0xba,0x78,0x16,0xbf,0x8f,0x01,0xcf,0xea,\
									0x41,0x41,0x40,0xde,0x5d,0xae,0x22,0x23,\
									0xb0,0x03,0x61,0xa3,0x96,0x17,0x7a,0x9c,\
									0xb4,0x10,0xff,0x61,0xf2,0x00,0x15,0xad};
	unsigned char sha384_input[] = "abc";
	unsigned char sha384_output[SHA384_OUTPUT_LEN] = \
									{0xcb,0x00,0x75,0x3f,0x45,0xa3,0x5e,0x8b,\
									0xb5,0xa0,0x3d,0x69,0x9a,0xc6,0x50,0x07,\
									0x27,0x2c,0x32,0xab,0x0e,0xde,0xd1,0x63,\
									0x1a,0x8b,0x60,0x5a,0x43,0xff,0x5b,0xed,\
									0x80,0x86,0x07,0x2b,0xa1,0xe7,0xcc,0x23,\
									0x58,0xba,0xec,0xa1,0x34,0xc8,0x25,0xa7};
	unsigned char sha512_input[] = "abc";
	unsigned char sha512_output[SHA512_OUTPUT_LEN] = \
									{0xdd,0xaf,0x35,0xa1,0x93,0x61,0x7a,0xba,\
									0xcc,0x41,0x73,0x49,0xae,0x20,0x41,0x31,\
									0x12,0xe6,0xfa,0x4e,0x89,0xa9,0x7e,0xa2,\
									0x0a,0x9e,0xee,0xe6,0x4b,0x55,0xd3,0x9a,\
									0x21,0x92,0x99,0x2a,0x27,0x4f,0xc1,0xa8,\
									0x36,0xba,0x3c,0x23,0xa3,0xfe,0xeb,0xbd,\
									0x45,0x4d,0x44,0x23,0x64,0x3c,0xe8,0x0e,\
									0x2a,0x9a,0xc9,0x4f,0xa5,0x4c,0xa4,0x9f};
	unsigned char output_buf[CRYPT_BUF_LEN],ignore_param = IGNORE_PARAM;
	int output_buf_len = 0;
	/*short loop_cntr = 0;*/

	printf("Now performing MD5 test \n");
	perform_crypto(md5_input,strlen((const char*)md5_input),NULL,0,NULL,0,
										OTZ_CRYPT_CMD_ID_MD5, output_buf,
												&output_buf_len,ignore_param);
	if((output_buf_len != MD5_OUTPUT_LEN) ||
						(memcmp(md5_output,output_buf,output_buf_len) != 0)) {
		printf("MD5 test failed \n");
	} else {
		printf("MD5 test passed \n");
	}
	printf("Now performing SHA1 test \n");
	perform_crypto(sha1_input,strlen((const char*)sha1_input),NULL,0,NULL,0,
									OTZ_CRYPT_CMD_ID_SHA1,output_buf,
												&output_buf_len,ignore_param);
	if((output_buf_len != SHA1_OUTPUT_LEN) ||
					(memcmp(sha1_output,output_buf,output_buf_len) != 0)) {
		printf("SHA1 test failed \n");
	} else {
		printf("SHA1 test passed \n");
	}
	printf("Now performing SHA224 test \n");
	perform_crypto(sha224_input,strlen((const char*)sha224_input),NULL,0,NULL,0,
										OTZ_CRYPT_CMD_ID_SHA224,output_buf,
												&output_buf_len,ignore_param);
	if((output_buf_len != SHA224_OUTPUT_LEN) ||
					(memcmp(sha224_output,output_buf,output_buf_len) != 0)) {
		printf("SHA224 test failed \n");
	} else {
		printf("SHA224 test passed \n");
	}
	printf("Now performing SHA256 test \n");
	perform_crypto(sha256_input,strlen((const char*)sha256_input),NULL,0,NULL,0,
										OTZ_CRYPT_CMD_ID_SHA256,output_buf,
												&output_buf_len,ignore_param);
	if((output_buf_len != SHA256_OUTPUT_LEN) ||
					(memcmp(sha256_output,output_buf,output_buf_len) != 0)) {
		printf("SHA256 test failed \n");
	} else {
		printf("SHA256 test passed \n");
	}
	printf("Now performing SHA384 test \n");
	perform_crypto(sha384_input,strlen((const char*)sha384_input),NULL,0,NULL,0,
										OTZ_CRYPT_CMD_ID_SHA384,output_buf,
												&output_buf_len,ignore_param);
	if((output_buf_len != SHA384_OUTPUT_LEN) ||
					(memcmp(sha384_output,output_buf,output_buf_len) != 0)) {
		printf("SHA384 test failed \n");
	} else {
		printf("SHA384 test passed \n");
	}
	printf("Now performing SHA512 test \n");
	perform_crypto(sha512_input,strlen((const char*)sha512_input),NULL,0,NULL,0,
											OTZ_CRYPT_CMD_ID_SHA512,output_buf,
												&output_buf_len,ignore_param);
	if((output_buf_len != SHA512_OUTPUT_LEN) ||
					(memcmp(sha512_output,output_buf,output_buf_len) != 0)) {
		printf("SHA512 test failed \n");
	} else {
		printf("SHA512 test passed \n");
	}
	return;
}
/**
 * @brief
 */
void verify_hmac_crypto_libs()
{
	unsigned char hmac_key[HMAC_KEY_LEN] = \
					 	 	 	 	 {0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,\
					 	 	 	 	  0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa};
	unsigned char hmac_input[HMAC_DATA_LEN] = \
									 {0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,\
									 0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,\
									 0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,\
									 0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,\
									 0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,\
									 0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,\
									 0xdd,0xdd};
	unsigned char hmac_md5_output[MD5_OUTPUT_LEN] = \
									 {0x56,0xbe,0x34,0x52,0x1d,0x14,0x4c,0x88,\
									  0xdb,0xb8,0xc7,0x33,0xf0,0xe8,0xb3,0xf6};
	unsigned char hmac_sha1_output[SHA1_OUTPUT_LEN] = \
									{0xd7,0x30,0x59,0x4d,0x16,0x7e,0x35,0xd5,\
									 0x95,0x6f,0xd8,0x00,0x3d,0x0d,0xb3,0xd3,\
									 0xf4,0x6d,0xc7,0xbb};
	unsigned char hmac_sha224_output[SHA224_OUTPUT_LEN] = \
									{0xcb,0xff,0x7c,0x27,0x16,0xbb,0xaa,0x7c,\
									 0x77,0xbe,0xd4,0xf4,0x91,0xd3,0xe8,0x45,\
									 0x6c,0xb6,0xc5,0x74,0xe9,0x2f,0x67,0x2b,\
									 0x29,0x1a,0xcf,0x5b};
	unsigned char hmac_sha256_output[SHA256_OUTPUT_LEN] = \
									{0x7d,0xda,0x3c,0xc1,0x69,0x74,0x3a,0x64,\
									 0x84,0x64,0x9f,0x94,0xf0,0xed,0xa0,0xf9,\
									 0xf2,0xff,0x49,0x6a,0x97,0x33,0xfb,0x79,\
									 0x6e,0xd5,0xad,0xb4,0xa,0x44,0xc3,0xc1};
	unsigned char hmac_sha384_output[SHA384_OUTPUT_LEN] = \
									{0x13,0x83,0xe8,0x2e,0x28,0x28,0x6b,0x91,\
									 0xf4,0xcc,0x7a,0xfb,0xd1,0x3d,0x5b,0x5c,\
									 0x6f,0x88,0x7c,0x5,0xe7,0xc4,0x54,0x24,\
									 0x84,0x4,0x3a,0x37,0xa5,0xfe,0x45,0x80,\
									 0x2a,0x94,0x70,0xfb,0x66,0x3b,0xd7,0xb6,\
									 0x57,0xf,0xe2,0xf5,0x3,0xfc,0x92,0xf5};
	unsigned char hmac_sha512_output[SHA512_OUTPUT_LEN] = \
									{0xad,0x9b,0x5c,0x7d,0xe7,0x26,0x93,0x73,\
									 0x7c,0xd5,0xe9,0xd9,0xf4,0x11,0x70,0xd1,\
									 0x88,0x41,0xfe,0xc1,0x20,0x1c,0x1c,0x1b,\
									 0x2,0xe0,0x5c,0xae,0x11,0x67,0x18,0x0,\
									 0x9f,0x77,0x1c,0xad,0x99,0x46,0xdd,0xbf,\
									 0x7e,0x3c,0xde,0x3e,0x81,0x8d,0x9a,0xe8,\
									 0x5d,0x91,0xb2,0xba,0xda,0xe9,0x41,0x72,\
									 0xd0,0x96,0xa4,0x4a,0x79,0xc9,0x1e,0x86};
	unsigned char output_buf[CRYPT_BUF_LEN],ignore_param=IGNORE_PARAM;
	int output_buf_len = 0;

	printf("Now performing HMAC-MD5 test \n");
	perform_crypto(hmac_input,HMAC_DATA_LEN,NULL,0,
						hmac_key,HMAC_KEY_LEN,OTZ_CRYPT_CMD_ID_HMAC_MD5,
										output_buf,&output_buf_len,ignore_param);
	if((output_buf_len != MD5_OUTPUT_LEN) ||
					(memcmp(hmac_md5_output,output_buf,output_buf_len) != 0)) {
		printf("HMAC-MD5 test failed \n");
	} else {
		printf("HMAC-MD5 test passed \n");
	}
	printf("Now performing HMCA-SHA1 test \n");
	perform_crypto(hmac_input,HMAC_DATA_LEN,NULL,0,
						hmac_key,HMAC_KEY_LEN,OTZ_CRYPT_CMD_ID_HMAC_SHA1,
										output_buf,&output_buf_len,ignore_param);
	if((output_buf_len != SHA1_OUTPUT_LEN) ||
					(memcmp(hmac_sha1_output,output_buf,output_buf_len) != 0)) {
		printf("HMAC-SHA1 test failed \n");
	} else {
		printf("HMAC-SHA1 test passed \n");
	}
	printf("Now performing HMAC-SHA224 test \n");
	perform_crypto(hmac_input,HMAC_DATA_LEN,NULL,0,
						hmac_key,HMAC_KEY_LEN,OTZ_CRYPT_CMD_ID_HMAC_SHA224,
										output_buf,&output_buf_len,ignore_param);
	if((output_buf_len != SHA224_OUTPUT_LEN) ||
				(memcmp(hmac_sha224_output,output_buf,output_buf_len) != 0)) {
		printf("HMAC-SHA224 test failed \n");
	} else {
		printf("HMAC-SHA224 test passed \n");
	}
	printf("Now performing HMAC-SHA256 test \n");
	perform_crypto(hmac_input,HMAC_DATA_LEN,NULL,0,
						hmac_key,HMAC_KEY_LEN,OTZ_CRYPT_CMD_ID_HMAC_SHA256,
										output_buf,&output_buf_len,ignore_param);
	if((output_buf_len != SHA256_OUTPUT_LEN) ||
				(memcmp(hmac_sha256_output,output_buf,output_buf_len) != 0)) {
		printf("HMAC-SHA256 test failed \n");
	} else {
		printf("HMAC-SHA256 test passed \n");
	}
	printf("Now performing HMAC-SHA384 test \n");
	perform_crypto(hmac_input,HMAC_DATA_LEN,NULL,0,
						hmac_key,HMAC_KEY_LEN,OTZ_CRYPT_CMD_ID_HMAC_SHA384,
										output_buf,&output_buf_len,ignore_param);
	if((output_buf_len != SHA384_OUTPUT_LEN) ||
			(memcmp(hmac_sha384_output,output_buf,output_buf_len) != 0)) {
		printf("HMAC-SHA384 test failed \n");
	} else {
		printf("HMAC-SHA384 test passed \n");
	}
	printf("Now performing HMAC-SHA512 test \n");
	perform_crypto(hmac_input,HMAC_DATA_LEN,NULL,0,
						hmac_key,HMAC_KEY_LEN,OTZ_CRYPT_CMD_ID_HMAC_SHA512,
									output_buf,&output_buf_len,ignore_param);
	if((output_buf_len != SHA512_OUTPUT_LEN) ||
				(memcmp(hmac_sha512_output,output_buf,output_buf_len) != 0)) {
		printf("HMAC-SHA512 test failed \n");
	} else {
		printf("HMAC-SHA512 test passed \n");
	}
	return;
}

void verify_cipher_crypto_libs()
{
	unsigned char aes_128_cbc_key[AES_128_CBC_LEN] = \
									{0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,\
									0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c};
	unsigned char aes_128_cbc_init_vector[AES_128_CBC_LEN] = \
									{0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,\
									 0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
	unsigned char aes_128_cbc_plain_text[AES_128_CBC_LEN] = \
									{0x6b,0xc1,0xbe,0xe2,0x2e,0x40,0x9f,0x96,\
									 0xe9,0x3d,0x7e,0x11,0x73,0x93,0x17,0x2a};
	unsigned char aes_128_cbc_cipher_text[AES_128_CBC_LEN] = \
									{0x76,0x49,0xab,0xac,0x81,0x19,0xb2,0x46,\
									 0xce,0xe9,0x8e,0x9b,0x12,0xe9,0x19,0x7d};
	unsigned char aes_128_ecb_key[AES_128_ECB_LEN] = \
									{0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,\
									 0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c};
	unsigned char aes_128_ecb_plain_text[AES_128_ECB_LEN] = \
									{0x6b,0xc1,0xbe,0xe2,0x2e,0x40,0x9f,0x96,\
									 0xe9,0x3d,0x7e,0x11,0x73,0x93,0x17,0x2a};
	unsigned char aes_128_ecb_cipher_text[AES_128_ECB_LEN] = \
									{0x3a,0xd7,0x7b,0xb4,0x0d,0x7a,0x36,0x60,\
									 0xa8,0x9e,0xca,0xf3,0x24,0x66,0xef,0x97};
	unsigned char aes_128_ctr_key[AES_128_CTR_LEN] = \
									{0x7e,0x24,0x06,0x78,0x17,0xfa,0xe0,0xd7,\
									 0x43,0xd6,0xce,0x1f,0x32,0x53,0x91,0x63};
	unsigned char aes_128_ctr_init_vector[AES_128_CTR_LEN] = \
									{0x00,0x6c,0xb6,0xdb,0xc0,0x54,0x3b,0x59,\
									 0xda,0x48,0xd9,0x0b,0x00,0x00,0x00,0x01};
	unsigned char aes_128_ctr_plain_text[AES_128_CTR_LEN*2] = \
									{0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,\
									 0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,\
									 0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,\
									 0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f};
	unsigned char aes_128_ctr_cipher_text[AES_128_CTR_LEN*2] = \
									{0x51,0x04,0xa1,0x06,0x16,0x8a,0x72,0xd9,\
									 0x79,0x0d,0x41,0xee,0x8e,0xda,0xd3,0x88,\
									 0xeb,0x2e,0x1e,0xfc,0x46,0xda,0x57,0xc8,\
									 0xfc,0xe6,0x30,0xdf,0x91,0x41,0xbe,0x28};
	unsigned char aes_128_xts_key[AES_128_XTS_LEN*2] = \
									{0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,\
									0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c,\
									0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,\
									0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c};
	unsigned char aes_128_xts_init_vector[AES_128_XTS_LEN] = \
									{0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,\
									 0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
	unsigned char aes_128_xts_plain_text[AES_128_XTS_LEN] = \
									{0x6b,0xc1,0xbe,0xe2,0x2e,0x40,0x9f,0x96,\
									 0xe9,0x3d,0x7e,0x11,0x73,0x93,0x17,0x2a};
	unsigned char aes_128_xts_cipher_text[AES_128_XTS_LEN] = \
									{0x36,0x75,0xa8,0xac,0x27,0xdd,0x37,0x15,\
									 0x89,0x5d,0x95,0xe8,0x41,0x1c,0x87,0xba};
	unsigned char des_ecb_key[DES_ECB_LEN] = \
									{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	unsigned char des_ecb_plain_text[DES_ECB_LEN] = \
									{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	unsigned char des_ecb_cipher_text[DES_ECB_LEN] = \
									{0x8c,0xa6,0x4d,0xe9,0xc1,0xb1,0x23,0xa7};
	unsigned char des_cbc_key[DES_CBC_LEN] = \
									{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	unsigned char des_cbc_plain_text[DES_CBC_LEN] = \
									{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	unsigned char des_cbc_cipher_text[DES_CBC_LEN] = \
									{0x8c,0xa6,0x4d,0xe9,0xc1,0xb1,0x23,0xa7};
	unsigned char des3_cbc_key[DES3_CBC_LEN*3] = \
									{0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,\
									 0xf1,0xe0,0xd3,0xc2,0xb5,0xa4,0x97,0x86,\
									 0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10};
	unsigned char des3_cbc_init_vector[DES3_CBC_LEN] = \
									{0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10};
	unsigned char des3_cbc_plain_text[DES3_CBC_LEN*4] = \
									{0x37,0x36,0x35,0x34,0x33,0x32,0x31,0x20,\
									 0x4e,0x6f,0x77,0x20,0x69,0x73,0x20,0x74,\
									 0x68,0x65,0x20,0x74,0x69,0x6d,0x65,0x20,\
									 0x66,0x6f,0x72,0x20,0x00,0x00,0x00,0x00};
	unsigned char des3_cbc_cipher_text[DES3_CBC_LEN*4] = \
									{0x3f,0xe3,0x01,0xc9,0x62,0xac,0x01,0xd0,\
									 0x22,0x13,0x76,0x3c,0x1c,0xbd,0x4c,0xdc,\
									 0x79,0x96,0x57,0xc0,0x64,0xec,0xf5,0xd4,\
									 0x1c,0x67,0x38,0x12,0xcf,0xde,0x96,0x75};
	unsigned char des3_ecb_key[DES3_ECB_LEN*3] = \
									{0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,\
									 0xf1,0xe0,0xd3,0xc2,0xb5,0xa4,0x97,0x86,\
									 0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10};
	unsigned char des3_ecb_init_vector[DES3_ECB_LEN] = \
									{0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10};
	unsigned char des3_ecb_plain_text[DES3_ECB_LEN*4] = \
									{0x37,0x36,0x35,0x34,0x33,0x32,0x31,0x20,\
									 0x4e,0x6f,0x77,0x20,0x69,0x73,0x20,0x74,\
									 0x68,0x65,0x20,0x74,0x69,0x6d,0x65,0x20,\
									 0x66,0x6f,0x72,0x20,0x00,0x00,0x00,0x00};
	unsigned char des3_ecb_cipher_text[DES3_CBC_LEN*4] = \
									{0x62,0xc1,0x0c,0xc9,0xef,0xbf,0x15,0xaa,\
									 0xa5,0xae,0x2e,0x48,0x7b,0x69,0x0e,0x56,\
									 0xd8,0xb1,0xdf,0xb8,0xf5,0xc5,0xb2,0x93,\
									 0x85,0x5e,0x77,0xdd,0x90,0x24,0xb1,0xb1};
	unsigned char output_buf[CRYPT_BUF_LEN];
	int output_buf_len = 0;
	unsigned char cipher_action = 0;
	short loop_cntr = 0;

	printf("Now performing AES-128-CBC encryption test \n");
	cipher_action = CIPHER_ENCRYPT;
	perform_crypto(aes_128_cbc_plain_text,AES_128_CBC_LEN,
			aes_128_cbc_init_vector,AES_128_CBC_LEN,
			aes_128_cbc_key,AES_128_CBC_LEN,
			OTZ_CRYPT_CMD_ID_CIPHER_AES_128_CBC,output_buf,&output_buf_len,
																cipher_action);
	if((output_buf_len != AES_128_CBC_LEN) ||
			(memcmp(aes_128_cbc_cipher_text,output_buf,output_buf_len) != 0)) {
		printf("AES-128-CBC encryption test failed \n");
	} else {
		printf("AES-128-CBC encryption test passed \n");
	}
	printf("Now performing AES-128-CBC decryption test \n");
	cipher_action = CIPHER_DECRYPT;
	perform_crypto(aes_128_cbc_cipher_text,AES_128_CBC_LEN,
			aes_128_cbc_init_vector,AES_128_CBC_LEN,
			aes_128_cbc_key,AES_128_CBC_LEN,
			OTZ_CRYPT_CMD_ID_CIPHER_AES_128_CBC,output_buf,&output_buf_len,
																cipher_action);
	if((output_buf_len != AES_128_CBC_LEN) ||
			(memcmp(aes_128_cbc_plain_text,output_buf,output_buf_len) != 0)) {
		printf("AES-128-CBC decryption test failed \n");
	} else {
		printf("AES-128-CBC decryption test passed \n");
	}
	printf("Now performing AES-128-ECB encryption test \n");
	cipher_action = CIPHER_ENCRYPT;
	perform_crypto(aes_128_ecb_plain_text,AES_128_ECB_LEN,NULL,0,
						aes_128_ecb_key,AES_128_ECB_LEN,
			OTZ_CRYPT_CMD_ID_CIPHER_AES_128_ECB,output_buf,&output_buf_len,
																cipher_action);
	if((output_buf_len != AES_128_ECB_LEN) ||
			(memcmp(aes_128_ecb_cipher_text,output_buf,output_buf_len) != 0)) {
		printf("AES-128-ECB encryption test failed \n");
	} else {
		printf("AES-128-ECB encryption test passed \n");
	}
	printf("Now performing AES-128-ECB decryption test \n");
	cipher_action = CIPHER_DECRYPT;
	perform_crypto(aes_128_ecb_cipher_text,AES_128_ECB_LEN,NULL,0,
				aes_128_ecb_key,AES_128_ECB_LEN,
			OTZ_CRYPT_CMD_ID_CIPHER_AES_128_ECB,output_buf,&output_buf_len,
																cipher_action);
	if((output_buf_len != AES_128_ECB_LEN) ||
			(memcmp(aes_128_ecb_plain_text,output_buf,output_buf_len) != 0)) {
		printf("AES-128-ECB decryption test failed \n");
	} else {
		printf("AES-128-ECB decryption test passed \n");
	}
	printf("Now performing AES-128-CTR encryption test \n");
	cipher_action = CIPHER_ENCRYPT;
	perform_crypto(aes_128_ctr_plain_text,AES_128_CTR_LEN*2,
					aes_128_ctr_init_vector,AES_128_CTR_LEN,
						aes_128_ctr_key,AES_128_CTR_LEN,
			OTZ_CRYPT_CMD_ID_CIPHER_AES_128_CTR,output_buf,&output_buf_len,
																cipher_action);
	if((output_buf_len != (AES_128_CTR_LEN*2)) ||
			(memcmp(aes_128_ctr_cipher_text,output_buf,output_buf_len) != 0)) {
		printf("AES-128-CTR encryption test failed \n");
	} else {
		printf("AES-128-CTR encryption test passed \n");
	}
	printf("Now performing AES-128-CTR decryption test \n");
	cipher_action = CIPHER_DECRYPT;
	perform_crypto(aes_128_ctr_cipher_text,AES_128_CTR_LEN*2,
					aes_128_ctr_init_vector,AES_128_CTR_LEN,
					aes_128_ctr_key,AES_128_CTR_LEN,
			OTZ_CRYPT_CMD_ID_CIPHER_AES_128_CTR,output_buf,&output_buf_len,
																cipher_action);
	if((output_buf_len != (AES_128_CTR_LEN*2)) ||
			(memcmp(aes_128_ctr_plain_text,output_buf,output_buf_len) != 0)) {
		printf("AES-128-CTR decryption test failed \n");
	} else {
		printf("AES-128-CTR decryption test passed \n");
	}
	printf("Now performing AES-128-XTS encryption test \n");
	cipher_action = CIPHER_ENCRYPT;
	perform_crypto(aes_128_xts_plain_text,AES_128_XTS_LEN,
			aes_128_xts_init_vector,AES_128_XTS_LEN,
			aes_128_xts_key,AES_128_XTS_LEN*2,
			OTZ_CRYPT_CMD_ID_CIPHER_AES_128_XTS,output_buf,&output_buf_len,
																cipher_action);
	if((output_buf_len != AES_128_XTS_LEN) ||
			(memcmp(aes_128_xts_cipher_text,output_buf,output_buf_len) != 0)) {
		printf("AES-128-XTS encryption test failed \n");
	} else {
		printf("AES-128-XTS encryption test passed \n");
	}
	printf("Now performing AES-128-XTS decryption test \n");
	cipher_action = CIPHER_DECRYPT;
	perform_crypto(aes_128_xts_cipher_text,AES_128_XTS_LEN,
			aes_128_xts_init_vector,AES_128_XTS_LEN,
			aes_128_xts_key,AES_128_XTS_LEN*2,
			OTZ_CRYPT_CMD_ID_CIPHER_AES_128_XTS,output_buf,&output_buf_len,
																cipher_action);
	if((output_buf_len != AES_128_XTS_LEN) ||
			(memcmp(aes_128_xts_plain_text,output_buf,output_buf_len) != 0)) {
		printf("AES-128-XTS decryption test failed \n");
	} else {
		printf("AES-128-XTS decryption test passed \n");
	}
	printf("Now performing DES-CBC encryption test \n");
	cipher_action = CIPHER_ENCRYPT;
	perform_crypto(des_cbc_plain_text,DES_CBC_LEN,NULL,0,
					des_cbc_key,DES_CBC_LEN,
			OTZ_CRYPT_CMD_ID_CIPHER_DES_CBC,output_buf,&output_buf_len,
																cipher_action);
	if((output_buf_len != DES_CBC_LEN) ||
			(memcmp(des_cbc_cipher_text,output_buf,output_buf_len) != 0)) {
		printf("DES-CBC encryption test failed \n");
	} else {
		printf("DES-CBC encryption test passed \n");
	}
	printf("Now performing DES-CBC decryption test \n");
	cipher_action = CIPHER_DECRYPT;
	perform_crypto(des_cbc_cipher_text,DES_CBC_LEN,NULL,0,
					des_cbc_key,DES_CBC_LEN,
			OTZ_CRYPT_CMD_ID_CIPHER_DES_CBC,output_buf,&output_buf_len,
																cipher_action);
	if((output_buf_len != DES_CBC_LEN) ||
			(memcmp(des_cbc_plain_text,output_buf,output_buf_len) != 0)) {
		printf("DES-CBC decryption test failed \n");
	} else {
		printf("DES-CBC decryption test passed \n");
	}
	printf("Now performing DES-ECB encryption test \n");
	cipher_action = CIPHER_ENCRYPT;
	perform_crypto(des_ecb_plain_text,DES_ECB_LEN,NULL,0,
					des_ecb_key,DES_ECB_LEN,
			OTZ_CRYPT_CMD_ID_CIPHER_DES_ECB,output_buf,&output_buf_len,
																cipher_action);
	if((output_buf_len != DES_ECB_LEN) ||
			(memcmp(des_ecb_cipher_text,output_buf,output_buf_len) != 0)) {
		printf("DES-ECB encryption test failed \n");
	} else {
		printf("DES-ECB encryption test passed \n");
	}
	printf("Now performing DES-ECB decryption test \n");
	cipher_action = CIPHER_DECRYPT;
	perform_crypto(des_ecb_cipher_text,DES_ECB_LEN,NULL,0,
					des_ecb_key,DES_ECB_LEN,
			OTZ_CRYPT_CMD_ID_CIPHER_DES_ECB,output_buf,&output_buf_len,
																cipher_action);
	if((output_buf_len != DES_ECB_LEN) ||
			(memcmp(des_ecb_plain_text,output_buf,output_buf_len) != 0)) {
		printf("DES-ECB decryption test failed \n");
	} else {
		printf("DES-ECB decryption test passed \n");
	}
	printf("Now performing 3-DES-CBC encryption test \n");
	cipher_action = CIPHER_ENCRYPT;
	perform_crypto(des3_cbc_plain_text,DES3_CBC_LEN*4,
					des3_cbc_init_vector,DES3_CBC_LEN,
					des3_cbc_key,DES3_CBC_LEN*3,
			OTZ_CRYPT_CMD_ID_CIPHER_DES3_CBC,output_buf,&output_buf_len,
																cipher_action);
	if((output_buf_len != (DES3_CBC_LEN*4)) ||
			(memcmp(des3_cbc_cipher_text,output_buf,output_buf_len) != 0)) {
		printf("DES-CBC encryption test failed \n");
	} else {
		printf("DES-CBC encryption test passed \n");
	}
	printf("Now performing 3-DES-CBC decryption test \n");
	cipher_action = CIPHER_DECRYPT;
	perform_crypto(des3_cbc_cipher_text,DES3_CBC_LEN*4,
						des3_cbc_init_vector,DES3_CBC_LEN,
						des3_cbc_key,DES3_CBC_LEN*3,
			OTZ_CRYPT_CMD_ID_CIPHER_DES3_CBC,output_buf,&output_buf_len,
																cipher_action);
	if((output_buf_len != (DES3_CBC_LEN*4)) ||
			(memcmp(des3_cbc_plain_text,output_buf,output_buf_len) != 0)) {
		printf("DES-CBC decryption test failed \n");
	} else {
		printf("DES-CBC decryption test passed \n");
	}
	printf("Now performing 3-DES-ECB encryption test \n");
	cipher_action = CIPHER_ENCRYPT;
	perform_crypto(des3_ecb_plain_text,DES3_ECB_LEN*4,
					des3_ecb_init_vector,DES3_ECB_LEN,
					des3_ecb_key,DES3_ECB_LEN*3,
			OTZ_CRYPT_CMD_ID_CIPHER_DES3_ECB,output_buf,&output_buf_len,
																cipher_action);
	if((output_buf_len != (DES3_ECB_LEN*4)) ||
			(memcmp(des3_ecb_cipher_text,output_buf,output_buf_len) != 0)) {
		printf("3-DES-ECB encryption test failed \n");
	} else {
		printf("3-DES-ECB encryption test passed \n");
	}
	printf("Now performing 3-DES-ECB decryption test \n");
	cipher_action = CIPHER_DECRYPT;
	perform_crypto(des3_ecb_cipher_text,DES3_ECB_LEN*4,
						des3_ecb_init_vector,DES3_ECB_LEN,
						des3_ecb_key,DES3_ECB_LEN*3,
			OTZ_CRYPT_CMD_ID_CIPHER_DES3_ECB,output_buf,&output_buf_len,
																cipher_action);
	if((output_buf_len != (DES3_ECB_LEN*4)) ||
			(memcmp(des3_ecb_plain_text,output_buf,output_buf_len) != 0)) {
		printf("3-DES-ECB decryption test failed \n");
	} else {
		printf("3-DES-ECB decryption test passed \n");
	}
	return;
}

#endif


#ifdef OTZONE_ASYNC_NOTIFY_SUPPORT
/**
 * @brief The function to test notification from secure world. 
 *
 **/
int test_notification()
{
    echo_data_t echo_data;
    otz_device_t device_otz;
    otz_session_t session_otz;
    otz_operation_t operation_otz;
    otz_return_t ret=0, service_ret;
    unsigned int out_data_len;
    char *out_data;


    device_otz.ui_state = OTZ_STATE_UNDEFINED;
    ret = otz_device_open("/dev/otz_client", (void*)O_RDWR, &device_otz);
    if (ret != OTZ_SUCCESS){
        perror("device open failed\n");
        return 0;
    }
    session_otz.ui_state = OTZ_STATE_UNDEFINED;
    operation_otz.ui_state = OTZ_STATE_UNDEFINED;
    ret = otz_operation_prepare_open(&device_otz, OTZ_SVC_ECHO, NULL, NULL,
                                        &session_otz, &operation_otz);
    if(ret != OTZ_SUCCESS) {
        goto end_func;
    }
    /* Call tz_operation_perform to open session */
    ret = otz_operation_perform(&operation_otz ,&service_ret);
    if(ret != OTZ_SUCCESS){
        if(ret == OTZ_ERROR_SERVICE)
            printf("%s \n",otz_strerror(service_ret));
        else
            perror("session open  failed\n");
        session_otz.ui_state = OTZ_STATE_UNDEFINED;
    }
    otz_operation_release(&operation_otz);
    if(ret != OTZ_SUCCESS){
        goto end_func;
    }

    operation_otz.ui_state = OTZ_STATE_UNDEFINED;
    ret = otz_operation_prepare_invoke(&session_otz,
                            OTZ_ECHO_CMD_ID_TEST_ASYNC_SEND_CMD, NULL,&operation_otz);
    if (ret != OTZ_SUCCESS) {
        goto handle_error_2;
    }
    memcpy(echo_data.data,"test notification", strlen("test notification")+1);
    echo_data.len = strlen("test notification")+1;
    otz_encode_uint32(&operation_otz, (void*)&echo_data.len, 
                      OTZ_PARAM_IN);

    if(operation_otz.enc_dec.enc_error_state != OTZ_SUCCESS) {
        perror("encode failed \n");
        goto handle_error_1;
    }

    otz_encode_array(&operation_otz, echo_data.data,
                                        echo_data.len, OTZ_PARAM_IN);
    if(operation_otz.enc_dec.enc_error_state != OTZ_SUCCESS) {
        perror("encode failed \n");
        goto handle_error_1;
    }

    otz_encode_array(&operation_otz, echo_data.response,
                                        echo_data.len, OTZ_PARAM_OUT);
    if(operation_otz.enc_dec.enc_error_state != OTZ_SUCCESS) {
        goto handle_error_1;
    }
    ret = otz_operation_perform(&operation_otz, &service_ret);
    if(ret != OTZ_SUCCESS) {
        if(ret == OTZ_ERROR_SERVICE)
            printf("%s \n",otz_strerror(service_ret));
        else
            perror("session open  failed\n");
        goto handle_error_1;
    }
    out_data = otz_decode_array_space(&operation_otz,(uint32_t *)&out_data_len);
    if(operation_otz.enc_dec.enc_error_state != OTZ_SUCCESS) {
        perror("decode error\n");
        goto handle_error_1;
    }

handle_error_1:
    otz_operation_release(&operation_otz);
handle_error_2:
    operation_otz.ui_state = OTZ_STATE_UNDEFINED;
    ret = otz_operation_prepare_close(&session_otz, &operation_otz);
    if(ret != OTZ_SUCCESS) {
        perror("operation prepare close failed \n");
    }
    ret = otz_operation_perform(&operation_otz, &service_ret);
    if(ret != OTZ_SUCCESS) {
        if(ret == OTZ_ERROR_SERVICE)
            printf("%s \n",otz_strerror(service_ret));
        else
            perror("operation close failed \n");
        operation_otz.ui_state = OTZ_STATE_INVALID;
    }
    otz_operation_release(&operation_otz);
end_func:
    ret = otz_device_close(&device_otz);
    if (ret != OTZ_SUCCESS){
        printf("device close failed\n");
    } else{
        printf("device close successful\n");
    }
    return(0);
}
#endif

/**
 * @brief 
 *
 * @return 
 */
int main()
{

/*    char *input_string = "This is a test for encryption/decryption";
    char output_buf[DATA_BUF_LEN];

    unsigned short input_buf_len=0,output_buf_len=0,loop_cntr=0;
*/

    perform_echo();
#ifdef _CRYPTO_BUILD_
	verify_digest_crypto_libs();
	verify_hmac_crypto_libs();
	verify_cipher_crypto_libs();
#endif

    /*input_buf_len = strlen(input_string);
    printf("Input for encryption is %s",input_string);
    perform_crypto((unsigned char*)input_string,input_buf_len,
                    OTZ_CRYPT_CMD_ID_ENCRYPT,(unsigned char*)output_buf,
                                                        (int*)&output_buf_len);
    printf("Encrypted string is ");
    for(loop_cntr=0;loop_cntr<output_buf_len;loop_cntr++) {
        printf("0x%x ",output_buf[loop_cntr]);
    }
    printf("\n");

    if(output_buf_len > 0) {
        printf("Now sending the encrypted string for decryption \n");
        input_buf_len = output_buf_len;
        perform_crypto((unsigned char*)output_buf,input_buf_len,
                        OTZ_CRYPT_CMD_ID_DECRYPT,(unsigned char*)output_buf,
                                                            (int*)&output_buf_len);
        output_buf[output_buf_len] = '\0';
        printf("Decrypted string is %s \n",output_buf);
    }*/
    printf("Creating task for testing otz mutexes \n");
    test_otz_mutex();
    printf("Mutex testing finished \n");

#ifdef OTZONE_ASYNC_NOTIFY_SUPPORT
    printf("Creating task for testing secure kernel notification feature\n");
    test_notification();
    printf("Notification testing finished \n");
#endif

    return(0);
}


