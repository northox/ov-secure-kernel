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

/* This is the header file for those libc functions that may be needed by other
 * apps. 
 *
 *    *****************    IMPORTANT   ************************
 *    Note that we could have done something simple like 
 *    #define memset sw_memset 
 *    and could have been done with it. However, gcc automatically inserts memcpy
 *    for structure assignments. Hence, we need something along the lines of
 *    function aliasing that is found in c++. For now, we will make these
 *    functions invoke our implementations.
 *    ********* END IMPORTANT SECTION ****************
 */


#ifndef LIBC_SOS_H
#define LIBC_SOS_H

/*
 * @brief memove function, it follows the same parameter format as the one in
 * libc
 */
void* memmove(void * dest,const void *src, unsigned int count);

/*
 * @brief memset function, it follows the same parameter format as the one in
 * libc
 **/
void* memset(void * dest, unsigned int c, unsigned int count);

/*
 * @brief memcpy function, it follows the same parameter format as the one in
 * libc. Further, the memory areas must not overlap.
 **/
void* memcpy(void *dst, const void *src, unsigned int count);

/*
 * @brief malloc function, it follows the same parameter format as the one in
 * libc.
 **/
void* malloc(unsigned int size);

/*
 * @brief realloc function, it follows the same parameter format as the one in
 * libc
 **/
void* realloc(void *ptr, unsigned int size);

/*
 * @brief calloc function, it follows the same parameter format as the one in
 * libc
 **/
void* calloc(unsigned int num_elements, unsigned int element_size);

/*
 * @brief free function, it follows the same parameter format as the one in
 * libc
 **/
void free(void *ptr);

/*
 * @brief strncmp function, it follows the same parameter format as the one in
 * libc
 **/
int strncmp(const char *s1, const char *s2, unsigned int n);

/*
 * @brief atoi function, it follows the same parameter format as the one in libc
 *
 **/
unsigned int atoi(const char *ptr);

/*
 * @brief strlen function, it follows the same parameter format as the one in
 * libc
 **/
unsigned int strlen(const char *ptr);

/*
 * @brief strcpy function, it follows the same parameter format as the one in
 * libc
 **/
char* strcpy(char *ptr1, const char *ptr2);

/*
 * @brief strcmp function, it follows the same parameter format as the one in
 * libc
 **/
int strcmp(const char *ptr1, const char *ptr2);

/*
 * @brief strcat function, it follows the same parameter format as the one in
 * libc
 **/
char* strcat(char *ptr1, const char *ptr2);

/**
* @brief 
*/
typedef struct {
    unsigned int init_param;
    volatile unsigned int lock_param;
} pthread_mutex_t;

#define PTHREAD_MUTEX_INITIALIZER {1,0}
/* We do not support attributes */
#define pthread_mutexattr_t void

/*
 * @brief pthread_mutex_init function, it follows the same parameter format as
 * the one in libc
 **/
int pthread_mutex_init(pthread_mutex_t *mutex, 
			const pthread_mutexattr_t *attribute);


/*
 * @brief pthread_mutex_destroy function, it follows the same parameter format
 * as the one in libc
 **/
int pthread_mutex_destroy(pthread_mutex_t *mutex);


/*
 * @brief pthread_mutex_lock function, it follows the same parameter format as
 * the one in libc
 **/
int pthread_mutex_lock(pthread_mutex_t *mutex);

/*
 * @brief pthread_mutex_unlock function, it follows the same parameter format as
 * the one in libc
 **/
int pthread_mutex_unlock(pthread_mutex_t *mutex);

/*
 * @brief pthread_mutex_trylock function, it follows the same parameter format
 * as the one in libc
 **/
int pthread_mutex_trylock(pthread_mutex_t *mutex);


/**
* @brief 
*/
typedef struct {
    unsigned int init_param;
    volatile unsigned int counter_param;
} sem_t;

/*
 * @brief sem_init function, it follows the same parameter format as the one in
 * libc
 **/
int sem_init(sem_t *sem, int shared, unsigned int value);

/*
 * @brief sem_wait function, it follows the same parameter format as the one in
 * libc
 **/
int sem_wait(sem_t *sem);

/*
 * @brief sem_post function, it follows the same parameter format as the one in
 * libc
 **/
int sem_post(sem_t *sem);

/*
 * @brief sem_destroy function, it follows the same parameter format as the one
 * in libc
 **/
int sem_destroy(sem_t *sem);


/*
 * @brief sem_getvalue function, it follows the same parameter format as the one
 * in libc
 **/
int sem_getvalue(sem_t *sem, int *value);

/*
 * @brief sleep function, it follows the same parameter format as the one
 * in libc
 **/
int sleep(unsigned int seconds);

/*
 * @brief usleep function, it follows the same parameter format as the one
 * in libc. Instead of microsecond sleep, we do a millisecond sleep.
 **/
int usleep(unsigned int seconds);

/**
* @brief
* truncate - function cause the regular file named by path 
*            to be truncated to a size of precisely length bytes
*
* @param tr_path - truncates the file whose name is specified in filename
* @param tr_lth -  The desired truncate size of the file in bytes
*
* @return - If the file contents is successfully truncated, a zero value is 
*           returned.On failure, non-Zero is returned
*/
int truncate(const char* tr_path, long tr_lth);

/**
 * @brief 
 * rename - Changes the name of the file or directory specified by oldname to 
 *          newname
 *
 * @param old_name -string containing the name of an existing file to be renamed
 * @param new_name -string containing the new name for the file
 *
 * @return - If the file is successfully renamed, a zero value is returned.
 *  On failure, a nonzero value is returned
 */
int rename(const char *old_name,const char *new_name);

#endif
