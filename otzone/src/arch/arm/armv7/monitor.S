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
 * Monitor functions implementation
 */

/* Monitor functions */
#include <cpu_asm.h>
#include <asm_macros.h>	
#include <sw_platform_asm.h>
#include <sw_board_asm.h>
#include <asm-offsets.h>

.macro exception_return 	
    ldmfd   sp!, {r0}
    msr     spsr, r0
    push    {r0}
    mov     r0, #0
    dsb
    pop     {r0}
    ldmfd   sp!, {r0-r12, pc}^
.endm	

.macro exception_return_arg	
    ldmfd   sp!, {r1}
    msr     spsr, r1
    push    {r1}
    mov     r1, #0
    dsb
    pop     {r1}
    ldmfd   sp!, {r1-r12, pc}^
.endm	

.macro call_c_function func_name
    stmfd   sp!, {r0-r12, lr}
    mrs     r4, spsr
    stmfd   sp!, {r4}
    bl      \func_name
.endm	

.macro call_c_function_arg func_name
    stmfd   sp!, {r1-r12, lr}
    mrs     r4, spsr
    stmfd   sp!, {r4}
    bl      \func_name
.endm	

.align 12
.global monitor
monitor: 
    @ Monitor's
monitor_reset:
    b   monitor_reset       @ Reset      - not used by Monitor
monitor_undef:
    b   monitor_undef       @ Undef      - not used by Monitor
    b   smc_handler         @ SMC_Handler
monitor_pref:
    b   monitor_pref        @ Prefetch   - can by used by Monitor
monitor_abort:
    b   monitor_abort       @ Data abort - can by used by Monitor
monitor_reserv:
    b   monitor_reserv      @ RESERVED
monitor_irq:
    b   monitor_irq         @ IRQ        - can by used by Monitor
    b  monitor_fiq_handler        @ Monitor FIQ Handler

@ FIQ exception handler enters and runs in "monitor" mode
@ (not "FIQ" mode)
@ With SCR.FIQ set, FIQs from both secure and non secure world
@ are routed to monitor mode fiq handler.
monitor_fiq_handler: 
    sub     lr, lr, #4
    push    {r0}
    mrc     p15, 0, r0, c1, c1, 0
    tst     r0, #SCR_NS_BIT
    pop     {r0}
    beq called_from_secure
    bne called_from_ns
called_from_ns:
     push    {r4, lr}  /* the corresponding pops happens from
                                         save_context */
     push    {r0 - r3}
     b      ns_fiq_handler
called_from_secure:
     call_c_function fiq_c_handler
     exception_return
	

@ r0 points to the base address where the register context get saved
@ r0 - r4, lr are available on the stack and the rest are expected to be intact
	
@ Note: {r0 - r3} has to be at the top of the stack followed by {r4, lr}
	
.func save_context
.global save_context
save_context:	
    @ Save general purpose registers, SPSR and LR
    @ --------------------------------------------
	
    mov     r4, r0                      
    pop     {r0 - r3}                   @ Retrieve from stack
    stmia   r4!, {r0 - r3}              @ r0 - r3
    mov     r0, r4

    mov     r2, lr                      @ Save return path
	
    pop     {r4, lr}                    @ Retrieve from stack
	
    stmia   r0!, {r4-r12}               @ r4 to r12
	
    mrs     r1, spsr                    @ SPSR, LR
    stmia   r0!, {r1, lr}

    mov     lr, r2                      @ restore return path 
	
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
    mov    pc, lr

.endfunc

@ r0 points to the base address from which the register context get restored
.func restore_context
.global restore_context
restore_context:	
    @ Restore other world's registers, SPSR and LR
    @ ---------------------------------------------
    push    {lr}

    mov	r4, r0                         @r0 - r3
    ldmia r4!, {r0 - r3}
	
    push {r0, r1}
    mov r0, r4

    ldmia r0!, {r4 - r12}              @r4 - r12
	
    ldmia r0!, {r1, lr}                @spsr, lr
    msr	spsr_cxsf, r1
	
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
    ldr    pc, [r13], #4
.endfunc

#ifdef CONFIG_NEON_SUPPORT
.func save_vfp_context
.global save_vfp_context
save_vfp_context:    
    push    {r1,r2,r3,r4}
    add     r0,r0, #NEON_OFFSET
    fmrx    r1, FPEXC
    tst     r1, #FPEXC_EN
    beq     1f
    fmrx    r2, FPSCR
    fmrx    r3, FPSID
    fmrx    r4, MVFR0
    stmia   r0!, {r1,r2,r3}
    vstm    r0!, {d0 - d15}
    and     r4,r4, #MVFR0_A_SIMD_MASK
    cmp     r4,#2
    vstmeq  r0!, {d16 - d31}
1:  pop     {r1,r2,r3,r4}
    mov     pc,lr
.endfunc

.func restore_vfp_context
.global restore_vfp_context
restore_vfp_context:

    push    {r1,r2,r3,r4}
    add     r0,r0, #NEON_OFFSET
    ldmia   r0!, {r1,r2,r3}
    fmxr    FPEXC,r1
    tst     r1, #FPEXC_EN
    beq     1f
    
    vldmia  r0!, {d0 - d15}
    fmrx    r4, MVFR0 
    and     r4,r4, #MVFR0_A_SIMD_MASK
    cmp     r4,#2
    vldmeq  r0!, {d16 - d31}
    fmxr    FPSCR,r2
    fmxr    FPSID,r3

1:  pop     {r1,r2,r3,r4}
    mov     pc,lr
.endfunc
#endif

smc_handler:
    cmn     r0, #0
    blt     board_smc
    CMP r0, # CALL_TRUSTZONE_API
    beq     tz_api
    CMP r0, #RET_FROM_SECURE_API
    beq     ret_from_secure_api
    CMP r0, #TZHYP_NSADMIN_RETURN
    beq     tzhyp_nsadmin_restore
    CMP r0, #INVOKE_NON_SECURE_KERNEL
    beq     call_non_secure_kernel  
    CMP r0, #TZHYP_NSCPU_CTXT_INIT
    beq     tzhyp_cpuctxt_init

    bx  lr

board_smc:
    call_c_function_arg board_smc_handler
    exception_return_arg

smc_error:
    call_c_function smc_error_handler
    exception_return
	
tz_api:
#ifdef CONFIG_SW_MULTICORE
    push     {r0}
    GET_CPU_ID  r0
    cmp      r0, #0
    pop      {r0}
    beq      1f
    b        smc_error
#endif
	
#ifndef CONFIG_SW_DEDICATED_TEE
1:  push    {r4, lr}  /* the corresponding pops happens from
	                                 save_context */
    push    {r0 - r3}

    /* Copy args to params stack */
    ldr     r4, =params_stack
    stmia   r4, {r0-r3}

    ldr r1, =valid_params_flag
    mov r2, #0x1
    str r2, [r1]
	
    b mon_switchto_sworld
#else
   /* Invalid case */   
#endif	
	
call_non_secure_kernel:
#ifndef CONFIG_SW_DEDICATED_TEE
    push    {r4, lr}  /* the corresponding pops happens from
	                                 save_context */
    push    {r0 - r3}

    b mon_switchto_nsworld
#else
    push    {lr}
    mov     r0, #0
    bl      start_secondary_linux
    pop    {lr}
    movs pc, lr
#endif

ret_from_secure_api:
#ifndef CONFIG_SW_DEDICATED_TEE	
    push    {r4, lr}  /* the corresponding pops happens from
	                                 save_context */
    push    {r0 - r3}

    ldr r0, =params_out_stack /* Update return value */
    ldr r1, [r0, #0]
    ldr r0, =ns_sys_current
    ldr r0, [r0]
    str r1, [r0, #0]           @ r0 at offset 0

    b mon_switchto_nsworld
#else
    push    {lr}
    bl  notify_smp_core
    pop     {lr}
    movs pc,lr
#endif


tzhyp_cpuctxt_init:
    call_c_function      mon_nscpu_context_init
    exception_return

tzhyp_nsadmin_restore:
    @ NS bit is already set
    @ restore the SCTRL
    GET_CORE_CONTEXT ns_sys_current
    mov     r1, r0	
    ldr	    r0, [r1, #SCTLR_OFFSET]		
    mcr	    p15, 0, r0, c1, c0, 0 /* SCTLR */
	
    @ restore SPSR
    ldr	    r0, [r1, #SPSR_MON_OFFSET]
    msr     spsr, r0
	
    @ restore lr
    ldr     r0, [r1, #LR_MON_OFFSET]
    mov     lr, r0	

    b       tzhyp_nsadmin_return @ continue to fiq handler return path

	
.func  mon_switchto_nsworld
mon_switchto_nsworld:
    GET_CORE_CONTEXT s_sys_current
    bl    save_context
    #ifdef CONFIG_NEON_SUPPORT
    GET_CORE_CONTEXT s_sys_current
    bl    save_vfp_context
    GET_CORE_CONTEXT ns_sys_current
    bl    restore_vfp_context
    #endif
    GET_CORE_CONTEXT ns_sys_current
    bl    restore_context
    @ Clear local monitor
    @ --------------------
    clrex                         

    push {r0}
    @Move to Non Secure
    scr_nsbit_set r0	
    pop  {r0}

	dsb
	isb

    movs    pc, lr
.endfunc

.func  mon_switchto_sworld
mon_switchto_sworld:

    @Move to Secure
    scr_nsbit_clear r0
	
    GET_CORE_CONTEXT ns_sys_current
    bl    save_context
    #ifdef CONFIG_NEON_SUPPORT
    GET_CORE_CONTEXT ns_sys_current
    bl 	  save_vfp_context
    GET_CORE_CONTEXT s_sys_current
    bl    restore_vfp_context
    #endif
    GET_CORE_CONTEXT s_sys_current
    bl    restore_context
    @ Clear local monitor
    @ --------------------
    clrex                         

    movs    pc, lr
.endfunc	

.func   ns_fiq_handler
ns_fiq_handler:
    @ Move to secure world	
    scr_nsbit_clear r0

    ldr	    r0, =ns_preempt_flag
    ldr     r1, =NS_PREEMPT_ENABLE	
    str     r1, [r0]
    dsb	

    GET_CORE_CONTEXT ns_sys_current
    bl    save_context
    #ifdef CONFIG_NEON_SUPPORT
    GET_CORE_CONTEXT ns_sys_current
    bl    save_vfp_context
    GET_CORE_CONTEXT s_sys_current
    bl    restore_vfp_context
    #endif
    GET_CORE_CONTEXT s_sys_current
    bl    restore_context

    bl fiq_c_handler
    #ifdef CONFIG_NEON_SUPPORT
    GET_CORE_CONTEXT ns_sys_current
    bl    restore_vfp_context
    #endif
    GET_CORE_CONTEXT ns_sys_current
    bl    restore_context

    push    {r0, r1}

    ldr	    r0, =ns_preempt_flag
    ldr     r1, =NS_PREEMPT_DISABLE	
    str     r1, [r0]

    @ Move to Non Secure
    scr_nsbit_set r0

    @ Left over of tzhyp context switching which cannot be done from
    @ secure world, accomplished by switching to "nsadmin" 
    ldr	    r0, =ns_switch_flag
    ldr     r0, [r0]
    tst     r0, #NS_SWITCH_ACTIVE
    beq     skip_nsadmin

    b       tzhyp_nsadmin	

tzhyp_nsadmin_return:
    @Clear switch flag
    ldr	    r0, =ns_switch_flag
    ldr     r1, =NS_SWITCH_CLEAR
    str	    r1, [r0]
	
skip_nsadmin:	
    pop     {r0, r1}	

	dsb
	isb

    movs     pc, lr
.endfunc

.func tzhyp_nsadmin
@ Switch to "nsadmin"(Non secure admin) for TLB maintainence
tzhyp_nsadmin:
	
    @ NS bit is already set

    ldr     r0, =SCTLR_RESET_VAL
    mcr	    p15, 0, r0, c1, c0, 0 /* SCTLR */
	
    ldr     r0, =CPSR_RESET_VAL
    msr	    spsr, r0              /* SPSR */
	
    ldr     lr, =NSADMIN_LOAD     /* lr   */

	dsb
	isb

    movs    pc, lr
.endfunc	


@ This section need to be placed in the Non secure area
@ This can be acheived by just copying code from secure area to non secure area	
@ We enter with MMU turned off

/* Fix me: Enabling a new section breaks the startaddr 
 * .section .nsadmin, "a"
 */
.global tzhyp_nsadmin_start
.global tzhyp_nsadmin_end
tzhyp_nsadmin_start:	
	mcr	p15, 0, r0, c8, c7, 0  /* Invalidate entire TLB */
	ldr	r0, nsadmin_retval
	dsb
	isb
	smc     #0
nsadmin_retval:	
	.word   TZHYP_NSADMIN_RETURN
tzhyp_nsadmin_end:	
@section end
	
	
