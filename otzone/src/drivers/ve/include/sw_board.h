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
 */

/**
versatile express RS1 board configuration
 */
#ifndef _VE_BOARD_H__
#define _VE_BOARD_H__

#include <sw_types.h>
#include <sw_board_asm.h>

#define VE_FRAME_BASE   0x90100000
#define VE_FRAME_SIZE   (1048576 * 3)

#define NORMAL_WORLD_RAM_START  0x80000000
#define NSK_LOAD_ADDRESS        NORMAL_WORLD_RAM_START  
/* 1MB below SECURE_WORLD_RAM_START is reserved for "nsadmin" section*/
#define GUEST_MEM_SIZE 0x8000000
#define SECURE_WORLD_RAM_START  (VE_FRAME_BASE + VE_FRAME_SIZE)
#define SECURE_WORLD_RAM_END    (0x940FFFFF - 0x00100000)

#define ELF_LOADER_START_ADDRESS ((u32 *)(0x93ffffff + 1))

#define BASE_LOAD_ADDRESS (SECURE_WORLD_RAM_START - 0x01000000)

#ifdef CONFIG_MULTI_GUESTS_SUPPORT
#define LINUX_INITRD_ADDR 0x88d00000
#define LINUX_INITRD_SIZE (8192 * 1024)
#endif

#define VE_RS1_L2CC (0x2c100000)

#define VE_RS1_SCU  (VE_RS1_MPIC + 0x0000)
#define VE_RS1_MPCORE_TWD   (VE_RS1_MPIC + 0x0600)

#define VE_SYSTEM_REGS  0x1C010000
#define VE_CLCD_BASE    0x1C1F0000
#define VE_SYS_FLAGSSET_ADDR (VE_SYSTEM_REGS + 0x30)
#define VE_SYS_FLAGSCLR_ADDR (VE_SYSTEM_REGS + 0x34)

/*
 * Peripheral addresses
 */
#define SYSCTL_BASE         0x1c020000

#define TIMER0_BASE         0x1c110000
#define TIMER1_BASE         0x1c110020
#define TIMER2_BASE         0x1c120000
#define TIMER3_BASE         0x1c120020
#define TIMER_COUNT_MAX	    0xFFFFFFFF

#define UART0_ADDR      0x1C090000
#define UART1_ADDR      0x1C0A0000
#define UART2_ADDR      0x1C0B0000
#define UART3_ADDR      0x1C0C0000
/*
 * Irqs
 */
#define IRQ_GIC_START           32
#define GIC_NR_IRQS         (IRQ_GIC_START + 64)
#define GIC_MAX_NR          1

/* VE RS1 IRQs numbers definitions */
#define IRQ_TIMER_PAIR0          (2 + IRQ_GIC_START)
#define IRQ_TIMER_PAIR1          (3 + IRQ_GIC_START)

#define FREE_RUNNING_TIMER_IRQ      IRQ_TIMER_PAIR1
#define TICK_TIMER_IRQ              IRQ_TIMER_PAIR1

#define FREE_RUNNING_TIMER_BASE     TIMER2_BASE
#define TICK_TIMER_BASE             TIMER3_BASE

#define SECURE_UART_BASE    UART0_ADDR

#define NO_OF_INTERRUPTS_IMPLEMENTED GIC_NR_IRQS

#ifdef OTZONE_ASYNC_NOTIFY_SUPPORT
#define NS_SGI_NOTIFY_INTERRUPT 0x7
#endif

#define GIC_ITLINES  2

#ifdef SCHEDULE_HIGH_PRIORITY_GUEST
#define HIGH_PRIORITY_GUEST 0
#define LOW_PRIORITY_GUEST_UART_IRQ	37
#endif

/**
 * @brief 
 *
 * @param pgd
 */
void board_map_secure_page_table(u32* pgd);

/**
 * @brief 
 *
 * @return 
 */
pa_t get_secure_ram_start_addr(void);

/**
 * @brief 
 *
 * @return 
 */
pa_t get_secure_ram_end_addr(void);

/**
 * @brief 
 */
void board_init(void);

/**
 * @brief Board specific eMMC init routine
 */
int board_mmc_init(void);


/**
 * @brief 
 */
void unmap_init_section(void);
#endif
