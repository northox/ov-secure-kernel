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
 * Application to send ack signal to hypervisor
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
#include "otz_tee_client_api.h"

/**
* @brief Sample application to show the implementation of Global Platform Client
* API specification.
*
* @return 
*/
int main(void)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;

    TEEC_Result result;

    TEEC_UUID svc_id = OTZ_SVC_VIRTUAL_KEYBOARD;

    uint32_t len;
    char    testData[256];

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

    operation.paramTypes = TEEC_PARAM_TYPES(
        TEEC_NONE,
        TEEC_NONE,
        TEEC_NONE,
        TEEC_NONE);
    
    operation.started = 1;

    result = TEEC_InvokeCommand(
                 &session,
                 OTZ_VIRTUAL_CMD_ID_PLAY_MEDIA,
                 &operation,
                 NULL);
    if (result != TEEC_SUCCESS)
    {
        goto cleanup_3;
    }

    printf("command success\n");
cleanup_3:
    TEEC_CloseSession(&session);
cleanup_2:
    TEEC_FinalizeContext(&context);
cleanup_1:
    return 0;
}
