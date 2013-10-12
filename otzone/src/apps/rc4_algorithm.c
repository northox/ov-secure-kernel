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
 */


/* Implementation of rc4 encryption-decryption algorithm 
 * This uses the ideas in the wikipedia page (http://en.wikipedia.org/wiki/RC4)
 */

#include "sw_mem_functions.h"

#ifdef _CRYPTO_BUILD_

#include <openssl/rc4.h>

/**
* @brief 
*
* @param input_buf
* @param input_len
* @param output_buf
* @param output_len
*
* @return 
*/
int libcrypt_rc4_algorithm(char *input_buf, int input_len,
				char *output_buf, int *output_len)
{
	RC4_KEY key;
	char *key_seq = ")*(&$%^!@#~`+="; /* key for encryption & decryption*/
	unsigned char key_seq_len = 14; /* length of the key_seq variable*/

	RC4_set_key(&key,key_seq_len, key_seq);
    RC4(&key,input_len,input_buf,output_buf);
	return(0);
}

#else

#define MODULO_VALUE 256
#define BUF_LEN 256
#define INITIALIZATION_CONST 0xff

#define SWAP(a,b) (((a) ^ (b)) && ((b) ^= (a) ^= (b), (a) ^= (b)))

/** 
 * @brief the function to encode and decode a buffer using rc4 algorithm.
 * The output buffer has to be atleast the same length as the input buffer.
 * Expect segmentation faults if not. 
 *
 * @param input_buf: the input buffer that needs to be converted
 * @param input_len: the length of the input buffer
 * @param output_buf: the output buffer into which the data needs to be filled.
 * The size has to be atleast the same size as the input buffer.
 * @param output_len: the length of the converted data
 *
 * @return 0 if successful. No other return values are expected for now 
 **/
int otzone_rc4_algorithm(char *input_buf, int input_len, char *output_buf, 
																int *output_len)
{
	char *key_seq = ")*(&$%^!@#~`+="; /* key for encryption & decryption*/
	unsigned char key_seq_len = 14; /* length of the key_seq variable*/
	unsigned char enc_array[MODULO_VALUE];
	unsigned short loop_cntr=0,index=0;

	sw_memset(enc_array,INITIALIZATION_CONST,(MODULO_VALUE-1)*sizeof(unsigned char));
	for(loop_cntr=0;loop_cntr<MODULO_VALUE;loop_cntr++) {
		index = (index+enc_array[loop_cntr]+
				(unsigned char)(key_seq[loop_cntr%key_seq_len]))%MODULO_VALUE;
		if(enc_array[loop_cntr] == INITIALIZATION_CONST) {
			enc_array[loop_cntr] = loop_cntr;
		}
		if(enc_array[index] == INITIALIZATION_CONST) {
			enc_array[index] = loop_cntr;
		}
		SWAP(enc_array[loop_cntr],enc_array[index]);
	}
	loop_cntr = 0;
	index = 0;
	for(*output_len = 0;*output_len<input_len;(*output_len)++) {
		loop_cntr = (loop_cntr+1)%MODULO_VALUE;
		index = (index+enc_array[loop_cntr])%MODULO_VALUE;
		SWAP(enc_array[loop_cntr],enc_array[index]);
		output_buf[*output_len] = input_buf[*output_len] ^ 
			(enc_array[(enc_array[loop_cntr]+enc_array[index])%MODULO_VALUE]);
	}
	return(0);
}

#endif



