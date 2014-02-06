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
 * String manipulation routines
 */

#include <sw_debug.h>
#include <sw_string_functions.h>
#include <elf_loader.h>
/**
 * @brief 
 *
 * @param str1
 * @param str2
 * @param n
 *
 * @return 
 */
int sw_strncmp(char * str1, char * str2, int n)
{
  int index = 0;
  while (index < n)
  {
    if (str1[index] != str2[index])
    {
      return -1;
    }
    index++;
  }
  return 0;
}

/**
 * @brief 
 *
 * @param str
 *
 * @return 
 */
u32 sw_strtoi(char * str)
{
  int  length = 8;
  int  bits_in_long = 32;
  int  index = 0;
  char digit_char = 0;
  ulong digit_int = 0;
  ulong ret_val = 0;

  while (index < length)
  {
    digit_char = str[index];
    if ( (digit_char < 0x30) || (digit_char > 0x39) )
    {
      return -1;
    }
    else
    {
      digit_int |= digit_char - 0x30;
      digit_int = digit_int << ( bits_in_long - ((index + 1) * 4) );
      ret_val = ret_val | digit_int; 
    }
    index = index + 1;
  } // while ends
  return ret_val;
}

/**
 * @brief 
 *  strlen - Find the length of a string
 *
 * @param s
 *
 * @return 
 */
u32 sw_strlen(char * s)
{
  char *sc;

  for (sc = s; *sc != '\0'; ++sc)
  {
    // do nothing
  }
  return sc - s;
}

/**
 * @brief 
 * strcpy - Copy a NULL terminated string
 *
 * @param dest
 * @param src
 *
 * @return 
 */
char * sw_strcpy(char * dest, char *src)
{
  char *tmp = dest;

  while ((*dest++ = *src++) != '\0')
  {
    // do nothing
  }
  return tmp;
}
EXPORT_SYMBOL(sw_strcpy);
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
char * sw_strncpy(char * dest, char *src, size_t n)
{
	if (n != 0) {
		char *d = dest;
		const char *s = src;

		do {
			if ((*d++ = *s++) == 0) {
				/* NUL pad the remaining n-1 bytes */
				while (--n != 0)
					*d++ = 0;
				break;
			}
		} while (--n != 0);
	}
	return (dest);
}


/**
 * @brief 
 * strcmp - compare strings
 *
 * @param s1
 * @param s2
 *
 * @return 
 */
int sw_strcmp (const char * s1, const char * s2)
{
  for(; *s1 == *s2; ++s1, ++s2)
    if(*s1 == 0)
      return 0;
  return *(unsigned char *)s1 < *(unsigned char *)s2 ? -1 : 1;
}

/**
 * @brief 
 *
 * @param dest
 * @param src
 *
 * @return 
 */
char *sw_strcat(char *dest, const char *src)
{
    char *save = dest;

    for (; *dest; ++dest) ;
    while ((*dest++ = *src++) != 0) ;

    return (save);
}

/**
 * @brief 
 *
 * @param dest
 * @param src
 * @param n
 *
 * @return 
 */
char * sw_strncat(char * dest, const char *src, size_t n)
{
	if (n != 0) {
		char *d = dest;
		const char *s = src;

		while (*d != 0)
			d++;
		do {
			if ((*d = *s++) == 0)
				break;
			d++;
		} while (--n != 0);
		*d = 0;
	}
	return (dest);
}

/**
 * @brief 
 *
 * @param s
 * @param c
 *
 * @return 
 */
char *sw_strchr(const char *s, int c)
{
  char *ptr = (char*)s;

  while (*ptr != (char)c) {
      if(*ptr == '\0')
        return NULL;
      ptr++;
    }

  return ptr;
}


/**
* @brief 
*
* @param print_buffer
* @param fmt
* @param ...
*
* @return 
*/
u32 sw_sprintf(char *print_buffer, const char *fmt, ...)
{
  va_list args;
  u32 i;
  va_start(args, fmt);

  i = sw_vsprintf(print_buffer, fmt, args);
  va_end(args);

  return i;
}

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
							size_t srcLen)
{
	if(n<=destLen || n<=srcLen) {
    	return sw_strncpy(dest,src,n);
	}
	else {
    	sw_printf("Input count Exceeds With Source and Dest Length\n");
    	return NULL;
    }
}
