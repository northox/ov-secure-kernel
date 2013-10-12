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

/**
 header file for buddy allocator in Trustzone Kernel
 */

#ifndef __SW_BUDDY_H_
#define __SW_BUDDY_H_

#include <sw_list.h>
#include <sw_types.h>

#ifndef CONFIG_BUDDY_HOUSE_KEEPING_PERCENT
#define CONFIG_BUDDY_HOUSE_KEEPING_PERCENT 25
#endif

#ifndef CONFIG_BUDDY_MIN_BLOCK_SIZE_SHIFT
#define CONFIG_BUDDY_MIN_BLOCK_SIZE_SHIFT 2
#endif

#ifndef CONFIG_BUDDY_MAX_BLOCK_SIZE_SHIFT
#define CONFIG_BUDDY_MAX_BLOCK_SIZE_SHIFT 12
#endif


#define HOUSE_KEEPING_PERCENT   (CONFIG_BUDDY_HOUSE_KEEPING_PERCENT)
/* Minimum alloc of bus width */
#define MIN_BLOCK_SIZE      (0x01UL << CONFIG_BUDDY_MIN_BLOCK_SIZE_SHIFT)
/* Maximum alloc of bus width */
#define MAX_BLOCK_SIZE      (0x01UL << CONFIG_BUDDY_MAX_BLOCK_SIZE_SHIFT)
#define BINS_MAX_ORDER      (CONFIG_BUDDY_MAX_BLOCK_SIZE_SHIFT - \
                                    CONFIG_BUDDY_MIN_BLOCK_SIZE_SHIFT + 1)

#define NUM_OF_HEAPS    8
#define HEAP_SIZE       (1 << 16) /* 64 KB */
#define COMMON_HEAP_ID   0

/**
 * @brief 
 */
struct sw_free_area {
    struct list head;
    void *map;
    unsigned int count;
} __attribute__ ((packed));

/**
 * @brief 
 */
struct sw_alloced_area {
    struct list head;
    void *map;
    unsigned int blk_sz;
    unsigned int bin_num;
    unsigned int count;
} __attribute__ ((packed));

/**
 * @brief 
 */
struct sw_heap {
    struct sw_free_area * hk_fn_array;
    unsigned int hk_fn_count;
    struct sw_alloced_area * hk_an_array;
    unsigned int hk_an_count;
    struct sw_alloced_area current;
    void *mem_start;
    unsigned int mem_size;
    void *heap_start;
    unsigned int heap_size;
    struct sw_free_area free_area[BINS_MAX_ORDER];  /* Bins holding free area. */
} __attribute__ ((packed));

/**
 * @brief 
 *
 * @param index
 * @param heap_start
 * @param heap_size
 *
 * @return 
 */
int buddy_init(int index, void *heap_start, unsigned int heap_size);

/**
 * @brief 
 *
 * @param index
 * @param size
 *
 * @return 
 */
void *sw_buddy_malloc(int index, unsigned int size);

/**
 * @brief 
 *
 * @param size
 *
 * @return 
 */
void *buddy_zalloc(unsigned int size);

/**
 * @brief 
 *
 * @param index
 * @param ptr
 */
void buddy_free(int index, void *ptr);

/**
 * @brief 
 */
void sw_buddy_print_state(void);

/**
 * @brief 
 */
void buddy_print_hk_state(int index);

/**
 * @brief 
 *
 * @param size
 *
 * @return 
 */
void *sw_malloc(u32 size);

/**
 * @brief 
 *
 * @param pointer
 */
void sw_free(void *pointer);

/**
 * @brief 
 *
 * @param heap_start
 * @param heap_size
 *
 * @return 
 */
int sw_heap_init(int index, void *heap_start, unsigned int heap_size);

/**
 * @brief function to get the size of a pointer. This is needed during realloc
 *
 * @param ptr - the pointer whose size we need to determine
 * @return - The size of the pointer
 *
 **/
unsigned int get_ptr_size(void* ptr);

/**
 * @brief 
 *
 * @param heap_id
 * @param size
 *
 * @return 
 */
void *sw_malloc_private(int heap_id ,u32 size);

/**
 * @brief 
 *
 * @param heap_id
 * @param pointer
 */
void sw_free_private(int heap_id, void *pointer);

/**
 * @brief 
 *
 * @param heap_id
 *
 * @return 
 */
int alloc_private_heap(int heap_id);

/**
 * @brief 
 *
 * @param heap_id
 *
 * @return 
 */
int free_private_heap(int heap_id);

#endif /* __SW_BUDDY_H_ */
