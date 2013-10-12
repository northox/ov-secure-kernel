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
 * Simple round-robin scheduler for tasks
 */

#include <sw_types.h>
#include <global.h>
#include <cpu.h>
#include <task.h>

/**
 * @brief Scheduler function
 *
 * This function implements the round-robin scheduler
 */
void scheduler(void) 
{
    struct sw_task *next_task;
    struct sw_task *current_task;
    
    current_task = get_current_task();
    next_task = get_next_task();

    if(current_task != next_task) {
        if(next_task) {
            next_task->state = TASK_STATE_RUNNING;
            update_current_task(next_task);
        }
        task_context_switch(next_task, current_task);

    }

    if(!next_task)
        disable_timer();
}
