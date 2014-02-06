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
 * secure main function
 */

/* main entry function */
#include <sw_types.h>
#include <monitor.h>
#include <cpu.h>
#include <cpu_mmu.h>
#include <page_table.h>
#include <uart.h>
#include <gic.h>
#ifdef CONFIG_SCU
#include <scu.h>
#endif
#include <sw_mem_functions.h>
#include <sw_string_functions.h>
#include <sw_debug.h>
#include <mem_mng.h>

#include <sw_board.h>

#include <sw_list.h>
#include <global.h>
#include <cpu_data.h>
#include <task.h>

#include <dispatcher_task.h>

#include <otz_id.h>
#include <secure_api.h>
#include <sw_modinit.h>

#include <sw_timer.h>
#include <cache.h>

#include <tzhyp.h>

#include <gp_internal_api_test.h>
#include <fat32.h>
#include <diskio.h>
#ifdef CONFIG_NEON_SUPPORT
#include "neon_app.h"
#endif

extern u32 kernel;
u32* pagetable_addr;

/**
 * @brief 
 *
 * @return 
 */
int secure_main(void)
{
    sa_config_t sa_config;
    int dispatch_task_id,ret_val,fs_ret;

    serial_init(0);
    serial_puts("\r\nSW: Entering Secure Main \r\n");
    
#if 1
    pagetable_addr = sw_meminfo_init();
    sw_malloc_int();
    global_init();
    
    board_init();

    cpu_mmu_enable();

    inv_branch_predict();
    enable_branch_prediction();
#endif  

#ifdef CONFIG_SCU
    scu_init();
#endif

    gic_dist_init();
    gic_cpu_init();

#ifdef CONFIG_KSPACE_PROTECTION
    modules_init();
    unmap_init_section();
#endif

#ifndef CONFIG_EMULATE_FIQ
    arm_irq_enable();
#endif
    init_sw_timer();
    timer_init();

#ifndef CONFIG_EMULATE_FIQ
    enable_timer(); 
#endif
#ifdef CONFIG_FILESYSTEM_SUPPORT
#ifdef CONFIG_MMC_SUPPORT
    fs_ret = mount_file_system(read_from_disk());
#else
    fs_ret = mount_file_system(get_sw_fs_start());
#endif
    sw_printf("the mount file system is 32 and the value is %x \n",fs_ret);
    if(fs_ret!=-1) {
    sw_printf("file system successfully mounted in FAT32 \n");
	}
#endif
#ifdef CONFIG_NEON_SUPPORT
    test_neon_app();
#endif
#ifdef CONFIG_GP_TEST
	test_gp_internal_apis();
#endif

#ifdef CONFIG_SW_MULTICORE
    flush_cache_all();
    dsb();
    isb();
    start_secondary_core();
#endif   

    tzhyp_init();

    ret_val = sa_create_entry_point(OTZ_SVC_GLOBAL, &sa_config);
    if(ret_val == OTZ_OK)
    {
        if(sa_open_session(&sa_config, (void*)&dispatch_task_id) == OTZ_OK) {
            start_task(dispatch_task_id, NULL);
        }
    }
    schedule();

    while(1);

    return OTZ_OK;
}

