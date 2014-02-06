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
 * Trustzone based hypervisor device implementation
 */

/*
 * Support functions for implementing (Trustzone based)hypervisor
 * running in monitor mode
 */

#include <sw_debug.h>
#include <system_context.h>
#include <tzhyp.h>
#include <tzhyp_global.h>
#include <gic.h>

/**
 * @brief 
 *
 * @param cur
 * @param next
 */
void tzhyp_device_switch(struct system_context *cur, 
			 struct system_context *next)
{
	if (get_cpu_id() == 0) {
		/* Save and restore gic state */
		gic_dist_save(&cur->sysctxt_gic);
		gic_dist_restore(&next->sysctxt_gic);
	}
}


/**
* @brief 
*/
void tzhyp_device_context_init(void)
{
	struct system_context *primary_ns_world;
	struct gic_context *x;

	primary_ns_world = (struct system_context *)
		GET_CORE_CONTEXT_BYID(ns_world, 0);
	x = &primary_ns_world->sysctxt_gic;

	gic_dist_save(x);
}
