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
 * exception handlers methods implementation
 */

#include <sw_types.h>
#include <sw_debug.h>
#include <cpu.h>
#include <cpu_mmu.h>
#include <exception_handlers.h>
#include <sw_mem_functions.h>
#include <sw_string_functions.h>
#include <page_table.h>
#include <gic.h>
#include <task.h>
#include <global.h>
#include <cpu_data.h>
#include <monitor.h>
#include <sw_timer.h>

/* board dependent code -- fix it */

#include <sw_board.h>
#include <sw_modinit.h>

#ifdef CONFIG_KSPACE_PROTECTION
#include <sw_syscalls_id.h>
#endif

#include <secure_timer.h>

#ifdef CONFIG_GUI_SUPPORT
#include <sw_fb.h>
#endif

/**
 * @brief Data abort fault strings
 */
static const char* data_abt_fault_string[] =
{
	"INVALID ENTRY",
	"Alignment Fault",
	"Debug Event",
	"Access Flag - Section",
	"Instruction Cache Maintainance Fault",
	"Translation Fault - Section",
	"Access Flag - Page",
	"Translation Fault - Page",
	"Synchronous External Abort",
	"Domain Fault - Section",
	"INVALID ENTRY",
	"Domain Fault - Page",
	"Translation Table Talk 1st Level sync abt",
	"Permission Fault - Section",
	"Translation Table Walk 2nd Level sync abt",
	"Permission Fault - Page",
	"INVALID ENTRY",                            /* 10 = 0b10000, */
	"INVALID ENTRY",                            /* 11 = 0b10001, */
	"INVALID ENTRY",                            /* 12 = 0b10010, */
	"INVALID ENTRY",                            /* 13 = 0b10011, */
	"IMPLEMENTATION DEFINED Lockdown",          /* 14 = 0b10100, */
	"INVALID ENTRY",                            /* 15 = 0b10101, */
	"Asynchronous external abort",              /* 16 = 0b10110, */
	"INVALID ENTRY",                            /* 17 = 0b10111, */
	"Memory Access Synchronous Parity Error",   /* 18 = 0b11000, */
	"Memory Access Synchronous Parity Error-2", /* 19 = 0b11001, */
	"IMPLEMENTATION DEFINED Coprocessor Abort", /* 1a = 0b11010, */
	"INVALID ENTRY",                            /* 1b = 0b11011, */
	"Translation Table Walk 1st Level parity",  /* 1c = 0b11100, */
	"INVALID ENTRY",                            /* 1d = 0b11101, */
	"Translation Table Walk 2nd Level parity",  /* 1e = 0b11110, */
	"INVALID ENTRY",                            /* 1f = 0b11111, */
};

/**
 * @brief Prefetch abort fault strings
 */
static const char* prefetch_abt_fault_string[] =
{
	"INVALID ENTRY",                            /*  0 = 0b00000, */
	"INVALID ENTRY",                            /*  1 = 0b00001, */
	"Debug Event",                              /*  2 = 0b00010, */
	"Access Flag Fault: Section",               /*  3 = 0b00011, */
	"INVALID ENTRY",                            /*  4 = 0b00100, */
	"Translation Fault: Section",               /*  5 = 0b00101, */
	"Access Flag Fault: Page",                  /*  6 = 0b00110, */
	"Translation Fault: Page",                  /*  7 = 0b00111, */
	"Synchronous External Abort",               /*  8 = 0b01000, */
	"Domain Fault: Section",                    /*  9 = 0b01001, */
	"INVALID ENTRY",                            /*  a = 0b01010, */
	"Domain Fault: Page",                       /*  b = 0b01011, */
	"Translation Table Talk 1st Level sync abt",/*  c = 0b01100, */
	"Permission Fault: Section",                 /*  d = 0b01101, */
	"Translation Table Walk 2nd Level sync abt",/*  e = 0b01110, */
	"Permission Fault: Page",                   /*  f = 0b01111, */
	"INVALID ENTRY",                            /* 10 = 0b10000, */
	"INVALID ENTRY",                            /* 11 = 0b10001, */
	"INVALID ENTRY",                            /* 12 = 0b10010, */
	"INVALID ENTRY",                            /* 13 = 0b10011, */
	"IMPLEMENTATION DEFINED Lockdown",          /* 14 = 0b10100, */
	"INVALID ENTRY",                            /* 15 = 0b10101, */
	"INVALID ENTRY",                            /* 16 = 0b10110, */
	"INVALID ENTRY",                            /* 17 = 0b10111, */
	"INVALID ENTRY",                            /* 18 = 0b11000, */
	"Memory Access Synchronous Parity Error",   /* 19 = 0b11001, */
	"IMPLEMENTATION DEFINED Coprocessor Abort", /* 1a = 0b11010, */
	"INVALID ENTRY",                            /* 1b = 0b11011, */
	"Translation Table Walk 1st Level parity",  /* 1c = 0b11100, */
	"INVALID ENTRY",                            /* 1d = 0b11101, */
	"Translation Table Walk 2nd Level parity",  /* 1e = 0b11110, */
	"INVALID ENTRY",                            /* 1f = 0b11111, */
};


/**
 * @brief Print data abort information
 */
void print_data_abort()
{
	u32 dfsr = read_dfsr();
	u32 dfar = read_dfar();
	u32 fault_status = (dfsr & 0xF) | ((dfsr & 0x400) >> 6);

	sw_printk("SW: Data Abort Address: %08x\n", dfar);
	sw_printk("SW: Fault type: ");
	sw_printk((char*)data_abt_fault_string[fault_status]);
	sw_printk("SW:  (%x), domain %x, Write not Read: %x, External: %x\n",
			fault_status, ((dfsr & FSR_DOMAIN_MASK) >> 4), ((dfsr & FSR_WNR_MASK) >> 11), ((dfsr & FSR_EXT_MASK) >> 12 ));
}

/**
 * @brief Print prefetch abort information
 */
void print_prefetch_abort()
{
	u32 ifsr = read_ifsr();
	u32 ifar = read_ifar();
	u32 fault_status = (ifsr & 0xF) | ((ifsr & 0x400) >> 6);

	sw_printk("SW: Prefetch Abort Address: %08x\n", ifar);
	sw_printk("SW: Fault type: ");
	sw_printk((char*)prefetch_abt_fault_string[fault_status]);
	sw_printk("SW:  (%x),  External: %x\n", fault_status, ((ifsr & FSR_EXT_MASK) >> 12));
}

#ifdef CONFIG_SW_DEDICATED_TEE

/**
 * @brief 
 */
void smp_monitor_fiq_c_handler(void)
{
	int cpu_id = SW_SECONDARY_CORE;
	unsigned int interrupt;
	interrupt = gic_active_irq(0);

	if(interrupt != 1023 && interrupt != 1022) {
		gic_ack_irq(0, interrupt);
		if(interrupt == SEC_SGI_TO_SECONDARY_CORE) {
			valid_return_params_flag[cpu_id] = 1;
		}
	}

	asm volatile("MOV R0, #0\n\t");
	asm volatile("DSB");
}
#endif

/**
 * @brief FIQ 'C' handler
 */
void fiq_c_handler(void) 
{
	int cpu_id = SW_SECONDARY_CORE;
	int iter;
	unsigned int interrupt;
	interrupt = gic_active_irq(0);

	if(interrupt != 1023 && interrupt != 1022) {
		gic_ack_irq(0, interrupt);
#ifndef TIMER_NOT_DEFINED
		if((interrupt == FREE_RUNNING_TIMER_IRQ) ||
				(interrupt == TICK_TIMER_IRQ)) {

			secure_timer_irq_handler(interrupt);
		}
#endif

		if(interrupt == SEC_SGI_TO_PRIMARY_CORE){

			for(iter = 0 ; iter < PARAM_STACK_SIZE; iter++) {
				params_stack[iter] = params_smp_stack[cpu_id][iter];
			}
			valid_params_flag = 1;
		}

	}

	asm volatile("MOV R0, #0\n\t");
	asm volatile("DSB");
	/*  
	    if(interrupt != IRQ_TIMER2) {
	    do_context_switch();        
	    }
	 */
}

/**
 * @brief 
 */
#ifdef CONFIG_SW_DEDICATED_TEE

/**
 * @brief 
 */
void notify_smp_core(void)
{
	int iter;
	int cpu_id = SW_SECONDARY_CORE;
	for(iter = 0 ; iter < PARAM_OUT_STACK_SIZE; iter++) {
		params_out_smp_stack[cpu_id][iter] = params_out_stack[iter];
	}

	generate_soft_int_to_core0(SEC_SGI_TO_SECONDARY_CORE);
}
#endif

/**
 * @brief Data abort handler
 *
 * @param pc: PC of the data abort instruction
 */
void data_abort_handler(u32 pc)
{
	sw_printk("SW: data_abort_handler: dabt at @ pc %08x\n", pc);

	print_data_abort();
	u32 fault_status = (read_dfsr() & 0xF) | ((read_dfsr() & 0x400) >> 6);
	switch(fault_status) {
		case dfs_translation_section:
		case dfs_translation_page:
			{
				DIE_NOW(0, "Translation fault \n");
				break;
			}
		case dfs_alignment_fault:
		case dfs_debug_event:
		case dfs_access_flag_section:
		case dfs_icache_maintenance:
		case dfs_access_flag_page:
		case dfs_sync_external_abt:
		case dfs_domain_section:
		case dfs_domain_page:
		case dfs_translation_table_walk_lvl1_sync_ext_abt:
		case dfs_permission_section:
		case dfs_translation_table_walk_lvl2_sync_ext_abt:
		case dfs_permission_page:
		case dfs_imp_dep_lockdown:
		case dfs_async_external_abt:
		case dfs_mem_access_async_parity_err:
		case dfs_mem_access_async_parity_err2:
		case dfs_imp_dep_coprocessor_abort:
		case dfs_translation_table_walk_lvl1_sync_parity_err:
		case dfs_translation_table_walk_lvl2_sync_parity_err:
		default:
			sw_printk("SW: data_abort_handler: unimplemented data abort type.\n");
			print_data_abort();
			DIE_NOW(0, "Entering infinite loop\n");
			break;
	}

	DIE_NOW(0, "At end of data abort handler. Stopping\n");
}


/**
 * @brief Prefetch abor handler
 */
void prefetch_abort_handler(void)
{
	u32 ifsr = read_ifsr();
	u32 fault_status = (ifsr & 0xF) | ((ifsr & 0x400) >> 6);

	switch(fault_status) {
		case ifs_translation_fault_page:
		case ifs_debug_event:
		case ifs_access_flag_fault_section:
		case ifs_translation_fault_section:
		case ifs_access_flag_fault_page:
		case ifs_synchronous_external_abort:
		case ifs_domain_fault_section:
		case ifs_domain_fault_page:
		case ifs_translation_table_walk_lvl1_sync_ext_abt:
		case ifs_permission_fault_section:
		case ifs_translation_table_walk_lvl2_sync_ext_abt:
		case ifs_permission_fault_page:
		case ifs_imp_dep_lockdown:
		case ifs_memory_access_sync_parity_err:
		case ifs_imp_dep_coprocessor_abort:
		case ifs_translation_table_walk_lvl1_sync_parity_err:
		case ifs_translation_table_walk_lvl2_sync_parity_err:
		default:
			print_prefetch_abort();
			DIE_NOW(0, "Unimplemented prefetch abort.");
	}
}

/**
 * @brief Undefined handler 
 */
void undefined_handler(void)
{
	DIE_NOW(0, "undefined handler, Implement me!\n");
}

/**
 * @brief SWI handler
 *
 * 0xbbbb: Used to emulate timer
 * 0xcccc: IPI notification 
 * 
 * @param swi_id: SWI ID
 * @param regs: Pointer to the task context
 */
void swi_handler(int swi_id, struct swi_temp_regs *regs)
{
	/*    int i = 0; */

#ifdef CONFIG_KSPACE_PROTECTION
	int ret=0;
	struct list *l1;
	struct sw_file_operations *dev_info;
#endif

	switch(swi_id) {
		case 0xbbbb:
		case 0xcccc:
			/*
			   for(i = 0; i < 14; i++)
			   tz_printf("swi temp regs[%x] = 0x%x\n", i, regs[i]);
			 */
			temp_swi_regs = regs;
			scheduler();
			break;
#ifdef CONFIG_KSPACE_PROTECTION
		case SW_SYSCALL_EXE_SMC:
			__execute_smc(regs->regs[0]);
			break;
		case SW_SYSCALL_UNMAP_NS:
			ret = __unmap_from_ns(regs->regs[0]);
			regs->regs[0] = ret;
			break;
		case SW_SYSCALL_MAP_NS:
			ret = __map_to_ns(regs->regs[0],(va_t*)regs->regs[1]);
			regs->regs[0] = ret;
			break;
		case SW_SYSCALL_OPEN:
			list_for_each(l1, &sw_dev_head.dev_list){
				dev_info = list_entry(l1,struct sw_file_operations, head);
				if(sw_strcmp(dev_info->sw_dev_name,(char*)regs->regs[0]) == 0){
					if(dev_info->open != NULL)
						dev_info->open();
					break;
				}
			}
			break;
		case SW_SYSCALL_CLOSE:
			list_for_each(l1, &sw_dev_head.dev_list){
				dev_info = list_entry(l1,struct sw_file_operations, head);
				if(sw_strcmp(dev_info->sw_dev_name,(char*)regs->regs[0]) == 0){
					if(dev_info->close != NULL)
						dev_info->close();
					break;
				}
			}
			break;
		case SW_SYSCALL_WRITE:
			list_for_each(l1, &sw_dev_head.dev_list){
				dev_info = list_entry(l1,struct sw_file_operations, head);
				if(sw_strcmp(dev_info->sw_dev_name,(char*)regs->regs[0]) == 0){
					if(dev_info->write != NULL)
						dev_info->write(regs->regs[1]);
					break;
				}
			}
			break;
		case SW_SYSCALL_READ:
			list_for_each(l1, &sw_dev_head.dev_list){
				dev_info = list_entry(l1,struct sw_file_operations, head);
				if(sw_strcmp(dev_info->sw_dev_name,(char*)regs->regs[0]) == 0){
					if(dev_info->read != NULL)
						dev_info->read();
					break;
				}
			}
			break;
		case SW_SYSCALL_IOCTL:
			list_for_each(l1, &sw_dev_head.dev_list){
				dev_info = list_entry(l1,struct sw_file_operations, head);
				if(sw_strcmp(dev_info->sw_dev_name,(char*)regs->regs[0]) == 0){
					if(dev_info->ioctl != NULL)
						dev_info->ioctl();
					break;
				}
			}
			break;
		case SW_SYSCALL_USLEEP:
			sw_usleep((int)regs->regs[0]);
			break;
#if defined (CONFIG_SW_ELF_LOADER_SUPPORT)
		case SW_SYSCALL_ELF_LOAD:
			ret = elf_load((sa_config_t *)regs->regs[0]);
			regs->regs[0] = ret;
			break;
#endif
#if defined(CONFIG_GUI_SUPPORT) && defined(CONFIG_VE_BOARD)
		case SW_SYSCALL_FRAMEBUFFER:
			init_lcd((int)regs->regs[0], (int)regs->regs[1]);
			break;

#endif
#if defined(CONFIG_GUI_SUPPORT) 
		case SW_SYSCALL_FB_FLUSH:
			sw_flush_cache();
			break;
#endif
#endif
		default:
			break;    
	}


}


/**
 * @brief TZ API error handler
 *
 * @param 
 */
void smc_error_handler(void)
{

	sw_printk("SW Error: TZ API calls cannot be routed directly to second core!\n");
	while(1);
}
