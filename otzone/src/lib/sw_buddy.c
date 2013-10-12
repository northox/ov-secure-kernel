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
 * Buddy allocator implementation.
 */

#include <sw_types.h>
#include <sw_debug.h>
#include <sw_list.h>
#include <sw_buddy.h>
#include <sw_mem_functions.h>
#include <cpu.h>
#include <global.h>
#include <elf_loader.h>
#ifndef NULL
#define NULL            ((void *)0)
#endif

#define _DEBUG          0

#if _DEBUG
#define OTZ_DPRINTK(fmt, ...)   sw_printf(fmt, ##__VA_ARGS__)
#else
#define OTZ_DPRINTK(fmt, ...)
#endif

#define IS_POW_TWO(x)       (x && !(x & (x - 1)))


static struct sw_heap buddy_heap[NUM_OF_HEAPS];

/**
* @brief 
*/
static struct taskid_hindex{
    int heap_id;
    int num_blocks_alloc;
}h_index[NUM_OF_HEAPS];

/**
 * @brief 
 *
 * @return 
 */
static struct sw_free_area *get_free_hk_node(int index)
{
    int idx = 0;
    struct sw_free_area *fren = buddy_heap[index].hk_fn_array;

    for (idx = 0; idx < buddy_heap[index].hk_fn_count; idx++) {
        if (fren->map) {
            fren++;
        } else {
            return fren;
        }
    }

    return NULL;
}

/**
 * @brief 
 *
 * @param node
 */
static void free_hk_node(struct sw_free_area *node)
{
    node->map = NULL;
}

/**
 * @brief 
 *
 * @return 
 */
static struct sw_alloced_area *get_free_ac_node(int index)
{
    int idx = 0;
    struct sw_alloced_area *fren = buddy_heap[index].hk_an_array;

    for (idx = 0; idx < buddy_heap[index].hk_an_count; idx++) {
        if (fren->map) {
            fren++;
        } else {
            return fren;
        }
    }

    return NULL;
}

/**
 * @brief 
 *
 * @param index
 * @param heap_start
 * @param heap_size
 *
 * @return 
 */
int buddy_init(int index, void *heap_start, unsigned int heap_size)
{
    int cntr = 0, tnodes = 0;
    void * mem_start;
    unsigned int  mem_size, hk_total_size;
    struct sw_free_area * freenode = NULL;

    /* We manage heap space only in power of two */
    if (!IS_POW_TWO(heap_size)) {
        return OTZ_EFAIL;
    }

    hk_total_size = (heap_size * HOUSE_KEEPING_PERCENT) / 100;

    /* keep 4K of initial heap area for our housekeeping */
    buddy_heap[index].hk_fn_array = (struct sw_free_area *)heap_start;
    buddy_heap[index].hk_fn_count = (hk_total_size / 2) / sizeof(struct sw_free_area);
    buddy_heap[index].hk_an_array = 
            (struct sw_alloced_area *)(heap_start + (hk_total_size / 2));
    buddy_heap[index].hk_an_count = (hk_total_size / 2) / 
                    sizeof(struct sw_alloced_area);
    sw_memset(buddy_heap[index].hk_fn_array, 0, hk_total_size);

    INIT_LIST_HEAD(&buddy_heap[index].current.head);

    mem_start = heap_start + hk_total_size;
    mem_size = heap_size - hk_total_size;
    buddy_heap[index].mem_start = mem_start;
    buddy_heap[index].mem_size = mem_size;
    buddy_heap[index].heap_start = heap_start;
    buddy_heap[index].heap_size = heap_size;
    sw_memset(&buddy_heap[index].free_area[0], 0, sizeof(buddy_heap[index].free_area));
    for (cntr = 0; cntr < BINS_MAX_ORDER; cntr++) {
        INIT_LIST_HEAD(&buddy_heap[index].free_area[cntr].head);
    }

    INIT_LIST_HEAD(&buddy_heap[index].free_area[BINS_MAX_ORDER - 1].head);

    while (mem_size > 0) {
        freenode = get_free_hk_node(index);
        if (!freenode) {
            OTZ_DPRINTK("no free node\n");
            return -1;
        }
        freenode->map = mem_start;
        mem_size -= MAX_BLOCK_SIZE;
        mem_start += MAX_BLOCK_SIZE;
        list_add_tail(&buddy_heap[index].free_area[BINS_MAX_ORDER - 1].head,
                  &freenode->head);
        buddy_heap[index].free_area[BINS_MAX_ORDER - 1].count++;
        tnodes++;

    }
    OTZ_DPRINTK("Total: 0x%x nodes of size 0x%x added to last bin.\n",
            tnodes, MAX_BLOCK_SIZE);

    return 0;
}

/**
 * @brief 
 *
 * @param num_blocks
 * @param idx
 *
 * @return 
 */
static struct sw_free_area *buddy_get_contiguous_block(int index, unsigned int num_blocks,
                            unsigned int idx)
{
    struct sw_free_area *snode = NULL, *cnode = NULL, *pnode = NULL;
    struct list *cnhead;
    unsigned int count = 0;

    if (idx >= BINS_MAX_ORDER) {
        return NULL;
    }

    if (list_empty(&buddy_heap[index].free_area[idx].head)) {
        return NULL;
    }

    /* First check if we have enough nodes, contiguous or non-contiguous. */
    if (buddy_heap[index].free_area[idx].count >= num_blocks) {
        /* okay we have enough nodes. Now try allocation contiguous nodes */
        list_for_each(cnhead, &buddy_heap[index].free_area[idx].head) {
            cnode = list_entry(cnhead, struct sw_free_area, head);
            if (cnode) {
                if (snode == NULL) {
                    snode = cnode;
                }
                if (pnode) {
                    if (pnode->map + MAX_BLOCK_SIZE ==
                        cnode->map) {
                        pnode = cnode;
                        if (++count == num_blocks) {
                            goto cont_blocks_found;
                        }
                        continue;
                    }
                    snode = NULL;
                } else {
                    pnode = cnode;
                }
            }
        }
cont_blocks_found:
        if (snode) {
            cnode = get_free_hk_node(index);
            
            if(!cnode) {
                DIE_NOW(0, "Panic: No free house keeping nodes for buddy allocator!\n");
            }

            sw_memcpy(cnode, snode, sizeof(struct sw_free_area));
            while (count) {
                /*
                 * Latch the next node to be released because
                 * after list_del information in snode will be gone.
                 */
                pnode =
                    list_entry(snode->head.next,
                           struct sw_free_area, head);
                list_del(&snode->head);
                buddy_heap[index].free_area[idx].count--;
                count--;
                snode = pnode;
            }
            if (buddy_heap[index].free_area[idx].count == 0) {
                INIT_LIST_HEAD(&buddy_heap[index].free_area[idx].head);
            }

            return cnode;
        }
    }

    return NULL;
}

/**
 * @brief 
 *
 * @param idx
 *
 * @return 
 */
static struct sw_free_area *buddy_get_block(int index, unsigned int idx)
{

    struct sw_free_area *farea = NULL, *rarea = NULL;
    struct list *lm;
    int blk_sz;

    if (idx >= BINS_MAX_ORDER) {
        return NULL;
    }

    if (list_empty(&buddy_heap[index].free_area[idx].head)) {
        /*
         * We borrowed a block from higher order. keep half of it and rest half
         * give to the caller.
         */
        /* This is what we keep for us for further allocation request. */
        farea = buddy_get_block(index, idx + 1);
        if (farea) {
            rarea = get_free_hk_node(index);
            if (rarea) {
                blk_sz = MIN_BLOCK_SIZE << idx;
                list_add_tail(&buddy_heap[index].free_area[idx].head,
                          &farea->head);
                buddy_heap[index].free_area[idx].count++;

                /* this is our buddy we will give to caller */
                rarea->map = farea->map + blk_sz;
            }
        }
    } else {
        lm = list_pop_tail(&buddy_heap[index].free_area[idx].head);
        buddy_heap[index].free_area[idx].count--;
        if (buddy_heap[index].free_area[idx].count == 0) {
            INIT_LIST_HEAD(&buddy_heap[index].free_area[idx].head);
        }
        rarea = list_entry(lm, struct sw_free_area, head);
    }

    return rarea;
}

/**
 * @brief 
 *
 * @param addr
 *
 * @return 
 */
static struct sw_alloced_area *search_for_allocated_block(int index, void *addr)
{
    struct sw_alloced_area *cnode;
    struct list *cnhead;

    list_for_each(cnhead, &buddy_heap[index].current.head) {
        cnode = list_entry(cnhead, struct sw_alloced_area, head);
        if (cnode && cnode->map == addr) {
            return cnode;
        }
    }

    return NULL;
}

/**
 * @brief 
 *
 * @param free_area
 * @param bin_num
 *
 * @return 
 */
static int add_free_area_to_bin(int index, struct sw_free_area *free_area,
                unsigned int bin_num)
{
    struct sw_free_area *carea = NULL;
    struct list *pos;

    if (list_empty(&buddy_heap[index].free_area[bin_num].head)) {
        list_add(&buddy_heap[index].free_area[bin_num].head, &free_area->head);
    } else {
        list_for_each(pos, &buddy_heap[index].free_area[bin_num].head) {
            carea = list_entry(pos, struct sw_free_area, head);
            if (carea->map > free_area->map) {
                break;
            }
        }
        list_add(&carea->head, &free_area->head);
    }
    buddy_heap[index].free_area[bin_num].count++;

    return 0;
}

/**
 * @brief 
 *
 * @param bin
 *
 * @return 
 */
static int coalesce_buddies(int index, unsigned int bin)
{
    struct list *pos;
    struct sw_free_area *lfa = NULL, *cfa = NULL;
    void *lmap = NULL;

restart:
    if (bin == BINS_MAX_ORDER - 1) {
        return 0;
    }

    list_for_each(pos, &buddy_heap[index].free_area[bin].head) {
        cfa = list_entry(pos, struct sw_free_area, head);
        if (lmap) {
            if ((lmap + (MIN_BLOCK_SIZE << bin)) == cfa->map) {
                OTZ_DPRINTK
                    ("Coalescing 0x%x and 0x%x and giving back to bin 0x%x\n",
                     (unsigned int)lfa->map,
                     (unsigned int)cfa->map, bin + 1);
                list_del(&cfa->head);
                list_del(&lfa->head);
                sw_memset(cfa, 0,
                       sizeof(struct sw_free_area));
                add_free_area_to_bin(index, lfa, bin + 1);
                lmap = NULL;
                lfa = NULL;
                cfa = NULL;
                buddy_heap[index].free_area[bin].count -= 2;

                /* restart the list afresh */
                goto restart;
            } else {
                lmap = cfa->map;
                lfa = cfa;
            }
        } else {
            lmap = cfa->map;
            lfa = cfa;
        }
    }

    coalesce_buddies(index, bin + 1);

    return 0;
}

/**
 * @brief 
 *
 * @param aarea
 *
 * @return 
 */
static int return_to_pool(int index, struct sw_alloced_area *aarea)
{
    struct sw_free_area *free_area = NULL;
    int bin_num = aarea->bin_num;

    free_area = get_free_hk_node(index);

    if (free_area) {
        free_area->map = aarea->map;
        add_free_area_to_bin(index, free_area, bin_num);
        if (buddy_heap[index].free_area[bin_num].count > 1) {
            coalesce_buddies(index, bin_num);
        }
    } else {
        return -1;
    }

    return 0;
}

/**
 * @brief 
 *
 * @param index
 * @param size
 *
 * @return 
 */
void *sw_buddy_malloc(int index, unsigned int size)
{
    int idx = 0;
    int curr_blk = MIN_BLOCK_SIZE;
    struct sw_free_area *farea = NULL;
    struct sw_alloced_area *aarea = NULL;
    u32 bneeded;

    if (size > buddy_heap[index].heap_size) {
        return NULL;
    }

    if (size > MAX_BLOCK_SIZE) {
        bneeded =
            (size %
             MAX_BLOCK_SIZE ? ((size / MAX_BLOCK_SIZE) +
                       1) : size / MAX_BLOCK_SIZE);
        farea = buddy_get_contiguous_block(index, bneeded, BINS_MAX_ORDER - 1);
        if (farea) {
            aarea = get_free_ac_node(index);

            if(!aarea) {
                DIE_NOW(0,
                       "Panic: No house keeping node available for buddy allocator.!\n");
            }

            aarea->map = farea->map;
            aarea->blk_sz = MAX_BLOCK_SIZE * bneeded;
            aarea->bin_num = BINS_MAX_ORDER - 1;
            list_add_tail(&buddy_heap[index].current.head, &aarea->head);
            buddy_heap[index].current.count++;
            free_hk_node(farea);
            return aarea->map;
        }
    }

    for (idx = 0; idx <= BINS_MAX_ORDER; idx++) {
        if (size > curr_blk) {
            curr_blk <<= 1;
        } else {
            farea = buddy_get_block(index, idx);
            if (farea) {
                aarea = get_free_ac_node(index);
                if (!aarea) {
                    OTZ_DPRINTK("Bummer! No free alloc node?\n");
                    return NULL;
                }
                aarea->map = farea->map;
                aarea->blk_sz = curr_blk;
                aarea->bin_num = idx;
                sw_memset(farea, 0,
                       sizeof(struct sw_free_area));
                free_hk_node(farea);
                list_add_tail(&buddy_heap[index].current.head,
                          &aarea->head);
                buddy_heap[index].current.count++;
                return aarea->map;
            }
        }
    }

    return NULL;
}

/**
 * @brief 
 *
 * @param size
 *
 * @return 
 */
void *buddy_zalloc(unsigned int size)
{
    int index, task_id = -1;

    task_id = get_current_task_id();

    for(index=0;index < NUM_OF_HEAPS ; index++){
        if(h_index[index].heap_id == task_id)
            break;
    }

    if(index == NUM_OF_HEAPS){
        OTZ_DPRINTK("No heap found for this task\n");
        return 0;
    }

    void *ptr = sw_buddy_malloc(index, size);

    if (ptr) {
        sw_memset(ptr, 0, size);
        h_index[index].num_blocks_alloc++;
    }

    return ptr;
}

/**
 * @brief 
 *
 * @param index
 * @param ptr
 */
void buddy_free(int index, void *ptr)
{

    /* FIXME: Handle the freeing of contiguously allocated nodes. */
    struct sw_alloced_area *freed_node = search_for_allocated_block(index, ptr);
    if (!freed_node) {
        OTZ_DPRINTK("Bugger! No allocations found for address 0x%x\n",
                (unsigned int)ptr);
        return;
    }

    OTZ_DPRINTK("Freeing 0x%x of block size: 0x%x bin: 0x%x\n",
            (unsigned int)ptr, freed_node->blk_sz, freed_node->bin_num);
    return_to_pool(index, freed_node);
    list_del(&freed_node->head);
    sw_memset(freed_node, 0, sizeof(struct sw_alloced_area));
}

/**
 * @brief 
 */
void sw_buddy_print_state(void)
{
    int idx = 0;
    struct sw_alloced_area *valloced;
    struct list *pos;
    int bfree = 0, balloced = 0;
    int index, task_id = -1;

    task_id = get_current_task_id();

    for(index=0;index < NUM_OF_HEAPS ; index++){
        if(h_index[index].heap_id == task_id)
            break;
    }

    if(index == NUM_OF_HEAPS){
        OTZ_DPRINTK("No heap found for this task\n");
        return;
    }

    OTZ_DPRINTK("Heap size: 0x%x KiB\n", (buddy_heap[index].heap_size / 1024));

    for (idx = 0; idx < BINS_MAX_ORDER; idx++) {
        OTZ_DPRINTK("[BLOCK 0x%x]: ", MIN_BLOCK_SIZE << idx);
        list_for_each(pos, &buddy_heap[index].free_area[idx].head) {
            bfree++;
        }
        list_for_each(pos, &buddy_heap[index].current.head) {
            valloced =
                list_entry(pos, struct sw_alloced_area, head);
            if (valloced->bin_num == idx) {
                balloced++;
            }
        }
        OTZ_DPRINTK("0x%x alloced, 0x%x free block(s)\n", balloced, bfree);
        bfree = 0;
        balloced = 0;
    }
}

/**
 * @brief 
 */
void buddy_print_hk_state(int index)
{
    u32 free = 0, idx;
    struct sw_free_area *fren = buddy_heap[index].hk_fn_array;
    struct sw_alloced_area *acn = buddy_heap[index].hk_an_array;

    for (idx = 0; idx < buddy_heap[index].hk_fn_count; idx++) {
        if (!fren->map) {
            free++;
        }
        fren++;
    }

    OTZ_DPRINTK("Free Node List: 0x%x nodes free out of 0x%x.\n", free,
           buddy_heap[index].hk_fn_count);

    free = 0;
    for (idx = 0; idx < buddy_heap[index].hk_an_count; idx++) {
        if (!acn->map) {
            free++;
        }
        acn++;
    }
    OTZ_DPRINTK("Alloced Node List: 0x%x nodes free out of 0x%x.\n", free,
           buddy_heap[index].hk_an_count);
}

/**
* @brief 
*
* @param heap_id
* @param size
*
* @return 
*/
void *sw_malloc_private(int heap_id ,u32 size)
{ 
    int index;
    u32 addr;

    for(index=0;index < NUM_OF_HEAPS ; index++){
        if(h_index[index].heap_id == heap_id)
            break;
    }

    if(index == NUM_OF_HEAPS){
        sw_printf("SW: No heap found for this task\n");
        return 0;
    }

    addr =  (u32)sw_buddy_malloc(index, size);
    if(addr > 0)
        h_index[index].num_blocks_alloc++;

#ifdef DEBUG
    sw_printf("SW: heap alloc 0x%x and pending alloc 0x%x\n",
        heap_id, h_index[index].num_blocks_alloc);
#endif
    return (u32*)addr;
}

/**
 * @brief 
 *
 * @param size
 *
 * @return 
 */
void *sw_malloc(u32 size)
{
    u32 addr;
    int task_id = -1;
    int index;

    task_id = get_current_task_id();

    for(index=0;index < NUM_OF_HEAPS ; index++){
        if(h_index[index].heap_id == task_id)
            break;
    }

    if(index == NUM_OF_HEAPS){
        sw_printf("SW: No heap found for this task\n");
        return 0;
    }
#ifdef DEBUG
    sw_printf("SW: alloc : %x \n", index);
#endif
    addr = (u32)sw_buddy_malloc(index, size);
    if(addr > 0)
        h_index[index].num_blocks_alloc++;
#ifdef DEBUG
    sw_printf("SW: addr 0x%08x size %x \n", addr, size);
#endif

#ifdef DEBUG
    sw_printf("SW: heap alloc 0x%x and pending alloc 0x%x\n",
        task_id, h_index[index].num_blocks_alloc);
#endif
    return (void*)addr;
}

EXPORT_SYMBOL(sw_malloc);
/**
 * @brief 
 *
 * @param heap_id
 * @param pointer
 */
void sw_free_private(int heap_id, void *pointer)
 {
    int index;

    for(index=0; index < NUM_OF_HEAPS; index++){
        if(h_index[index].heap_id == heap_id)
            break;
    }

    if(index == NUM_OF_HEAPS){
        sw_printf("SW: No heap found for this task\n");
        return;
    }

    buddy_free(index, pointer);
    h_index[index].num_blocks_alloc--;

#ifdef DEBUG
    sw_printf("SW: heap free 0x%x and pending alloc 0x%x\n",
        heap_id, h_index[index].num_blocks_alloc);
#endif
}

/**
 * @brief 
 *
 * @param pointer
 */
void sw_free(void *pointer)
{
    int task_id = -1;
    int index;

    task_id = get_current_task_id();  

    for(index=0; index < NUM_OF_HEAPS; index++){
        if(h_index[index].heap_id == task_id)
            break;
    }

    if(index == NUM_OF_HEAPS){
        sw_printf("SW: No heap found for this task\n");
        return; 
    }

#ifdef DEBUG
    sw_printf("SW: free : %x \n", index);
    sw_printf("SW: addr 0x%08x  \n", (u32)pointer);
#endif
    buddy_free(index, pointer);
    h_index[index].num_blocks_alloc--;

#ifdef DEBUG
    sw_printf("SW: heap free 0x%x and pending alloc 0x%x\n",
        task_id, h_index[index].num_blocks_alloc);
#endif
}

EXPORT_SYMBOL(sw_free);
 
/**
 * @brief 
 *
 * @param index
 * @param heap_start
 * @param heap_size
 *
 * @return 
 */
int sw_heap_init(int index, void *heap_start, unsigned int heap_size)
{
    h_index[index].heap_id = -1;
    h_index[index].num_blocks_alloc = 0;
    return buddy_init(index, heap_start, heap_size);
}


/**
* @brief 
*
* @param heap_id
*
* @return 
*/
int alloc_private_heap(int heap_id)
{
    int index;

    for(index=0; index < NUM_OF_HEAPS; index++){
        if(h_index[index].heap_id == heap_id){
            return 0;
        }
    }
    
    for(index=0; index < NUM_OF_HEAPS; index++){
        if(h_index[index].heap_id == -1){
            h_index[index].heap_id = heap_id;
            return 0;
        }
    }
    
    sw_printf("SW: Heap allocation for task : %x failed \n", heap_id);
    return -1;
}

/**
* @brief 
*
* @param heap_id
*
* @return 
*/
int free_private_heap(int heap_id)
{
    int index;

    for(index=0; index < NUM_OF_HEAPS; index++){
        if(h_index[index].heap_id == heap_id){
            break;
        }
    }
    
    if(index == NUM_OF_HEAPS){
        sw_printf("SW:  No heap available for this task\n");
        return -1;
    }

    if(h_index[index].num_blocks_alloc != 0){
        sw_printf("SW: Error : This heap not to be freed 0x%x and \
            pending free 0x%x\n", heap_id, h_index[index].num_blocks_alloc);
        return -1;
    }
    else{
        h_index[index].heap_id = -1;
        return 0;
    }
}

/**
* @brief 
*
* @param ptr
*
* @return 
*/
u32 get_ptr_size(void* ptr)
{
    int task_id = -1;
    int index;

    task_id = get_current_task_id();

    for(index=0; index < NUM_OF_HEAPS; index++){
        if(h_index[index].heap_id == task_id)
            break;
    }

    if(index == NUM_OF_HEAPS){
        OTZ_DPRINTK("No heap found for this task\n");
        return -1;
    }

    struct sw_alloced_area *node = search_for_allocated_block(index, ptr);
	if(node == NULL) {
		return(0);
	}
	return(node->blk_sz);
}
