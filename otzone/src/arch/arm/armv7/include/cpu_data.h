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
 * declaration of .data and .bss section variables
 * 
 */

#ifndef __TZ_CPU_DATA_H__
#define __TZ_CPU_DATA_H__

#include <page_table.h>
#include <cpu_asm.h>
#include <sw_board.h>
#include <sw_mcore.h>

#define TASK_STACK_SIZE 2048

#define SW_PRIMARY_CORE     0x1
#define SW_SECONDARY_CORE   0x0




/**
* @brief Structure to store register in SWI handler
*/
struct swi_temp_regs {
/*! spsr */
    u32 spsr;
/*! regs r0 - r12 */
    u32 regs[13];   
/*! link register */
    u32 lr;         
};


extern u32 _SW_KSYMTAB;
extern u32 _SW_KSYMTAB_END;

/**
 * @brief Secure page table
 */
extern u32 secure_page_table[PAGE_TABLE_ENTRIES];

/**
 * @brief User stack
 */
extern u32 user_stack[MAX_CORES][STACK_SIZE/4];

/**
 * @brief Supervisor stack
 */
extern u32 service_stack[MAX_CORES][STACK_SIZE/4];

/**
 * @brief Abort stack
 */
extern u32 abort_stack[MAX_CORES][STACK_SIZE/4];

/**
 * @brief Undefined stack
 */
extern u32 undefined_stack[MAX_CORES][STACK_SIZE/4];

/**
 * @brief IRQ stack
 */
extern u32 irq_stack[MAX_CORES][STACK_SIZE/4];

/**
 * @brief FIQ stack
 */
extern u32 fiq_stack[MAX_CORES][STACK_SIZE/4];

/**
 * @brief Monitor stack
 */
extern u32 monitor_stack[MAX_CORES][STACK_SIZE/4];

/**
 * @brief Parameters stack which is used to SMC call parameters
 */
extern u32 params_stack[PARAM_STACK_SIZE];

/**
 * @brief 
 */
extern u32 params_smp_stack[MAX_CORES][PARAM_STACK_SIZE];

/**
 * @brief Helps in emulating secure interrupts
 */
extern u32 secure_interrupt_set;

/**
 * @brief Temporary register storage
 */
extern u32 temp_regs[32];

/**
 * @brief Parameters out stack which is used store the return value of SMC call
 */
extern u32 params_out_stack[PARAM_OUT_STACK_SIZE];

/**
 * @brief Parameters out stack which is used store the return value of SMC call
 * One Per CPU
 */
extern u32 params_out_smp_stack[MAX_CORES][PARAM_OUT_STACK_SIZE];

/**
* @brief Temp registers used in SWI handler
*/
extern struct swi_temp_regs *temp_swi_regs;


/**
 * @brief Valid params flag
 */
extern u32 valid_params_flag;

/**
 * @brief Valid return params flag
 */
extern u32 valid_return_params_flag[MAX_CORES];

/**
 * @brief multi core mode
 */
extern u32 multi_core_mode;

/**
 * @brief 
 *  This function returns the start of the virtual address
 *  of the secure world from the linker script
 * @return 
 */
va_t* get_sw_code_start(void);

/**
 * @brief 
 *  This function returns the end address(virtual)
 *  of the secure world code with the help of the linker script
 *
 * @return 
 */
va_t* get_sw_code_end(void);

/**
 * @brief 
 *  Function returns the starting address of the text section
 *  which is given in the Linker script
 * @return 
 */
va_t* get_sw_text_start(void);

/**
 * @brief 
 *  Function returns the size of text section
 *  which is given in the Linker script
 *
 * @return 
 */
int get_sw_text_size(void);

/**
* @brief 
*   Function returns the start of the file system
*   which is given in the Linker script
* @return 
*/
va_t* get_sw_fs_start(void);

/**
 * @brief 
 *  Function returns the starting address of the data section
 *  which is given in the Linker script
 * @return 
 */
va_t* get_sw_data_start(void);

/**
 * @brief 
 *  Function returns the size of data section
 *  which is given in the Linker script
 *
 * @return 
 */
int get_sw_data_size(void);

/**
 * @brief 
 *  Function returns the starting address of the bss section
 *  which is given in the Linker script
 * @return 
 */
va_t* get_sw_bss_start(void);

/**
 * @brief 
 *  Function returns the size of bss section
 *  which is given in the Linker script
 *
 * @return 
 */
int get_sw_bss_size(void);

/**
 * @brief 
 *      This function returns the start address of the region
 *      which has the module initialization codes
 * @return 
 */

va_t* get_mod_init_start_addr(void);

/**
 * @brief 
 *      This function returns the end address of the region
 *      which has the module initialization codes
 *
 * @return 
 */
va_t* get_mod_init_end_addr(void);

#endif /* __TZ_CPU_DATA_H__ */
