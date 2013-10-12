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
 * syscalls implementation
 *
 */

#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/times.h>
#include <sys/time.h>
#include <sys/errno.h>
#include "fat32.h"

#define SEEK_SET    0    /* Seek from beginning of file.  */
#define SEEK_CUR    1    /* Seek from current position.  */
#define SEEK_END    2    /* Seek from end of file.  */


#define   FILE_CNTR_START_INDEX   3
#define   MAX_OPEN_FILES   (32+FILE_CNTR_START_INDEX)
#define   MAX_FILENAME_LENGTH   255
 
char *__env[1] = {0};
char **environ = __env;

typedef struct 
{
	int fd;
}FILE;

FILE file_pointers[MAX_OPEN_FILES];
u8 file_names[MAX_OPEN_FILES][MAX_FILENAME_LENGTH];

/**
 * @brief 
 *
 * @param file
 *
 * @return 
 */
int _close(int file) 
{
#ifdef CONFIG_FILESYSTEM_SUPPORT
    int retVal = 0;
    if(file_names[file][0] == '\0') {
        errno = EBADF;
        return(-1);
    }
    retVal = file_close(file_pointers[file].fd);
    file_names[file][0] = '\0';
    if(retVal != -1) {
        errno = 0;
        return(0);
    } else {
        /*todo set errno */
        return(-1);
    }
#else
        sw_printk("file system configuration is not defined. \
                Enable CONFIG_FILESYSTEM_SUPPORT.\n");

        errno= EBADF;
        return (-1);
#endif
}

/**
 * @brief 
 *
 * @param file
 * @param st
 *
 * @return 
 */
int _fstat(int file, struct stat *st) 
{
    /*FILINFO file_info;

    if((file == SW_STDERR) ||(file == SW_STDOUT) || (file == SW_STDIN)) {
        st->st_mode = S_IFCHR;
    } else {
        if(file_names[file][0] == '\0') {
            errno = EBADF;
            return(-1);
        }
#ifdef NOT_IMPLEMENTED
        file_fstat(file_names[file],&file_info);
        st->st_mode = S_IFREG;
        st->st_size = file_info.fsize;
        st->st_mtime = file_info.ftime;
#endif
    }
    errno = 0;*/
    return(0);
}

/**
 * @brief 
 *
 * @param file
 *
 * @return 
 */
int _isatty(int file) 
{
    if((file == SW_STDERR) ||(file == SW_STDOUT) || (file == SW_STDIN)) {
        errno = 0;
        return(1);
    }
    if(file < MAX_OPEN_FILES){
        errno = ENOTTY;
        return(0);
    }
    errno = EBADF;
    return(0);
}

/**
 * @brief 
 *
 * @param file
 * @param offset
 * @param flags
 *
 * @return 
 */
off_t _lseek(int file, off_t offset, int flags)
{
    off_t local_offset = offset;
    if((file_pointers[file].fd == SW_STDERR) ||
			(file_pointers[file].fd == SW_STDOUT) || 
			(file_pointers[file].fd == SW_STDIN)) {
        return(offset);
    }
#ifdef CONFIG_FILESYSTEM_SUPPORT
    if(file_names[file][0] == '\0') {
        errno = EBADF;
        return(-1);
    }
#if 0
    switch(flags) {
        case SEEK_SET:
            break;
        case SEEK_CUR:
            local_offset = offset + get_current_offset(&(file_pointers[file]));
            break;
        case SEEK_END:
            local_offset = offset + get_eof_offset(&(file_pointers[file]));
            break;
        default:
            errno = EINVAL;
            return(-1);
    }
#endif
    if(local_offset = file_seek(file_pointers[file].fd,(int)offset, 
								flags) == -1) {
        /*todo set errno appropriately */
        return(-1);
    }
#else
        sw_printk("file system configuration is not defined. \
                Enable CONFIG_FILESYSTEM_SUPPORT.\n");

        errno= EBADF;
        return (-1);
#endif
    errno = 0;
    return(local_offset);
}


/**
 * @brief 
 *
 * @return 
 */
static int get_free_index()
{
	int i = FILE_CNTR_START_INDEX;

	while(i < MAX_OPEN_FILES) {
		if(file_names[i][0] == '\0') {
			return(i);
		}
		i += 1;
	}
	return(0);
}

/**
 * @brief 
 *
 * @param name
 * @param flags
 * @param ...
 *
 * @return 
 */
int _open(const char *name, int flags, ...) 
{ 
#ifdef CONFIG_FILESYSTEM_SUPPORT
    int free_index = 0;
    u8 mode_val;

    if((free_index = get_free_index()) == 0) {
        errno = EMFILE;
        return(-1);
    }
	switch(flags) {
        case 0: 
            mode_val = FILE_READ;
            break;
        case 2: 
            mode_val = FILE_READ | FILE_WRITE;
            break;
        case 1537:
            mode_val = FILE_WRITE | FILE_CREATE_ALWAYS | FILE_CREATE_NEW;
            break;
        case 1538:
            mode_val = FILE_READ | FILE_WRITE | FILE_CREATE_ALWAYS | 
					   FILE_CREATE_NEW;
            break;
        default:
            errno = EINVAL;
            return(-1);
    }
	
    if((file_pointers[free_index].fd = file_open(name, mode_val)) == -1) {
        /*todo map the return code to an error number */
        return(-1);
    }
    sw_strcpy(file_names[free_index],name);
    errno = 0;
    return(free_index);
#else
        sw_printk("file system configuration is not defined. \
                Enable CONFIG_FILESYSTEM_SUPPORT.\n");

        errno= EBADF;
        return (-1);
#endif
}


/**
* @brief 
*
* @param file
* @param ptr
* @param len
*
* @return 
*/
int _read(int file, char *ptr, int len)
{
	int bytes_read = 0;

	if(len == 0)
		return 0;
	if((file_pointers[file].fd == SW_STDERR) ||
			(file_pointers[file].fd == SW_STDOUT) || 
			(file_pointers[file].fd == SW_STDIN)) {
		/*while(UART_FR(UART0_ADDR) & UART_FR_RXFE);
		*ptr++ = UART_DR(UART0_ADDR);
		for(bytes_read = 1; bytes_read < len; bytes_read++) {
			if(UART_FR(UART0_ADDR) & UART_FR_RXFE) {
				break;
			}
			*ptr++ = UART_DR(UART0_ADDR);
		}*/
	} else {
#ifdef CONFIG_FILESYSTEM_SUPPORT
	    if(file_names[file][0] == '\0') {
	        errno = EBADF;
	        return(-1);
	    }
	    bytes_read=file_read(file_pointers[file].fd,ptr,len);
#else
        sw_printk("file system configuration is not defined. \
                Enable CONFIG_FILESYSTEM_SUPPORT.\n");

        errno= EBADF;
        return (-1);
#endif
	}
	errno = 0;
	return(bytes_read);
}

unsigned char *heap_end = 0;
unsigned char *heap_low = 0,*heap_top = 0;

/**
* @brief 
*
* @param incr
*
* @return 
*/
caddr_t sbrk(int incr) 
{
	unsigned char *prev_heap_end; 
	if(heap_low == 0) {
		heap_low = (unsigned char*)sw_vir_page_alloc(1<<20);
		heap_top = heap_low + (1<<20);
	}
	if (heap_end == 0) {
		heap_end = heap_low;
	}
	prev_heap_end = heap_end;
	if (heap_end + incr > heap_top) {
		/* Heap and stack collision */
		return (caddr_t)0;
	} 
	heap_end += incr;
	return (caddr_t) prev_heap_end;
}

/**
 * @brief 
 *
 * @param incr
 *
 * @return 
 */
caddr_t _sbrk(int incr) 
{
	sbrk(incr);
}

/**
 * @brief 
 *
 * @param file
 * @param ptr
 * @param len
 *
 * @return 
 */
int _write(int file, char *ptr, int len) 
{
    int todo = 0;
    u8 *local_buff = NULL;

    if((file_pointers[file].fd == SW_STDERR) || 
			(file_pointers[file].fd == SW_STDOUT) || 
			(file_pointers[file].fd == SW_STDIN)) {
        if((local_buff = (u8*)sw_malloc((len+1)*sizeof(u8))) != NULL) {
            sw_memcpy(local_buff,ptr,len);
            local_buff[len] = '\0';
            sw_printf(local_buff);
            sw_free(local_buff);
        }
        /*for (todo = 0; todo < len; todo++) {
            UART_DR(UART0_ADDR) = *ptr++;
        }*/
        todo = len;
    } else {
#ifdef CONFIG_FILESYSTEM_SUPPORT
        if(file_names[file][0] == '\0') {
            errno = EBADF;
            return(-1);
        }
        todo=file_write(file_pointers[file].fd,ptr,len);
#else
		sw_printk("file system configuration is not defined. \
				Enable CONFIG_FILESYSTEM_SUPPORT.\n");

		errno= EBADF;
		return (-1);
#endif
}
    errno = 0;
    return(todo);
}

/**
* @brief 
*
* @param val
*
* @return 
*/
int _exit(int val)
{
  return(val);
}

/**
* @brief 
*
* @param name
* @param argv
* @param env
*
* @return 
*/
int _execve(char *name, char **argv, char **env)
{
	errno = ENOMEM;
	return -1;
}

#define __MYPID 1

/**
* @brief 
*
* @return 
*/
int _getpid()
{
  return __MYPID;
}


/**
* @brief 
*
* @return 
*/
int _fork(void)
{
	errno = ENOTSUP;
	return -1;
}

/**
* @brief 
*
* @param pid
* @param sig
*
* @return 
*/
int _kill(int pid, int sig)
{
	if(pid == __MYPID)
		_exit(sig); 
	errno = EINVAL;
	return -1;
}

/**
 * @brief 
 *
 * @param status
 *
 * @return 
 */
int _wait(int *status)
{ 
	errno = ECHILD;
	return -1;
}

/**
* @brief 
*
* @param old
* @param new
*
* @return 
*/
int _link(char *old, char *new)
{
	errno = EMLINK;
	return -1;
}

/**
* @brief 
*
* @param file
* @param st
*
* @return 
*/
int _stat(const char* file, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
}

/**
* @brief 
*
* @param name
*
* @return 
*/
int _unlink(char *name)
{
#ifdef CONFIG_FILESYSTEM_SUPPORT
	int ul_val;
	ul_val=file_remove(name);
	if(ul_val!=-1)
	{
		sw_printf("file is unlink from file system successfully\n");
		return 0;
	}
	errno = ENOENT;
	sw_printf("file is not unlink from file system successfully\n");
	return -1;
#else
        sw_printk("file system configuration is not defined. \
                Enable CONFIG_FILESYSTEM_SUPPORT.\n");

        errno= EBADF;
        return (-1);
#endif
}

/**
* @brief 
*
* @param buf
*
* @return 
*/
clock_t _times(struct tms *buf)
{
	errno = ENOTSUP;
	return -1;
}

/**
* @brief 
*
* @param buf
* @param buf2
*
* @return 
*/
int _gettimeofday(struct timeval *buf, void *buf2)
{
	errno = ENOTSUP;
	return -1;
}

/**
 * @brief 
 *
 * @param clk_id
 * @param tp
 *
 * @return 
 */
int clock_gettime(clockid_t clk_id, struct timespec *tp)
{
	errno = ENOTSUP;
	return(-1);
}

