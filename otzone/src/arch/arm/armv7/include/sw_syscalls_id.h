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
 * Header for Syscall Numbers
 */

#ifndef __SW_SYSCALL_H_
#define __SW_SYSCALL_H_

/* 
 * Syscall Numbers 
 */
#define SW_SYSCALL_OPEN         0xfff0
#define SW_SYSCALL_CLOSE        0xfff1
#define SW_SYSCALL_MAP_NS       0xfff2
#define SW_SYSCALL_UNMAP_NS     0xfff3
#define SW_SYSCALL_EXE_SMC      0xfff4
#define SW_SYSCALL_READ         0xfff5
#define SW_SYSCALL_WRITE        0xfff6
#define SW_SYSCALL_IOCTL        0xfff7
#define SW_SYSCALL_USLEEP       0xfff8

#ifdef CONFIG_GUI_SUPPORT
#ifdef CONFIG_VE_BOARD
#define SW_SYSCALL_FRAMEBUFFER	0xfff9
#endif
#define SW_SYSCALL_FB_FLUSH	0xfffa
#endif
#define SW_SYSCALL_ELF_LOAD   	0Xfffb

#endif
