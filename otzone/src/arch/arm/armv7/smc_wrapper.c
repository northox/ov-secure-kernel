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
 * SMC wrapper functions implementation
 */

/* SMC wrapper functions */

#include <sw_types.h>
#include <cpu_asm.h>
#include <cpu_data.h>
/* Fix Me */
#include <system_context.h>

#ifdef CONFIG_KSPACE_PROTECTION
#include <sw_syscall.h>
#endif

/**
* @brief smc system call implementation
*
* @param smc_arg: smc system call parameter
*/
void __execute_smc(u32 smc_arg)
{
       register u32 r0 asm("r0") = smc_arg;
        asm volatile(
                    "smc    #0  @ switch to Non secure world\n");
        return;
}

/**
 * @brief 
 */
void invoke_ns_kernel(void)
{
    register u32 r0 asm("r0") = INVOKE_NON_SECURE_KERNEL;
    do {

        /* Execute SMC and go to non-secure world*/
#ifdef CONFIG_KSPACE_PROTECTION
            __asm_execute_smc();
#else
        asm volatile(
            "smc    #0  @ switch to Non secure world\n"); 
#endif

    } while (0);

    return;
}

/**
 * @brief 
 *
 * @param retval
 */
void return_secure_api(u32 retval)
{
    register u32 r0 asm("r0") = RET_FROM_SECURE_API;
    params_out_stack[0] = retval;
    do {

        /* Execute SMC and go to non-secure world*/
#ifdef CONFIG_KSPACE_PROTECTION
            __asm_execute_smc();
#else
        asm volatile(
            "smc    #0  @ switch to secure world\n");
#endif

    } while (0);

    return;
}

/**
* @brief 
*/
void smc_nscpu_context_init(void)
{
    register u32 r0 asm("r0") = TZHYP_NSCPU_CTXT_INIT;
    do {

        /* Execute SMC and go to non-secure world*/
#ifdef CONFIG_KSPACE_PROTECTION
            __asm_execute_smc();
#else
        asm volatile(
            "smc    #0  @ switch to secure world\n");
#endif

    } while (0);

    return;
}

