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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA	02111-1307, USA.
 *
 * cpu_boot_entry implementation
 */

/* Entry point */
#include <cpu_asm.h>
#include <sw_board_asm.h>	
#include <sw_platform_asm.h>

.text
.align 4
.globl _start_boot
_start_boot:
	b 	_start_boot_1


.align 12
.global secure_boot_exception_vectors
secure_boot_exception_vectors:
/*
Vector table 
*/
	b	_boot_reset_handler
	b	_boot_undefined_handler
	b	_boot_swi_handler
	b	_boot_prefetch_handler
	b	_boot_abort_handler
	b	_boot_reserved_vector
	b	_boot_irq_handler
	b 	_boot_fiq_handler

_boot_reset_handler:
	b	_boot_reset_handler

_boot_reserved_vector:
	b	_boot_reserved_vector

_boot_fiq_handler:
	b	_boot_fiq_handler

_boot_undefined_handler:
	b	_boot_undefined_handler

_boot_swi_handler:
	b	_boot_undefined_handler

_boot_prefetch_handler:
	b	_boot_prefetch_handler

_boot_abort_handler:
	b	_boot_abort_handler

_boot_irq_handler:
	b 	_boot_irq_handler

.global get_cpu_id
.func get_cpu_id
	@ uint32_t get_cpu_id(void)
	@ Returns the CPU ID (0 to 3) of the CPU executed on
get_cpu_id:
	mrc	p15, 0, r0, c0, c0, 5	@ Read CPU ID register
	and	r0, r0, #0x03	@ Mask off, leaving the CPU ID field
	bx	lr
.endfunc

.global go_to_sleep
.func go_to_sleep
	@ void go_to_sleep(void)
go_to_sleep:
	wfi					@ Go into standby
	b	go_to_sleep 	@ Catch in case of rogue events
	bx	lr
.endfunc


.globl _start_boot_1
_start_boot_1:
@	b 		_start_boot_1
	bl		get_cpu_id		@ Returns CPU ID in r0
	cmp		r0, #0			@ Is this CPU0?
	blne	go_to_sleep

__boot_secure_core:
@ Set VBAR
	ldr		r1, =secure_boot_exception_vectors	@ Load base address into r0
	mcr		p15, 0, r1, c12, c0, 0				@ Write Current world VBA

	msr		cpsr_c,#(Mode_SVC | I_BIT | F_BIT)
	ldr		sp, =_IRAM_STACK

	mov 	r3, pc
	ldr 	r4, =SECURE_BL_START_ADDR
	ldr		r5, =0xffff0000
	and		r3, r3, r5
	and		r4, r4, r5
	cmp		r3, r4
	bne		cont_init		
	ldr		r0, =SECURE_BL_START_ADDR
	ldr		r1, =SECURE_BL_RELOCATE_ADDR
	ldr		r2, =_code_size
	bl		_copy
	ldr		r1, =SECURE_BL_RELOCATE_ADDR
	mov		pc, r1

cont_init:
	/* Call CPU init function */
	bl	cpu_boot_init
	
	bl	c_boot_env_init
	
	bl	secure_boot
	/* We should never reach here */
	b	.


_copy:
	mov	r3, r2
_copy_loop:
	cmp	r3, #0
	beq	_copy_done
	cmp	r3, #16
	bge	_copy_chunk
_copy_word:
	ldmia	r0!, {r8}
	stmia	r1!, {r8}
	sub		r3, r3, #4
	b		_copy_loop
_copy_chunk:
	ldmia	r0!, {r8 - r11}
	stmia	r1!, {r8 - r11}
	sub		r3, r3, #16
	b		_copy_loop
_copy_done:
	mov	r0, r2
	bx	lr
_copy_end:



.global c_boot_env_init
c_boot_env_init:
	/* bss init */
	ldr	r1, =_SW_BOOT_BSS_START
	ldr	r2, =_SW_BOOT_CODE_END
	mov	r0, #0	
2:	cmp	r1, r2	
	bgt	1f
	str	r0,	[r1], #4
	b	2b	
1:	mov	pc, lr		

.global cpu_boot_init
.func cpu_boot_init
cpu_boot_init:
	push {r1, lr}
@ Disable MMU 
	mrc p15, 0, r1, c1, c0, 0	@ Read Configuration Register
	bic r1, r1, #0x1
	mcr p15, 0, r1, c1, c0, 0	@ Write Configuration Register 

@ Disable L1 Caches 
	mrc p15, 0, r1, c1, c0, 0
	bic r1, r1, #(0x1 << 12)	@ Disable I cache 
	bic r1, r1, #(0x1 << 2)	@ Disable D cache 
	mcr p15, 0, r1, c1, c0, 0

@ Invalidate Instruction cache 
	mov r1,#0
	mcr p15, 0, r1, c7, c5, 0

@ Invalidate Data caches 
	bl clean_invalidate_dcache

@ Invalidate Branch predictor arrays 
	mov r1,#0
	mcr	p15, 0, r1, c7, c5,	6

@ Invalidate TLBs 
	mov r1, #0x0
	mcr p15, 0, r1, c8, c3, 0

	pop {r1, lr}
	bx lr
.endfunc

.global jump_to
jump_to:
	ldr r0, =SECURE_OS_ENTRY_ADDR
	mov pc, r0
	
