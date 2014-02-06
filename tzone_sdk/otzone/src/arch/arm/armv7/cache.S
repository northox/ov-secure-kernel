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
 *   ARMV7 Cache function
 *
 */

/**
 * @brief 
 */
.global v7_flush_dcache_all
.func v7_flush_dcache_all
v7_flush_dcache_all:
    dmb                 @ ensure ordering with previous memory accesses
    mrc p15, 1, r0, c0, c0, 1       @ read clidr
    ands    r3, r0, #0x7000000      @ extract loc from clidr
    mov r3, r3, lsr #23         @ left align loc bit field
    beq finished            @ if loc is 0, then no need to clean
    mov r10, #0             @ start clean at cache level 0
loop1:
    add r2, r10, r10, lsr #1        @ work out 3x current cache level
    mov r1, r0, lsr r2          @ extract cache type bits from clidr
    and r1, r1, #7          @ mask of the bits for current cache only
    cmp r1, #2              @ see what cache we have at this level
    blt skip                @ skip if no cache, or just i-cache
    mcr p15, 2, r10, c0, c0, 0      @ select current cache level in cssr
    isb                 @ isb to sych the new cssr&csidr
    mrc p15, 1, r1, c0, c0, 0       @ read the new csidr
    and r2, r1, #7          @ extract the length of the cache lines
    add r2, r2, #4          @ add 4 (line length offset)
    ldr r4, =0x3ff
    ands    r4, r4, r1, lsr #3      @ find maximum number on the way size
    clz r5, r4              @ find bit position of way size increment
    ldr r7, =0x7fff
    ands    r7, r7, r1, lsr #13     @ extract max number of the index size
loop2:
    mov r9, r4              @ create working copy of max way size
loop3:
    orr r11, r10, r9, lsl r5        @ factor way and cache number into r11
    orr r11, r11, r7, lsl r2        @ factor index number into r11
    mcr p15, 0, r11, c7, c14, 2     @ clean & invalidate by set/way
    subs    r9, r9, #1          @ decrement the way
    bge loop3
    subs    r7, r7, #1          @ decrement the index
    bge loop2
skip:
    add r10, r10, #2            @ increment cache number
    cmp r3, r10
    bgt loop1
finished:
    mov r10, #0             @ swith back to cache level 0
    mcr p15, 2, r10, c0, c0, 0      @ select current cache level in cssr
    dsb
    isb
    mov pc, lr
 .endfunc

 /**
  * @brief 
  */
.global flush_page_table_entry
.func flush_page_table_entry
flush_page_table_entry:
    mcr p15, 0, r0, c7, c10, 1      @ clean D entry
    dsb
    mcr p15, 0, r0, c7, c5, 6       @ flush the branch target cache
    dsb
    isb

    dsb 
    mov pc, lr
.endfunc

/**
 * @brief 
 */
.global invalidate_data_cache_line
.func invalidate_data_cache_line
invalidate_data_cache_line :
    mcr p15, 0, r0, c7, c6, 1       @ Invalidate D entry
    dsb
    isb
    mov pc, lr
.endfunc

/*
 * cache_line_size - get the cache line size from the CSIDR register
 * (available on ARMv7+). It assumes that the CSSR register was configured
 * to access the L1 data cache CSIDR.
 */
        .macro  dcache_line_size, reg, tmp
        mrc     p15, 1, \tmp, c0, c0, 0         @ read CSIDR
        and     \tmp, \tmp, #7                  @ cache line size encoding
        mov     \reg, #16                       @ size offset
        mov     \reg, \reg, lsl \tmp            @ actual cache line size
        .endm


/**
 * @brief 
 */
.global clean_dcache_area
.func clean_dcache_area
clean_dcache_area:
#ifndef TLB_CAN_READ_FROM_L1_CACHE
    PUSH {R2,R3}
        dcache_line_size r2, r3
1:      mcr     p15, 0, r0, c7, c10, 1          @ clean D entry
        add     r0, r0, r2
        subs    r1, r1, r2
        bhi     1b
    POP { R2,R3}
        dsb
#endif
        mov     pc, lr
.endfunc



/**
 * @brief 
 */
.global clean_invalidate_dcache
.func clean_invalidate_dcache
/*
  ; void clean_invalidate_dcache(void)
*/
clean_invalidate_dcache:
  PUSH    {r0-r12}
  
/*
  ; Based on code example given in section 11.2.4 of ARM DDI 0406B
*/

  MRC     p15, 1, r0, c0, c0, 1     /*  Read CLIDR */
  ANDS    r3, r0, #0x7000000
  MOV     r3, r3, LSR #23           /*  Cache level value (naturally aligned) */
  BEQ     clean_invalidate_dcache_finished
  MOV     r10, #0

clean_invalidate_dcache_loop1:
   ADD     r2, r10, r10, LSR #1      /*  Work out 3xcachelevel */
  MOV     r1, r0, LSR r2            /*  bottom 3 bits are the Cache type for this level */
  AND     r1, r1, #7                /*  get those 3 bits alone */
  CMP     r1, #2
  BLT     clean_invalidate_dcache_skip /*  no cache or only instruction cache at this level */
  MCR     p15, 2, r10, c0, c0, 0    /*  write the Cache Size selection register */
  ISB                               /*  ISB to sync the change to the CacheSizeID reg */
  MRC     p15, 1, r1, c0, c0, 0     /*  reads current Cache Size ID register */
  AND     r2, r1, #0x7               /*  extract the line length field */
  ADD     r2, r2, #4                /*  add 4 for the line length offset (log2 16 bytes) */
  LDR     r4, =0x3FF
  ANDS    r4, r4, r1, LSR #3        /*  R4 is the max number on the way size (right aligned) */
  CLZ     r5, r4                    /*  R5 is the bit position of the way size increment */
  LDR     r7, =0x00007FFF
  ANDS    r7, r7, r1, LSR #13       /*  R7 is the max number of the index size (right aligned) */

clean_invalidate_dcache_loop2:
  MOV     r9, R4                    /*  R9 working copy of the max way size (right aligned) */

clean_invalidate_dcache_loop3:
  ORR     r11, r10, r9, LSL r5      /*  factor in the way number and cache number into R11 */
  ORR     r11, r11, r7, LSL r2      /*  factor in the index number */
  @MCR     p15, 0, r11, c7, c14, 2   /*  DCCISW - clean and invalidate by set/way */
  MCR     p15, 0, r11, c7, c6, 2   /*  DCISW - Invalidate by set/way */
  SUBS    r9, r9, #1                /*  decrement the way number */
  BGE     clean_invalidate_dcache_loop3
  SUBS    r7, r7, #1                /*  decrement the index */
  BGE     clean_invalidate_dcache_loop2

clean_invalidate_dcache_skip:
  ADD     r10, r10, #2              /*  increment the cache number */
  CMP     r3, r10
  BGT     clean_invalidate_dcache_loop1

clean_invalidate_dcache_finished:
        mov     r10, #0                         @ swith back to cache level 0  
        mcr     p15, 2, r10, c0, c0, 0          @ select current cache level in cssr 
        dsb                 @ 
        isb                 @ 
  POP     {r0-r12}

   BX      lr
.endfunc


/*
 *  v7_flush_cache_all()
 *
 *  Flush the entire cache system.
 *  The data cache flush is now achieved using atomic clean / invalidates
 *  working outwards from L1 cache. This is done using Set/Way based cache
 *  maintainance instructions.
 *  The instruction cache can still be invalidated back to the point of
 *  unification in a single instruction.
 *
 */

/**
 * @brief 
 */
.global flush_cache_all
.func flush_cache_all
flush_cache_all:

/*  push    { r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, lr} */
        stmfd   sp!, {r0-r12, lr}
    bl  v7_flush_dcache_all 
    mov r0, #0
    mcr p15, 0, r0, c7, c5, 0       @ I+BTB cache invalidate
    dsb
    isb
/*  pop     { r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, lr} */
       ldmfd   sp!, {r0-r12, lr}
    mov pc, lr
.endfunc

/**
 * @brief 
 */
.global flush_tlb_all
.func flush_tlb_all
flush_tlb_all:
@        mcr     p15, 0, r0, c8, c7,  0
        mcr     p15, 0, r0, c8, c3,  0
        mcr     p15, 0, r0, c7, c5,  6  @ Inval. branch predict. array
        dsb
        isb
        mov pc, lr
.endfunc

var1:
  .word 0x000003ff
var2:
  .word 0x00007fff
