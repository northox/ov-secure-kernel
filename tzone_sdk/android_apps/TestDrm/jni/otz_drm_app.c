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
#include <otz_drm_app.h>
#include <utils/Log.h>
/**
* @brief Sample application to show the implementation of Global Platform Client
* API specification.
*
* @return 
*/

static    int valid_session = 0;
static    TEEC_Context context;
static    TEEC_Session session;
static    TEEC_SharedMemory sharedMem;

void smc_drm_close(void)
{
	if(!valid_session)
		return;
    TEEC_ReleaseSharedMemory(&sharedMem);
    TEEC_CloseSession(&session);
    TEEC_FinalizeContext(&context);
	valid_session = 0;
	return;
}

int smc_drm_open(void)
{
    TEEC_Result result;

    TEEC_UUID svc_id = OTZ_SVC_DRM;
#if 1
	cpu_set_t mask; 
	int ret; 

	CPU_ZERO(&mask); 
	CPU_SET((int)0, &mask); 
	ret = sched_setaffinity(0, sizeof mask, &mask);
#endif
    result = TEEC_InitializeContext(
               NULL,
               &context);

    if(result != TEEC_SUCCESS) {
    LOGD("Failed to Init\n");
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
    LOGD("Failed session open\n");
        goto cleanup_2;
    }

    sharedMem.size = 1024 * 2;
    sharedMem.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT ;

    result = TEEC_AllocateSharedMemory(
                &context,
                &sharedMem);

    if(result != TEEC_SUCCESS) {
    LOGD("Failed Alloc\n");
        goto cleanup_4;
    }
	valid_session = 1;
	return 1;

cleanup_7:
cleanup_6:
cleanup_5:
    TEEC_ReleaseSharedMemory(&sharedMem);
cleanup_4:
cleanup_3:
    TEEC_CloseSession(&session);
cleanup_2:
    TEEC_FinalizeContext(&context);
cleanup_1:
    return 0;

}

int smc_drm(void* buffer, int *ret, void* decrypt_buffer, int size){
	
   TEEC_Result result;
	TEEC_Operation operation;

	if(!valid_session)
		return 0;

    memcpy(sharedMem.buffer, decrypt_buffer, size);

    operation.paramTypes = TEEC_PARAM_TYPES(
        TEEC_VALUE_INPUT,
        TEEC_MEMREF_PARTIAL_INPUT,
        TEEC_MEMREF_PARTIAL_OUTPUT,
        TEEC_NONE);

    operation.started = 1;
    operation.params[0].value.a = size;
    operation.params[0].value.b = TEEC_VALUE_UNDEF;


    operation.params[1].memref.parent = &sharedMem;
    operation.params[1].memref.offset = 0;
    operation.params[1].memref.size = size;

    operation.params[2].memref.parent = &sharedMem;
    operation.params[2].memref.offset = 1024;
    operation.params[2].memref.size = size;
    
    result = TEEC_InvokeCommand(
                 &session,
                 OTZ_DRM_CMD_ID_SEND_CMD_SHARED_BUF,
                 &operation,
                 NULL);
    if (result != TEEC_SUCCESS)
    {
    LOGD("Failed Invoc\n");
        return 0;
    }

    
    memcpy(buffer,(void*)((uint32_t)sharedMem.buffer + 1024), size);
    *ret = size;

    return 1;
}
