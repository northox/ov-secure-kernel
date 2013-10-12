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
 * A dummy driver application
 */

#include <sw_debug.h>
#include <sw_modinit.h>

#define SW_TEST_DEV "sw_testdev1"

/**
 * @brief 
 */
static void sw_test_dev_open(void)
{
    sw_printf("SW:  TEST Device -1 open call\n");
    return;
}

/**
 * @brief 
 */
static void sw_test_dev_close(void)
{
    sw_printf("SW: TEST Device -1 close call\n");
    return;
}

/**
 * @brief 
 */
static struct sw_file_operations sw_test_dev_fops = {
    .open = sw_test_dev_open,
    .close = sw_test_dev_close,
    .sw_dev_name = SW_TEST_DEV
};

/**
 * @brief 
 *
 * @return 
 */
static void __init sw_test_dev_init(void)
{
    sw_device_register(&sw_test_dev_fops);
    return;
}

/**
 * @brief 
 */
static void sw_test_dev_exit(void)
{
    sw_device_unregister(&sw_test_dev_fops);
    return;
}

SW_MODULE_INIT(&sw_test_dev_init);
SW_MODULE_EXIT(&sw_test_dev_exit);
