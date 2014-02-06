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
 * Header for User Management implementation functons
 *
 */

#ifndef SW_USER_MGMT_H
#define SW_USER_MGMT_H

#include <task.h>

/**
 * @brief 
 *
 * @param d_id
 *
 * @return 
 */
int getPermission(int d_id);

/**
 * @brief 
 *
 * @param new_task
 *
 * @return 
 */
int setPermission(struct sw_task *new_task);

/**
 * @brief 
 *
 * @param d_id
 * @param g_id
 * @param u_id
 */
void updateGlobalACL(int d_id, int g_id, int u_id);
#endif
