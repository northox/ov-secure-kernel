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

/* This Header file for translate functions from libc to FAT filesystem 
  functions */



#ifndef SW_SYSLIBC_H
#define SW_SYSLIBC_H

#include <sw_types.h>

/**
* @brief 
* f_open-function shall open the file whose pathname is the string pointed to by
*       filename, and associates a stream with it
*
* @param name-string containing the name of the file to be opened
* @param flags-integer containing a file access mode value
* @param ....
*
* @return -Upon successful completion, fopen() shall return a pointer to the 
*          object controlling the stream. Otherwise, a null pointer shall be 
*          returned
*/
int f_open(const unsigned char *name, int flags, ...);

/**
 * @brief 
 * f_read - Reads an array of count elements from an inputstream 
 *
 * @param file - The file descriptor of the file
 * @param ptr -  Pointer to a block of memory with a size of at least 
 *                 (size*count) bytes
 * @param len -  Number of elements, each one with a size of size bytes
 *
 * @return -       The total number of elements successfully read is returned
 *                 if this number differs from the count parameter, either a 
 *                 reading error occurred or the end-of-file was reached while 
 *                 reading. In both cases, -1 is returned
 */
int f_read(int file, unsigned char *ptr, int len);

/**
 * @brief 
 * f_write - Writes an array of count elements to an outputstream
 *
 * @param file - The file descriptor of the file
 * @param ptr -   Pointer to a FILE object that specifies an output stream
 * @param len -   Number of elements, each one with a size of size bytes
 *
 * @return - The total number of elements successfully written is returned.
 *           If this number differs from the count parameter -1 is returned
 */
int f_write(int file, unsigned char *ptr, int len);

/**
* @brief
* f_lseek - Sets the position indicator associated with the stream to a new 
*         position 
*
* @param file -   The file descriptor of the file
* @param offset - Number of bytes to offset from origin
* @param flags - Position used as reference for the offset value
*
* @return - If successful, the function returns zero.
*           Otherwise, it returns non-zero value
*/
long f_lseek(int file, long offset, int flags);

/**
* @brief 
* f_close - function  will  flushes  the stream pointed to by filepointer 
*
* @param file -   The file descriptor of the file
*
* @return - If the stream is successfully closed, a zero value is returned.
*           On failure, EOF is returned
*/
int f_close(int file);

/**
 * @brief 
 * f_remove - Deletes the file whose name is specified in filename
 *
 * @param rm_p - string containing the name of the file to be deleted
 *
 * @return - if the file is successfully deleted, a zero value is returned.
 *           On failure, a non-zero value is returned
 */
int f_remove(const char *rm_p);

#endif
