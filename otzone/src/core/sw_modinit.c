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
 * module initialization implementation functions
 */

#include <sw_types.h>
#include <cpu_data.h>
#include <sw_modinit.h>
#include <sw_syscall.h>
#include <uart.h>

/**
 * @brief 
 *      Initializes the list head which will hold the list of registered devices
 */
static void sw_dev_head_init(void)
{
    INIT_LIST_HEAD(&sw_dev_head.dev_list);
}

/**
 * @brief 
 *      This function is called by each module for registration
 *
 * @param sw_dev
 *      File operations structure of the device
 */
void sw_device_register(struct sw_file_operations* sw_dev)
{
    list_add(&sw_dev_head.dev_list, &sw_dev->head);    
}

/**
 * @brief 
 *      This function is called by each module to unregister them 
 * @param sw_dev
 *      File operations structure of the device
 */

void sw_device_unregister(struct sw_file_operations* sw_dev)
{
    list_del(&sw_dev->head);    
}

/**
 * @brief 
 *  open system call for devices
 */
void sw_open(char *dev_name)
{
    if(dev_name == NULL){
        serial_puts("\n\rSW: dev_name is null.\r\n");
        return;
    }
#ifdef CONFIG_KSPACE_PROTECTION
    __sw_open();
#endif
    return;
}

/**
 * @brief 
 *  close system call for devices
 */
void sw_close(char *dev_name)
{
    if(dev_name == NULL){
        serial_puts("\n\rSW: dev_name is null.\r\n");
        return;
    }
#ifdef CONFIG_KSPACE_PROTECTION
    __sw_close();
#endif
    return;
}


/**
 * @brief 
 *  read system call for devices
 */
void sw_read(void)
{
#ifdef CONFIG_KSPACE_PROTECTION
    __sw_read();
#endif
    return;
}

/**
 * @brief 
 *  write system call for devices
 */
void sw_write()
{
#ifdef CONFIG_KSPACE_PROTECTION
    __sw_write();
#endif
    return;
}

/**
 * @brief 
 *  ioctl system call for devices
 */
void sw_ioctl(void)
{
#ifdef CONFIG_KSPACE_PROTECTION
    __sw_ioctl();
#endif
    return;
}

/**
 * @brief 
 *  This function calls the initialization function of all the modules
 */
#ifdef CONFIG_KSPACE_PROTECTION
void modules_init(void)
{
    sw_dev_head_init();
    mod_init_fn_call init_fn;
    u32* init_fn_addr, init_start_addr, init_end_addr;
    init_start_addr = get_mod_init_start_addr();
    init_end_addr = get_mod_init_end_addr();
    for(init_fn_addr = init_start_addr; init_fn_addr < init_end_addr; init_fn_addr++){
    init_fn = *(init_fn_addr);
    if(init_fn != NULL){
        init_fn();
    }
   }
}
#endif
