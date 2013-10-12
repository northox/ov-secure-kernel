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
 * asm_macros functions implementation
 */

#ifndef __ARMV7_ASM_MACROS_H__
#define __ARMV7_ASM_MACROS_H__

@ rt is temporary register
.macro scr_nsbit_set rt
	mrc     p15, 0, \rt, c1, c1, 0  @ Read Secure Configuration Register
	orr	\rt, \rt, #SCR_NS_BIT
	mcr     p15, 0, \rt, c1, c1, 0  @ Write Secure Configuration Register
.endm

.macro scr_nsbit_clear rt
	mrc     p15, 0, \rt, c1, c1, 0  @ Read Secure Configuration Register
	bic	\rt, \rt, #SCR_NS_BIT
	mcr     p15, 0, \rt, c1, c1, 0  @ Write Secure Configuration Register
.endm	

.macro GET_CPU_ID rt
	mrc     p15, 0, \rt, c0, c0, 5   @ Read CPU ID register
	and     \rt, \rt, #0x03           @ Mask off, leaving the CPU ID field
.endm

/* 
 * Returns the core specific context
 * r0 holds return value
 */
.macro GET_CORE_CONTEXT contextp
        push    {r1}
	GET_CPU_ID    r1
	ldr     r0, =\contextp             
	ldr     r0, [r0]
	add     r0, r0, r1, lsl #SYS_CONTEXT_CORE_SHIFT
        pop     {r1}
.endm
 
#endif
