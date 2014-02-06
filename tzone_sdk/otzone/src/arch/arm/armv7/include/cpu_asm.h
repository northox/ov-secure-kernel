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
 * Header for cpu_asm implementation
 */

#ifndef __ARMV7_CPU_ASM_H__
#define __ARMV7_CPU_ASM_H__

#include <smc_id.h>

/* assembler definitions */
#define  Mode_USR       0x10
#define  Mode_FIQ       0x11
#define  Mode_IRQ       0x12
#define  Mode_SVC       0x13
#define  Mode_MON       0x16
#define  Mode_ABT       0x17
#define  Mode_UNDEF         0x1B
#define  Mode_SYS       0x1F


#define  I_BIT          0x80    
#define  F_BIT          0x40    


#define  SCR_NS_BIT     0x1
#define  SCR_FIQ_BIT        0x4
#define  SCR_IRQ_BIT        0x2

#define CPSR_RESET_VAL     0x00000193
#define SCTLR_RESET_VAL    0x00c50078

#define AUXREG_SMP          0x41

#define AUXREG_SMP_ENABLE  0x40
#define AUXREG_FW          0x1

#ifdef CONFIG_NEON_SUPPORT
/* VFP Unit values*/
#define FPEXC_EX           (1 << 31)
#define FPEXC_EN           (1 << 30)
#define MVFR0_A_SIMD_BIT   (0)
#define MVFR0_A_SIMD_MASK  (0xf << MVFR0_A_SIMD_BIT)
#endif

/* Fix Me  - Need to be auto generated and moved to better place */
#define SCTLR_OFFSET       120
#define SPSR_MON_OFFSET    52
#define LR_MON_OFFSET      56

/* TzHyp */
#define NS_PREEMPT_ENABLE   0x1
#define NS_PREEMPT_DISABLE  0x0
#define NS_SWITCH_ACTIVE    0x1
#define NS_SWITCH_CLEAR     0x0

/* SMC Identifiers for secure world functions */
#define INVOKE_NON_SECURE_KERNEL    0x0ffffff1
#define RET_FROM_SECURE_API         0x0ffffff2
#define INVOKE_NS_KER_SMP           0x0ffffff3
#define TZHYP_NSCPU_CTXT_INIT       0x0ffffff4
#define TZHYP_NSADMIN_RETURN        0x0ffffff5

#define SEC_SGI_TO_SECONDARY_CORE        0xB
#define SEC_SGI_TO_PRIMARY_CORE          0xA

#define STACK_SIZE        4096
#define STACK_SIZE_SHIFT  12

#define PARAM_STACK_SIZE        8
#define PARAM_STACK_SIZE_SHIFT  5     /* ( 3 + 2) */

#define PARAM_OUT_STACK_SIZE        8
#define PARAM_OUT_STACK_SIZE_SHIFT  5  /* (3 + 2) */

#define SYS_CONTEXT_CORE_SHIFT      8

#endif
