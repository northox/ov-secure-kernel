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
 * cpu_task.h header file for cpu dependent task declarations and defintions
 */

#ifndef __SW_CPU_TASK_H__
#define __SW_CPU_TASK_H__

/**
 * @brief Task registers context
 */
struct sw_task_cpu_regs {
/*! Registers r0 -r12 */
    u32 regs[13];
/*! Stack pointer of the task */
    u32 sp;
/*! Link register of the task */
    u32 lr;
/*! SPSR of the task */
    u32 spsr;
/*! Current PC of the task */
    u32 pc; 
};

/**
 * @brief Archiecture specific task init function
 *
 * This function get called from start task operation
 * @param task: Pointer to task structure
 *
 * @return otz_return_t:
 * OTZ_OK \n
 * OTZ_* - An implementation-defined error code for any other error.\n
 */
int cpu_task_init(void *task);

/**
 * @brief Archiecture specific task exit function
 *
 * Thius function get called before the task destruction.
 *
 * @param task: Pointer to task structure
 *
 * @return otz_return_t:
 * OTZ_OK \n
 * OTZ_* - An implementation-defined error code for any other error.\n
 */
int cpu_task_exit(void *task);

/**
 * @brief Print task register values
 *
 * @param task: Pointer to the task structure
 */
void print_cpu_task_regs(void *task);

/* cpu_task_context.S */
/**
 * @brief Save task context 
 *
 * This function used to save the task context registers
 *
 * @param regs: Pointer task registers structure
 */
void save_task_context_regs(struct sw_task_cpu_regs *regs);

/**
 * @brief Restore task context 
 *
 * This function used to restore the task context registers
 *
 * @param regs: Pointer task registers structure
 */
void restore_task_context_regs(struct sw_task_cpu_regs *regs);



#endif /* __SW_CPU_TASK_H__ */
