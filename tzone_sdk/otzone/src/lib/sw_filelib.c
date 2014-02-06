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

/* This file contains libc functions that needed by storage-api apps */

#include "sw_filelib.h"
#include "sw_syslibc.h"
#include "sw_types.h"
#include "sw_buddy.h"
#include "sw_mem_functions.h"
#include "sw_string_functions.h"
#include "sw_semaphores.h"
#include "sw_debug.h"
#include "fat32.h"

FILE BUF_IO[SW_FILEOPEN_MAX] = {    /* stdin, stdout, stderr */
       { 0, 0, SW_READ,0,(unsigned char *) 0,(unsigned char *) 0},
       { 0, 1, SW_WRITE,0,(unsigned char *) 0,(unsigned char *) 0},
       { 0, 2, SW_WRITE | SW_IOLBF,0,(unsigned char *) 0,(unsigned char *) 0},
       0
};

/**
* @brief 
*
* @param instream
*
* @return 
*/
static s32int sw_clear_stream(FILE *instream)
{
    s32int move = 0,i = 0,num_items,write_count,clear_val = 0;
    if(!instream){
        while(i < SW_FILEOPEN_MAX)
            if( &BUF_IO[i] && sw_clear_stream(&BUF_IO[i]))
                clear_val = EOF;
        return clear_val;
    }

    if((!(instream->_flags & SW_READ) && !(instream->_flags & SW_WRITE)) 
				|| (!(instream->_buf_base) || (instream->_flags & SW_UNBUF)))
        return 0;
    if(instream->_flags & SW_READ){
        if(instream->_flags & SW_IOFIFO)
            return 0;
        if(instream->_buf_base && !(instream->_flags & SW_UNBUF))
            move = -(instream->_chr_count);
        instream->_chr_count = 0;
        if(f_lseek(instream->_fdesp,(slong)move,FILE_SEEK_CUR) == -1){
            instream->_flags |= SW_ERROR;
            return EOF;
        }
        instream->_chr_ptr = instream->_buf_base;
		return 0;
    }
    num_items = instream->_chr_ptr - instream->_buf_base;
    instream->_chr_ptr = instream->_buf_base;
    if(num_items <= 0)
        return 0;
    if(instream->_flags & SW_APPEND){
        if(f_lseek(instream->_fdesp,0,FILE_SEEK_END) == -1){
            instream->_flags |= SW_ERROR;
            return EOF;
        }
    }
    write_count = f_write((int)instream->_fdesp,(char *)instream->_buf_base,
							(int)num_items);
    instream->_chr_count = 0;
    if(num_items != write_count){
        instream->_flags |= SW_ERROR;
        return EOF;
    }
    return 0;
}

/**
 * @brief 
 * occupy_buffer - Allocate and fill the buffer
 * @param stream
 *
 * @return 
 */
static s32int sw_occupy_buffer(FILE *stream)
{
    s32int buf_size = SIZE_BUF;
    u8 i=0,un_ch[SW_FILEOPEN_MAX];
    stream->_chr_count = 0;
    if((stream->_fdesp < 0) || (stream->_flags & (SW_EOF | SW_ERROR)))
        return EOF;

    if((stream->_flags & SW_WRITE) && (stream->_flags & SW_READ))
        goto skip_next;

    if((stream->_flags & SW_WRITE) || !(stream->_flags & SW_READ)){
        stream->_flags |= SW_ERROR;
        sw_printf("file not opened in read mode\n");
        return EOF;
    }

    skip_next:
    if(!(stream->_flags & SW_UNBUF) && !stream->_buf_base){
        stream->_buf_base = (u8 *)sw_malloc((u32)buf_size);
        if(stream->_buf_base){
            stream->_flags |= SW_MYBUF;
            stream->_buf_size = buf_size;
        }
        else{
            stream->_flags |= SW_UNBUF;
        }
    }

    while(i < SW_FILEOPEN_MAX){
        if(&BUF_IO[i])
            if(BUF_IO[i]._flags & SW_IOLBF)
                if(BUF_IO[i]._flags & SW_WRITE)
                    (void) sw_clear_stream(&BUF_IO[i]);
        i++;
    }

    if(!stream->_buf_base){
        buf_size = 1;
        stream->_buf_base = &un_ch[stream->_fdesp];
    }

    stream->_buf_size = buf_size;
    stream->_chr_ptr = stream->_buf_base;
    stream->_chr_count = f_read((int)stream->_fdesp, (char*)stream->_chr_ptr, 
								(int)stream->_buf_size);
    if(stream->_chr_count <= 0) {
        if(stream->_chr_count == 0)
        {
            stream->_flags |= SW_EOF;
            sw_printf("end of file EOF is reached\n");
        }
        else{
            stream->_flags |= SW_ERROR;
            sw_printf("error in read\n");
        }
        stream->_buf_base = NULL;
        stream->_chr_ptr = stream->_buf_base;
        stream->_chr_count = 0;
        return EOF;
    }
    return 1;
}

/**
 * @brief 
 *
 * @param cont
 * @param strm
 *
 * @return 
 */
static s32int sw_flushbuffer(s32int cont,FILE* strm) {
    s32int i=0;
    //clean up
    while(i<SW_FILEOPEN_MAX) {
        if(&BUF_IO[i]) {
            if((BUF_IO[i]._flags & SW_WRITE)||(BUF_IO[i]._flags & SW_APPEND)) {
            (void) sw_clear_stream(&BUF_IO[i]);
        }
        }
        i++;
    }
    /* file descriptor conditions */
    if((strm->_fdesp)<0) {
        sw_printf("file descriptor values are less \n");
        return -1;
          }
    /*checking other than write conditions*/
    if(((strm->_flags & SW_READ) &&
        !(strm->_flags & SW_EOF)) || (!(strm->_flags & SW_WRITE))) {
#ifdef FILE_LIB_DBG
        sw_printf("the value of strm_flag is %x \n",strm->_flags);
#endif
		        return EOF;
    }
    strm->_flags &= ~SW_READ;
    strm->_flags |= SW_WRITE;

    /*allocate memory and size of the buffer if not allocated*/
    if((!(strm->_flags & SW_UNBUF)) && (!strm->_buf_base)) {
        strm->_buf_base=(u8*)sw_malloc(SIZE_BUF);
        if(strm!=stdout) {
            if (strm->_buf_base) {
                strm->_buf_size=SIZE_BUF;
#ifdef FILE_LIB_DBG
                sw_printf("the value of buf_size is %x \n",strm->_buf_size);
#endif
                strm->_flags|=SW_MYBUF;
#ifdef FILE_LIB_DBG
                sw_printf("the value of strm_flag is %x \n",strm->_flags);
#endif
                if(strm->_flags & SW_IOLBF) {
                    strm->_chr_count=-1;
#ifdef FILE_LIB_DBG
                    sw_printf("the value of strm_count is %x \n",
                             strm->_chr_count);
#endif
                }
                else {
                    strm->_chr_count=SIZE_BUF-1;
#ifdef FILE_LIB_DBG
                    sw_printf("the value of strm_count is %x \n",
                             strm->_chr_count);
#endif
                }
                }
                      else {
                strm->_flags|= SW_UNBUF;
#ifdef FILE_LIB_DBG
                sw_printf("the value of strm_flag is %x \n",strm->_flags);
#endif
            }
        }
        else {
            if (strm->_buf_base) {
                strm->_buf_size=SIZE_BUF;
#ifdef FILE_LIB_DBG
                sw_printf("the strm_buffer size is %x \n",strm->_buf_size);
#endif
                strm->_flags= SW_IOLBF|SW_MYBUF;
#ifdef FILE_LIB_DBG
                sw_printf("the value of strm_flag is %x \n",strm->_flags);
#endif
                strm->_chr_count=-1;
#ifdef FILE_LIB_DBG
                sw_printf("the value of strm_count is %x\n",strm->_chr_count);
#endif
            }
            else {
                strm->_flags|= SW_UNBUF;
#ifdef FILE_LIB_DBG
                sw_printf("the value of strm_flag is %x \n",strm->_flags);
#endif
            }
        }
        strm->_chr_ptr=strm->_buf_base;
    }
    /*values updated into strm*/
	
     if(strm->_flags & SW_IOLBF) {
        *strm->_chr_ptr++=cont;
        if(cont=='\n' || strm->_chr_count==-strm->_buf_size) {
            int sw_count =-strm->_chr_count;
            strm->_chr_ptr=strm->_buf_base;
            strm->_chr_count=0;
             if(!f_write(strm->_fdesp,(char*)(strm->_buf_base),sw_count)) {
                strm->_flags|= SW_ERROR;
#ifdef FILE_LIB_DBG
                sw_printf("the value of strm_flag is %x \n",strm->_flags);
#endif
                return EOF;
				            }
            if((strm->_flags & SW_APPEND) &&
            (f_lseek(strm->_fdesp,0L,FILE_SEEK_END) == -1)) {
                strm->_flags|= SW_ERROR;
#ifdef FILE_LIB_DBG
                sw_printf("the value of strm_flag is %x \n",strm->_flags);
#endif
                return EOF;
            }
        }
     }

     else if(strm->_flags & SW_UNBUF) {
        u8 *c_val=cont;
        strm->_chr_count=0;
        if (f_write(strm->_fdesp,(char*)&c_val,1)!=1) {
            strm->_flags|= SW_ERROR;
#ifdef FILE_LIB_DBG
            sw_printf("the value of strm flag is %x \n",strm->_flags);
#endif
            return EOF;
        }
        if((strm->_flags & SW_APPEND) &&
            (f_lseek(strm->_fdesp,0L,FILE_SEEK_END)==-1)) {
            strm->_flags|= SW_ERROR;
#ifdef FILE_LIB_DBG
            sw_printf("the value of strm flag is %x \n",strm->_flags);
#endif
            return EOF;
        }
        return (u8)cont;
    }

    else {
        s32int fw_count=(s32int)(strm->_chr_ptr-strm->_buf_base);
        strm->_chr_count=strm->_buf_size-1;
        strm->_chr_ptr=strm->_buf_base+1;
        if(fw_count>0) {
            if(!f_write(strm->_fdesp,(char*)(strm->_buf_base),fw_count)) {
                *(strm->_buf_base)=cont;
                strm->_flags|= SW_ERROR;
#ifdef FILE_LIB_DBG
                sw_printf("the value of strm_flag is %x \n",strm->_flags);
#endif
                return EOF;
            }
            if((strm->_flags & SW_APPEND) &&
            (f_lseek(strm->_fdesp,0L,FILE_SEEK_END)==-1)) {
                strm->_flags|= SW_ERROR;
#ifdef FILE_LIB_DBG
                sw_printf("the value of strm_flag is %x \n",strm->_flags);
#endif
                return EOF;
            }
        }
        *(strm->_buf_base) = cont;
    }
    return (u8)cont;
}

/**
* @brief 
* fopen-function shall open the file whose pathname is the string pointed to by 
*		filename, and associates a stream with it
*
* @param name-string containing the name of the file to be opened
* @param mode-string containing a file access mode
*
* @return -Upon successful completion, fopen() shall return a pointer to the 
*		   object controlling the stream. Otherwise, a null pointer shall be 
*		   returned
*/
FILE *fopen(const unsigned char *name,const unsigned char *mode)
{
    s32int fd,i;
    slong seek_val = 0 ;
    FILE *fptr;
	for(i=0;((BUF_IO[i]._flags & SW_RDWR) != 0);i++) {
		if(i == (SW_FILEOPEN_MAX-1)) {
        sw_printf("Number of files opened size exceeds with MaximumFileopen\n");
        errno=ENOSPC;
        return NULL;
    	}
	}
	fptr = &BUF_IO[i];

	if(*mode == 'r') {
        fptr->_flags = SW_READ;
#ifdef FILE_LIB_DBG
        sw_printf("READ MODE\n");
#endif
        if(*(++mode) == '+'){
            fptr->_flags |= SW_WRITE;
#ifdef FILE_LIB_DBG
            sw_printf("AND WRITE MODE\n");
#endif
        }
        if((fd = (s32int)f_open(name,fptr->_flags)) != -1) {
#ifdef FILE_LIB_DBG
            sw_printf("opened successfully\n");
#endif
		}
    }
    else if(*mode == 'w'){
        fptr->_flags = SW_WRITE | SW_CREATE_ALWAYS | SW_CREATE_NEW;
#ifdef FILE_LIB_DBG
        sw_printf("WRITE MODE\n");
#endif
        if(*(++mode) == '+'){
            fptr->_flags |= SW_READ;
#ifdef FILE_LIB_DBG
            sw_printf("AND READ MODE\n");
#endif
        }
        if((fd = (s32int)f_open(name,fptr->_flags)) != -1) {
#ifdef FILE_LIB_DBG
            sw_printf("opened successfully\n");
#endif
		}
    }
    else if(*mode == 'a'){
        fptr->_flags = SW_WRITE | SW_CREATE_ALWAYS | SW_CREATE_NEW;
#ifdef FILE_LIB_DBG
        sw_printf("APPEND MODE\n");
#endif
        if(*(++mode) == '+'){
            fptr->_flags |= SW_READ;
#ifdef FILE_LIB_DBG
            sw_printf(" WRITE MODE AND READ MODE\n");
#endif
        }
        if((fd = (s32int)f_open(name,fptr->_flags)) != -1) {
#ifdef FILE_LIB_DBG
            sw_printf("opened successfully\n");
#endif
            if((seek_val = f_lseek(fd, 0, FILE_SEEK_END)) == -1)
                sw_printf("seek failed\n");
#ifdef FILE_LIB_DBG
            sw_printf("current file offset : %x\n", seek_val);
#endif
			fptr->_flags |= SW_APPEND;
        }
    }
    else{sw_printf("mode access failed\n");
        errno = EACCES;
        return NULL;
    }
	
    if(fd < 0) {
		sw_printf("Ther is no file Descriptor value for perform open\n");
        errno=ENOENT;
        return NULL;
    }
#ifdef FILE_LIB_DBG
    sw_printf("file descriptor : %x\n",fd);
#endif
    fptr->_chr_count = 0;
    fptr->_fdesp     = fd;
    fptr->_buf_base  = NULL;
    return fptr;
}

/**
 * @brief 
 * fread - Reads an array of count elements from an inputstream 
 *
 * @param outbuf - Pointer to a block of memory with a size of at least 
 *				   (size*count) bytes
 * @param size -   Size in bytes, of each element to be read
 * @param count -  Number of elements, each one with a size of size bytes
 * @param instream-Pointer to a FILE object that specifies an input stream
 *
 * @return -	   The total number of elements successfully read is returned
 *				   if this number differs from the count parameter, either a 
 *			   	   reading error occurred or the end-of-file was reached while 
 *                 reading. In both cases, -1 is returned
 */
unsigned int fread(void *out_buf,unsigned int size,unsigned int count,
				  FILE *inputstream){
    u8 *localbuf = (u8 *)out_buf;
    unsigned int read_count = 0,element_size = 0;
    if( size == 0 || count == 0){
        sw_printf("value of size and count is <= 0 \n");
        sw_printf("can't read a file\n");
        return -1;
    }
    while(read_count < count){
        element_size = size;
        do{
            if(--(inputstream)->_chr_count >= 0){
                *localbuf++ = *(inputstream)->_chr_ptr++;
            }
            else if(sw_occupy_buffer(inputstream) != EOF){
                --(inputstream)->_chr_count;
                *localbuf++ = *(inputstream)->_chr_ptr++;
            }
            else
                goto ret_val;
        }while(--element_size);
        read_count++;
    }
    ret_val:
        if(inputstream->_flags & SW_ERROR)
            return -1;
#ifdef FILE_LIB_DBG 
        sw_printf("number of elements read : %x\n",read_count);
#endif
        return read_count;
}

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
 *			 If this number differs from the count parameter -1 is returned
 */
unsigned int fwrite(const void *content,unsigned int size,unsigned int count,
					FILE* pfile) {
    unsigned int s,n,new,w_tval;
    s32int w_count;
    const u8 *cp = content;
#ifdef FILE_LIB_DBG
    sw_printf("----------------------------------------------------------\n");
    /*checking minimum values*/
    sw_printf("checking value of count and size is minimum or not? \n");
#endif
    if(count==0x00000000) {
		sw_printf("value of count is minimum \n");
        return -1;
    }
    if(size==0x00000000) {
		sw_printf("value of size is minimum \n");
        return -1;
    }
#ifdef FILE_LIB_DBG
    sw_printf("value of count and size is not minimum \n");
    sw_printf("----------------------------------------------------------\n");
    /*checking maximum values*/
    sw_printf("checking value of count and size is maximum or not? \n");
#endif
    if (((count | size) > 0xFFFF) &&
        (count > SIZE_BUF / size)) {
		sw_printf("value of count and size is maximum\n");
        pfile->_flags=0x00000000;
        errno=ENOSPC;
        return -1;
    }
#ifdef FILE_LIB_DBG
    sw_printf("value of count and size is not maximum\n");
#endif
    for(new=0;new<count;new++) {
    s=size;
    do {
    if(((--(pfile)->_chr_count>=0) ?
        (int)(*(pfile)->_chr_ptr++=((int)*cp)) :
        sw_flushbuffer(((int)*cp),(pfile)))==EOF)
        return new;
        cp++;
    }while(--s);
    }
#ifdef FILE_LIB_DBG
    sw_printf("_write is called by sw_fwrite func \n");
#endif
    w_count=(s32int)(f_write(pfile->_fdesp,(char*)content,(int)count));
    if(w_count==0) {
        sw_printf("sub_function-file opened in read only mode \n");
        sw_printf("--------------------------------------------------------\n");
        return 0;
    }
#ifdef FILE_LIB_DBG
    sw_printf("calculate total bytes \n");
#endif
    n=(size_t)(w_count*size);
#ifdef FILE_LIB_DBG
    sw_printf("no of bytes write is %x \n",new);
    sw_printf("Testing : no of bytes write is %x \n",n);
    sw_printf("----------------------------------------------------------\n");
#endif
    w_tval=n/size;
    if(new!=w_tval) {
        sw_printf("error in no of bytes written into the file \n");
        sw_printf("sw_fwrite is not successfully worked \n");
        sw_printf("--------------------------------------------------------\n");
        return -1;
    }
#ifdef FILE_LIB_DBG
    sw_printf("sw_fwrite is successfully worked \n");
    sw_printf("----------------------------------------------------------\n");
#endif
    return n;
}

/**
* @brief 
* fclose - function  will  flushes  the stream pointed to by filepointer 
*
* @param pfile - Pointer to a FILE object that specifies the stream to be closed
*
* @return - If the stream is successfully closed, a zero value is returned.
*			On failure, EOF is returned
*/
int fclose(FILE* pfile)
    {
    s32int ret_val,i=0,val_r;
#ifdef FILE_LIB_DBG
    /*checking maximum sw_file open conditions*/
    sw_printf("----------------------------------------------------\n");
    sw_printf("******sw_fclose implementation********\n");

    /*fopen maximum condtions*/
    sw_printf("fclose:checking fopen maximum conditions \n");
#endif
    while(i<SW_FILEOPEN_MAX) {
        if(pfile==&BUF_IO[i]) {
            BUF_IO[i]._chr_count=0;
            BUF_IO[i]._flags=0;
            BUF_IO[i]._buf_size=0;
            BUF_IO[i]._buf_base=NULL;
            BUF_IO[i]._chr_ptr=NULL;
            break;
        }
        i++;
    }
    if(i==(SW_FILEOPEN_MAX-1)) {
        sw_printf("Number of files opened size exceeds with MaximumFileopen\n");
        return EOF;
    }
#ifdef FILE_LIB_DBG
    /*clean up*/
    sw_printf("fclose:cleanup \n");
#endif
    if(sw_clear_stream(pfile)){
#ifdef FILE_LIB_DBG
        sw_printf("fclose:cleanup the file pointer \n");
#endif
        val_r=EOF;
    }

    ret_val=f_close(pfile->_fdesp);
    val_r=ret_val;
    if(val_r==-1) {
        sw_printf("sw_fclose function is not closed the file \n");
        return val_r;
    }

	/*free the memory allocated buffer*/
    if(pfile->_buf_base && (pfile->_flags & SW_MYBUF)) {
        sw_printf("free the buffer of the file pointer \n");
        sw_free((void*)(pfile->_buf_base));
    }

 /*checking stdin,stdout and stderr conditions*/
    if(pfile!=stdin && pfile!=stdout && pfile!=stderr) {
		sw_printf("free the allocated memory of the file pointer \n");
        sw_free((void *)pfile);
    }

#ifdef FILE_LIB_DBG
    sw_printf("sw_fclose function is closed the file \n");
    sw_printf("sw_fclose is successfully worked \n");
    sw_printf("------------------------------------------------------\n");
#endif
    return val_r;
}

/**
* @brief 
* feof - Checks whether the end-of-File indicator associated with stream is set
*
* @param file_stream - Pointer to a FILE object that identifies the stream 
*
* @return - A non-zero value is returned in the case that the end-of-file 
*           indicator associated with the stream is set.Otherwise, zero is 
*			returned
*/
int (feof)(FILE *file_stream)
{
    return ((file_stream->_flags & SW_EOF) != 0);
}

/**
* @brief
* fseek - Sets the position indicator associated with the stream to a new 
*		  position 
*
* @param seekstream - Pointer to a FILE object that identifies the stream
* @param offset - Number of bytes to offset from origin
* @param whence - Position used as reference for the offset for reference 
*				  sw_filelibc.h
*
* @return - If successful, the function returns zero.
*		    Otherwise, it returns non-zero value
*/
int fseek(FILE *seekstream, long offset, int whence)
{
    slong pos_in_file;
    s32int move = 0,clear_flags;

    clear_flags = ~(SW_EOF | SW_ERROR);
    seekstream->_flags &= clear_flags;

    if(seekstream->_flags & SW_READ){
        if(seekstream->_buf_base && !(seekstream->_flags & SW_UNBUF)
                                 && whence == FILE_SEEK_CUR)
            move = seekstream ->_chr_count;
        seekstream->_chr_count = 0;
    }
    else if(seekstream->_flags & SW_WRITE)
        sw_clear_stream(seekstream);
    else
        sw_printf("inappropriate flags\n");

    pos_in_file = f_lseek(seekstream->_fdesp,(long)(offset-move),whence);
    seekstream->_chr_ptr = seekstream->_buf_base;
    if(pos_in_file == -1)
        return -1;
    else
        return 0;
}

/**
 * @brief 
 * remove - Deletes the file whose name is specified in filename
 *
 * @param r_pth - string containing the name of the file to be deleted
 *
 * @return - if the file is successfully deleted, a zero value is returned.
 *  		 On failure, a non-zero value is returned
 */
int remove(const char *r_pth)
{
	s32int rm_val;
	rm_val=(s32int)(f_remove(r_pth));
	if(rm_val==-1) {
		sw_printf("the file is not removed from the Mentioned Path\n");
		return -1;
	}
#ifdef FILE_LIB_DBG
	sw_printf("the file is removed from the Mentioned Path\n");
#endif
	return 0;
}
