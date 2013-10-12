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

#define _GNU_SOURCE 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "otz_id.h"
#include "otz_tee_client_api.h"

/**
* @brief Sample application to show the implementation of Global Platform Client
* API specification.
*
* @return 
*/
int main(int argc, char* argv[])
{
	int cpu = 0;
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_SharedMemory sharedMem, sharedMem1, sharedMem2;
    TEEC_SharedMemory nonZeroCopysharedMem;

    TEEC_Result result;

    TEEC_UUID svc_id = OTZ_SVC_ECHO;

    uint32_t len;
    char    testData[256];

	int	check_output;

	cpu_set_t mask; 
	int ret; 

	if (argc > 1 && (strcmp(argv[1], "cpu") == 0)) {
		cpu = atoi(argv[1]);
	}
	CPU_ZERO(&mask); 
	CPU_SET((int)cpu, &mask); 
	ret = sched_setaffinity(0, sizeof mask, &mask);

    result = TEEC_InitializeContext(
               NULL,
               &context);

    if(result != TEEC_SUCCESS) {
        goto cleanup_1;
    }

    result = TEEC_OpenSession(
                &context,
                &session,
                &svc_id,
                TEEC_LOGIN_PUBLIC,
                NULL,
                NULL,
                NULL);

    if(result != TEEC_SUCCESS) {
        goto cleanup_2;
    }

    printf("session id 0x%x\n", session.session_id);

    nonZeroCopysharedMem.buffer = testData;
    nonZeroCopysharedMem.size = 256;
    nonZeroCopysharedMem.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT ;

    result = TEEC_RegisterSharedMemory(
                &context,
                &nonZeroCopysharedMem);

    if(result != TEEC_SUCCESS) {
        goto cleanup_3;
    }


    sharedMem.size = 1024 * 2;
    sharedMem.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT ;

    result = TEEC_AllocateSharedMemory(
                &context,
                &sharedMem);

    if(result != TEEC_SUCCESS) {
        goto cleanup_4;
    }

    sharedMem1.size = 1024;
    sharedMem1.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;

    result = TEEC_AllocateSharedMemory(
                &context,
                &sharedMem1);

    if(result != TEEC_SUCCESS) {
        goto cleanup_5;
    }

    sharedMem2.size = 1024;
    sharedMem2.flags = TEEC_MEM_OUTPUT;

    result = TEEC_AllocateSharedMemory(
                &context,
                &sharedMem2);

    if(result != TEEC_SUCCESS) {
        goto cleanup_6;
    }
#if 1

    strcpy(sharedMem1.buffer, 
        "test global platform client api: full memory reference  testing");
    len = strlen(
        "test global platform client api: full memory reference  testing") + 1;

    operation.paramTypes = TEEC_PARAM_TYPES(
        TEEC_VALUE_INPUT,
        TEEC_MEMREF_WHOLE,
        TEEC_NONE,
        TEEC_NONE);
    
    operation.started = 1;
    operation.params[0].value.a = len;
    operation.params[0].value.b = TEEC_VALUE_UNDEF;


    operation.params[1].memref.parent = &sharedMem1;
    operation.params[1].memref.offset = 0;
    operation.params[1].memref.size = 0;

/*
    operation.params[2].memref.parent = &sharedMem2;
    operation.params[2].memref.offset = 0;
    operation.params[2].memref.size = 0;
*/
#if 1
    result = TEEC_InvokeCommand(
                 &session,
                 OTZ_ECHO_CMD_ID_SEND_CMD_SHARED_BUF,
                 &operation,
                 NULL);
    if (result != TEEC_SUCCESS)
    {
        goto cleanup_7;
    }

    printf("TEEC output buffer %p: %s \n", sharedMem1.buffer, 
            (char*)(sharedMem1.buffer));
#endif

    strcpy(sharedMem.buffer, 
        "test global platform client api: zero copy testing - inout");
    len = strlen(
        "test global platform client api: zero copy testing - inout") + 1;

    operation.paramTypes = TEEC_PARAM_TYPES(
        TEEC_VALUE_INPUT,
        TEEC_MEMREF_PARTIAL_INOUT,
/*        TEEC_MEMREF_PARTIAL_OUTPUT, */
        TEEC_NONE,
        TEEC_NONE);
    
    operation.params[0].value.a = len;
    operation.params[0].value.b = TEEC_VALUE_UNDEF;


    operation.params[1].memref.parent = &sharedMem;
    operation.params[1].memref.offset = 0;
    operation.params[1].memref.size = len;

/*
    operation.params[2].memref.parent = &sharedMem;
    operation.params[2].memref.offset = 512;
    operation.params[2].memref.size = len;
*/

    result = TEEC_InvokeCommand(
                 &session,
                 OTZ_ECHO_CMD_ID_SEND_CMD_SHARED_BUF,
                 &operation,
                 NULL);
    if (result != TEEC_SUCCESS)
    {
        goto cleanup_7;
    }

/*    printf("TEEC output buffer %s \n", (char*)(sharedMem.buffer + 512)); */
    printf("TEEC output buffer %s \n", (char*)(sharedMem.buffer));

/*
    strcpy(sharedMem.buffer, 
        "test global platform client api: zero copy testing");
    len = strlen("test global platform client api: zero copy testing") + 1;
*/
	len = 1024;
	memset(sharedMem.buffer, 'a', len);


    operation.paramTypes = TEEC_PARAM_TYPES(
        TEEC_VALUE_INPUT,
        TEEC_MEMREF_PARTIAL_INPUT,
        TEEC_MEMREF_PARTIAL_OUTPUT,
        TEEC_NONE);
    
    operation.params[0].value.a = len;
    operation.params[0].value.b = TEEC_VALUE_UNDEF;


    operation.params[1].memref.parent = &sharedMem;
    operation.params[1].memref.offset = 0;
    operation.params[1].memref.size = len;


    operation.params[2].memref.parent = &sharedMem;
    operation.params[2].memref.offset = 1024;
    operation.params[2].memref.size = len;

    result = TEEC_InvokeCommand(
                 &session,
                 OTZ_ECHO_CMD_ID_SEND_CMD_SHARED_BUF,
                 &operation,
                 NULL);
    if (result != TEEC_SUCCESS)
    {
        goto cleanup_7;
    }

		
	check_output = memcmp(sharedMem.buffer, (void*)((uint32_t)sharedMem.buffer+1024), len);
	if(check_output != 0)
		printf("Shared memory in and out test failed\n");
	else
		printf("Shared memory in and out test success\n");  

    operation.paramTypes = TEEC_PARAM_TYPES(
        TEEC_VALUE_INPUT,
        TEEC_MEMREF_PARTIAL_INPUT,
        TEEC_MEMREF_PARTIAL_OUTPUT, 
        TEEC_NONE);

    strcpy(testData, "test global platform client api: non-zero copy");
    len = strlen("test global platform client api: non-zero copy") + 1;

    operation.params[0].value.a = len;
    operation.params[0].value.b = TEEC_VALUE_UNDEF;


    operation.params[1].memref.parent = &nonZeroCopysharedMem;
    operation.params[1].memref.offset = 0;
    operation.params[1].memref.size = len;


    operation.params[2].memref.parent = &nonZeroCopysharedMem;
    operation.params[2].memref.offset = 128;
    operation.params[2].memref.size = len;

    result = TEEC_InvokeCommand(
                 &session,
                 OTZ_ECHO_CMD_ID_SEND_CMD,
                 &operation,
                 NULL);
    if (result != TEEC_SUCCESS)
    {
        goto cleanup_7;
    }

    printf("TEEC output buffer %s \n", 
        (char*)((uint32_t)nonZeroCopysharedMem.buffer + 128));


    operation.paramTypes = TEEC_PARAM_TYPES(
        TEEC_VALUE_INPUT,
        TEEC_MEMREF_TEMP_INPUT,
        TEEC_MEMREF_TEMP_OUTPUT,
        TEEC_NONE);
    
    strcpy(testData, 
        "test global platform client api: testing temp memory reference");
    len = strlen(
        "test global platform client api: testing temp memory reference") + 1;

    operation.params[0].value.a = len;
    operation.params[0].value.b = TEEC_VALUE_UNDEF;


    operation.params[1].tmpref.buffer = testData;
    operation.params[1].tmpref.size = len;


    operation.params[2].tmpref.buffer = testData + 128;
    operation.params[2].tmpref.size = len;


    result = TEEC_InvokeCommand(
                 &session,
                 OTZ_ECHO_CMD_ID_SEND_CMD,
                 &operation,
                 NULL);
    if (result != TEEC_SUCCESS)
    {
        goto cleanup_7;
    }

    printf("TEEC output buffer %s \n", (char*)(testData + 128));

#endif
cleanup_7:
    TEEC_ReleaseSharedMemory(&sharedMem2);
cleanup_6:
    TEEC_ReleaseSharedMemory(&sharedMem1);
cleanup_5:
    TEEC_ReleaseSharedMemory(&sharedMem);
cleanup_4:
    TEEC_ReleaseSharedMemory(&nonZeroCopysharedMem);
cleanup_3:
    TEEC_CloseSession(&session);
cleanup_2:
    TEEC_FinalizeContext(&context);
cleanup_1:
    return 0;
}
