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
 * Header for Trustzone based hypervisor configuration
 */

#ifndef TZHYP_CONFIG_H
#define TZHYP_CONFIG_H


#ifndef GUESTS_NO
#ifdef CONFIG_MULTI_GUESTS_SUPPORT
#define GUESTS_NO 2
#else
#define GUESTS_NO 1
#endif
#endif

#ifdef CONFIG_SW_MULTICORE
#ifdef CONFIG_MULTI_GUESTS_SUPPORT
#error "Multiple guests(Hypervisor) NOT supported in multicore mode"
#endif
#endif

#endif
