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

#ifndef __OTZ_COMMON_H_
#define __OTZ_COMMON_H_


#define OTZ_MAX_REQ_PARAMS  12
#define OTZ_MAX_RES_PARAMS  4
#define OTZ_1K_SIZE 1024

/**
 * @brief SMC return values
 */
/*enum otz_smc_ret {
    SMC_ENOMEM = -5,
    SMC_EOPNOTSUPP = -4,
    SMC_EINVAL_ADDR = -3,
    SMC_EINVAL_ARG = -2,
    SMC_ERROR = -1,
    SMC_INTERRUPTED = 1,
    SMC_PENDING = 2,
    SMC_SUCCESS = 0
};
*/


/**
 * @brief Command status 
 */
enum otz_cmd_status {
    OTZ_STATUS_INCOMPLETE = 0,
    OTZ_STATUS_COMPLETE,
    OTZ_STATUS_MAX  = 0x7FFFFFFF
};

/**
 * @brief Command type
 */
enum otz_cmd_type {
    OTZ_CMD_TYPE_INVALID = 0,
    OTZ_CMD_TYPE_NS_TO_SECURE,
    OTZ_CMD_TYPE_SECURE_TO_NS,
    OTZ_CMD_TYPE_SECURE_TO_SECURE,
    OTZ_CMD_TYPE_MAX  = 0x7FFFFFFF
};

/**
 * @brief Parameters type
 */
enum otzc_param_type {
    OTZC_PARAM_IN = 0,
    OTZC_PARAM_OUT
};

/**
 * @brief Shared memory for Notification
 */
struct otzc_notify_data {
    int dev_file_id;
    int service_id;
    int client_pid;
    int session_id;
    int enc_id;
};

/**
 * @brief Metadata used for encoding/decoding
 */
struct otzc_encode_meta {
    int type;
    int len;
    unsigned int usr_addr;
    int ret_len;
};

/**
 * @brief SMC command structure
 */
struct otz_smc_cmd {
    unsigned int    id;
    unsigned int    context;
    unsigned int    enc_id;
   
    unsigned int    src_id;
    unsigned int    src_context;

    unsigned int    req_buf_len;
    unsigned int    resp_buf_len;
    unsigned int    ret_resp_buf_len;
    unsigned int    cmd_status;
    unsigned int    req_buf_phys;
    unsigned int    resp_buf_phys;
    unsigned int    meta_data_phys;
    unsigned int    dev_file_id;
};

#endif /* __OTZ_COMMON_H_ */    
