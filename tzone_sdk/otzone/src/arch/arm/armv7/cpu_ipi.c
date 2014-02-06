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
 * Architecture specific IPI routines
 */

#include <sw_types.h>
#include <otz_id.h>
#include <secure_api.h>
#include <smc_id.h>
#include <cpu_data.h>
#include <cpu.h>

/**
* @brief Invoke IPI  
*
* This function formulate the parameters for IPI and suspend the current task
* and invoke the SWI command for IPI notification
*
* @param smc_cmd: Pointer to command structure
*
*/
void otz_ipi(struct otz_smc_cmd *smc_cmd)
{
    params_stack[0] = CALL_TRUSTZONE_API;
    params_stack[1] = (u32)smc_cmd;
    params_stack[2] = OTZ_CMD_TYPE_SECURE_TO_SECURE;
    params_stack[3] = 0;
    valid_params_flag = 1;

    suspend_task(smc_cmd->src_context, TASK_STATE_WAIT);
#ifdef CONFIG_EMULATE_FIQ
    emulate_swi_handler(0xcccc);
#else
     asm volatile("swi #0xcccc");
#endif
}


/**
* @brief Return for IPI
*
* This function sets the IPI return value in the originated task. Reschedules
* the originated task.
*
* @param smc_cmd: Command parameter which got passed in IPI
* @param ret_val: Return value of the target task result.
*/
void otz_ipi_return(struct otz_smc_cmd *smc_cmd, int ret_val)
{
    irq_flags_t irqflags;
    struct sw_task *task = get_task(smc_cmd->src_context);
    if(task) {
        task->tls->ipi_ret_val = ret_val;

        spin_lock_irqsave(&global_val.ready_to_run_lock, &irqflags);
        schedule_task(task);
        spin_unlock_irqrestore(&global_val.ready_to_run_lock,irqflags);
    }
}

