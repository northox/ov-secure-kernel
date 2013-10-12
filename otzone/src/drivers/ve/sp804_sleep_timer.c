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
 * SP804 timer driver implementation for various sleep functions
 */


#include <sp804_sleep_timer.h>

#if 0
static u64 current_ticks = 0;
void sp804_sleep_timer_init(u32 timer_base)
{
	u32 ctrl_addr = timer_base + SLEEP_TIMER_OFFSET + TIMER_CTRL;
	/* Run mode settings. We are going to use periodic */
	sw_writel((sw_readl((volatile void*)ctrl_addr) | TIMER_CTRL_PERIODIC),
						(volatile void *)ctrl_addr);
	/* Run mode settings. We are going to use free-running */
	/*sw_writel((sw_readl((volatile void*)ctrl_addr) & (~TIMER_CTRL_PERIODIC)),
						(volatile void *)ctrl_addr);*/
	/* We are going to disable irq on this line */
	/*sw_writel((sw_readl((volatile void*)ctrl_addr) & (~TIMER_CTRL_IE)),
						(volatile void *)ctrl_addr);*/
	/* We are going to enable irq on this line */
	sw_writel((sw_readl((volatile void*)ctrl_addr) | (TIMER_CTRL_IE)),
						(volatile void *)ctrl_addr);
	/* We are going to use wrapping mode*/
	sw_writel((sw_readl((volatile void*)ctrl_addr) & (~TIMER_CTRL_ONESHOT)),
						(volatile void *)ctrl_addr);
	/* 32 bit timers */
	sw_writel((sw_readl((volatile void*)ctrl_addr) | TIMER_CTRL_32BIT),
						(volatile void *)ctrl_addr);
	/* We are going to use 1 tick every micro second, and hence, 1 irq every
	 * millisecond */
	sw_writel(SLEEP_TIMER_DURATION,
		(volatile void*)(timer_base+SLEEP_TIMER_OFFSET+TIMER_LOAD));
	/* enable the timer to be used for various sleep functions */
	gic_unmask(0, IRQ_TIMER1);
	sw_writel((sw_readl((volatile void*)ctrl_addr) | TIMER_CTRL_ENABLE),
						(volatile void *)ctrl_addr);
	return;
}

#if 0
void sp804_sleep_timer_enable(u32 timer_base)
{
	/* load the timer value */

	return;
}
#endif

u32 sp804_sleep_timer_read_value(u32 timer_base)
{
	return(sw_readl(
		(volatile void*)(timer_base+SLEEP_TIMER_OFFSET+TIMER_VALUE)));
}

void sp804_sleep_timer_irq_handler(u32 timer_base)
{
	sw_writel(1,(volatile void*)(timer_base+SLEEP_TIMER_OFFSET+TIMER_INTCLR));
	current_ticks ++;
	return;
}

u64 get_current_ticks()
{
	return(current_ticks);
}

#endif
