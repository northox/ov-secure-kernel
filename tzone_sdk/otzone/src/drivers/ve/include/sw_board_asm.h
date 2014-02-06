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
 * Header for sw_board_asm implementation
 */

#ifndef __SW_BOARD_ASM_H__
#define __SW_BOARD_ASM_H__

#define SCU_BASE    (0x2c000000)
#define VE_RS1_MPIC (0x2c000000)

#ifdef CONFIG_SW_DEDICATED_TEE
#define KERNEL_START_ADDR 0x80000000
#endif

#ifndef CONFIG_CORTEX_A15
#define GIC_CPU     (VE_RS1_MPIC + 0x0100)
#define GIC_DIST    (VE_RS1_MPIC + 0x1000)
#else
#define GIC_CPU     (VE_RS1_MPIC + 0x2000)
#define GIC_DIST    (VE_RS1_MPIC + 0x1000)
#endif

#define GIC_BANK_OFFSET 0x0

#define NSADMIN_LOAD            0x90000000
#endif
