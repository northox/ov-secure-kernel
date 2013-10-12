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
 * cpu entry implementation
 */

/* Entry point */
#include <cpu_asm.h>
#include <sw_board_asm.h>	
#include <sw_platform_asm.h>
#include <asm_macros.h>	

.text
.align 4
.globl _start
_start:
    b _start_1


.align 12
.global secure_exception_vectors
secure_exception_vectors:
/*
Vector table 
*/
    B      _reset_handler
    B      _undefined_handler
    B      _swi_handler
    B      _prefetch_handler
    B      _abort_handler
    b      _reserved_vector
    B      _irq_handler
    B      _fiq_handler

_reset_handler:
    B       _reset_handler

_reserved_vector:
    B       _reserved_vector

_fiq_handler:
    B		_fiq_handler

/*
Handlers for unused exceptions
*/

.global _undefined_handler
_undefined_handler:
    bl		undefined_handler

.global _swi_handler
.func   _swi_handler
_swi_handler:
    stmfd   sp!, {r0-r12, lr}
    ldr     r0,  [lr, #-4] 
    bic     r0, r0, #0xff000000
    mrs     r2, spsr
    stmfd   sp!, {r2}
    mov   r1, sp 
    bl      swi_handler
    ldmfd   sp!, {r2}
    msr     spsr, r2
    push    {r0}
    mov     r0, #0
    dsb
    pop     {r0}
    ldmfd   sp!, {r0-r12, pc}^ 
.endfunc


.global emulate_swi_handler
.func   emulate_swi_handler
emulate_swi_handler:
    cps     #0x11
    stmfd   sp, {r0-r12, lr}^
    sub     sp, sp, #(14*4)
    bic     r0, r0, #0xff000000
    mrs     r2, spsr
    stmfd   sp!, {r2}
    mov     r1, sp
    bl      swi_handler
    ldmfd   sp!, {r2}
    msr     spsr, r2
    ldmfd   sp, {r0-r12, lr}^
    add     sp, sp, #(14*4)
    cps     #0x1f
    mov     pc, lr    
.endfunc

.global _prefetch_handler
_prefetch_handler:
    bl       prefetch_abort_handler

.global _abort_handler
_abort_handler:
    bl      data_abort_handler

/*
Interrupt Handler
*/
_irq_handler:
    B _irq_handler

.global get_cpu_cores
.func get_cpu_cores
  @ uint32_t get_cpu_cores(void)
  @ Returns the cpu cores present on the system
get_cpu_cores:

#ifdef CONFIG_CORTEX_A15
    mrc     p15, 1, r0, c9, c0, 2
    ror     r0, #24
    and     r0, r0, #0x03           @ Mask off, leaving the CPU ID field
    add     r0, r0, #1
#endif
#ifdef CONFIG_CORTEX_A9

    mrc     p15,4,r0,c15,c0,0
    cmp     r0, #0
    beq     ret_cores
    ldr     r0, =__scu_base_reg   
    ldr     r0, [r0]   
    add     r0, r0, #0x4    @ SCU Config register
    ldr     r1, [r0]
    and     r0, r1, #0x3
    add     r0, r0, #1
#endif
#ifdef CONFIG_CORTEX_A8
/*
    mrc     p15,4,r0,c15,c0,0
    cmp     r0, #0
    beq     ret_cores
    ldr     r0, =__scu_base_reg   
    ldr     r0, [r0]   
    add     r0, r0, #0x4    @ SCU Config register
    ldr     r1, [r0]
    and     r0, r1, #0x3
*/
    mov     r0, #1
#endif


ret_cores:
    bx      lr
.endfunc



.global get_cpu_id
.func get_cpu_id
  @ uint32_t get_cpu_id(void)
  @ Returns the CPU ID (0 to 3) of the CPU executed on
get_cpu_id:
    mrc     p15, 0, r0, c0, c0, 5   @ Read CPU ID register
    and     r0, r0, #0x03           @ Mask off, leaving the CPU ID field
    bx      lr
.endfunc

.global go_to_sleep
.func go_to_sleep
  @ void go_to_sleep(void)
go_to_sleep:
    wfi                 @ Go into standby
    b       go_to_sleep @ Catch in case of rogue events
    bx      lr
.endfunc


.globl _start_1
_start_1:

/*  In uniprocessor system,  run secure and non-secure os on core 0.
    In multi-processor system,  run secure in core 1 and non-secure os on other 
    cores.
*/
#ifdef CONFIG_SW_DEDICATED_TEE
    bl      get_cpu_cores
    cmp     r0, #1
    bne     check_cpu_id
    mov     r0, #0
    b       __start_secure_core

check_cpu_id:
    ldr     r1, =multi_core_mode
    mov     r2, #1
    str     r2, [r1]

    bl      get_cpu_id              @ Returns CPU ID in r0
    cmp     r0, #1                  @ Is this CPU0?
    blne    _secondary_cpus_start   @ call non-secure core entry function
#else
    GET_CPU_ID    r0
    cmp     r0,	#0                  @ Is this CPU0?
#ifdef CONFIG_SW_MULTICORE

#ifdef CONFIG_SW_NOBOOTLOADER
    blne    go_to_wfe
#endif	/* SW_NOBOOTLOADER */
	
#else
    blne    go_to_sleep
#endif /* SW_MULTICORE */
	
#endif /* SW_DEDICATED_TEE */

__start_secure_core:
@ Set VBAR
    ldr     r1, =secure_exception_vectors   @ Load base address into r0
    mcr     p15, 0, r1, c12, c0, 0          @ Write Current world VBA

@ Install monitor
    LDR     r1, =monitor                    @ Get address of Monitor's vector table
    MCR     p15, 0, r1, c12, c0, 1          @ Write Monitor Vector Base Address Register

@ Initialize stacks for all modes 
@ set IRQ stack 
    msr     CPSR_c,#(Mode_IRQ | I_BIT | F_BIT)
    ldr     r1,=irq_stack
    add     r1, r1, r0, lsl #STACK_SIZE_SHIFT
    add     sp, r1, #STACK_SIZE

@ set FIQ stack 
    msr     CPSR_c,#(Mode_FIQ | I_BIT | F_BIT)
    ldr     r1,=fiq_stack
    add     r1, r1, r0, lsl #STACK_SIZE_SHIFT
    add     sp, r1, #STACK_SIZE

@ set ABT stack 
    msr     CPSR_c,#(Mode_ABT | I_BIT | F_BIT)
    ldr     r1,=abort_stack
    add     r1, r1, r0, lsl #STACK_SIZE_SHIFT
    add     sp, r1, #STACK_SIZE

@ set UND stack 
    msr     CPSR_c,#(Mode_UNDEF | I_BIT | F_BIT)
    ldr     r1,=undefined_stack
    add     r1, r1, r0, lsl #STACK_SIZE_SHIFT
    add     sp, r1, #STACK_SIZE

@ set user/system stack 
    msr     CPSR_c,#(Mode_SYS | I_BIT | F_BIT)
    ldr     r1,=user_stack
    add     r1, r1, r0, lsl #STACK_SIZE_SHIFT
    add     sp, r1, #STACK_SIZE

@ set monitor stack 
    msr     CPSR_c,#(Mode_MON | I_BIT )
    ldr     r1,=monitor_stack
    add     r1, r1, r0, lsl #STACK_SIZE_SHIFT
    add     sp, r1, #STACK_SIZE

@ set SVC stack 
    msr     CPSR_c,#(Mode_SVC | I_BIT | F_BIT)
    ldr     r1,=service_stack
    add     r1, r1, r0, lsl #STACK_SIZE_SHIFT
    add     sp, r1, #STACK_SIZE

    /* Call CPU init function */
    bl cpu_init

#ifdef CONFIG_SW_MULTICORE	
    GET_CPU_ID    r0
    cmp		r0, #0
    blne    secondary_main
#endif
	
    bl c_env_init
	
    bl secure_main
    /* We should never reach here */
    b   .

.global c_env_init
c_env_init:
    /* bss init */
    ldr    r1, =_SW_BSS_START
    ldr    r2, =_SW_CODE_END
    mov    r0, #0	
2:  cmp    r1, r2	
    bgt    1f
    str    r0,	[r1], #4
    b      2b	
1:  mov    pc, lr		

.global cpu_init
.func cpu_init
cpu_init:
    push {r1, lr}
@ Disable MMU 
    mrc p15, 0, r1, c1, c0, 0       @ Read Configuration Register
    bic r1, r1, #0x1
    mcr p15, 0, r1, c1, c0, 0       @ Write Configuration Register 

@ Disable L1 Caches 
    mrc p15, 0, r1, c1, c0, 0
    bic r1, r1, #(0x1 << 12)    @ Disable I cache 
    bic r1, r1, #(0x1 << 2)     @ Disable D cache 
    mcr p15, 0, r1, c1, c0, 0

@ Invalidate Instruction cache 
    mov R1,#0
    mcr p15, 0, r1, c7, c5, 0

@ Invalidate Data caches 
    bl clean_invalidate_dcache

@ Invalidate Branch predictor arrays 
    mov r1,#0
    mcr     p15, 0, r1, c7, c5,  6

@ Invalidate TLBs 
    mov r1, #0x0
    mcr p15, 0, r1, c8, c3, 0

    mrc     p15, 0, r1, c1, c0, 0        @ Read SCTLR
    bic     r1, r1, #0x10000000
    bic     r1, r1, #0x00002000
    mcr     p15, 0, r1, c1, c0, 0

    @  NSACR configuration
    mrc     p15, 0, r1, c1, c1, 2        @ Read Non-Secure Access Control Register
    ldr     r2, =NSACR_REG_VAL
    orr     r1, r1, r2
    mcr     p15, 0, r1, c1, c1, 2

    @ @ Set the FIQ bit so as to route FIQs to monitor mode
    mrc     p15, 0, r1, c1, c1, 0        @ Read Secure Configuration Register data
    ldr     r2, =SCR_FIQ_BIT             
    orr     r1, r1, r2	
    mcr     p15, 0, r1, c1, c1, 0        @ Write Secure Configuration Register data


#if CONFIG_SW_MULTICORE 
    mrc     p15, 0, r0, c1, c0, 1      /* Auxillary control register */
    ldr     r1, =AUXREG_SMP
    orr     r0, r0, r1
    mcr     p15, 0, r0, c1, c0, 1
#endif
	
#ifdef CONFIG_NEON_SUPPORT 
     @ @ Enabling the NEON Coprocessor registers
    mov     r1,#0x00F00000               @Give full access to cp10 and cp11
    mcr     p15, 0, r1, c1, c0, 2

    mov     r1, #0x40000000              @Enable the EN bit in FPEXC register
    msr     fpexc, r1

    mrc     p15, 0, r1, c1, c0, 2        @Read CPACR registers
    bic     r1, r1, #0x80000000          @Clear the ASEDIS bit in CPACR register
    mcr     p15, 0, r1, c1, c0, 2
#endif

    pop {r1, lr}
    bx lr
.endfunc

#ifdef CONFIG_CORTEX_A9
.globl __scu_base_reg
__scu_base_reg:
    .word SCU_BASE
#endif    

.section normal_image, "a"
.global kernel_start
.global kernel_end	
kernel_start:
.incbin "normal.bin";
kernel_end:

#ifdef CONFIG_MULTI_GUESTS_SUPPORT	
.section normal_image_2, "a"
.global kernel_2_start
.global kernel_2_end	
kernel_2_start:
.incbin "normal_2.bin";
kernel_2_end:

#ifndef CONFIG_ZYNQ7_BOARD
.section initrd_image, "a"
.global initrd_image_start
.global initrd_image_end	
initrd_image_start:
.incbin "linux2_initrd.bin.gz";
initrd_image_end:
#endif

#endif	

.section fs_image, "a"
.global fs_image_start
.global fs_image_end
fs_image_start:
#ifdef CONFIG_FILESYSTEM_SUPPORT
#ifndef CONFIG_MMC_SUPPORT
.incbin "mmc_fs";
#endif
#endif
fs_image_end:
