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
 * SCU functions implementation
 */

/**
SCU functions
 */
#include <sw_io.h>
#include <cpu_io.h>
#include <sw_board.h>
#include <gic.h>

/**
 * @brief 
 */
void scu_init(void)
{
    /* Set SCU non-secure control register for global timer, private timer 
     * and SCU component registers */
    u32 scu_ns_ctrl;
    int val;

    scu_ns_ctrl = 0xfff;

    sw_writel(0x1, (void *)(VE_RS1_SCU + 0x50));
    sw_writel(scu_ns_ctrl, (void *)(VE_RS1_SCU + 0x54));

#ifdef CONFIG_SW_MULTICORE
    /* Enable SCU */
    sw_printk("SW: Enabling SCU...\n");
    val = sw_readl((void *)(SCU_BASE + 0x0)); /* SCU control register */
    val |= SCU_EN;
    sw_writel(val, (void *)(SCU_BASE + 0x0));
#endif

}
