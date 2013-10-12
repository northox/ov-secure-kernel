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
 * Header for sw_syscalls implementation
 */

#ifndef __SW_SYSCALL_H_
#define __SW_SYSCALL_H_

/**
 * @brief 
 *     Device open system call. 
 * @param dev_name
 */
void __sw_open();

/**
 * @brief 
 *      Device close system call
 * @param dev_name
 */
void __sw_close();

/**
 * @brief 
 *      Device read system call
 * @param dev_name
 */
void __sw_read(void);

/**
 * @brief 
 *      Device write system call
 * @param dev_name
 */
void __sw_write();

/**
 * @brief 
 *      Device ioctl system call
 * @param dev_name
 */
void __sw_ioctl(void);

/**
 * @brief 
 *      Execute smc system call
 */
void __asm_execute_smc(void);

/**
 * @brief 
 *     system call to  map a address as non-secure entry
 * @param phy_addr
 * @param va_addr
 *
 * @return 
 */
int __asm_map_to_ns(pa_t phy_addr, va_t *va_addr);

/**
 * @brief 
 *     system call to Unmap a non-secure address 
 *
 * @param va_addr
 *
 * @return 
 */
int __asm_unmap_from_ns(va_t va_addr);

/**
 * @brief 
 *     system call to usleep
 *
 * @param va_addr
 *
 * @return 
 */
int __sw_usleep(int seconds);


/**
 * @brief 
 *     system call to load a elf
 *
 * @param filename
 * @param sa_config
 *
 * @return 
 */
int __elf_load( void *sa_config);

#if defined(CONFIG_GUI_SUPPORT) && defined(CONFIG_VE_BOARD)
/**
 * @brief 
 *     system call to usleep
 *
 * @param va_addr
 *
 * @return 
 */


int __init_lcd(int width, int height);
#endif

#if defined(CONFIG_GUI_SUPPORT) 
/**
 * @brief 
 *     system call to flush the frame buffer
*/
void __sw_fb_flush(void);
#endif

#endif
