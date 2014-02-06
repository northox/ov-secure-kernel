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
 * secure boot implementation function
 */

/* main entry function */
#include <sw_types.h>
#include <sw_debug.h>
#include <sw_board.h>
#include <cpu.h>
#include <uart.h>
#include <mmc.h>
#include <block_dev.h>
#include <sw_io.h>

volatile u8* g_buffer = (u8*)SECURE_OS_ENTRY_ADDR;

/**
 * @brief 
 *
 * @return 
 */
int secure_boot(void)
{
	struct mmc *mmc;
	int blocks = SECURE_OS_SIZE/512;
    serial_init(0);
    serial_puts("\r\nSW: Entering Secure Boot \r\n");

	enable_l1_cache();	
	if(mmc_initialize()) {
		DIE_NOW(0, "mmc initialize failed");
	}

	int k = 0, ret_blkcnt; 
	mmc = find_mmc_device(0);
	block_dev_desc_t *block_dev = &mmc->block_dev;
	emmc_boot_open(mmc);
	for (k = 0; k < blocks; k++) {
		ret_blkcnt = block_dev->block_read(block_dev->dev, 
			k+SECURE_OS_MMC_START_SECTOR, 1, 
			(void*)((u32)g_buffer + k * 512));

		if(ret_blkcnt != 1) {
			sw_printf("error in reading block 0x%x\n", k);
		}
#ifdef SW_BL_DBG
		if( k == 0 || k == blocks -1) {
			for(i=0; i<32; i++){
				sw_printf(" %02x |", *(g_buffer + k * 512 + i));
				if(j++ == 4){
					j=0;
					sw_printf("!\n");
				}
			}
			sw_printf("!\n");
		}
#endif
	}
	emmc_boot_close(mmc);
	sw_printf("jump to secure OS\n");
	jump_to();	    

	while(1);

    return OTZ_OK;
}
