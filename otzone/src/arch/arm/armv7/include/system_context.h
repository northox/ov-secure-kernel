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
 * Header for system context declarations
 */

#ifndef SYSTEM_CONTEXT_H
#define SYSTEM_CONTEXT_H

#include <sw_types.h>
#include <sw_board.h>  /* for GIC_ITLINES */
#include <cpu.h>


/**
* @brief 
*/
struct core_context {
	u32 r0;
	u32 r1;
	u32 r2;
	u32 r3;
	u32 r4;
	u32 r5;
	u32 r6;
	u32 r7;
	u32 r8;
	u32 r9;
	u32 r10;
	u32 r11;
	u32 r12;
	u32 spsr_mon;
	u32 lr_mon;
	u32 spsr_svc;
	u32 r13_svc;
	u32 lr_svc;
	u32 r13_sys;
	u32 lr_sys;
	u32 spsr_abt;
	u32 r13_abt;
	u32 lr_abt;
	u32 spsr_undef;
	u32 r13_undef;
	u32 lr_undef;
	u32 spsr_irq;
	u32 r13_irq;
	u32 lr_irq;		
};

/**
* @brief 
*/
struct cp15_context { 
	u32 c0_CSSELR;      /* Cache Size Selection Register */
	u32 c1_SCTLR;       /* System Control Register */
	u32 c1_ACTLR;       /* Auxilliary Control Register */
	u32 c2_TTBR0;       /* Translation Table Base Register 0 */
	u32 c2_TTBR1;       /* Translation Table Base Register 1 */
	u32 c2_TTBCR;       /* Translation Table Base Register Control */
	u32 c3_DACR;        /* Domain Access Control Register */
	u32 c5_DFSR;        /* Data Fault Status Register */
	u32 c5_IFSR;        /* Instruction Fault Status Register */
#if 0
	u32 c5_ADFSR;       /* Data Fault Status Register */
	u32 c5_AIFSR;       /* Instruction Fault Status Register */
#endif
	u32 c6_DFAR;        /* Data Fault Address Register */
	u32 c6_IFAR;        /* Instruction Fault Address Register */
	u32 c7_PAR;         /* Physical Address Register */
	u32 c10_PRRR;       /* PRRR */
	u32 c10_NMRR;       /* NMRR */
	u32 c12_VBAR;       /* VBAR register */
	u32 c13_FCSEIDR;    /* FCSE PID Register */
	u32 c13_CONTEXTIDR; /* Context ID Register */
	u32 c13_TPIDRURW;   /* User Read/Write Thread and Process ID */
	u32 c13_TPIDRURO;   /* User Read-only Thread and Process ID */
	u32 c13_TPIDRPRW;   /* Privileged only Thread and Process ID */
};


/**
* @brief 
*/
struct gic_context {
	u32 gic_icdiser[GIC_ITLINES];
};

#ifdef CONFIG_NEON_SUPPORT
/**
* @brief 
*   vfp-neon double word registers 
*/
typedef struct {
	u32 d_low_word;
	u32 d_high_word;	
} double_word_reg;

/**
* @brief 
*    vfp-neon register bank
*/
struct vfp_context {
	u32 FPEXC;              /* Floating point Exception Register*/
 	u32 FPSCR;	        /* Floating point Status and control Register*/
	u32 FPSID;              /* Floating point system ID Register*/
	double_word_reg d[32];  /* 32 double word Registers*/
};
#endif
/* 
 * Please do not change the order of the members, until we remove the 
 * below assumption made by cpu context (core registers) switching code in 
 * monitor mode
 *
 * struct system_context x; 
 * "&x.sysctxt_core" is same as "&x"
 */
struct system_context {
	/* CPU context */
	struct core_context sysctxt_core;
	struct cp15_context sysctxt_cp15;
	/* Devices */
	struct gic_context sysctxt_gic;	

	#ifdef CONFIG_NEON_SUPPORT
  	struct vfp_context sysctxt_vfp;
	#endif
	u32 guest_no;
        /* 
	 * to make the size a power of 2, so that multiplication can be acheived 
         * by logical shift
         */
          
	u32 pad[8];  
} __attribute__ ((aligned (CACHELINE_SIZE)));


/**
 * @brief 
 *
 * @param 
 * @param 
 */
extern void tzhyp_sysregs_switch(struct cp15_context *, struct cp15_context *);

/**
 * @brief 
 *
 * @param 
 */
extern void tzhyp_sysregs_save(struct cp15_context *);

#endif
