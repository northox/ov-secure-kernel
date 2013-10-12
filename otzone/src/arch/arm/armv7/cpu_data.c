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
 * .data and .bss section variables implementation
 */

/*
.data and .bss section variables
*/
#include <sw_types.h>
#include <cpu_data.h>

/* .data section variables */
/**
 * @brief Secure page table
 */
u32 secure_page_table[PAGE_TABLE_ENTRIES]   __attribute__ ((section (".data")))
                                    __attribute__ ((aligned (16384)));

/* .bss section variables */
/**
 * @brief User stack
 */
u32 user_stack[MAX_CORES][STACK_SIZE/4]  __attribute__ ((aligned (4)));

/**
 * @brief Supervisor stack
 */
u32 service_stack[MAX_CORES][STACK_SIZE/4]  __attribute__ ((aligned (4)));

/**
 * @brief Abort stack
 */
u32 abort_stack[MAX_CORES][STACK_SIZE/4]    __attribute__ ((aligned (4)));

/**
 * @brief Undefined stack
 */
u32 undefined_stack[MAX_CORES][STACK_SIZE/4]   __attribute__ ((aligned (4)));

/**
 * @brief IRQ stack
 */
u32 irq_stack[MAX_CORES][STACK_SIZE/4]      __attribute__ ((aligned (4)));

/**
 * @brief FIQ stack
 */
u32 fiq_stack[MAX_CORES][STACK_SIZE/4]      __attribute__ ((aligned (4)));

/**
 * @brief Monitor stack
 */
u32 monitor_stack[MAX_CORES][STACK_SIZE/4]  __attribute__ ((aligned (4)));

/**
 * @brief Temporary register storage
 */
u32 temp_regs[32]           __attribute__ ((section (".bss")));

/**
 * @brief Parameters stack which is used to SMC call parameters
 */
u32 params_stack[PARAM_STACK_SIZE] __attribute__ ((section (".bss"))) 
                                                 __attribute__ ((aligned (4)));

/**
 * @brief Parameters stack which is used to SMC call parameters 
 *        One-per CPU
 */
u32 params_smp_stack[MAX_CORES][PARAM_STACK_SIZE] __attribute__ ((section (".bss"))) 
                                                 __attribute__ ((aligned (4)));
/**
* @brief Parameters out stack which is used store the return value of SMC call
*/
u32 params_out_stack[PARAM_OUT_STACK_SIZE] __attribute__ ((section (".bss"))) 
                                                 __attribute__ ((aligned (4)));

/**
 * @brief Parameters out stack which is used store the return value of SMC call
 *      One-Per CPU
 */
u32 params_out_smp_stack[MAX_CORES][PARAM_OUT_STACK_SIZE] __attribute__ ((section (".bss"))) 
                                                 __attribute__ ((aligned (4)));

/**
 * @brief Helps in emulating secure interrupts
 */
u32 secure_interrupt_set __attribute__ ((section (".bss")))
                                                  __attribute__ ((aligned (4)));

/**
 * @brief Temp registers used in SWI handler
 */
struct swi_temp_regs *temp_swi_regs;

/**
 * @brief Valid params flag 
 */
u32 valid_params_flag = 0;

/**
 * @brief valid_return_params_flag 
 */
u32 valid_return_params_flag[MAX_CORES];

/**
 * @brief multi core mode
 */
u32 multi_core_mode = 0;

extern u8 _SW_CODE_START;
extern u8 _SW_CODE_END;
extern u8 _SW_TEXT_END;
extern u8 _SW_BSS_START;
extern u8 _SW_FS_START;
extern u8 _text_size;
extern u8 _data_size;
extern u8 _bss_size;
extern u8 _init_size;

extern u8 _MOD_INIT_SECTION_START;
extern u8 _MOD_INIT_SECTION_END;
extern u8 _MOD_INIT_PADDING_END;

/**
 * @brief 
 *  Function returns the starting address of the secure world code
 *  which is given in the Linker script
 * @return 
 */
va_t* get_sw_code_start(void)
{
    return (va_t*)&_SW_CODE_START;
}

/**
 * @brief 
 *  Function returns the End address of the secure world code
 *  which is given in the Linker script
 *
 * @return 
 */
va_t* get_sw_code_end(void)
{
    return (va_t*)&_SW_CODE_END;
}

/**
 * @brief 
 *  Function returns the starting address of the text section
 *  which is given in the Linker script
 * @return 
 */
va_t* get_sw_text_start(void)
{
    return (va_t*)&_SW_CODE_START;
}

#ifdef CONFIG_FILESYSTEM_SUPPORT
/**
* @brief 
*   Function returns the starting address of the file system
*   which is given in the Linker script
* @return 
*/
va_t* get_sw_fs_start(void)
{
    return (va_t*)&_SW_FS_START;
}
#endif

/**
 * @brief 
 *  Function returns the size of text section
 *  which is given in the Linker script
 *
 * @return 
 */
int get_sw_text_size(void)
{
	return (int)&_text_size;
}

/**
 * @brief 
 *  Function returns the starting address of the data section
 *  which is given in the Linker script
 * @return 
 */
va_t* get_sw_data_start(void)
{
    return (va_t*)&_MOD_INIT_PADDING_END;
}

/**
 * @brief 
 *  Function returns the size of data section
 *  which is given in the Linker script
 *
 * @return 
 */
int get_sw_data_size(void)
{
	return (int)&_data_size;
}

/**
 * @brief 
 *  Function returns the starting address of the bss section
 *  which is given in the Linker script
 * @return 
 */
va_t* get_sw_bss_start(void)
{
    return (va_t*)&_SW_BSS_START;
}

/**
 * @brief 
 *  Function returns the size of bss section
 *  which is given in the Linker script
 *
 * @return 
 */
int get_sw_bss_size(void)
{
	return (int)&_bss_size;
}

#ifdef CONFIG_KSPACE_PROTECTION
/**
 * @brief 
 *      This function returns the start address of the region
 *      which has the module initialization codes
 * @return 
 */

va_t* get_mod_init_start_addr(void)
{
    return (va_t*)&_MOD_INIT_SECTION_START;
}

/**
 * @brief 
 *      This function returns the start address of the region
 *      which has the initialization codes
 * @return 
 */

va_t* get_init_start_addr(void)
{
    return (va_t*)&_SW_TEXT_END;
}

/**
 * @brief 
 *      This function returns the end address of the region
 *      which has the module initialization codes
 *
 * @return 
 */
va_t* get_mod_init_end_addr(void)
{
    return (va_t*)&_MOD_INIT_SECTION_END;
}

/**
 * @brief 
 *      This function returns the size of the region
 *      which has the initialization codes and module initialization lies
 * @return 
 */

int get_init_size(void)
{
    return (int)&_init_size;
}
#endif
