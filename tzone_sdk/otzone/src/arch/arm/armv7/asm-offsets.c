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
 * asm-offsets implementation 
 */

#include <sw_types.h>
#include <cpu_task.h>
#include <cpu_data.h>
#ifdef CONFIG_NEON_SUPPORT
#include "system_context.h"
#endif

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define DEFINE(sym, val) \
        asm volatile("\n->" #sym " %0 " #val : : "i" (val))

/**
 * @brief 
 *
 * @return 
 */
int main(void)
{
DEFINE(TASK_PC_OFFSET,	    offsetof(struct sw_task_cpu_regs, pc));
DEFINE(TASK_SPSR_OFFSET,	offsetof(struct sw_task_cpu_regs, spsr));
DEFINE(TEMP_SWI_REGS_LR_OFFSET,	offsetof(struct swi_temp_regs, lr));
DEFINE(TEMP_SWI_REGS_SPSR_OFFSET,	offsetof(struct swi_temp_regs, spsr));
DEFINE(TEMP_SWI_REGS_R0_OFFSET,	offsetof(struct swi_temp_regs, regs));
#ifdef CONFIG_NEON_SUPPORT
DEFINE(NEON_OFFSET, offsetof(struct system_context, sysctxt_vfp));
#endif
return 0;
}
