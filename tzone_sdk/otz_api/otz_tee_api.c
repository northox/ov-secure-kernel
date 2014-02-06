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
 * Trustzone client API implementation.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>

#include <otz_tee_client_api.h>
#include <otz_client.h>
#include <otz_id.h>
#include <otz_common.h>


/**
* @brief TEEC error string values
*
*/
static const char* TEEC_errorlist[] =
{
    "The operation succeeded",
    "Non-specific cause",
    "Access privileges are not sufficient",
    "The operation was cancelled",
    "Concurrent accesses caused conflict",
    "Too much data for the requested operation was passed",
    "Input data was of invalid format",
    "Input parameters were invalid",
    "Operation is not valid in the current state",
    "The requested data item is not found",
    "The requested operation should exist but is not yet implemented",
    "The requested operation is valid but is not supported in this \
Implementation",
    "Expected data was missing",
    "System ran out of resources",
    "The system is busy working on something else",
    "Communication with a remote party failed",
    "A security fault was detected",
    "The supplied buffer is too short for the generated output",
};


/**
* @brief Service error string values
*/
static const char* TEEC_service_errorlist[] =
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

/**
* @brief Returns error string.
* 
* This function returns the error string value based on error number and 
* return origin.
*
* @param error:  Error number.
* @param returnOrigin:  Origin of the return.
*
* @return char*: Error string value.
* 
*/
char* TEEC_GetError(int error, int returnOrigin) 
{                        
    if(returnOrigin == TEEC_ORIGIN_TRUSTED_APP)   
        return (char*)TEEC_service_errorlist[error];  
    else
        return (char*)TEEC_errorlist[error];  
}

/**
* @brief Initialize Context
*
* @param name: A zero-terminated string that describes the TEE to connect to. 
* If this parameter is set to NULL the Implementation MUST select a default TEE.
* 
* @param context: A TEEC_Context structure that MUST be initialized by the 
* Implementation.
*
* @return TEEC_Result:
* TEEC_SUCCESS: The initialization was successful. \n
* TEEC_ERROR_*: An implementation-defined error code for any other error.
*/
TEEC_Result TEEC_InitializeContext(
    const char*   name,
    TEEC_Context* context)
{
    int ret=0;
    char temp_name[256];

    if(context == NULL) {
        printf("TEEC_InitializeContext : Context is null\n");
        return TEEC_ERROR_BAD_PARAMETERS;
    }
    if(name == NULL) {
#ifdef OTZ_DEBUG
        printf("%s is assigned as default context\n", 
            OTZ_CLIENT_FULL_PATH_DEV_NAME );
#endif
        strcpy(temp_name, OTZ_CLIENT_FULL_PATH_DEV_NAME);
    }
    else {
        strcpy(temp_name, name);
    }
    ret = open(temp_name, O_RDWR); 
    if( ret == -1){
        perror("TEEC_InitializeContext : device open failed\n");
        context->s_errno = errno;
        return TEEC_ERROR_GENERIC;
    }
    else{
        context->fd = ret ;
        context->session_count = 0;
        context->shared_mem_cnt = 0;
        INIT_LIST_HEAD(&context->shared_mem_list);
    }

#ifdef OTZ_DEBUG
    printf("TEEC_InitializeContext success\n");
#endif
    return TEEC_SUCCESS;

}

/**
* @brief Finalizes an initialized TEE context.
*
* @param context: An initialized TEEC_Context structure which is to be 
* finalized.
*/
void TEEC_FinalizeContext(TEEC_Context* context)
{
    if(!context)
    {
        return;
    }
    if(context->session_count != 0) {
        printf("warning: pending open sessions %d\n", 
                    context->session_count);
    }

    if(context->shared_mem_cnt != 0) {
        printf("warning: unreleased shared memory blocks %d\n", 
                    context->shared_mem_cnt);
    }

    if( (context->session_count == 0) && (context->shared_mem_cnt == 0)) {
        printf("device closed \n");
        close(context->fd);
        context->fd = 0;
    }

    return;
}



/**
* @brief  Allocate a shared memory block.
*
* @param context: Pointer to the context
* @param sharedMem: Pointer to the shared memory
*
* @return TEEC_Result:
* TEEC_SUCCESS: The allocation was successful. \n
* TEEC_ERROR_*: An implementation-defined error code for any other error.
*/
TEEC_Result TEEC_AllocateSharedMemory(
    TEEC_Context*      context,
    TEEC_SharedMemory* sharedMem)
{
    int mmap_flags;

    if(context == NULL || sharedMem == NULL ) {
        printf("TEEC_AllocateSharedMemory : Error Illegal argument\n");
        return TEEC_ERROR_BAD_PARAMETERS;
    }

    if((sharedMem->size == 0) ||
        ((sharedMem->flags != TEEC_MEM_INPUT) && 
         (sharedMem->flags != TEEC_MEM_OUTPUT) && 
         (sharedMem->flags != (TEEC_MEM_INPUT | TEEC_MEM_OUTPUT)))) {
        printf("TEEC_AllocateSharedMemory : Error Illegal argument\n");
        return TEEC_ERROR_BAD_PARAMETERS;
    }

    sharedMem->buffer = NULL;

    mmap_flags = PROT_READ | PROT_WRITE;

    sharedMem->buffer = mmap(0, sharedMem->size,
                        mmap_flags , MAP_SHARED,
                        context->fd, 0);

    if(sharedMem->buffer == MAP_FAILED) {
        perror("TEEC_AllocateSharedMemory - mmap failed\n"); 
        sharedMem->s_errno = errno;
        sharedMem->buffer = NULL;
        return TEEC_ERROR_OUT_OF_MEMORY;
    }
    sharedMem->allocated = 1;
    sharedMem->context = context;
    sharedMem->operation_count = 0;

    INIT_LIST_HEAD(&sharedMem->head_ref);
    list_add_tail(&context->shared_mem_list, &sharedMem->head_ref);
    context->shared_mem_cnt++;
    return TEEC_SUCCESS;

}

/**
* @brief Register a allocated shared memory block.
*
* @param context: Pointer to the context
* @param sharedMem: Pointer to the shared memory
*
* @return TEEC_Result:
* TEEC_SUCCESS: The device was successfully opened. \n
* TEEC_ERROR_*: An implementation-defined error code for any other error.
*/
TEEC_Result TEEC_RegisterSharedMemory(
    TEEC_Context*      context,
    TEEC_SharedMemory* sharedMem)
{
    if(context == NULL || sharedMem == NULL ) {
        printf("TEEC_AllocateSharedMemory : Error Illegal argument\n");
        return TEEC_ERROR_BAD_PARAMETERS;
    }

    if((sharedMem->size == 0) ||
        ((sharedMem->flags != TEEC_MEM_INPUT) && 
         (sharedMem->flags != TEEC_MEM_OUTPUT) && 
         (sharedMem->flags != (TEEC_MEM_INPUT | TEEC_MEM_OUTPUT)))) {
        printf("TEEC_AllocateSharedMemory : Error Illegal argument\n");
        return TEEC_ERROR_BAD_PARAMETERS;
    }

    if(sharedMem->buffer == NULL) {
        printf("TEEC_RegisterSharedMemory :shared memory buffer is NULL\n");
        return TEEC_ERROR_BAD_PARAMETERS;
    }
    sharedMem->allocated = 0;
    sharedMem->context = context;
    sharedMem->operation_count = 0;

    INIT_LIST_HEAD(&sharedMem->head_ref);
    list_add_tail(&context->shared_mem_list, &sharedMem->head_ref);
    context->shared_mem_cnt++;
    return TEEC_SUCCESS;

}

/**
* @brief Release a shared memory block
*
* @param sharedMem: Pointer to the shared memory
*/
void TEEC_ReleaseSharedMemory(
    TEEC_SharedMemory* sharedMem)
{
    struct list *l;
    TEEC_SharedMemory* tempSharedMem;
    int found = 0;

    if(sharedMem == NULL){
        return;
    }
    if(sharedMem->operation_count != 0) {
        printf("TEEC_ReleaseSharedMemory - pending operations count %d\n", 
                sharedMem->operation_count);
        return;
    }

    if(sharedMem->allocated) {
        munmap(sharedMem->buffer, sharedMem->size);
    }

    sharedMem->buffer = NULL;
    sharedMem->size = 0;

    list_for_each(l, &sharedMem->context->shared_mem_list) {
        tempSharedMem = list_entry(l, TEEC_SharedMemory, head_ref);
        if (tempSharedMem == sharedMem) {
            found = 1;
            break;
        }
    }

    if(found) {
        list_del(&sharedMem->head_ref);
        sharedMem->context->shared_mem_cnt--;
    }
    sharedMem->context = NULL;
}

/**
* @brief Open a session with a Trusted application
*
* @param context: Pointer to the context
* @param session: Pointer to the session
* @param destination: Service UUID
* @param connectionMethod: Connection method
* @param connectionData: Connection data used for authentication
* @param operation: Pointer to optional operation structure
* @param returnOrigin: Pointer to the return origin
*
* @return TEEC_Result:
* TEEC_SUCCESS: The session was successfully opened. \n
* TEEC_ERROR_*: An implementation-defined error code for any other error.
*/
TEEC_Result TEEC_OpenSession (
    TEEC_Context*    context,
    TEEC_Session*    session,
    const TEEC_UUID* destination,
    uint32_t         connectionMethod,
    const void*      connectionData,
    TEEC_Operation* operation,
    uint32_t*        returnOrigin)
{
    int ret;
    struct ser_ses_id ses_open;

    if((context == NULL) || (session == NULL) ||
        (destination == NULL)) {
        printf("TEEC_OpenSession : Error Illegal argument\n");
        return TEEC_ERROR_BAD_PARAMETERS;
    }

    switch(connectionMethod) {
        case TEEC_LOGIN_PUBLIC: {
            if(connectionData != NULL) {
                printf("TEEC_OpenSession : connection method requires \
empty connection data\n");
                return TEEC_ERROR_BAD_PARAMETERS;            
            }
            break;
        }
        case TEEC_LOGIN_USER:
        case TEEC_LOGIN_APPLICATION:
        case TEEC_LOGIN_USER_APPLICATION: {
            if(connectionData != NULL) {
                printf("TEEC_OpenSession : connection method requires \
empty connection data\n");
                return TEEC_ERROR_BAD_PARAMETERS;            
            }
            printf("TEEC_OpenSession : connection method \
is not implemented \n");
            return TEEC_ERROR_NOT_IMPLEMENTED;
            break;
        }
        case TEEC_LOGIN_GROUP:
        case TEEC_LOGIN_GROUP_APPLICATION: {
            if(connectionData == NULL) {
                printf("TEEC_OpenSession : connection method requires \
valid connection data\n");
                return TEEC_ERROR_BAD_PARAMETERS;            
            }
            return TEEC_ERROR_NOT_IMPLEMENTED;
            break;
        }
    }

    ses_open.service_id = *destination;

    ret = ioctl(context->fd,
                OTZ_CLIENT_IOCTL_SES_OPEN_REQ, &ses_open);    

    if(ret < 0){
        if(returnOrigin)
            *returnOrigin = TEEC_ORIGIN_API;

        context->s_errno = errno;
        
        perror("TEEC_OpenSession: Session client open request failed\n");
        if(ret == -ENOMEM)
            return TEEC_ERROR_OUT_OF_MEMORY;
        if(ret == -EFAULT)
         return  TEEC_ERROR_ACCESS_DENIED;
        if(ret == -EINVAL)
         return  TEEC_ERROR_BAD_PARAMETERS;
        
        return TEEC_ERROR_GENERIC;
    }
    else if(ret > 0) {
        if(returnOrigin)
            *returnOrigin = TEEC_ORIGIN_TRUSTED_APP;

        printf("TEEC_OpenSession: service return error -  %s\n",
            TEEC_GetError(ret, TEEC_ORIGIN_TRUSTED_APP) );
        return ret;
    }

    context->session_count++;
    session->operation_cnt = 0;
    session->session_id = ses_open.session_id;
    session->service_id = *destination;
    session->device = context;
    
    return TEEC_SUCCESS;
}

/**
* @brief Closes a session which has been opened with trusted application
*
* @param session: Pointer to the session structure
*/
void TEEC_CloseSession (
    TEEC_Session* session)
{
    int ret = 0;
    struct ser_ses_id ses_close;

    if(session == NULL) {
        printf("TEEC_CloseSession: Warning: Session pointer is NULL\n");
        return;
    }

    if(session->operation_cnt) {
        printf("TEEC_CloseSession: Warning: Pending operations %d\n", 
            session->operation_cnt);
        return;
    }
    ses_close.service_id = session->service_id ;
    ses_close.session_id = session->session_id ;

    ret = ioctl(session->device->fd,
                OTZ_CLIENT_IOCTL_SES_CLOSE_REQ, &ses_close);

    if(ret == 0){
        session->device->session_count--;
        session->device = NULL;
        session->session_id = -1;
    }
    else {
        perror("TEEC_CloseSession: Session client close request failed\n");
    }
}

/**
* @brief Invokes a command within the specified session
*
* @param session: Pointer to session
* @param commandID: Command ID 
* @param operation: Pointer to operation structure
* @param returnOrigin: Pointer to the return origin
*
* @return TEEC_Result:
* TEEC_SUCCESS: The command was successfully invoked. \n
* TEEC_ERROR_*: An implementation-defined error code for any other error.
*/
TEEC_Result TEEC_InvokeCommand(
    TEEC_Session*     session,
    uint32_t          commandID,
    TEEC_Operation*   operation,
    uint32_t*         returnOrigin)
{
	int ret = TEEC_SUCCESS;
    int rel_ret;
    unsigned char inout = 0; /* in = 0; out = 1; inout = 2 */
    uint32_t param_types[4], param_count;
    struct otz_client_encode_cmd enc;

    if(session == NULL) {
        printf("TEEC_InvokeCommand : Illegal argument\n");    
        return TEEC_ERROR_BAD_PARAMETERS;
    }

    enc.encode_id = -1;
    enc.cmd_id = commandID;
    enc.service_id = session->service_id ;
    enc.session_id = session->session_id ;

/* Need to support cancellation in future releases */
    if(operation && !operation->started) {
        printf(
            "TEEC_InvokeCommand : cancellation support not yet implemented\n");   
        return TEEC_ERROR_NOT_IMPLEMENTED;
    }

    session->operation_cnt++;
/* Encode the data */
    if(operation->paramTypes != 0) {
        param_types[0] = operation->paramTypes & 0xf;
        param_types[1] = (operation->paramTypes >> 4) & 0xf;        
        param_types[2] = (operation->paramTypes >> 8) & 0xf;        
        param_types[3] = (operation->paramTypes >> 12) & 0xf;        

        for(param_count = 0; param_count < 4; param_count++) {
            if( (param_types[param_count] == TEEC_VALUE_INPUT) ||
                (param_types[param_count] == TEEC_VALUE_INOUT) ||
                (param_types[param_count] == TEEC_VALUE_OUTPUT)) {
                enc.data = (void*)&operation->params[param_count].value.a;
                enc.len  = sizeof(uint32_t);

                if(param_types[param_count] == TEEC_VALUE_INPUT)
                    inout = 0;
                else if(param_types[param_count] == TEEC_VALUE_OUTPUT)
                    inout = 1;
                else if(param_types[param_count] == TEEC_VALUE_INOUT)
                    inout = 2;
                                
                if((inout == 0) | (inout == 2))
                    enc.param_type = OTZC_PARAM_IN; 
                else if(inout == 1)
                    enc.param_type = OTZC_PARAM_OUT; 

                ret = ioctl(session->device->fd, 
                            OTZ_CLIENT_IOCTL_ENC_UINT32, &enc);
                if (ret) {
                    if(returnOrigin)
                        *returnOrigin = TEEC_ORIGIN_API;
                    session->s_errno = errno;
                    perror("TEEC_InvokeCommand: encoding value data failed\n");
                    break;
                }

                if(inout == 2) {
                    enc.param_type = OTZC_PARAM_OUT; 
                    ret = ioctl(session->device->fd, 
                                OTZ_CLIENT_IOCTL_ENC_UINT32, &enc);
                    if (ret) {
                        if(returnOrigin)
                            *returnOrigin = TEEC_ORIGIN_API;
                        session->s_errno = errno;
                        perror(
                            "TEEC_InvokeCommand: encoding value data failed\n");
                        break;
                    }
                }


                if(TEEC_VALUE_UNDEF != 
                    operation->params[param_count].value.b) {
                    enc.data = (void*)&operation->params[param_count].value.b;
                    enc.len  = sizeof(uint32_t);
                    if((inout == 0) | (inout == 2))
                        enc.param_type = OTZC_PARAM_IN; 
                    else if(inout == 1)
                        enc.param_type = OTZC_PARAM_OUT; 

                    ret = ioctl(session->device->fd, 
                                OTZ_CLIENT_IOCTL_ENC_UINT32, &enc);
                    if (ret) {
                        if(returnOrigin)
                            *returnOrigin = TEEC_ORIGIN_API;
                        session->s_errno = errno;
                        perror(
                            "TEEC_InvokeCommand: encoding value data failed\n");
                        break;
                    }
                    if(inout == 2) {
                        enc.param_type = OTZC_PARAM_OUT; 
                        ret = ioctl(session->device->fd, 
                                    OTZ_CLIENT_IOCTL_ENC_UINT32, &enc);
                        if (ret) {
                            if(returnOrigin)
                                *returnOrigin = TEEC_ORIGIN_API;
                            session->s_errno = errno;
                            perror("TEEC_InvokeCommand: \
encoding value data failed\n");

                            break;
                        }
                    }
                }
            } /* end if for TEEC_VALUE check */
            else if( (param_types[param_count] == TEEC_MEMREF_WHOLE) ||
                (param_types[param_count] == TEEC_MEMREF_PARTIAL_INPUT) ||
                (param_types[param_count] == TEEC_MEMREF_PARTIAL_INOUT) ||
                (param_types[param_count] == TEEC_MEMREF_PARTIAL_OUTPUT)) {

                if(!operation->params[param_count].memref.parent) {
                    if(returnOrigin)
                        *returnOrigin = TEEC_ORIGIN_API;
                    ret = TEEC_ERROR_NO_DATA;
                    printf("TEEC_InvokeCommand: \
memory reference parent is NULL\n");

                    break; 
                }

                if(!operation->params[param_count].memref.parent->buffer) {
                    if(returnOrigin)
                        *returnOrigin = TEEC_ORIGIN_API;
                    ret = TEEC_ERROR_NO_DATA;
                    printf("TEEC_InvokeCommand: \
memory reference parent data is NULL\n");
                    break; 
                }

                if(param_types[param_count] == TEEC_MEMREF_PARTIAL_INPUT) {
                    if(!(operation->params[param_count].memref.parent->flags 
                                        & TEEC_MEM_INPUT)) {
                        if(returnOrigin)
                            *returnOrigin = TEEC_ORIGIN_API;
                        ret = TEEC_ERROR_BAD_FORMAT;
                    printf("TEEC_InvokeCommand: \
memory reference direction is invalid\n");
                        break;
                    }
                }
                if(param_types[param_count] == TEEC_MEMREF_PARTIAL_OUTPUT) {
                    if(!(operation->params[param_count].memref.parent->flags 
                                        & TEEC_MEM_OUTPUT)) {
                        if(returnOrigin)
                            *returnOrigin = TEEC_ORIGIN_API;
                        ret = TEEC_ERROR_BAD_FORMAT;
                    printf("TEEC_InvokeCommand: \
memory reference direction is invalid\n");
                        break;
                    }
                }

                if(param_types[param_count] == TEEC_MEMREF_PARTIAL_INOUT) {
                    if(!(operation->params[param_count].memref.parent->flags 
                                        & TEEC_MEM_INPUT)) {
                        if(returnOrigin)
                            *returnOrigin = TEEC_ORIGIN_API;
                        ret = TEEC_ERROR_BAD_FORMAT;
                    printf("TEEC_InvokeCommand: \
memory reference direction is invalid\n");
                        break;
                    }
                    if(!(operation->params[param_count].memref.parent->flags 
                                        & TEEC_MEM_OUTPUT)) {
                        if(returnOrigin)
                            *returnOrigin = TEEC_ORIGIN_API;
                        ret = TEEC_ERROR_BAD_FORMAT;
                    printf("TEEC_InvokeCommand: \
memory reference direction is invalid\n");
                        break;
                    }
                }

                if((param_types[param_count] == TEEC_MEMREF_PARTIAL_INPUT) || 
                (param_types[param_count] == TEEC_MEMREF_PARTIAL_INOUT) ||
                (param_types[param_count] == TEEC_MEMREF_PARTIAL_OUTPUT)) {
                    if((operation->params[param_count].memref.offset + 
                            operation->params[param_count].memref.size > 
                                operation->
                                    params[param_count].memref.parent->size) ) {
                        if(returnOrigin)
                            *returnOrigin = TEEC_ORIGIN_API;
                        ret = TEEC_ERROR_EXCESS_DATA;
                    printf("TEEC_InvokeCommand: \
memory reference offset + size is greater than the actual memory size\n");
                        break;
                    }                
                }

                if(param_types[param_count] == TEEC_MEMREF_PARTIAL_INPUT) {
                    inout = 0;
                }
                else if(param_types[param_count] 
                            == TEEC_MEMREF_PARTIAL_OUTPUT) {
                    inout = 1;
                }
                else if(param_types[param_count] 
                            == TEEC_MEMREF_PARTIAL_INOUT) {
                    inout = 2;
                }
                else if(param_types[param_count] == TEEC_MEMREF_WHOLE) {

                    if(operation->
                            params[param_count].memref.parent->flags == 
                            (TEEC_MEM_INPUT | TEEC_MEM_OUTPUT))
                        inout = 2;
                    else if(operation->
                            params[param_count].memref.parent->flags == 
                            TEEC_MEM_INPUT)
                        inout = 0;
                    else if(operation->
                            params[param_count].memref.parent->flags == 
                            TEEC_MEM_OUTPUT)
                        inout = 1;
                }

                if(param_types[param_count] == TEEC_MEMREF_WHOLE) {
                    enc.len = operation->
                                params[param_count].memref.parent->size;
                    enc.offset = 0;
                }
                else {
                    enc.len  = operation->params[param_count].memref.size;
                    enc.offset = operation->params[param_count].memref.offset;
                }

                if(inout == 0) {
                    enc.flags = OTZ_MEM_SERVICE_RO;
                    enc.param_type = OTZC_PARAM_IN; 
                }
                else if(inout == 1) {
                    enc.flags = OTZ_MEM_SERVICE_WO;
                    enc.param_type = OTZC_PARAM_OUT;
                }
                else if(inout == 2) {
                    enc.flags = OTZ_MEM_SERVICE_RO;
                    enc.param_type = OTZC_PARAM_IN; 
                }

                if(operation->params[param_count].memref.parent->allocated) {
                    enc.data = 
                        operation->params[param_count].memref.parent->buffer;

                    ret = ioctl(session->device->fd, 
                                OTZ_CLIENT_IOCTL_ENC_MEM_REF, &enc);
                }
                else {
                    enc.data = 
                        (void*) ((uint32_t)operation->params[param_count].
                                memref.parent->buffer + 
                                enc.offset);
                    enc.offset = 0;
                    ret = ioctl(session->device->fd, 
                                OTZ_CLIENT_IOCTL_ENC_ARRAY, &enc);
                }
                if (ret){
                    if(returnOrigin)
                        *returnOrigin = TEEC_ORIGIN_API;
                    session->s_errno = errno;
                    perror("TEEC_InvokeCommand: \
encoding data in client driver failed\n");
                    break;
                }
                else {
/*                     operation->
                        params[param_count].memref.parent->operation_count++; */
                }
                if(inout == 2) {
                    enc.flags = OTZ_MEM_SERVICE_WO;
                    enc.param_type = OTZC_PARAM_OUT; 

                    if(operation->
                        params[param_count].memref.parent->allocated) {
                        ret = ioctl(session->device->fd, 
                                    OTZ_CLIENT_IOCTL_ENC_MEM_REF, &enc);
                    }
                    else {
                        ret = ioctl(session->device->fd, 
                                    OTZ_CLIENT_IOCTL_ENC_ARRAY, &enc);
                    }

                    if (ret){
                        if(returnOrigin)
                            *returnOrigin = TEEC_ORIGIN_API;
                        session->s_errno = errno;
                    perror("TEEC_InvokeCommand: \
encoding data in client driver failed\n");
                        break;
                    }
                }
            } /* end if for TEEC_MEM_REF check */
            else if((param_types[param_count] == TEEC_MEMREF_TEMP_INPUT) ||
               (param_types[param_count] == TEEC_MEMREF_TEMP_OUTPUT) ||
               (param_types[param_count] == TEEC_MEMREF_TEMP_INOUT)) {

                if(!operation->params[param_count].tmpref.buffer) {
                    if(returnOrigin)
                        *returnOrigin = TEEC_ORIGIN_API;
                    ret = TEEC_ERROR_NO_DATA;
                    printf("TEEC_InvokeCommand: \
temporary memory reference buffer is NULL\n");
                    break; 
                }
                /* This is a variation of API spec. */
                if(operation->params[param_count].tmpref.size == 0) {
                    if(returnOrigin)
                        *returnOrigin = TEEC_ORIGIN_API;
                    ret = TEEC_ERROR_NO_DATA;
                    printf("TEEC_InvokeCommand: \
temporary memory reference size zero is not supported\n");
                    break; 
                }

                if(param_types[param_count] == TEEC_MEMREF_TEMP_INPUT) {
                    inout = 0;
                }
                else if(param_types[param_count] 
                            == TEEC_MEMREF_TEMP_OUTPUT) {
                    inout = 1;
                }
                else if(param_types[param_count] 
                            == TEEC_MEMREF_TEMP_INOUT) {
                    inout = 2;
                }  
                enc.len  = operation->params[param_count].tmpref.size;

                if(inout == 0) {
                    enc.flags = OTZ_MEM_SERVICE_RO;
                    enc.param_type = OTZC_PARAM_IN; 
                }
                else if(inout == 1) {
                    enc.flags = OTZ_MEM_SERVICE_WO;
                    enc.param_type = OTZC_PARAM_OUT;
                }
                else if(inout == 2) {
                    enc.flags = OTZ_MEM_SERVICE_RO;
                    enc.param_type = OTZC_PARAM_IN; 
                }

                enc.data = 
                    operation->params[param_count].tmpref.buffer;
                ret = ioctl(session->device->fd, 
                            OTZ_CLIENT_IOCTL_ENC_ARRAY, &enc);

                if (ret){
                    if(returnOrigin)
                        *returnOrigin = TEEC_ORIGIN_API;
                    session->s_errno = errno;
                    perror("TEEC_InvokeCommand: \
encoding data in client driver failed\n");
                    break;
                }

                if(inout == 2) {
                    enc.flags = OTZ_MEM_SERVICE_WO;
                    enc.param_type = OTZC_PARAM_OUT; 

                    ret = ioctl(session->device->fd, 
                                OTZ_CLIENT_IOCTL_ENC_ARRAY, &enc);
                    if (ret){
                        if(returnOrigin)
                            *returnOrigin = TEEC_ORIGIN_API;
                        session->s_errno = errno;
                    perror("TEEC_InvokeCommand: \
encoding data in client driver failed\n");
                        break;
                    }
                }
            } /* end if for temp reference */
        }/* end for */
    } /* end paramtype */

    if(ret) {
        printf("error in encoding the data\n");
        goto operation_release;
    }

    /* Invoke the command */
    ret = ioctl(session->device->fd,
                    OTZ_CLIENT_IOCTL_SEND_CMD_REQ, &enc);

    if(ret < 0){
        if(returnOrigin)
            *returnOrigin = TEEC_ORIGIN_API;

        session->s_errno = errno;

        if(ret == -EFAULT)
         ret = TEEC_ERROR_ACCESS_DENIED;
        if(ret == -EINVAL)
         ret = TEEC_ERROR_BAD_PARAMETERS;

        perror("TEEC_InvokeCommand: \
command submission in client driver failed\n");
    }
    else if(ret > 0) {
        if(returnOrigin)
            *returnOrigin = TEEC_ORIGIN_TRUSTED_APP;

        /* check the service error code match with global platform 
            error constants */
        printf("TEEC_InvokeCommand: \
command submission failed in trusted application - %s\n", 
        TEEC_GetError(ret, TEEC_ORIGIN_TRUSTED_APP));
    }

    if(ret != 0) {
        goto operation_release;
    }


/* Decode the data */ 
    if(operation->paramTypes != 0) {
        for(param_count = 0; param_count < 4; param_count++) {

            if((param_types[param_count] == TEEC_VALUE_INOUT) ||
                (param_types[param_count] == TEEC_VALUE_OUTPUT)) {
                enc.len  = sizeof(uint32_t);

                ret = ioctl(session->device->fd, 
                            OTZ_CLIENT_IOCTL_DEC_UINT32, &enc);
                if (ret) {
                    if(returnOrigin)
                        *returnOrigin = TEEC_ORIGIN_API;
                    session->s_errno = errno;
                    perror("TEEC_InvokeCommand: \
decoding data in client driver failed\n");
                    break;
                }

                operation->params[param_count].value.a = *((uint32_t*)enc.data);

                if(TEEC_VALUE_UNDEF != operation->params[param_count].value.b) {
                    enc.len  = sizeof(uint32_t);
                    ret = ioctl(session->device->fd, 
                                OTZ_CLIENT_IOCTL_DEC_UINT32, &enc);
                    if (ret) {
                        if(returnOrigin)
                            *returnOrigin = TEEC_ORIGIN_API;
                        session->s_errno = errno;
                    perror("TEEC_InvokeCommand: \
decoding data in client driver failed\n");
                        break;
                    }
                    operation->params[param_count].value.b = 
                                                *((uint32_t*)enc.data);
                }
            } /* end if for TEEC_VALUE check */
            else if( (param_types[param_count] == TEEC_MEMREF_WHOLE) ||
                (param_types[param_count] == TEEC_MEMREF_PARTIAL_INOUT) ||
                (param_types[param_count] == TEEC_MEMREF_PARTIAL_OUTPUT) ||
                (param_types[param_count] == TEEC_MEMREF_TEMP_INOUT) ||
                (param_types[param_count] == TEEC_MEMREF_TEMP_OUTPUT)) {

                inout = 2;

                if(param_types[param_count] == TEEC_MEMREF_WHOLE) {   
                    if(operation->
                            params[param_count].memref.parent->flags == 
                            (TEEC_MEM_INPUT | TEEC_MEM_OUTPUT))
                        inout = 2;
                    else if(operation
                            ->params[param_count].memref.parent->flags == 
                            TEEC_MEM_INPUT)
                        inout = 0;
                    else if(operation
                            ->params[param_count].memref.parent->flags == 
                            TEEC_MEM_OUTPUT)
                        inout = 1;
                }

                if(inout == 0)
                    continue;

                ret = ioctl(session->device->fd, 
                            OTZ_CLIENT_IOCTL_DEC_ARRAY_SPACE, &enc);
                if (ret){
                    if(returnOrigin)
                        *returnOrigin = TEEC_ORIGIN_API;
                    session->s_errno = errno;
                    perror("TEEC_InvokeCommand: \
decoding data in client driver failed\n");
                    break;
                }

                if( (param_types[param_count] == TEEC_MEMREF_WHOLE) ||
                    (param_types[param_count] == TEEC_MEMREF_PARTIAL_INOUT) ||
                    (param_types[param_count] == TEEC_MEMREF_PARTIAL_OUTPUT)) {
                    operation->params[param_count].memref.size = enc.len;
                }
                else {
                    operation->params[param_count].tmpref.size = enc.len;
                }
            } /* end if for TEEC_MEM_REF check */
        }/* end for */
    } /* end paramtype */

    if(ret != TEEC_SUCCESS) {
        printf("error in decoding the data\n");
    }
operation_release:
    /* release the operation */
    rel_ret = ioctl(session->device->fd, 
                    OTZ_CLIENT_IOCTL_OPERATION_RELEASE, &enc);
    if (rel_ret){
        perror("Operation release failed\n");
    }

    session->operation_cnt--;
    return ret;
}

/**
* @brief Requests the cancellation of a pending open Session operation or 
* a Command invocation operation.
*
*
* @param operation: Pointer to TEEC operation structure
*/
void TEEC_RequestCancellation(
    TEEC_Operation* operation)
{
    return;
}
