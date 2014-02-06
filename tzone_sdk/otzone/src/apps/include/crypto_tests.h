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
 * Header for Crypto tests implementation
 */

#ifndef CRYPTO_TESTS_H
#define CRYPTO_TESTS_H

#include "sw_types.h"
#include "otz_id.h"

/**
* @brief 
*
* @param input_buf
* @param input_len
* @param output_buf
* @param output_len
* @param svc_cmd_id
*
* @return 
*/
int test_md5_api(u8 *input_buf,u32 input_len,u8* output_buf,
			u32 *output_len,u32 svc_cmd_id);

/**
* @brief 
*
* @param input_buf
* @param input_len
* @param key_buf
* @param key_len
* @param output_buf
* @param output_len
* @param svc_cmd_id
*
* @return 
*/
int test_hmac_api(u8 *input_buf,u32 input_len,u8 *key_buf, u32 key_len,
				u8* output_buf,u32 *output_len,u32 svc_cmd_id);

/**
* @brief 
*
* @param input_buf
* @param input_len
* @param key_buf
* @param key_len
* @param output_buf
* @param output_len
* @param svc_cmd_id
*
* @return 
*/
int compare_hmac_values(u8 *input_buf,u32 input_len,u8 *key_buf, u32 key_len,
				u8* output_buf,u32 *output_len,u32 svc_cmd_id);

/**
* @brief 
*
* @param input_buf
* @param input_len
* @param init_vector
* @param init_vector_len
* @param key_buf
* @param key_len
* @param output_buf
* @param output_len
* @param svc_cmd_id
* @param cipher_choice
*
* @return 
*/
int test_cipher_api(u8 *input_buf,u32 input_len,u8 *init_vector,
				u32 init_vector_len,u8 *key_buf, u32 key_len,
					u8* output_buf,u32 *output_len,
					u32 svc_cmd_id,u32 cipher_choice);


#endif
