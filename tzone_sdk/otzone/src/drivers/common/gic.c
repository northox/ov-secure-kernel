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
 * GIC implementation
 */

#include <sw_io.h>
#include <cpu_io.h>

#include <sw_board.h>
#include <sw_platform.h>

#include <gic.h>
#include <sw_debug.h>

#define max(a,b)    ((a) < (b) ? (b) : (a))

/**
 * @brief 
 */
struct gic_chip_data {
    u32 irq_offset;
    va_t dist_base;
    va_t cpu_base;
};

static struct gic_chip_data gic_data[GIC_MAX_NR];


#ifdef SCHEDULE_HIGH_PRIORITY_GUEST
/**
* @brief 
*
* @param guest
*
* @return 
*/
int is_guest_irq_active(u32 guest)
{
    int ret = -1, i, gic_nr = 0;
    u32 guest_irq;
    u32 secure_timer_irq_set = 0;
    u32 guest_timer_irq_set =0;
    u32 val;

    if(guest == HIGH_PRIORITY_GUEST) {
        guest_irq = LOW_PRIORITY_GUEST_UART_IRQ;

        val = gic_read(gic_data[gic_nr].dist_base +
                  GIC_DIST_PENDING_SET + (guest_irq / 32) * 4);

        if(val != 0) {
            return 0;
        }
        else {
            return 1;
        }
    }
    return 0;    
}
#endif

/**
 * @brief 
 *
 * @param gic_nr
 *
 * @return 
 */
int gic_active_irq(u32 gic_nr)
{
    int ret = -1;

    if (GIC_MAX_NR <= gic_nr) {
        return OTZ_EFAIL;
    }

    ret = gic_read(gic_data[gic_nr].cpu_base +
		   GIC_BANK_OFFSET * get_cpu_id() +
                GIC_CPU_INTACK) & 0x3FF;
    ret += gic_data[gic_nr].irq_offset;

    return ret;
}

/**
 * @brief 
 *
 * @param gic_nr
 * @param irq
 *
 * @return 
 */
int gic_ack_irq(u32 gic_nr, u32 irq)
{
    u32 gic_irq;

    if (GIC_MAX_NR <= gic_nr) {
        return OTZ_EFAIL;
    }

    if (irq < gic_data[gic_nr].irq_offset) {
        return OTZ_EFAIL;
    }

    gic_irq = irq - gic_data[gic_nr].irq_offset;

    gic_write(gic_irq, gic_data[gic_nr].cpu_base + 
	      GIC_BANK_OFFSET * get_cpu_id() +
	      GIC_CPU_EOI);
    return OTZ_OK;
}

/**
 * @brief 
 *
 * @param gic_nr
 * @param irq
 *
 * @return 
 */
int gic_mask(u32 gic_nr, u32 irq)
{
    u32 mask = 1 << (irq % 32);
    u32 gic_irq;

    if (GIC_MAX_NR <= gic_nr) {
        return OTZ_EFAIL;
    }

    if (irq < gic_data[gic_nr].irq_offset) {
        return OTZ_EFAIL;
    }

    gic_irq = irq - gic_data[gic_nr].irq_offset;

    gic_write(mask, gic_data[gic_nr].dist_base +
               GIC_DIST_ENABLE_CLEAR + (gic_irq / 32) * 4);
    return OTZ_OK;
}

/**
 * @brief 
 *
 * @param gic_nr
 * @param irq
 *
 * @return 
 */
int gic_unmask(u32 gic_nr, u32 irq)
{
    u32 mask = 1 << (irq % 32);
    u32 gic_irq;

    if (GIC_MAX_NR <= gic_nr) {
        return OTZ_EFAIL;
    }

    if (irq < gic_data[gic_nr].irq_offset) {
        return OTZ_EFAIL;
    }

    gic_irq = irq - gic_data[gic_nr].irq_offset;

    gic_write(mask, gic_data[gic_nr].dist_base +
               GIC_DIST_ENABLE_SET + (gic_irq / 32) * 4);

    return OTZ_OK;
}

/**
 * @brief 
 *
 * @return 
 */
int gic_dist_init(void)
{
    u32 gic_nr = 0;
    va_t base = GIC_DIST;
    u32 irq_start = IRQ_GIC_START;
    unsigned int max_irq, i, it_lines_number;

    /* Current processor id */
    /* It is the primary processor */
    u32 cpumask = 1 << get_cpu_id();

    if (GIC_MAX_NR <= gic_nr) {
        return OTZ_EFAIL;
    }


    cpumask |= cpumask << 8;
    cpumask |= cpumask << 16;

    gic_data[gic_nr].dist_base = base;
    gic_data[gic_nr].irq_offset = (irq_start - 1) & ~31;

    gic_write(0, base + GIC_DIST_CTRL);

    /*
     * Find out how many interrupts are supported.
     */
    max_irq = gic_read(base + GIC_DIST_CTR) & 0x1f;
    it_lines_number = max_irq;

    max_irq = (max_irq + 1) * 32;

    /*
     * The GIC only supports up to 1020 interrupt sources.
     * Limit this to either the architected maximum, or the
     * platform maximum.
     */
    if (max_irq > max(1020, GIC_NR_IRQS))
        max_irq = max(1020, GIC_NR_IRQS);

/*  Set ALL interrupts as non-secure interrupts */
#if 1


    for(i = 1; i <= it_lines_number; i++) {
        gic_write(0xffffffff,
                base + GIC_DIST_SECURITY + i * 4);  
#ifndef CONFIG_KSPACE_PROTECTION
        sw_printf("SW: gic security register 0x%x and value %x\n",
            i, gic_read(base + GIC_DIST_SECURITY + i *4));
#endif
    }
#endif
#ifndef TIMER_NOT_DEFINED
#ifndef CONFIG_EMULATE_FIQ
{
/*  Modify the Free Running Timer Interrupt used by secure kernel
 *  as Group 0 interrupt */
    u32 mask;
    u32 gic_irq;

    gic_irq = FREE_RUNNING_TIMER_IRQ - gic_data[gic_nr].irq_offset;
    
    mask = gic_read(base + GIC_DIST_SECURITY + (gic_irq / 32) * 4);

    mask &= (~(1 << (FREE_RUNNING_TIMER_IRQ % 32)) & 0xffffffff);
    gic_write(mask, base +
               GIC_DIST_SECURITY + (gic_irq / 32) * 4);

/*  Modify the Tick Timer Interrupt used by secure kernel
 *  as Group 0 interrupt */
    gic_irq = TICK_TIMER_IRQ - gic_data[gic_nr].irq_offset;
    
    mask = gic_read(base + GIC_DIST_SECURITY + (gic_irq / 32) * 4);

    mask &= (~(1 << (TICK_TIMER_IRQ % 32)) & 0xffffffff);
    gic_write(mask, base +
               GIC_DIST_SECURITY + (gic_irq / 32) * 4);
}
#endif
#endif
    /*
     * Set all global interrupts to be level triggered, active low.
     */
    for (i = 32; i < max_irq; i += 16)
        gic_write(0, base + GIC_DIST_CONFIG + i * 4 / 16);

    /*
     * Set all global interrupts to this CPU only.
     */
    for (i = 32; i < max_irq; i += 4)
        gic_write(cpumask, base + GIC_DIST_TARGET + i * 4 / 4);

    /*
     * Set priority on all interrupts.
     */
    for (i = 0; i < max_irq; i += 4)
        gic_write(0xa0a0a0a0, base + GIC_DIST_PRI + i * 4 / 4);

    /*
     * Disable all interrupts.
     */
    for (i = 32; i < max_irq; i += 32)
        gic_write(0xffffffff,
                   base + GIC_DIST_ENABLE_CLEAR + i * 4 / 32);

    /* Enable both group0 and group1 interrupts */
    gic_write(0x3, base + GIC_DIST_CTRL);

    return OTZ_OK;
}

/**
 * @brief 
 *
 * @return 
 */
int gic_cpu_init(void)
{
    u32 gic_nr = 0;
    int cpuid = get_cpu_id();
    va_t base = GIC_CPU + GIC_BANK_OFFSET * cpuid;
    va_t dist_base = GIC_DIST + GIC_BANK_OFFSET * cpuid;

    if (GIC_MAX_NR <= gic_nr) {
        return OTZ_EFAIL;
    }

    gic_data[gic_nr].cpu_base = base;

#ifndef CONFIG_KSPACE_PROTECTION
    sw_printf("SW: gic cpu base 0x%x\n", gic_data[gic_nr].cpu_base);
#endif

    /* Set the Per-CPU interrupts 15-7 as Secure and the rest
     * as Non-secure */
    gic_write(0xffff00ff,
            dist_base + GIC_DIST_SECURITY);  

    /* Enable all SGIs for the CPU */
    gic_write(0x0000ffff, dist_base + GIC_DIST_ENABLE_SET);
    /* Disable all PPIs for the CPU */
    gic_write(0xffff0000, dist_base + GIC_DIST_ENABLE_CLEAR);

/*  Set priority to higher or equal than 0x80 for non-secure access */
    gic_write(0xf0, base + GIC_CPU_PRIMASK);
/*  Select ackctl, IRQ enable and Group0 and Group1 interrupts */
#ifndef CONFIG_EMULATE_FIQ
    gic_write(0xb, base + GIC_CPU_CTRL);
#else
    gic_write(0xf, base + GIC_CPU_CTRL);
#endif

    return OTZ_OK;
}


/**
 * @brief 
 *
 * @param int_id
 */
#ifdef CONFIG_SW_DEDICATED_TEE
void generate_soft_int(u32 int_id)
{
    va_t base = GIC_DIST;

    u32 sgi_reg_val;

    u32 mask;
    u32 gic_irq;
    u32 gic_nr = 0;
    u32 is_group1 = 0;

    gic_irq = int_id - gic_data[gic_nr].irq_offset;
    
    mask = gic_read(base + GIC_DIST_SECURITY + (gic_irq / 32) * 4);

    is_group1 = mask & (1 << (int_id % 32));

    sgi_reg_val = gic_read(base + GIC_DIST_SOFTINT);
    sgi_reg_val |= GIC_DIST_SOFTINT_TAR_CORE1; /* Enable the interrupt for CPU 1 */
    sgi_reg_val |= (int_id & 0xf);
    if(is_group1)
        sgi_reg_val |= GIC_DIST_SOFTINT_NSATT_SET;
    gic_write(sgi_reg_val, base + GIC_DIST_SOFTINT);
}
#endif

/**
* @brief 
*
* @param int_id
*/
void generate_soft_int_to_core0(u32 int_id)
{
    va_t base = GIC_DIST;

    u32 sgi_reg_val;

    u32 mask;
    u32 gic_irq;
    u32 gic_nr = 0;
    u32 is_group1 = 0;

    gic_irq = int_id - gic_data[gic_nr].irq_offset;
    
    mask = gic_read(base + GIC_DIST_SECURITY + (gic_irq / 32) * 4);

    is_group1 = mask & (1 << (int_id % 32));

    sgi_reg_val = gic_read(base + GIC_DIST_SOFTINT);
    sgi_reg_val |= GIC_DIST_SOFTINT_TAR_CORE0; /* Enable the interrupt for CPU 0 */
    sgi_reg_val |= (int_id & 0xf);
    if(is_group1)
        sgi_reg_val |= GIC_DIST_SOFTINT_NSATT_SET;
    gic_write(sgi_reg_val, base + GIC_DIST_SOFTINT);
}
