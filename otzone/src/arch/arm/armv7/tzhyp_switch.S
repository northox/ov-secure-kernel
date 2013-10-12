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
 * Trustzone based hypervisor monitor mode functions implementation
 */

/*
 * Support functions for implementing (Trustzone based)hypervisor
 * running in monitor mode.
 */

#include <cpu_asm.h>
#include <asm_macros.h>

@ r0 has to be initialized with the context pointer
@ r2 used as temp register	
.macro save_cp15_context	
	mrc	p15, 2, r2, c0, c0, 0 /* CSSELR */
	str	r2, [r0], #4
	mrc	p15, 0, r2, c1, c0, 0 /* SCTLR */
	str	r2, [r0], #4
	mrc	p15, 0, r2, c1, c0, 1 /* ACTLR */
	str	r2, [r0], #4
	mrc	p15, 0, r2, c2, c0, 0 /* TTBR0 */
	str	r2, [r0], #4
	mrc	p15, 0, r2, c2, c0, 1 /* TTBR1 */
	str	r2, [r0], #4
	mrc	p15, 0, r2, c2, c0, 2 /* TTBCR */
	str	r2, [r0], #4
	mrc	p15, 0, r2, c3, c0, 0 /* DACR */
	str	r2, [r0], #4
	mrc	p15, 0, r2, c5, c0, 0 /* DFSR */
	str	r2, [r0], #4
	mrc	p15, 0, r2, c5, c0, 1 /* IFSR */
	str	r2, [r0], #4
	mrc	p15, 0, r2, c6, c0, 0 /* DFAR */
	str	r2, [r0], #4
	mrc	p15, 0, r2, c6, c0, 2 /* IFAR */
	str	r2, [r0], #4
	mrc	p15, 0, r2, c7, c4, 0 /* PAR */
	str	r2, [r0], #4
	mrc	p15, 0, r2, c10, c2, 0 /* PRRR */
	str	r2, [r0], #4
	mrc	p15, 0, r2, c10, c2, 1 /* NMRR */
	str	r2, [r0], #4
	mrc	p15, 0, r2, c12, c0, 0 /* VBAR */
	str	r2, [r0], #4
	mrc	p15, 0, r2, c13, c0, 0 /* FCSEIDR */
	str	r2, [r0], #4
	mrc	p15, 0, r2, c13, c0, 1 /* CONTEXTIDR */
	str	r2, [r0], #4
	mrc	p15, 0, r2, c13, c0, 2 /* TPIDRURW */
	str	r2, [r0], #4
	mrc	p15, 0, r2, c13, c0, 3 /* TPIDRURO */
	str	r2, [r0], #4
	mrc	p15, 0, r2, c13, c0, 4 /* TPIDRPRW */
	str	r2, [r0], #4
.endm

@ r0 has to be initialized with the context pointer
@ r2 used as temp register		
.macro restore_cp15_context	
	ldr	r2, [r0], #4
	mcr	p15, 2, r2, c0, c0, 0 /* CSSELR */
	ldr	r2, [r0], #4
	mcr	p15, 0, r2, c1, c0, 0 /* SCTLR */
	ldr	r2, [r0], #4
	mcr	p15, 0, r2, c1, c0, 1 /* ACTLR */
	ldr	r2, [r0], #4
	mcr	p15, 0, r2, c2, c0, 0 /* TTBR0 */
	ldr	r2, [r0], #4
	mcr	p15, 0, r2, c2, c0, 1 /* TTBR1 */
	ldr	r2, [r0], #4
	mcr	p15, 0, r2, c2, c0, 2 /* TTBCR */
	ldr	r2, [r0], #4
	mcr	p15, 0, r2, c3, c0, 0 /* DACR */
	ldr	r2, [r0], #4
	mcr	p15, 0, r2, c5, c0, 0 /* DFSR */
	ldr	r2, [r0], #4
	mcr	p15, 0, r2, c5, c0, 1 /* IFSR */
	ldr	r2, [r0], #4
	mcr	p15, 0, r2, c6, c0, 0 /* DFAR */
	ldr	r2, [r0], #4
	mcr	p15, 0, r2, c6, c0, 2 /* IFAR */
	ldr	r2, [r0], #4
	mcr	p15, 0, r2, c7, c4, 0 /* PAR */
	ldr	r2, [r0], #4
	mcr	p15, 0, r2, c10, c2, 0 /* PRRR */
	ldr	r2, [r0], #4
	mcr	p15, 0, r2, c10, c2, 1 /* NMRR */
	ldr	r2, [r0], #4
	mcr	p15, 0, r2, c12, c0, 0 /* VBAR */
	ldr	r2, [r0], #4
	mcr	p15, 0, r2, c13, c0, 0 /* FCSEIDR */
	ldr	r2, [r0], #4
	mcr	p15, 0, r2, c13, c0, 1 /* CONTEXTIDR */
	ldr	r2, [r0], #4
	mcr	p15, 0, r2, c13, c0, 2 /* TPIDRURW */
	ldr	r2, [r0], #4
	mcr	p15, 0, r2, c13, c0, 3 /* TPIDRURO */
	ldr	r2, [r0], #4
	mcr	p15, 0, r2, c13, c0, 4 /* TPIDRPRW */
.endm
	
	
@ r0 points to current cp15 context
@ r1 points to next cp15 context
	
@ Note: 
@ TLBs of NS world has to be flushed when we do the cp15 switch.
@ This cannot be handled from the secure world(not even by setting the NS bit)
@ and is possible only after switching to NS world. So, we just set the
@ NS_SWITCH_ACTIVE flag and return.
@ The flag is then processed during the fiq handler exit path.
@	
@ It is assumed that tzhyp_sysregs_switch is always invoked from the fiq handler
@ context
	
	
.global tzhyp_sysregs_switch
.func tzhyp_sysregs_switch
tzhyp_sysregs_switch:

	@ Move to NS
	scr_nsbit_set r2
	
	@ save c15 context
	save_cp15_context

	@restore cp15 context
	mov	r0, r1
	restore_cp15_context

	@ Indicate that we need to do a tlb flush as part of CP15 switch
	ldr	r2, =ns_switch_flag
	ldr     r3, =NS_SWITCH_ACTIVE
	str	r3, [r2]
	dsb

	@ Move back to secure
	scr_nsbit_clear r2
	
	bx      lr
.endfunc

@ r0 points to current cp15 context
.global tzhyp_sysregs_save
.func tzhyp_sysregs_save
tzhyp_sysregs_save:

	@ Move to NS
	scr_nsbit_set r2
	
	@ save c15 context
	save_cp15_context
	
	@ Move back to secure
	scr_nsbit_clear r2
	
	bx      lr
.endfunc
	
