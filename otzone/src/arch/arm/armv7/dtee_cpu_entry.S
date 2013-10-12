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
 * dtee_cpu_entry functions implementation
 */

/* Entry point */
#include <cpu_asm.h>
#include <sw_board_asm.h>

.align 12
.global secondary_core_exception_vectors
secondary_core_exception_vectors:
/*
Vector table 
*/
    B      _sec_reset_handler
    B      _sec_undefined_handler
    B      _sec_swi_handler
    B      _sec_prefetch_handler
    B      _sec_abort_handler
    b      _sec_reserved_vector
    B      _sec_irq_handler
    B      _sec_fiq_handler

_sec_reset_handler:
    B       _sec_reset_handler

_sec_reserved_vector:
    B       _sec_reserved_vector

_sec_fiq_handler:
    B       _sec_fiq_handler

_sec_undefined_handler:
    B       _sec_undefined_handler

_sec_swi_handler:
    B       _sec_swi_handler
/*
Handlers for unused exceptions
*/

.global _sec_prefetch_handler
_sec_prefetch_handler:
    bl       prefetch_abort_handler

.global _sec_abort_handler
_sec_abort_handler:
    bl      data_abort_handler

/*
Interrupt Handler
*/
_sec_irq_handler:
    B _sec_irq_handler

.global _secondary_cpus_start
_secondary_cpus_start:
    bl      get_cpu_id              @ Returns CPU ID in r0

    mov    r1, #0
    ldr    r2, =secondary_start_config_reg
    add    r2, r2, r0, lsl #2
    str    r1, [r2]

@ Set VBAR
    ldr     r1, =secondary_core_exception_vectors   @ Load base address into r0
    mcr     p15, 0, r1, c12, c0, 0                  @ Write Current world VBA

@ Install monitor
    LDR     r1, =smp_monitor                @ Get address of Monitor's vector table
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

    bl cpu_init
    bl gic_cpu_init

switch_to_non_secure_mode:
    ldr r0,_invoke_kernel_non_secure
    smc #0  @Switch to non-secure world



.global sec_cpu_wait
sec_cpu_wait:
    bl      get_cpu_id
@1:  wfe
1:  
    ldr     r1, =secondary_start_config_reg
    add     r1, r1, r0, lsl #2
    ldr     r1, [r1]
    cmp     r1, #0
    beq     1b

    mov     lr, r1

    mov     r0,  #0
    mov     r1,  #0
    mov     r2,  #0
    mov     r3,  #0
    mov     r4,  #0
    mov     r5,  #0
    mov     r6,  #0
    mov     r7,  #0
    mov     r8,  #0
    mov     r9,  #0
    mov     r10, #0
    mov     r11, #0
    mov     r12, #0

    movs    pc, lr

.globl secondary_start_config_reg
secondary_start_config_reg:
    .word 0
    .word 0
    .word 0
    .word 0
  
_invoke_kernel_non_secure:
    .word INVOKE_NS_KER_SMP

_gic_cpu_init_addr:
    .word GIC_CPU

