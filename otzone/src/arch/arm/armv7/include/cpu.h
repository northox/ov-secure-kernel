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
 * Header for cpu info implementation
 */

#ifndef __CPU_ARCH__CPU_H__
#define __CPU_ARCH__CPU_H__

#include <sw_types.h>

#define CACHELINE_SIZE 32


#define __asmeq(x, y)  ".ifnc " x "," y " ; .err ; .endif\n\t"


#define SCTLR_M     0x00000001  /* MMU bit        */
#define SCTLR_C     (1 << 2)    /* Dcache enable      */
#define SCTLR_I     (1 << 12)   /* Icache enable      */
#define SCTLR_Z     (1 << 11)   /* Branch prediction enable      */

#define isb() __asm__ __volatile__("ISB")
#define dmb() __asm__ __volatile__("DMB")
#define dsb() __asm__ __volatile__("DSB")

/**
 * @brief 
 *
 * NOP must be encoded as 'MOV r0,r0' in ARM code and 'MOV r8,r8' 
 * in Thumb code, see ARMv7-A/R ARM C.2
 *
 */
#define nop() __asm__ __volatile__("mov\tr0,r0\t@ nop\n\t");

/**
 * @brief 
 */
#define get_mpid()      ({ u32 rval; asm volatile(\
                " mrc     p15, 0, %0, c0, c0, 5\n\t" \
                : "=r" (rval) : : "memory", "cc"); rval;})
/**
 * @brief 
 */
#define read_sctlr()        ({ u32 rval; asm volatile(\
                " mrc     p15, 0, %0, c1, c0, 0\n\t" \
                : "=r" (rval) : : "memory", "cc"); rval;})

/**
 * @brief 
 */
#define write_sctlr(val)    ({ asm volatile(\
                " mcr     p15, 0, %0, c1, c0, 0\n\t" \
                " isb \n\t" \
                :: "r" ((val)) : "memory", "cc");})

/**
 * @brief 
 */
#define read_dfsr()     ({ u32 rval; asm volatile(\
                " mrc     p15, 0, %0, c5, c0, 0\n\t" \
                : "=r" (rval) : : "memory", "cc"); rval;})

/**
 * @brief 
 */
#define read_dfar()     ({ u32 rval; asm volatile(\
                " mrc     p15, 0, %0, c6, c0, 0\n\t" \
                : "=r" (rval) : : "memory", "cc"); rval;})

/**
 * @brief 
 */
#define read_ifsr()     ({ u32 rval; asm volatile(\
                " mrc     p15, 0, %0, c5, c0, 1\n\t" \
                : "=r" (rval) : : "memory", "cc"); rval;})

/**
 * @brief 
 */
#define read_ifar()     ({ u32 rval; asm volatile(\
                " mrc     p15, 0, %0, c6, c0, 2\n\t" \
                : "=r" (rval) : : "memory", "cc"); rval;})

/**
 * @brief 
 */
#define read_vbar()     ({ u32 rval; asm volatile(\
                " mrc     p15, 0, %0, c12, c0, 0\n\t" \
                : "=r" (rval) : : "memory", "cc"); rval;})

/**
 * @brief 
 */
#define clear_icache()      ({ asm volatile(\
                " mcr     p15, 0, r0, c7, c5, 0\n\t" \
                ::: "memory", "cc");})

/**
 * @brief 
 */
#define inv_branch_predict()    ({ asm volatile(\
                " mcr     p15, 0, r0, c7, c5, 6\n\t" \
                ::: "memory", "cc");})

/**
 * @brief 
 */
#define get_ttbcr()     ({ u32 rval; asm volatile(\
                " mrc     p15, 0, %0, c2, c0, 0\n\t" \
                : "=r" (rval) : : "memory", "cc"); rval;})

/**
 * @brief 
 */
#define set_ttbcr(val)      ({ asm volatile(\
                " mcr     p15, 0, %0, c2, c0, 0\n\t" \
                :: "r" ((val)) : "memory", "cc");})

/**
 * @brief 
 */
#define read_domain()       ({ u32 rval; asm volatile(\
                " mrc     p15, 0, %0, c3, c0, 0\n\t" \
                : "=r" (rval) : : "memory", "cc"); rval;})

/**
 * @brief 
 */
#define write_domain(val)   ({ asm volatile(\
                " mcr     p15, 0, %0, c3, c0, 0\n\t" \
                :: "r" ((val)) : "memory", "cc");})

/**
 * @brief 
 */
#define read_ttbr0()        ({ u32 rval; asm volatile(\
                " mrc     p15, 0, %0, c2, c0, 0\n\t" \
                : "=r" (rval) : : "memory", "cc"); rval;})

/**
 * @brief 
 */
#define write_ttbr0(val)    asm volatile(\
                " mcr     p15, 0, %0, c2, c0, 0\n\t" \
                :: "r" ((val)) : "memory", "cc")

/**
 * @brief 
 */
#define read_ttbr1()        ({ u32 rval; asm volatile(\
                " mrc     p15, 0, %0, c2, c0, 1\n\t" \
                : "=r" (rval) : : "memory", "cc"); rval;})

/**
 * @brief 
 */
#define write_ttbr1(val)    asm volatile(\
                " mcr     p15, 0, %0, c2, c0, 1\n\t" \
                :: "r" ((val)) : "memory", "cc")


/**
* @brief 
*
*/
#define read_thread_id() ({u32 rval; asm volatile(\
            "mrc    p15, 0, %0, c13, c0, 4\n\t" \
            : "=r" (rval) : : "memory", "cc"); rval;})

/**
* @brief 
*  
*/
#define write_thread_id(val)   asm volatile(\
            "mcr    p15, 0, %0, c13, c0, 4\n\t" \
            :: "r" ((val)) : "memory" , "cc")

/**
 * @brief 
 * Infinite loop waiting for interrupts (even if they are masked)
 */
#define infinite_idle_loop() \
  { \
    while (TRUE) \
    { \
      __asm__ __volatile__ ("WFI"); \
    } \
  }


/**
* @brief 
*/
void icache_enable(void);

/**
 * @brief 
 */
void icache_disable(void);

/**
 * @brief 
 */
void dcache_enable(void);

/**
 * @brief 
 */
void dcache_disable(void);

/**
 * @brief 
 */
void enable_l1_cache(void);

/**
 * @brief 
 */
void disable_l1_cache(void);

/**
 * @brief 
 *
 * @return 
 */
irq_flags_t cpu_irq_save(void);

/**
 * @brief 
 *
 * @param flags
 */
void cpu_irq_restore(irq_flags_t flags);

/**
 * @brief 
 */
void cpu_wait_for_irq(void);
/**
 * @brief 
 */
void clear_data_cache(void);

/**
 * @brief 
 *
 * @return 
 */
u32 get_cpu_id(void);

/**
 * @brief 
 */
void enable_branch_prediction(void);

/**
 * @brief 
 */
void arm_irq_enable(void);

/**
 * @brief 
 */
void arm_irq_disable(void);

/**
 * @brief 
 */
void emulate_timer_irq(void);


/**
* @brief 
*
* @param swi_id
*/
void emulate_swi_handler(int swi_id);

/**
 * @brief 
 */
void data_memory_barrier(void);

/**
 * @brief 
 */
void data_sync_barrier(void);

/**
 * @brief 
 */
void instruction_sync_barrier(void);

/**
 * @brief 
 */
void start_secondary_core(void);

#endif
