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
 * A UART driver ## Have to be merged with orginal board specific uart driver.
 */

#include <uart.h>
#include <sw_modinit.h>
#include <sw_debug.h>
#include <sw_user_mgmt.h>
#include <otz_id.h>

#define DEVICE "UART"
#define DEVICE_ID 10001

/**
 * @brief 
 */
static void uart_dev_open(void)
{
    serial_puts("SW: UART is implicit open. Use read and write.\n");
    return;
}

/**
 * @brief 
 */
static void uart_dev_close(void)
{
    serial_puts("SW: UART cannot be closed.\n");
    return;
}

/**
 * @brief 
 */
static void uart_dev_read(void)
{
    serial_puts("SW: UART read\n");
    return;
}

/**
 * @brief 
 *
 * @param print_buffer
 */
static void uart_dev_write(char *print_buffer)
{
    if(getPermission(DEVICE_ID) == 1){
        serial_puts(print_buffer);
    }else{
        serial_puts("\n\rSW: No Permission to access UART device \r\n");
    }
    return;
}

/**
 * @brief 
 */
static void uart_dev_ioctl(void)
{
    serial_puts("SW: UART ioctl\n");
    return;
}
/**
 * @brief 
 */
static struct sw_file_operations uart_dev_fops = {
    .open = uart_dev_open,
    .close = uart_dev_close,
    .read = uart_dev_read,
    .write = uart_dev_write,
    .ioctl = uart_dev_ioctl,
    .sw_dev_name = DEVICE,
    .dev_id = DEVICE_ID
};

/**
 * @brief 
 *
 * @return 
 */
static void __init uart_dev_init(void)
{
    sw_device_register(&uart_dev_fops);
    updateGlobalACL(DEVICE_ID,1,OTZ_SVC_GLOBAL);
    updateGlobalACL(DEVICE_ID,1,OTZ_SVC_ECHO);
    updateGlobalACL(DEVICE_ID,1,OTZ_SVC_DRM);
    updateGlobalACL(DEVICE_ID,1,OTZ_SVC_CRYPT);
    updateGlobalACL(DEVICE_ID,1,OTZ_SVC_MUTEX_TEST);
    updateGlobalACL(DEVICE_ID,1,OTZ_SVC_VIRTUAL_KEYBOARD);
    serial_puts("\n\rSW: UART driver initialized successfully\r\n");
    return;
}

/**
 * @brief 
 */
static void uart_dev_exit(void)
{
    sw_device_unregister(&uart_dev_fops);
    return;
}

SW_MODULE_INIT(&uart_dev_init);
