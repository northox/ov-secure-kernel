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
 * Trustzone client driver defintions.
 */

#ifndef __OTZ_CLIENT_H_
#define __OTZ_CLIENT_H_

#define OTZ_CLIENT_FULL_PATH_DEV_NAME "/dev/otz_client"
#define OTZ_CLIENT_DEV "otz_client"

#define OTZ_CLIENT_IOC_MAGIC 0x775B777F /* "OTZ Client" */

#undef TDEBUG
#ifdef OTZ_DEBUG
#define TDEBUG(fmt, args...) printk(KERN_DEBUG "%s(%i, %s): " fmt "\n", \
        __func__, current->pid, current->comm, ## args)
#else
#define TDEBUG(fmt, args...)
#endif

#undef TERR
#define TERR(fmt, args...) printk(KERN_ERR "%s(%i, %s): " fmt "\n", \
        __func__, current->pid, current->comm, ## args)

/** IOCTL request */


/**
 * @brief Encode command structure
 */
struct otz_client_encode_cmd {
    unsigned int len;
    void* data;
    int   offset;
    int   flags;
    int   param_type;

    int encode_id;
    int service_id;
    int session_id;
    unsigned int cmd_id;    
};

/**
 * @brief Session details structure
 */
struct ser_ses_id{
    int service_id;
    int session_id;
};

/**
 * @brief Shared memory information for the session
 */
struct otz_session_shared_mem_info{
    int service_id;
    int session_id;
    unsigned int user_mem_addr;
};

/**
 * @brief Shared memory used for smc processing
 */
struct otz_smc_cdata {
    int cmd_addr;
    int ret_val;    
};

/* For general service */
#define OTZ_CLIENT_IOCTL_SEND_CMD_REQ \
    _IOWR(OTZ_CLIENT_IOC_MAGIC, 3, struct otz_client_encode_cmd)
#define OTZ_CLIENT_IOCTL_SES_OPEN_REQ \
    _IOW(OTZ_CLIENT_IOC_MAGIC, 4, struct ser_ses_id)
#define OTZ_CLIENT_IOCTL_SES_CLOSE_REQ \
    _IOWR(OTZ_CLIENT_IOC_MAGIC, 5, struct ser_ses_id)
#define OTZ_CLIENT_IOCTL_SHR_MEM_FREE_REQ \
    _IOWR(OTZ_CLIENT_IOC_MAGIC, 6, struct otz_session_shared_mem_info )

#define OTZ_CLIENT_IOCTL_ENC_UINT32 \
    _IOWR(OTZ_CLIENT_IOC_MAGIC, 7, struct otz_client_encode_cmd)
#define OTZ_CLIENT_IOCTL_ENC_ARRAY \
    _IOWR(OTZ_CLIENT_IOC_MAGIC, 8, struct otz_client_encode_cmd)
#define OTZ_CLIENT_IOCTL_ENC_ARRAY_SPACE \
    _IOWR(OTZ_CLIENT_IOC_MAGIC, 9, struct otz_client_encode_cmd)
#define OTZ_CLIENT_IOCTL_ENC_MEM_REF \
    _IOWR(OTZ_CLIENT_IOC_MAGIC, 10, struct otz_client_encode_cmd)

#define OTZ_CLIENT_IOCTL_DEC_UINT32 \
    _IOWR(OTZ_CLIENT_IOC_MAGIC, 11, struct otz_client_encode_cmd)
#define OTZ_CLIENT_IOCTL_DEC_ARRAY_SPACE \
    _IOWR(OTZ_CLIENT_IOC_MAGIC, 12, struct otz_client_encode_cmd)
#define OTZ_CLIENT_IOCTL_OPERATION_RELEASE \
    _IOWR(OTZ_CLIENT_IOC_MAGIC, 13, struct otz_client_encode_cmd)
#define OTZ_CLIENT_IOCTL_SHR_MEM_ALLOCATE_REQ \
    _IOWR(OTZ_CLIENT_IOC_MAGIC, 14, struct otz_session_shared_mem_info)
#define OTZ_CLIENT_IOCTL_GET_DECODE_TYPE \
    _IOWR(OTZ_CLIENT_IOC_MAGIC, 15, struct otz_client_encode_cmd)

#endif /* __OTZ_CLIENT_H_ */    
