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
 * Echo application defintions. 
 *
 */

#ifndef __OTZ_APP_EG_H
#define __OTZ_APP_EG_H

#define DATA_BUF_LEN 1024
#define CRYPT_BUF_LEN 256
#define DRM_BUFFER_SIZE 1024

/**
 * @brief 
 */
struct our_data_buffer {
    int len;
    char data[DATA_BUF_LEN];
    char response[DATA_BUF_LEN];
};

/**
 * @brief 
 */
struct drm_data_buffer {
    int len;
    char data[DRM_BUFFER_SIZE];
    char response[DRM_BUFFER_SIZE];
};

/**
 * @brief 
 */
struct crypt_data_buffer {
    int len;
    char data[CRYPT_BUF_LEN];
    char response[CRYPT_BUF_LEN];
};

/**
 * @brief 
 */
typedef struct our_data_buffer echo_data_t;
/**
 * @brief 
 */
typedef struct drm_data_buffer drm_data_t;
/**
 * @brief 
 */
typedef struct crypt_data_buffer crypto_data_t;
/**
 * @brief 
 */
typedef struct our_data_buffer otz_mutex_test_data_t;


#endif

