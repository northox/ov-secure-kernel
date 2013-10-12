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
 * cache-l2x0 implementation
 */

#include <sw_types.h>
#include <sw_io.h>
#include <sw_debug.h>
#include <sw_semaphores.h>
#include <cache-l2x0.h>

#define CACHE_LINE_SIZE		32

static u32 l2x0_base = 0;
static struct spinlock l2x0_lock; 

static u32 l2x0_inited = 0;
static u32 l2x0_way_mask;	/* Bitmask of active ways */
static u32 l2x0_size;
static u32 l2x0_cache_id;
static u32 l2x0_sets;
static u32 l2x0_ways;

/**
* @brief 
*
* @param val
* @param offset
*/
static inline void sw_l2x_writel(u32 val, u32 offset)
{
    sw_writel(val, (volatile void *) (l2x0_base + offset));
}

/**
* @brief 
*
* @param offset
*
* @return 
*/
static inline u32 sw_l2x_readl(u32 offset)
{
    return sw_readl((volatile void *) (l2x0_base + offset));
}


/**
* @brief 
*
* @param rev
*
* @return 
*/
static inline bool is_pl310_rev(int rev)
{
	return (l2x0_cache_id &
		(L2X0_CACHE_ID_PART_MASK | L2X0_CACHE_ID_REV_MASK)) ==
			(L2X0_CACHE_ID_PART_L310 | rev);
}


/**
* @brief 
*
* @param reg
* @param mask
*/
static inline void cache_wait_way(u32 reg, unsigned long mask)
{
	/* wait for cache operation by line or way to complete */
	while (sw_readl((volatile void *)reg) & mask)
		;
}

#ifdef CONFIG_CACHE_PL310

/**
* @brief 
*
* @param reg
* @param mask
*/
static inline void cache_wait(void  *reg, unsigned long mask)
{
	/* cache operations by line are atomic on PL310 */
}
#else
#define cache_wait	cache_wait_way
#endif

/**
* @brief 
*/
static inline void cache_sync(void)
{
#ifdef CONFIG_ARM_ERRATA_753970
	/* write to an unmmapped register */
    sw_l2x_writel(0, L2X0_DUMMY_REG);
#else
    sw_l2x_writel(0, L2X0_CACHE_SYNC);
#endif
	cache_wait((void*)(l2x0_base + L2X0_CACHE_SYNC), 1);
}

/**
* @brief 
*
* @param addr
*/
static inline void l2x0_clean_line(u32 addr)
{
	cache_wait((void*)((u32)l2x0_base + L2X0_CLEAN_LINE_PA), 1);
    sw_l2x_writel(addr, L2X0_CLEAN_LINE_PA);
}

/**
* @brief 
*
* @param addr
*/
static inline void l2x0_inv_line(u32  addr)
{
	cache_wait((void*)((u32)l2x0_base + L2X0_INV_LINE_PA), 1);
    sw_l2x_writel(addr, L2X0_INV_LINE_PA);
}

#if defined(CONFIG_PL310_ERRATA_588369) || defined(CONFIG_PL310_ERRATA_727915)

#define debug_writel(val)	l2x0_set_debug(val)

/**
* @brief 
*
* @param val
*/
void l2x0_set_debug(u32 val)
{
	if(!l2x0_inited)
		return;

    sw_l2x_writel(val, L2X0_DEBUG_CTRL);
}
#else
/* Optimised out for non-errata case */

/**
* @brief 
*
* @param val
*/
static inline void debug_writel(u32 val)
{
}

#define l2x0_set_debug	NULL
#endif


#ifdef CONFIG_PL310_ERRATA_588369

/**
* @brief 
*
* @param addr
*/
static inline void l2x0_flush_line(u32 addr)
{
	/* Clean by PA followed by Invalidate by PA */
	cache_wait((void*)((u32)l2x0_base + L2X0_CLEAN_LINE_PA), 1);
    sw_l2x_writel(addr, L2X0_CLEAN_LINE_PA);
	cache_wait((void*)((u32)l2x0_base + L2X0_INV_LINE_PA), 1);
    sw_l2x_writel(addr, L2X0_INV_LINE_PA);
}
#else

/**
* @brief 
*
* @param addr
*/
static inline void l2x0_flush_line(u32 addr)
{
	cache_wait((void*)((u32)l2x0_base + L2X0_CLEAN_INV_LINE_PA), 1);
    sw_l2x_writel(addr, L2X0_CLEAN_INV_LINE_PA);
}
#endif

/**
* @brief 
*/
static void l2x0_cache_sync(void)
{
	u32 flags;

//	spin_lock_irqsave(&l2x0_lock, &flags);
	cache_sync();
//	spin_unlock_irqrestore(&l2x0_lock, flags);
}

#ifdef CONFIG_PL310_ERRATA_727915

/**
* @brief 
*
* @param reg
*/
static void l2x0_for_each_set_way(void  *reg)
{
	int set;
	int way;
	u32 flags;

	for (way = 0; way < l2x0_ways; way++) {
//		spin_lock_irqsave(&l2x0_lock, &flags);
		for (set = 0; set < l2x0_sets; set++)
			sw_writel((way << 28) | (set << 5), (volatile void*)reg);
		cache_sync();
//		spin_unlock_irqrestore(&l2x0_lock, flags);
	}
}
#endif


/**
* @brief 
*/
static void __l2x0_flush_all(void)
{
	debug_writel(0x03); 
	sw_writel(l2x0_way_mask, (volatile void *)(l2x0_base + L2X0_CLEAN_INV_WAY));
	cache_wait_way((u32)l2x0_base + L2X0_CLEAN_INV_WAY, l2x0_way_mask);
	cache_sync();
	debug_writel(0x00); 
}

/**
* @brief 
*/
void l2x0_flush_all(void)
{
	u32 flags;

	if(!l2x0_inited)
		return;


#ifdef CONFIG_PL310_ERRATA_727915
	if (is_pl310_rev(REV_PL310_R2P0)) {
		l2x0_for_each_set_way((void*)(l2x0_base + L2X0_CLEAN_INV_LINE_IDX));
		return;
	}
#endif

	/* clean all ways */
//	spin_lock_irqsave(&l2x0_lock, &flags);
	__l2x0_flush_all();
//	spin_unlock_irqrestore(&l2x0_lock, flags);
}

/**
* @brief 
*/
void l2x0_clean_all(void)
{
	u32 flags;

	if(!l2x0_inited)
		return;


#ifdef CONFIG_PL310_ERRATA_727915
	if (is_pl310_rev(REV_PL310_R2P0)) {
		l2x0_for_each_set_way((void*)(l2x0_base + L2X0_CLEAN_LINE_IDX));
		return;
	}
#endif

	/* clean all ways */
//	spin_lock_irqsave(&l2x0_lock, &flags);
	debug_writel(0x03);
	sw_l2x_writel(l2x0_way_mask, L2X0_CLEAN_WAY);
	cache_wait_way((u32)l2x0_base + L2X0_CLEAN_WAY, l2x0_way_mask);
	cache_sync();
	debug_writel(0x00);
//	spin_unlock_irqrestore(&l2x0_lock, flags);
}


/**
* @brief 
*/
void l2x0_inv_all(void)
{
	u32 val;
	u32 flags; 

	if(!l2x0_inited)
		return;

	/* invalidate all ways */
//	spin_lock_irqsave(&l2x0_lock, &flags); 

	/* invalidate only if controller is disabled */
	val = sw_l2x_readl(L2X0_CTRL);
	if(val & 0x1)
		return;

	sw_l2x_writel(l2x0_way_mask, L2X0_INV_WAY);
	cache_wait_way((u32)l2x0_base + L2X0_INV_WAY, l2x0_way_mask);
	cache_sync();
//	spin_unlock_irqrestore(&l2x0_lock, flags); 
}


/**
* @brief 
*
* @param start
* @param end
*/
static void l2x0_inv_range(unsigned long start, unsigned long end)
{
	u32 flags;

//	spin_lock_irqsave(&l2x0_lock, &flags);
	if (start & (CACHE_LINE_SIZE - 1)) {
		start &= ~(CACHE_LINE_SIZE - 1);
		debug_writel(0x03);
		l2x0_flush_line(start);
		debug_writel(0x00);
		start += CACHE_LINE_SIZE;
	}

	if (end & (CACHE_LINE_SIZE - 1)) {
		end &= ~(CACHE_LINE_SIZE - 1);
		debug_writel(0x03);
		l2x0_flush_line(end);
		debug_writel(0x00);
	}

	while (start < end) {
		unsigned long blk_end = start + min(end - start, 4096UL);

		while (start < blk_end) {
			l2x0_inv_line(start);
			start += CACHE_LINE_SIZE;
		}

		if (blk_end < end) {
//			spin_unlock_irqrestore(&l2x0_lock, flags);
//			spin_lock_irqsave(&l2x0_lock, &flags);
		}
	}
	cache_wait((void*)((u32)l2x0_base + L2X0_INV_LINE_PA), 1);
	cache_sync();
//	spin_unlock_irqrestore(&l2x0_lock, flags);
}

/**
* @brief 
*
* @param start
* @param end
*/
static void l2x0_clean_range(unsigned long start, unsigned long end)
{
	u32 flags;

	if ((end - start) >= l2x0_size) {
		l2x0_clean_all();
		return;
	}

//	spin_lock_irqsave(&l2x0_lock, &flags);
	start &= ~(CACHE_LINE_SIZE - 1);
	while (start < end) {
		unsigned long blk_end = start + min(end - start, 4096UL);

		while (start < blk_end) {
			l2x0_clean_line(start);
			start += CACHE_LINE_SIZE;
		}

		if (blk_end < end) {
//			spin_unlock_irqrestore(&l2x0_lock, flags);
//			spin_lock_irqsave(&l2x0_lock, &flags);
		}
	}

	cache_wait((void*)((u32)l2x0_base + L2X0_CLEAN_LINE_PA), 1);
	cache_sync();
//	spin_unlock_irqrestore(&l2x0_lock, flags);
}

/**
* @brief 
*
* @param start
* @param end
*/
void l2x0_flush_range(unsigned long start, unsigned long end)
{
	u32 flags;

	if(!l2x0_inited)
		return;

	if ((end - start) >= l2x0_size) {
		l2x0_flush_all();
		return;
	}

//	spin_lock_irqsave(&l2x0_lock, &flags);
	start &= ~(CACHE_LINE_SIZE - 1);
	while (start < end) {
		unsigned long blk_end = start + min(end - start, 4096UL);

		debug_writel(0x03);
		while (start < blk_end) {
			l2x0_flush_line(start);
			start += CACHE_LINE_SIZE;
		}
		debug_writel(0x00);

		if (blk_end < end) {
//			spin_unlock_irqrestore(&l2x0_lock, flags);
//			spin_lock_irqsave(&l2x0_lock, &flags);
		}
	}
	cache_wait((void*)((u32)l2x0_base + L2X0_CLEAN_INV_LINE_PA), 1);
	cache_sync();
//	spin_unlock_irqrestore(&l2x0_lock, flags);
}

/* enables l2x0 after l2x0_disable, does not invalidate */
void l2x0_enable(void)
{
	u32 flags;

	if(!l2x0_inited)
		return;

//	spin_lock_irqsave(&l2x0_lock, &flags);
	sw_l2x_writel(1, L2X0_CTRL);
//	spin_unlock_irqrestore(&l2x0_lock, flags);
}

void l2x0_init_disable(void)
{
   	sw_l2x_writel(0, L2X0_CTRL);
	data_sync_barrier();
	
	/* Invalidate L2 */
	sw_l2x_writel(l2x0_way_mask, L2X0_INV_WAY);
	cache_wait_way((u32)l2x0_base + L2X0_INV_WAY, l2x0_way_mask);
	cache_sync();
}

/**
* @brief 
*
* @param flush
*/
void l2x0_disable(bool flush)
{
	u32 flags;

	if(!l2x0_inited)
		return;

//	spin_lock_irqsave(&l2x0_lock, &flags);
    if(flush) 
	__l2x0_flush_all();
    sw_l2x_writel(0, L2X0_CTRL);
	data_sync_barrier();
//	spin_unlock_irqrestore(&l2x0_lock, flags);
}

/**
* @brief 
*
* @param cache_id
*/
static void  l2x0_unlock(u32 cache_id)
{
	int lockregs;
	int i;

	cache_id &= L2X0_CACHE_ID_PART_MASK;

	if (cache_id == L2X0_CACHE_ID_PART_L310)
		lockregs = 8;
	else
		/* L210 and unknown types */
		lockregs = 1;

	for (i = 0; i < lockregs; i++) {
		sw_l2x_writel(0x0, L2X0_LOCKDOWN_WAY_D_BASE +
			       i * L2X0_LOCKDOWN_STRIDE);
		sw_l2x_writel(0x0, L2X0_LOCKDOWN_WAY_I_BASE +
			       i * L2X0_LOCKDOWN_STRIDE);
	}
}


/**
* @brief 
*
* @param base
*/
void l2x0_init_vars(void *base)
{
    l2x0_base = (u32)base;
    l2x0_lock.lock = 0;
}

void l2x0_set_prefetch(u32 prefetch)
{
	sw_l2x_writel(prefetch, L2X0_PREFETCH_CTRL);
}

/**
* @brief 
*
* @param aux_val
* @param aux_mask
* @param tag_latency
* @param data_latency
* @param power
*/
void l2x0_init(u32 aux_val, u32 aux_mask, 
                    u32 tag_latency, u32 data_latency, u32 power)
{
	u32 aux;
	u32 way_size = 0;
	const char *type;

    if(l2x0_base == 0)
        return;

	l2x0_cache_id = sw_l2x_readl(L2X0_CACHE_ID);
	aux = sw_l2x_readl(L2X0_AUX_CTRL);

	aux &= aux_mask;
	aux |= aux_val;

	/* Determine the number of ways */
	switch (l2x0_cache_id & L2X0_CACHE_ID_PART_MASK) {
	case L2X0_CACHE_ID_PART_L310:
		if (aux & (1 << 16))
			l2x0_ways = 16;
		else
			l2x0_ways = 8;
		type = "L310";
		break;
	case L2X0_CACHE_ID_PART_L210:
		l2x0_ways = (aux >> 13) & 0xf;
		type = "L210";
		break;
	default:
		/* Assume unknown chips have 8 ways */
		l2x0_ways = 8;
		type = "L2x0 series";
		break;
	}

	l2x0_way_mask = (1 << l2x0_ways) - 1;

	/*
	 * L2 cache Size =  Way size * Number of ways
	 */
	way_size = (aux & L2X0_AUX_CTRL_WAY_SIZE_MASK) >> 17;
	way_size = 1024 << (way_size + 3);
	l2x0_size = l2x0_ways * way_size;
	l2x0_sets = way_size / CACHE_LINE_SIZE;

	/*
	 * Check if l2x0 controller is already enabled.
	 */
	if (!(sw_l2x_readl(L2X0_CTRL) & 1)) {
		/* Make sure that I&D is not locked down when starting */
		l2x0_unlock(l2x0_cache_id);

		/* l2x0 controller is disabled */
		sw_l2x_writel(aux, L2X0_AUX_CTRL);

		/* Invalidate L2 */
		sw_l2x_writel(l2x0_way_mask, L2X0_INV_WAY);
		cache_wait_way((u32)l2x0_base + L2X0_INV_WAY, l2x0_way_mask);
		cache_sync();

         sw_l2x_writel(tag_latency, L2X0_TAG_LATENCY_CTRL);
		 sw_l2x_writel(data_latency, L2X0_DATA_LATENCY_CTRL);

         sw_l2x_writel(power, L2X0_POWER_CTRL);
		/* enable L2X0 */
		sw_l2x_writel(1, L2X0_CTRL);
	}

	l2x0_inited = 1;

#ifdef L2CC_DBG
	sw_printf("SW: %s cache controller enabled\n", type);
	sw_printf("SW: l2x0: 0x%x ways, CACHE_ID 0x%x, AUX_CTRL 0x%x, \
Cache size: 0x%x B\n", l2x0_ways, l2x0_cache_id, aux, l2x0_size);
#endif
}
