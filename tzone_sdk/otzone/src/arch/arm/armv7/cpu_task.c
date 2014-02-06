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
 * cpu_task implementation
 */

/* cpu task implementation */
#include <sw_types.h>
#include <task.h>
#include <cpu_task.h>
#include <cpu_asm.h>
#include <sw_debug.h>

/**
 * @brief Helper function to initialize task registers
 *
 * Initialize r0 - task id, r1 - task local storage, r2-r12 - zero, 
 * lr - zero, sp - top of the stack, pc - task entry function
 * spsr - System mode
 *
 * @param task: Pointer to the task 
 * @param regs: Pointer to the task registers
 * @param task_sp: Stack pointer value for the task
 *
 * @return otz_return_t:
 * OTZ_OK \n
 * OTZ_* - An implementation-defined error code for any other error.\n
 */
static int cpu_task_regs_init(struct sw_task *task, 
            struct sw_task_cpu_regs *regs, 
            u32 task_sp)
{
    int cnt = 0;
    regs->regs[0] = task->task_id;
    regs->regs[1] = (u32)task->tls;

    for(cnt = 2; cnt < 13; cnt++)
        regs->regs[cnt] = 0;

    regs->lr  = 0;
    regs->pc = task->entry_addr;
    regs->sp = task_sp + task->task_sp_size;
#ifdef CONFIG_KSPACE_PROTECTION
    regs->spsr = Mode_USR | I_BIT; /* User mode */
# else
    regs->spsr = Mode_SYS | I_BIT; /* System mode */
#endif
    return OTZ_OK;
}

/**
 * @brief Archiecture specific task init function
 *
 * This function get called from start task operation. 
 * Initialize task registers.
 *
 * @param task: Pointer to task structure
 *
 * @return otz_return_t:
 * OTZ_OK \n
 * OTZ_* - An implementation-defined error code for any other error.\n
 */
int  cpu_task_init(void *task)
{
    int ret;
    struct sw_task *new_task = (struct sw_task *) task;
    ret = cpu_task_regs_init(new_task, &new_task->regs, (u32)new_task->task_sp);
    return ret;
}


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
int  cpu_task_exit(void *task)
{
    return OTZ_OK;
}


/**
 * @brief Print task register values
 *
 * @param task: Pointer to the task structure
 */
void print_cpu_task_regs(void *task)
{
    int i;
    struct sw_task *pr_task = (struct sw_task *) task;
    for(i=0; i < 13; i++)
        sw_printf("SW: task regs[%x] 0x%x\n", i, pr_task->regs.regs[i]);

    sw_printf("SW: task regs sp 0x%x\n", pr_task->regs.sp);
    sw_printf("SW: task regs lr 0x%x\n", pr_task->regs.lr);
    sw_printf("SW: task regs pc 0x%x\n", pr_task->regs.pc);
}
