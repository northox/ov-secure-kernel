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
 * SP804 timer driver implementation
 */

#if  defined(CONFIG_EB_BOARD) || defined(CONFIG_VE_BOARD)
#include <sw_types.h>
#include <sw_io.h>

#include <sw_board.h>

#include <gic.h>
#include <secure_timer.h>
#include <sp804_timer.h>
#include <sw_timer.h>


/**
 * @brief 
 *
 * @param timer_base
 * @param tim_mod_index
 */
void secure_timer_enable(u32 timer_base, u32 tim_mod_index)
{
    u32 ctrl, reg_offset;

    if(timer_base == TIMER0_BASE)
        gic_unmask(0, IRQ_TIMER_PAIR0);
    else if(timer_base == TIMER1_BASE)
        gic_unmask(0, IRQ_TIMER_PAIR0);
    else if(timer_base == TIMER2_BASE)
        gic_unmask(0, IRQ_TIMER_PAIR1);
    else if(timer_base == TIMER3_BASE)
        gic_unmask(0, IRQ_TIMER_PAIR1);

    switch(tim_mod_index){
    case 0:
        reg_offset = 0x0;
        break;
    case 1:
        reg_offset = 0x20;
        break;
    default:
        reg_offset = 0;
    }

    ctrl = sw_readl((void *)(timer_base + TIMER_CTRL  + reg_offset ));
    ctrl |= TIMER_CTRL_ENABLE;
    sw_writel(ctrl, (void *)(timer_base + TIMER_CTRL + reg_offset ));
}

/**
 * @brief 
 *
 * @param timer_base
 * @param tim_mod_index
 */
void secure_timer_disable(u32 timer_base, u32 tim_mod_index)
{
    u32 ctrl, reg_offset, mask_gic = 1;

    switch(tim_mod_index){
    case 0:
        reg_offset = 0x0;
        break;
    case 1:
        reg_offset = 0x20;
        break;
    default:
        reg_offset = 0;
    }

    ctrl = sw_readl((void *)(timer_base + TIMER_CTRL + reg_offset));
    ctrl &= ~TIMER_CTRL_ENABLE;
    sw_writel(ctrl, (void *)(timer_base + TIMER_CTRL + reg_offset));

#if 1
    switch(timer_base){
        case TIMER0_BASE:{
            mask_gic = (sw_readl((void *)(TIMER1_BASE + TIMER_CTRL))
                                                           & TIMER_CTRL_ENABLE);
            if(!mask_gic)
                gic_mask(0, IRQ_TIMER_PAIR0);
        }
        case TIMER1_BASE:{
            mask_gic = (sw_readl((void *)(TIMER0_BASE + TIMER_CTRL))
                                                           & TIMER_CTRL_ENABLE);
            if(!mask_gic)
                gic_mask(0, IRQ_TIMER_PAIR0);
        }
        case TIMER2_BASE:{
            mask_gic = (sw_readl((void *)(TIMER3_BASE + TIMER_CTRL))
                                                           & TIMER_CTRL_ENABLE);
            if(!mask_gic)
                gic_mask(0, IRQ_TIMER_PAIR1);
        }
        case TIMER3_BASE:{
            mask_gic = (sw_readl((void *)(TIMER2_BASE + TIMER_CTRL))
                                                           & TIMER_CTRL_ENABLE);
            if(!mask_gic)
                gic_mask(0, IRQ_TIMER_PAIR1);
        }
    }
#endif
}

/**
 * @brief 
 *
 * @param timer_base
 * @param tim_mod_index
 */
void secure_timer_clearirq(u32 timer_base, u32 tim_mod_index)
{
    u32 reg_offset;

    switch(tim_mod_index){
    case 0:
        reg_offset = 0x0;
        break;
    case 1:
        reg_offset = 0x20;
        break;
    default:
        reg_offset = 0;
    }

    sw_writel(1, (void *)(timer_base + TIMER_INTCLR + reg_offset));
}

/**
* @brief 
*
* @param timer_base
* @param tim_mod_index
*
* @return 
*/
u32 secure_timer_read_ris(u32 timer_base, u32 tim_mod_index)
{
    u32 tmp_reg, reg_offset;

    switch(tim_mod_index){
    case 0:
        reg_offset = 0x0;
        break;
    case 1:
        reg_offset = 0x20;
        break;
    default:
        reg_offset = 0;
    }

    tmp_reg = sw_readl((void *)(timer_base + TIMER_RIS + reg_offset));
    return tmp_reg;
#ifdef SECURE_TIMER_DBG
    sw_printf("SW: ris %x - 0x%x \n",tim_mod_index, tmp_reg);
#endif
}

/**
* @brief 
*
* @param timer_base
* @param tim_mod_index
*
* @return 
*/
u32 secure_timer_read_mis(u32 timer_base, u32 tim_mod_index)
{
    u32 tmp_reg, reg_offset;

    switch(tim_mod_index){
    case 0:
        reg_offset = 0x0;
        break;
    case 1:
        reg_offset = 0x20;
        break;
    default:
        reg_offset = 0;
    }
   
    tmp_reg = sw_readl((void *)(timer_base + TIMER_MIS + reg_offset));
    return tmp_reg;
#ifdef SECURE_TIMER_DBG
    sw_printf("SW: mis %x - 0x%x \n",tim_mod_index, tmp_reg);
#endif
}

/**
 * @brief 
 *
 * @param timer_base
 */
void secure_timer_init(u32 timer_base)
{
    u32 scctrl;
    int init_done = FALSE;
    /* Select 1MHz TIMCLK as the reference clock for SP804 timers */
    scctrl = sw_readl((void *)(SYSCTL_BASE + SCCTRL));

    if(timer_base == TIMER0_BASE)
        init_done = (scctrl & SCCTRL_TIMEREN0SEL_TIMCLK) ? 1 :  0;
    else if(timer_base == TIMER1_BASE)
        init_done  = scctrl & SCCTRL_TIMEREN1SEL_TIMCLK ? 1: 0 ;
    else if(timer_base == TIMER2_BASE)
        init_done  = scctrl & SCCTRL_TIMEREN2SEL_TIMCLK ? 1: 0 ;
    else if(timer_base == TIMER3_BASE)
        init_done  = scctrl & SCCTRL_TIMEREN3SEL_TIMCLK ? 1: 0 ;

    /* If already a clock is selected for the Timer then return */
    if(init_done)
      return;

    if(timer_base == TIMER0_BASE)
        scctrl |= SCCTRL_TIMEREN0SEL_TIMCLK;
    else if(timer_base == TIMER1_BASE)
        scctrl |= SCCTRL_TIMEREN1SEL_TIMCLK;
    else if(timer_base == TIMER2_BASE)
        scctrl |= SCCTRL_TIMEREN2SEL_TIMCLK;
    else if(timer_base == TIMER3_BASE)
        scctrl |= SCCTRL_TIMEREN3SEL_TIMCLK;

    sw_writel(scctrl, (void*)(SYSCTL_BASE + SCCTRL));
}

/**
* @brief 
*
* @param timer_base
* @param tim_mod_index
*
* @return 
*/
u32 secure_read_timer(u32 timer_base, u32 tim_mod_index)
{
    u32 reg_offset;

    switch(tim_mod_index){
    case 0:
        reg_offset = 0x0;
        break;
    case 1:
        reg_offset = 0x20;
        break;
    default:
        reg_offset = 0;
    }
    
    return sw_readl((void*)(timer_base + TIMER_VALUE + reg_offset));
}

/**
* @brief 
*
* @param timer_base
* @param tim_mod_index
* @param mode
* @param usecs
*/
void secure_set_timer(u32 timer_base, u32 tim_mod_index, u32 mode, u32 usecs)
{
    u32 scctrl, val, reg_offset;
    int sel_clk = FALSE;

    scctrl = sw_readl((void *)(SYSCTL_BASE + SCCTRL));

    if(timer_base == TIMER0_BASE)
        sel_clk = (scctrl & SCCTRL_TIMEREN0SEL_TIMCLK) ? 1 :  0;
    else if(timer_base == TIMER1_BASE)
        sel_clk  = scctrl & SCCTRL_TIMEREN1SEL_TIMCLK ? 1: 0 ;
    else if(timer_base == TIMER2_BASE)
        sel_clk  = scctrl & SCCTRL_TIMEREN2SEL_TIMCLK ? 1: 0 ;
    else if(timer_base == TIMER3_BASE)
        sel_clk  = scctrl & SCCTRL_TIMEREN3SEL_TIMCLK ? 1: 0 ;

  if(!sel_clk)
      return;

    switch(tim_mod_index){
    case 0:
        reg_offset = 0x0;
        break;
    case 1:
        reg_offset = 0x20;
        break;
    default:
        reg_offset = 0;
    }
    
    /* Setup Timer for generating irq */
    val = sw_readl((void *)(timer_base + TIMER_CTRL + reg_offset));
    val &= ~TIMER_CTRL_ENABLE;
    val |= (TIMER_CTRL_32BIT | TIMER_CTRL_IE);

    if(mode == MODE_FREE_RUNNING) {
        val &= ~TIMER_CTRL_ONESHOT;
        val &= ~TIMER_CTRL_PERIODIC;
    }
    else if(mode == MODE_ONESHOT) {
        val |= TIMER_CTRL_ONESHOT;
    }
    else{       /* Mode is Periodic */
        val &= ~TIMER_CTRL_ONESHOT;
        val |= TIMER_CTRL_PERIODIC;
    }

    sw_writel(val, (void *)(timer_base + TIMER_CTRL + reg_offset ));
    sw_writel(usecs, (void *)(timer_base + TIMER_LOAD + reg_offset));
}

/**
 * @brief 
 *
 * @return 
 */
u32 get_clock_period_us(void)
{
	return TIMER_PERIOD_US; 
}

/**
* @brief 
*
* @param timer_base
*
* @return 
*/
u32 read_timer_value(u32 timer_base)
{
    return secure_read_timer(timer_base, 0);
}

/**
 * @brief 
 *
 * @return 
 */
u32 read_freerunning_cntr(void)
{
	/* decrement type */
    return ~read_timer_value(FREE_RUNNING_TIMER_BASE);
}

/**
 * @brief 
 *
 * @return 
 */
u64 get_timer_period(void)
{
    timeval_t timer_period;
    timer_period.tval.sec = 4294;
    timer_period.tval.nsec = 967296000;
    return timer_period.tval64;
}

/**
* @brief 
*
* @return 
*/
u64 get_clock_period(void)
{
    timeval_t clock_period;
    clock_period.tval.sec = 0;
    clock_period.tval.nsec = 1000;
    return clock_period.tval64;
}

void secure_timer_irq_handler(int irq)
{

        if(secure_timer_read_mis(TICK_TIMER_BASE, 0)) {
		secure_timer_clearirq(TICK_TIMER_BASE,0);
		timer_interrupt(); 

        } else if(secure_timer_read_mis(FREE_RUNNING_TIMER_BASE, 0)) {
		secure_timer_clearirq(FREE_RUNNING_TIMER_BASE, 0);
		sw_printf("SW: Update jiffies \n");
		__free_running_cntr_intr();
        }
}

#endif
