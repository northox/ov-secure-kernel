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
 * sw_syscalls implementation
 */

#ifdef CONFIG_KSPACE_PROTECTION

#include <sw_syscalls_id.h>

.global __sw_open
.func __sw_open
__sw_open:
    push {lr}
    swi SW_SYSCALL_OPEN
    pop {lr}
    bx  lr
.endfunc

.global __sw_close
.func __sw_close
__sw_close:
    push {lr}
    swi SW_SYSCALL_CLOSE
    pop {lr}
    bx lr
.endfunc

.global __sw_read
.func __sw_read
__sw_read:
    push {lr}
    swi SW_SYSCALL_READ
    pop {lr}
    bx lr
.endfunc

.global __sw_write
.func __sw_write
__sw_write:
    push {lr}
    swi SW_SYSCALL_WRITE
    pop {lr}
    bx lr
.endfunc

.global __sw_ioctl
.func __sw_ioctl
__sw_ioctl:
    push {lr}
    swi SW_SYSCALL_IOCTL
    pop {lr}
    bx lr
.endfunc

.global __asm_map_to_ns
.func __asm_map_to_ns
__asm_map_to_ns:
    push {lr}
    swi SW_SYSCALL_MAP_NS
    pop {lr}
    bx lr
.endfunc

.global __asm_unmap_from_ns
.func __asm_unmap_from_ns
__asm_unmap_from_ns:
    push {lr}
    swi SW_SYSCALL_UNMAP_NS
    pop {lr}
    bx lr
.endfunc

.global __asm_execute_smc
.func __asm_execute_smc
__asm_execute_smc :
    push {lr}
    swi SW_SYSCALL_EXE_SMC
    pop {lr}
    bx lr
.endfunc

.global __sw_usleep
.func __sw_usleep
__sw_usleep :
    push {lr}
    swi SW_SYSCALL_USLEEP
    pop {lr}
    bx lr
.endfunc

#ifdef CONFIG_GUI_SUPPORT
#ifdef CONFIG_VE_BOARD
.global __init_lcd
.func __init_lcd
__init_lcd :
    push {lr}
    swi SW_SYSCALL_FRAMEBUFFER
    pop {lr}
    bx lr
.endfunc

.global __elf_load
.func __elf_load
__elf_load:
    push {lr}
    swi SW_SYSCALL_ELF_LOAD
    pop {lr}
    bx lr
.endfunc
#endif

.global __sw_fb_flush
.func __sw_fb_flush
__sw_fb_flush :
	push {lr}
    swi SW_SYSCALL_FB_FLUSH
    pop {lr}
    bx lr
.endfunc
#endif
#endif
