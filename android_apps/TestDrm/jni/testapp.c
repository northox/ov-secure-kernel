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
 * Linux trustzone example application.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>


#define SAMSUNG_MFC_DEV_NAME           "/dev/s3c-mfc"

#define IOCTL_MFC_DEC_INIT              (0x00800001)
#define IOCTL_MFC_ENC_INIT              (0x00800002)
#define IOCTL_MFC_DEC_EXE               (0x00800003)
#define IOCTL_MFC_ENC_EXE               (0x00800004)

#define IOCTL_MFC_GET_IN_BUF            (0x00800010)
#define IOCTL_MFC_FREE_BUF              (0x00800011)
#define IOCTL_MFC_GET_REAL_ADDR         (0x00800012)
#define IOCTL_MFC_GET_MMAP_SIZE         (0x00800014)
#define IOCTL_MFC_SET_IN_BUF            (0x00800018)

#define IOCTL_MFC_SET_CONFIG            (0x00800101)
#define IOCTL_MFC_GET_CONFIG            (0x00800102)

#define IOCTL_MFC_SET_BUF_CACHE         (0x00800201)

#define IOCTL_MFC_SET_DBP_REG			(0x00800301)

enum mfc_ret_code {
    MFC_OK = 1,
    MFC_FAIL = -1000,
    MFC_OPEN_FAIL = -1001,
    MFC_CLOSE_FAIL = -1002,

    MFC_DEC_INIT_FAIL = -2000,
    MFC_DEC_EXE_TIME_OUT = -2001,
    MFC_DEC_EXE_ERR = -2002,
    MFC_DEC_GET_INBUF_FAIL = 2003,
    MFC_DEC_SET_INBUF_FAIL = 2004,
    MFC_DEC_GET_OUTBUF_FAIL = -2005,
    MFC_DEC_GET_CONF_FAIL = -2006,
    MFC_DEC_SET_CONF_FAIL = -2007,

    MFC_ENC_INIT_FAIL = -3000,
    MFC_ENC_EXE_TIME_OUT = -3001,
    MFC_ENC_EXE_ERR = -3002,
    MFC_ENC_GET_INBUF_FAIL = -3003,
    MFC_ENC_SET_INBUF_FAIL = -3004,
    MFC_ENC_GET_OUTBUF_FAIL = -3005,
    MFC_ENC_SET_OUTBUF_FAIL = -3006,
    MFC_ENC_GET_CONF_FAIL = -3007,
    MFC_ENC_SET_CONF_FAIL = -3008,

    MFC_STATE_INVALID = -4000,
    MFC_DEC_HEADER_FAIL = -4001,
    MFC_DEC_INIT_BUF_FAIL = -4002,
    MFC_ENC_HEADER_FAIL = -5000,
    MFC_ENC_PARAM_FAIL = -5001,
    MFC_FRM_BUF_SIZE_FAIL = -6000,
    MFC_FW_LOAD_FAIL = -6001,
    MFC_FW_INIT_FAIL = -6002,
    MFC_INST_NUM_EXCEEDED_FAIL = -6003,
    MFC_MEM_ALLOC_FAIL = -6004,
    MFC_MEM_INVALID_ADDR_FAIL = -6005,
    MFC_MEM_MAPPING_FAIL = -6006,
    MFC_GET_CONF_FAIL = -6007,
    MFC_SET_CONF_FAIL = -6008,
    MFC_INVALID_PARAM_FAIL = -6009,
    MFC_API_FAIL = -9000,

    MFC_CMD_FAIL = -1003,
    MFC_SLEEP_FAIL = -1010,
    MFC_WAKEUP_FAIL = -1020,

    MFC_CLK_ON_FAIL = -1030,
    MFC_CLK_OFF_FAIL = -1030,
    MFC_PWR_ON_FAIL = -1040,
    MFC_PWR_OFF_FAIL = -1041,
} ;

struct mfc_get_real_addr_arg {
    unsigned int key;
    unsigned int addr;
};


struct mfc_set_dpb_regs {
	unsigned int chroma_idx;
	unsigned int luma_idx;
	unsigned int chroma_addr;
	unsigned int luma_addr;
};

union mfc_args {
	unsigned int mapped_size;
	struct mfc_get_real_addr_arg real_addr;
	struct mfc_set_dpb_regs dpb_regs;
};


struct mfc_common_args {
    enum mfc_ret_code ret_code;	/* [OUT] error code */
    union mfc_args args;
};

static char *mfc_dev_name = SAMSUNG_MFC_DEV_NAME;


#define MIN_H264_DPB 6

void main(int argc, char* argv[]) 
{
    int hMFCOpen;
    unsigned int mapped_addr, tmp_addr;
	unsigned int chroma_addr, luma_addr;
    int mapped_size;
    struct mfc_common_args CommonArg;
	struct mfc_common_args user_addr_arg, phys_addr_arg, dpb_regs;
	int dpb = MIN_H264_DPB, i, ret_code;

    hMFCOpen = open(mfc_dev_name, O_RDWR | O_NDELAY);
    if (hMFCOpen < 0) {
        printf("SsbSipMfcDecOpen] MFC Open failure\n");
        return;
    }

	printf("device opened\n");

    mapped_size = ioctl(hMFCOpen, IOCTL_MFC_GET_MMAP_SIZE, &CommonArg);
    if ((mapped_size < 0) || (CommonArg.ret_code != MFC_OK)) {
        printf("SsbSipMfcDecOpen] IOCTL_MFC_GET_MMAP_SIZE failed\n");
        close(hMFCOpen);
        return;
    }

	printf("device get mmap size success\n");

    mapped_addr = (unsigned int)mmap(0, mapped_size, PROT_READ | PROT_WRITE, MAP_SHARED, hMFCOpen, 0);
    if (!mapped_addr) {
        printf("SsbSipMfcDecOpen] FIMV5.x driver address mapping failed\n");
        close(hMFCOpen);
        return;
    }

	printf("device mmap success\n");

	tmp_addr = 0;
	for (i = 0; i < dpb; i++) {
		luma_addr = 0;

		tmp_addr += 2048;

		chroma_addr = tmp_addr;
 
		dpb_regs.args.dpb_regs.chroma_idx = i;
		dpb_regs.args.dpb_regs.luma_idx = i;
		dpb_regs.args.dpb_regs.chroma_addr = chroma_addr;
		dpb_regs.args.dpb_regs.luma_addr = luma_addr;
		
		ret_code = ioctl(hMFCOpen, IOCTL_MFC_SET_DBP_REG, &dpb_regs);
		if (ret_code < 0) {
		    printf("SsbSipMfcDecGetInBuf] IOCTL_MFC_SET_DBP_REG failed");
		    return;
		}

		tmp_addr += 2048;
	}

	memset(mapped_addr, 0x80, (tmp_addr - mapped_addr));
	printf("mapped addr %x and size %d\n", mapped_addr, mapped_size);
    munmap((void*)mapped_addr, mapped_size);
    close(hMFCOpen);
	return;
}
