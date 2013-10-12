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
 * versatile express RS1 board configuration
 */

#include <sw_board.h>
#include <page_table.h>
#include <cpu_data.h>
#include <sw_io.h>
#include <cpu.h>
#include <sw_board_asm.h>

static const struct devmap ve_devmap[] =  {
	{0}
};

static const struct devmap ve_usr_access_devmap[] =  {
	{
		.dv_va = SECURE_UART_BASE & 0xfff00000,
		.dv_pa = SECURE_UART_BASE & 0xfff00000,
		.dv_size = 0x100000,
			
	},
	{
		.dv_va = FREE_RUNNING_TIMER_BASE & 0xfff00000,
		.dv_pa = FREE_RUNNING_TIMER_BASE & 0xfff00000,
		.dv_size = 0x100000,
	},
	{
		.dv_va = VE_RS1_MPIC,
		.dv_pa = VE_RS1_MPIC,
		.dv_size = 0x100000,
			
	},
	{0}
};

#ifdef CONFIG_SW_DEDICATED_TEE
extern u32 secondary_start_config_reg[MAX_CORES];
#endif

/**
* @brief 
*/
void board_init(void)
{
}

#ifdef CONFIG_SW_DEDICATED_TEE

/**
* @brief 
*
* @param cpu_id
*/
void start_secondary_linux(u32 cpu_id)
{
    switch (cpu_id) {
        case 0:
            secondary_start_config_reg[cpu_id] = KERNEL_START_ADDR;
            break;
        case 1:
            secondary_start_config_reg[cpu_id] = KERNEL_START_ADDR;
            break;
        default:
            break;
    }
    dmb();
}
#endif

/**
* @brief 
*
* @param pgd
*/
void board_map_secure_page_table(u32* pgd)
{
	va_t va;
	pa_t pa;

	/* Map kernel code */

	/* text */
	va = get_sw_text_start();
	pa = va;
	map_secure_memory(va, pa, get_sw_text_size(), PTF_PROT_URO| PTF_EXEC);

	/*init and mod init section*/
	va = get_init_start_addr();
	pa = va;
	map_secure_memory(va, pa, get_init_size(), PTF_PROT_URO| PTF_EXEC);

	/*Mapping frame buffer memory*/
	va = VE_FRAME_BASE;
    pa = va;
    map_secure_memory(va, pa, VE_FRAME_SIZE, PTF_PROT_URW);


	/* data */
	va = get_sw_data_start();
	pa = va;
	map_secure_memory(va, pa, get_sw_data_size(), PTF_PROT_URW);

	/* bss */
	va = get_sw_bss_start();
	pa = va;
	map_secure_memory(va, pa, get_sw_bss_size(), PTF_PROT_URW);

#if 1
	/* 
	 * map support data structures for heap - This is a workaround  
	 * and has to be fixed in the memory manager
	 */
	va = get_sw_code_end();
	pa = va;
	map_secure_memory(va, pa, 0x100000, PTF_PROT_URW);

#endif

	/* Map devices */
	map_device_table(ve_devmap);

	/* Map User access devices */
	map_user_access_device_table(ve_usr_access_devmap);

}

/**
 * @brief 
 *      This functions unmaps init and mod init from secure ram
 */
void unmap_init_section(void){
	va_t va;
	va = get_init_start_addr();
	unmap_secure_memory(va, get_init_size());
}

/**
 * @brief 
 *      This functions returns the starting address of the 
 *      Secure RAM
 * @return 
 *      Starting address of the Secure RAM
 */
pa_t get_secure_ram_start_addr(void)
{   
        return SECURE_WORLD_RAM_START;
}

/**
 * @brief 
 *      This function returns the End address of the Secure RAM
 * @return 
 *      End address of the Secure RAM
 */
pa_t get_secure_ram_end_addr(void)
{
        return SECURE_WORLD_RAM_END;
}

/**
 * @brief 
 *      Dummy funtion to handle board smc
*/
void board_smc_handler(){
}
