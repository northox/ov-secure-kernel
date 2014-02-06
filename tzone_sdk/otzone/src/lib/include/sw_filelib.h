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

/* This Header file for libc functions that needed by storage-api apps */

#include "sw_types.h"

#define SW_READ         0x001
#define SW_WRITE        0x002
#define SW_RDWR         (SW_READ | SW_WRITE)
#define SW_CREATE_NEW       0x004
#define SW_CREATE_ALWAYS    0x008
#define SW_APPEND           0x010

#define SW_UNBUF        0x004
#define SW_EOF          0x010
#define SW_ERROR        0x020
#define SW_IOFIFO       0x40
#define SW_MYBUF        0x008
#define SW_IOLBF        0x040
#define L(n)    ((long) (n))

#define SW_FILEOPEN_MAX 20
#define SIZE_BUF        1024
#define EOF             (-1)

/**
 * @brief 
 */
struct _FILE_CON {
    int   _chr_count;
    int   _fdesp;
    int   _flags;
    int   _buf_size;
    unsigned char    *_buf_base;
    unsigned char    *_chr_ptr;
};

#ifndef _ERROR_NO_H_
#define _ERROR_NO_H_
int errno;
#define ENOENT      2   /* No such file or directory */
#define ENOMEM      12  /* Out of memory */
#define EACCES      13  /* Permission denied */
#define ENOSPC      28  /* No space on device*/
#endif

#ifndef _STDIO_H_
/**
 * @brief 
 */
typedef struct _FILE_CON FILE;
extern FILE BUF_IO[SW_FILEOPEN_MAX];

#define stdin       (&BUF_IO[0])
#define stdout      (&BUF_IO[1])
#define stderr      (&BUF_IO[2])

/**
* @brief 
* feof - Checks whether the end-of-File indicator associated with stream is set
*
* @param file_stream - Pointer to a FILE object that identifies the stream 
*
* @return - A non-zero value is returned in the case that the end-of-file 
*           indicator associated with the stream is set.Otherwise, zero is 
*           returned
*/
int (feof)(FILE *file_stream);

/**
* @brief 
* fopen-function shall open the file whose pathname is the string pointed to by 
*       filename, and associates a stream with it
*
* @param name-string containing the name of the file to be opened
* @param mode-string containing a file access mode
*
* @return -Upon successful completion, fopen() shall return a pointer to the 
*          object controlling the stream. Otherwise, a null pointer shall be 
*          returned
*/
FILE *fopen(const unsigned char *name,const unsigned char *mode);

/**
 * @brief 
 * fread - Reads an array of count elements from an inputstream 
 *
 * @param outbuf - Pointer to a block of memory with a size of at least 
 *                 (size*count) bytes
 * @param size -   Size in bytes, of each element to be read
 * @param count -  Number of elements, each one with a size of size bytes
 * @param instream-Pointer to a FILE object that specifies an input stream
 *
 * @return -       The total number of elements successfully read is returned
 *                 if this number differs from the count parameter, either a 
 *                 reading error occurred or the end-of-file was reached while 
 *                 reading. In both cases, -1 is returned
 */
unsigned int fread(void *outbuf, unsigned int size, unsigned int count,
				   FILE *inputstream);
/**
 * @brief 
 * fwrite - Writes an array of count elements to an outputstream
 *
 * @param content - Pointer to the array of elements to be written
 * @param size -    Size in bytes of each element to be written
 * @param count -   Number of elements, each one with a size of size bytes
 * @param pfile -   Pointer to a FILE object that specifies an output stream
 *
 * @return - The total number of elements successfully written is returned.
 *           If this number differs from the count parameter -1 is returned
 */
unsigned int fwrite(const void *content, unsigned int size, unsigned int count,
					FILE* pfile);
/**
* @brief
* fseek - Sets the position indicator associated with the stream to a new 
*         position 
*
* @param seekstream - Pointer to a FILE object that identifies the stream
* @param offset - Number of bytes to offset from origin
* @param whence - Position used as reference for the offset for reference 
*                 sw_filelibc.h
*
* @return - If successful, the function returns zero.
*           Otherwise, it returns non-zero value
*/
int fseek(FILE *seekstream, long offset, int whence);

/**
* @brief 
* fclose - function  will  flushes  the stream pointed to by filepointer 
*
* @param pfile - Pointer to a FILE object that specifies the stream to be closed
*
* @return - If the stream is successfully closed, a zero value is returned.
*           On failure, EOF is returned
*/
int fclose(FILE* pfile);

/**
 * @brief 
 * remove - Deletes the file whose name is specified in filename
 *
 * @param r_pth - string containing the name of the file to be deleted
 *
 * @return - if the file is successfully deleted, a zero value is returned.
 *           On failure, a non-zero value is returned
 */
int remove(const char *r_pth);

#endif
