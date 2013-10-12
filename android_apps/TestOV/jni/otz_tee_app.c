/* 
 * OpenVirtualization: 
 * For additional details and support contact developer@sierraware.com.
 * Additional documentation can be found at www.openvirtualization.org
 * 
 * Copyright (C) 2011 SierraWare
 *
 * This file is part of the Sierraware OpenTrustPlatform.
 *
 * Sierraware OpenTrustPlatform is free software: you can redistribute it 
 * and/or modify it under the terms of the GNU Lesser General Public License 
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Sierraware OpenTrustPlatform is distributed in the hope that it 
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Sierraware OpenTrustPlatform.  If not, 
 * see <http://www.gnu.org/licenses/>.
 * 
 * Linux trustzone example application.
 */

#include<string.h>
#include <sys/types.h> /* pid_t */
#include <unistd.h>
#include<com_example_testov_JniInterface.h>
#include <func.h>
#include<stdio.h>
#include<stdlib.h>
/**
* @brief Sample application to show the implementation of Global Platform Client
* API specification.
*
* @return 
*/



#define BUF_SIZE 256

JNIEXPORT jstring JNICALL Java_com_example_testov_JniInterface_testOV
							(JNIEnv *jEnv, jclass jClass, jstring input_param)
{
	char result[BUF_SIZE];
	char* otz_tee_app_ret;
	const jbyte *input;
	char* user_input;
	input = (jbyte*)(*jEnv)->GetStringUTFChars(jEnv,input_param,NULL);
	if(input == NULL) {
		strcpy(result,"Hello from OV Stack. No parameter passed ");
	} else {
		strcpy(result,"Hello from OV Stack. Output:  ");
		strcat(result,(char*)input);
	}
	(*jEnv)->ReleaseStringUTFChars(jEnv,input_param, (char*)input);
	 
	otz_tee_app_ret = main(result);
	strcat(result,otz_tee_app_ret);
	return((*jEnv)->NewStringUTF(jEnv,result));
}
	  
	
