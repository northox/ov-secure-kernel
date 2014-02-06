/*
 * boot.S - simple register setup code for stand-alone Linux booting
 *
 * Copyright (C) 2009 ARM Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

    .text

    .globl    _start
_start:
    @
    @ Determine whether MPIDR is available
    @
    mrc    p15, 0, r0, c0, c0, 0  @ MIDR
    and    r0, r0, #0xf0000       @ Architecture
    cmp    r0, #0xf0000           @ Uses CPUID scheme? (v7, some v6s)
    bne    2f
    @ 1176 uses CPUID but has no MPIDR, so must check for it
    mrc    p15, 0, r0, c0, c0, 0  @ MIDR
    ldr    r1, =0xb760            @ Part number for 1176
    and    r0, r0, r1
    cmp    r0, r1                 @ Is an 1176?
    beq    2f

    @
    @ CPU initialisation
    @
    mrc    p15, 0, r0, c0, c0, 5  @ MPIDR (ARMv7 and 11MPCore only)
    and    r0, r0, #15            @ CPU number
    cmp    r0, #0                 @ primary CPU?
    beq    2f

    @
    @ Secondary CPUs (following the RealView SMP booting protocol)
    @
    ldr    r1, =0x70000000 + 0x800000 - 0x100
    adr    r2, 1f
    ldmia  r2, {r3 - r7}         @ move the code to a location
    stmia  r1, {r3 - r7}         @ less likely to be overridden
    ldr    r0, =0x10000030       @ RealView SYS_FLAGS register
    mov    pc, r1                @ branch to the relocated code
1:  wfe
    ldr    r1, [r0]
    cmp    r1, #0
    beq    1b
    mov    pc, r1                @ branch to the given address

    @
    @ UART initialisation (38400 8N1)
    @
2:  ldr    r0, =0x10009000       @ UART base (RealView/EB)
    mov    r1, #0x10             @ ibrd
    str    r1, [r0, #0x24]
    mov    r1, #0xc300
    orr    r1, #0x0001           @ cr
    str    r1, [r0, #0x30]

    @
    @ Kernel parameters
    @
    mov    r0, #0
    ldr    r1, =827            @ RealView/EB
    adr    r2, atags
    mov    r3, #0
    ldr    lr, =kernel
@    orr    lr, lr, #1          @ Thumb-2 kernel
    mov     pc, lr             @ jump to the kernel

    .org    0x100
atags:
    @ ATAG_CORE
    .long    2
    .long    0x54410001

    @ ATAG_CMDLINE
    .long    (1f - .) >> 2
    .long    0x54410009
    .asciz    "root=/dev/mmcblk0 rootwait ip=none mem=256M console=ttyAMA0"
    .align    2
1:

    @ ATAG_NONE
    .long    0
    .long    0x00000000
   
	
