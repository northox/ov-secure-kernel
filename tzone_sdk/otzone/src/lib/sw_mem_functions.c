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
 * Memory management functions
 */

#include <sw_debug.h>
#include <sw_mem_functions.h>
#include <elf_loader.h> 
/**
 * @brief 
 *
 * memory move
 *
 * @param dest
 * @param src
 * @param count
 *
 * @return 
 */
void * sw_memmove(void * dest,const void *src, u32 count)
{
  char *tmp, *s;

  if (dest <= src)
  {
    tmp = (char *) dest;
    s = (char *) src;
    while (count--)
    {
      *tmp++ = *s++;
    }
  }
  else
  {
    tmp = (char *) dest + count;
    s = (char *) src + count;
    while (count--)
    {
      *--tmp = *--s;
    }
  }
  return dest;
}

/**
 * @brief 
 * memory set
 *
 * @param dest
 * @param c
 * @param count
 *
 * @return 
 */
void * sw_memset(void * dest, u32 c, u32 count)
{
    //Standard bytewise memset
    char* d;
    d = (char*) dest;

    while(count--)
    {
      *d = c;
      d++;
    }

  return dest;
}


/**
 * @brief 
 * This version of memcpy assumes disjoint ptrs src, dst 
 *
 * @param dst
 * @param src
 * @param count
 *
 * @return 
 */
void *sw_memcpy(void *dst, const void *src, u32 count)
{
  int i;
  char *dst_tmp = dst;
  const char *src_tmp = src;

  if (!((unsigned int)src & 0xC) && !((unsigned int)dst & 0xC))
  {
    //word aligned so we can safely do word copies
    for (i=0; i < count; i+=4)
    {
      if (i + 3 > count - 1)
        break; //don't copy too much

      *(u32 *)dst_tmp = *(u32 *)src_tmp;
      dst_tmp += 4;
      src_tmp += 4;
    }
    if (i <= count - 1)
    {
      for (; i < count; i++)
      {
        *dst_tmp = *src_tmp;
        dst_tmp++;
        src_tmp++;
      }
    }
  }
  else
  {
    //generic version
    for (i=0; i < count; i++)
      dst_tmp[i] = src_tmp[i];
  }
  return dst;
}

EXPORT_SYMBOL(sw_memcpy);

/**
 * @brief custom version of memcmp function
 */
int sw_memcmp(void *src, void *dest, u32 length)
{
	u32 counter = -1;
	u8 *localSrc = (u8*)src, *localDest = (u8*)dest;
	while((++counter <length) && (localSrc[counter] - localDest[counter] == 0));
	if(counter == (int)length) {
		return(localSrc[counter-1] - localDest[counter-1]);
	}
	return(localSrc[counter] - localDest[counter]);
}

/**
* @brief
* Copies the values of Num bytes from the location pointed by source directly 
* to the memory block pointed by destination By checking the values of 
* Num bytes with destination and source Maximum Length
*
* @param dst - Pointer to the destination array where the content is to be
*  			   copied
* @param src - Pointer to the source of data to be copied
* @param count - Number of bytes to copy
* @param dstLen - destination Maximum Length
* @param srcLen - source Maximum Length
*
* @return 
*/
void *sw_memncpy(void *dst, const void *src, u32 count,u32 dstLen,u32 srcLen)
{
  if(count<=dstLen || count<=srcLen) {
  	return sw_memcpy(dst,src,count);		
  }
  else {
	sw_printf("Memory Overflow Error\n");
	return NULL;
  }
}
