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
 * dtee_monitor functions implementation
 */

/* Monitor functions */
#include <cpu_asm.h>
#include <sw_platform_asm.h>
#include <sw_board_asm.h>
#include <asm_macros.h>

.align 12

.global smp_monitor
smp_monitor:
    @ Monitor's
smp_monitor_reset:
    b   smp_monitor_reset       @ Reset      - not used by Monitor
smp_monitor_undef:
    b   smp_monitor_undef       @ Undef      - not used by Monitor
    b   smp_smc_handler         @ SMC_Handler
smp_monitor_pref:
    b   smp_monitor_pref        @ Prefetch   - not used by Monitor
smp_monitor_abort:
    b   smp_monitor_abort       @ Data abort - not used by Monitor
smp_monitor_reserv:
    b   smp_monitor_reserv      @ RESERVED
smp_monitor_irq:
    b   smp_monitor_irq         @ IRQ        - not used by Monitor
smp_monitor_fiq:
    b   smp_monitor_fiq_handler         @ FIQ        - not used by Monitor


@ r0 points to the base address where the register context get saved
@ Its assumed r4 - r12, lr are intact on the way to save_context
.macro smp_save_context temp_params
    @ Save general purpose registers, SPSR and LR
    @ --------------------------------------------
    push    {r4, r5}                     @ r0 to r3

    mov     r4, r0
    ldr     r5, =\temp_params

    push    {r0, lr}    
    bl get_cpu_id
    add r5, r5, r0, lsl #PARAM_STACK_SIZE_SHIFT    
    pop     {r0, lr}

    ldmia   r5, {r0-r3}
    stmia   r4!, {r0-r3}
    mov     r0, r4
    pop     {r4, r5}

    stmia   r0!, {r4-r12}               @ r4 to r12

    mrs     r1, spsr                    @ SPSR, LR
    stmia   r0!, {r1, lr}               

    @ Save banked registers
    cps #Mode_SVC   
    mrs     r1, spsr
    stmia   r0!, {r1, r13, lr}

    cps #Mode_SYS   
    stmia   r0!, {r13, lr}

    cps #Mode_ABT   
    mrs     r1, spsr
    stmia   r0!, {r1, r13, lr}

    cps #Mode_UNDEF 
    mrs     r1, spsr
    stmia   r0!, {r1, r13, lr}

    cps #Mode_IRQ   
    mrs     r1, spsr
    stmia   r0!, {r1, r13, lr}

    cps #Mode_MON

.endm

@ r0 points to the base address from which the register context get restored
.macro smp_restore_context
    @ Restore other world's registers, SPSR and LR
    @ ---------------------------------------------

    mov r4, r0                         @r0 - r3
    ldmia r4!, {r0 - r3}

    push {r0, r1}
    mov r0, r4

    ldmia r0!, {r4 - r12}              @r4 - r12

    ldmia r0!, {r1, lr}                @spsr, lr
    msr spsr_cxsf, r1

    @Restore banked registers
    cps #Mode_SVC   
    ldmia   r0!, {r1, r13, lr} 
    msr spsr_cxsf, r1

    cps #Mode_SYS   
    ldmia   r0!, {r13, lr} 

    cps #Mode_ABT   
    ldmia   r0!, {r1, r13, lr} 
    msr spsr_cxsf, r1

    cps #Mode_UNDEF 
    ldmia   r0!, {r1, r13, lr} 
    msr spsr_cxsf, r1

    cps #Mode_IRQ   
    ldmia   r0!, {r1, r13, lr} 
    msr spsr_cxsf, r1

    cps #Mode_MON   

    pop     {r0, r1}
.endm


.global smp_monitor_fiq_handler
smp_monitor_fiq_handler:
    sub lr, lr, #0x4

    push {lr}
    bl smp_monitor_fiq_c_handler

    ldmfd sp!, {pc}^

.global smp_smc_handler
.func   smp_smc_handler
smp_smc_handler:

    CMP r0, #INVOKE_NS_KER_SMP
    beq call_non_secure_kernel_smp
    CMP r0, #CALL_TRUSTZONE_API
    beq     call_secure_tz_handler
    bx lr

call_non_secure_kernel_smp:
    mrc     p15, 0, r1, c1, c1, 2        @ Read Non-Secure Access Control Register
    ldr r2, =NSACR_REG_VAL
    orr     r1, r1, r2
    mcr     p15, 0, r1, c1, c1, 2

    @ change it to non-secure
    scr_nsbit_set r3

    @ Set the SPSR to be in SVC mode
    msr     spsr_cxsf, #Mode_SVC         @ Set SPSR to be SVC m

    b sec_cpu_wait

call_secure_tz_handler:

    push    {r7,lr}

    push    {r0}
    bl      get_cpu_id 
    ldr r7, =params_smp_stack
    add r7, r7, r0, lsl #PARAM_STACK_SIZE_SHIFT
    pop     {r0}

    stmia   r7, {r0-r3}      

    bl      get_cpu_id 
    ldr r7,=valid_return_params_flag
    add r7, r7, r0, lsl #2
    mov r0, #0
    str r0, [r7]

    pop     {r7,lr}

    /* Change to Secure World */
    mrc     p15, 0, r0, c1, c1, 0
    bic     r0, r0, #SCR_NS_BIT 
    mcr     p15, 0, r0, c1, c1, 0

    ldr r0,=smp_ns_context
    smp_save_context params_smp_stack

    cps  #Mode_SVC 
    cpsie f

    mov r0, #SEC_SGI_TO_PRIMARY_CORE
    push {lr}
    bl generate_soft_int
    pop  {lr}


    /* Wait here till it gets the completion interrupt from secure kernel */
wfi_sec_ser_complete:
    wfi

    bl      get_cpu_id 
    ldr r7,=valid_return_params_flag
    add r7, r7, r0, lsl #2
    ldr r0, [r7,#0]
    cmp r0, #1
    bne wfi_sec_ser_complete

    cps #Mode_MON

    ldr r0,=smp_ns_context
    smp_restore_context

    push    {r1, lr}
    bl      get_cpu_id
    ldr r1,=params_out_smp_stack
    add r1, r1, r0, lsl #PARAM_OUT_STACK_SIZE_SHIFT
    ldr r0, [r1, #0]

    /* Change to Non-Secure World */
    mrc     p15, 0, r1, c1, c1, 0
    orr     r1, r1, #SCR_NS_BIT 
    mcr     p15, 0, r1, c1, c1, 0

    pop     {r1, lr}
    movs pc, lr

.endfunc

_call_trustzone_api:
    .word  CALL_TRUSTZONE_API

.bss
smp_ns_context:
  .space  256

