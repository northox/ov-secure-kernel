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
 * task context switch functions implementation
 */

/* Implement task context switch functions */
#include <asm-offsets.h>

.global save_task_context_regs
.func save_task_context_regs
  @ void save_task_context_regs(struct tz_task_cpu_regs *regs)
save_task_context_regs:
    push {r0-r12, lr}

    ldr lr, =temp_reg_val       @ store the regs param
    str r0, [lr]

    ldr lr, =temp_swi_regs
    ldr lr, [lr]
    add lr, lr, #TEMP_SWI_REGS_R0_OFFSET
    ldmia lr, {r0-r12}^


    ldr lr, =temp_reg_val
    ldr lr, [lr]
    stmia lr, {r0-r14}^
    add lr, lr, #(15*4)

    ldr r1, =temp_swi_regs
    ldr r1, [r1]
    add r1, r1, #TEMP_SWI_REGS_SPSR_OFFSET
    ldmia r1, {r2}
    stmia lr!, {r2}

    ldr r1, =temp_swi_regs
    ldr r1, [r1]
    add r1, r1, #TEMP_SWI_REGS_LR_OFFSET
    ldmia r1, {r2}
    stmia lr!, {r2}

    pop  {r0-r12, lr}
    bx lr
.endfunc
    
.global restore_task_context_regs
.func restore_task_context_regs
  @ void restore_task_context_regs(struct tz_task_cpu_regs *regs)
restore_task_context_regs:
    push {r0-r12, lr}
    ldr lr, =temp_reg_val       @ store the regs param
    str r0, [lr]

    mov lr, r0
    ldmia lr, {r0-r14}^

    ldr lr, =temp_swi_regs
    ldr lr, [lr]
    add lr, lr, #TEMP_SWI_REGS_R0_OFFSET
    stmia lr, {r0-r12}^

    ldr r1, =temp_reg_val
    ldr r1, [r1]
    add r3, r1, #TASK_SPSR_OFFSET
    ldmda r3, {r2}

    ldr r4, =temp_swi_regs
    ldr r4, [r4]
    add r5, r4, #TEMP_SWI_REGS_SPSR_OFFSET
    stmia r5, {r2}

    add r3, r1, #TASK_PC_OFFSET
    ldmda r3, {r2}
    add r5, r4, #TEMP_SWI_REGS_LR_OFFSET
    stmia r5, {r2}

    pop  {r0-r12, lr}
    bx lr
.endfunc
    
.bss
temp_reg_val:
  .word     0 
