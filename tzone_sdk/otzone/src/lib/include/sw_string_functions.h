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
 * Header for String manipulation routines
 *
 */

#ifndef __LIB__STRING_FUNCTIONS_H__
#define __LIB__STRING_FUNCTIONS_H__

#include <sw_types.h>

/**
 * @brief 
 *
 * @param str1
 * @param str2
 * @param n
 *
 * @return 
 */
int sw_strncmp(char * str1, char * str2, int n);

/**
 * @brief 
 *
 * @param s1
 * @param s2
 *
 * @return 
 */
int sw_strcmp (const char * s1, const char * s2);

/**
 * @brief 
 *
 * @param str
 *
 * @return 
 */
u32 sw_strtoi(char * str);

/**
 * @brief 
 *
 * @param s
 *
 * @return 
 */
u32 sw_strlen(char * s);

/**
 * @brief 
 *
 * @param dest
 * @param src
 *
 * @return 
 */
char * sw_strcpy(char * dest, char *src);

/**
 * @brief 
 * strcpy - Copy a NULL terminated string
 *
 * @param dest
 * @param src
 * @param n
 *
 * @return 
 */
char *sw_strncpy(char *dest, char *src, size_t n);

/**
 * @brief 
 *
 * @param dest
 * @param src
 *
 * @return 
 */
char * sw_strcat(char * dest, const char *src);

/**
 * @brief 
 * strcpy - Copy a NULL terminated string
 *
 * @param dest
 * @param src
 * @param n
 *
 * @return 
 */
char *sw_strncpy(char *dest, char *src, size_t n);

/**
* @brief 
*
* @param print_buffer
* @param fmt
* @param ...
*
* @return 
*/
u32 sw_sprintf(char *print_buffer, const char *fmt, ...);

/**
 * @brief 
 * Copies the first num characters of source to destination
 * upto Null Terminate String Found
 *
 * @param dest - Pointer to the destination array where the content is to be
 *               copied
 * @param src  - string to be copied
 * @param n    - number of characters to be copied from source
 * @param destLen - destination Maximum Length
 * @param srcLen  - source Maximum Length
 *
 * @return 
 */
char * sw_maxstrncpy(char * dest, char *src, size_t n,size_t destLen,
                            size_t srcLen);

#endif
