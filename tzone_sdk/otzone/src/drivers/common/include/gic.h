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
 * Header file GIC functions.
 */

#ifndef __COMMON_GIC_H__
#define __COMMON_GIC_H__

#include <sw_types.h>
#include <system_context.h> /* tzhyp switching support */
#include <sw_board.h>
#include <sw_io.h>
#include <sw_debug.h>

#define GIC_CPU_CTRL            0x00
#define GIC_CPU_PRIMASK         0x04
#define GIC_CPU_BINPOINT        0x08
#define GIC_CPU_INTACK          0x0c
#define GIC_CPU_EOI         0x10
#define GIC_CPU_RUNNINGPRI      0x14
#define GIC_CPU_HIGHPRI         0x18

#define GIC_DIST_CTRL           0x000
#define GIC_DIST_CTR            0x004
#define GIC_DIST_SECURITY       0x80
#define GIC_DIST_ENABLE_SET     0x100
#define GIC_DIST_ENABLE_CLEAR       0x180
#define GIC_DIST_PENDING_SET        0x200
#define GIC_DIST_PENDING_CLEAR      0x280
#define GIC_DIST_ACTIVE_BIT     0x300
#define GIC_DIST_CLEAR_ACTIVE_BIT   0x380
#define GIC_DIST_PRI            0x400
#define GIC_DIST_TARGET         0x800
#define GIC_DIST_CONFIG         0xc00
#define GIC_DIST_SOFTINT        0xf00

#define GIC_DIST_SOFTINT_NSATT_SET     (1 << 15)

#define GIC_DIST_SOFTINT_TAR_CORE0     (1 << 16)
#define GIC_DIST_SOFTINT_TAR_CORE1     (1 << 17)

#ifdef SCHEDULE_HIGH_PRIORITY_GUEST
/**
* @brief 
*
* @param guest
*
* @return 
*/
int is_guest_irq_active(u32 guest);
#endif

/**
 * @brief 
 *
 * @param gic_nr
 *
 * @return 
 */
int gic_active_irq(u32 gic_nr);

/**
 * @brief 
 *
 * @param gic_nr
 * @param irq
 *
 * @return 
 */
int gic_ack_irq(u32 gic_nr, u32 irq);

/**
 * @brief 
 *
 * @param gic_nr
 * @param irq
 *
 * @return 
 */
int gic_mask(u32 gic_nr, u32 irq);

/**
 * @brief 
 *
 * @param gic_nr
 * @param irq
 *
 * @return 
 */
int gic_unmask(u32 gic_nr, u32 irq);

/**
 * @brief 
 *
 * @return 
 */
int gic_dist_init(void);

/**
 * @brief 
 *
 * @return 
 */
int gic_cpu_init(void);

/**
 * @brief 
 *
 * @param int_id
 */
void generate_soft_int(u32 int_id);

/**
 * @brief 
 *
 * @param int_id
 */
void generate_soft_int_to_core0(u32 int_id);

/** @brief 
 *
 * @param val
 * @param addr
 */
static inline void gic_write(u32 val, va_t addr)
{
    sw_writel(val, (void *)(addr));
}

/**
 * @brief 
 *
 * @param addr
 *
 * @return 
 */
static inline u32 gic_read(va_t addr)
{
    return sw_readl((void *)(addr));
}

/**
* @brief 
*
* @param x
*/
static inline void gic_dist_restore(struct gic_context *x)
{
    int i;

    for (i = 0; i <= GIC_ITLINES; i++) {
	    gic_write(x->gic_icdiser[i],
		      GIC_DIST + GIC_DIST_ENABLE_SET + i * 4 );
    }
}


/**
* @brief 
*
* @param x
*/
static inline  void gic_dist_save(struct gic_context *x)
{
    int i;

    for (i = 0; i <= GIC_ITLINES; i++) {
	    x->gic_icdiser[i] = 
		    gic_read(GIC_DIST + GIC_DIST_ENABLE_SET + i * 4 );
	    /* disable the interrupts for a clean restore */
	    gic_write(x->gic_icdiser[i],
		      GIC_DIST + GIC_DIST_ENABLE_CLEAR + i * 4 );
    }	

}
#endif
