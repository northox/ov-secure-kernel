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
 * SP804 timer declarations.
 */

#ifndef __SP804_TIMER_H__
#define __SP804_TIMER_H__

#include <sw_types.h>

#define SCCTRL          0x000
#define SCCTRL_TIMEREN0SEL_TIMCLK   (1 << 15)
#define SCCTRL_TIMEREN1SEL_TIMCLK   (1 << 17)
#define SCCTRL_TIMEREN2SEL_TIMCLK   (1 << 19)
#define SCCTRL_TIMEREN3SEL_TIMCLK   (1 << 21)

#define TIMER_LOAD      0x00
#define TIMER_VALUE     0x04
#define TIMER_CTRL      0x08
#define TIMER_CTRL_ONESHOT  (1 << 0)
#define TIMER_CTRL_32BIT    (1 << 1)
#define TIMER_CTRL_DIV1     (0 << 2)
#define TIMER_CTRL_DIV16    (1 << 2)
#define TIMER_CTRL_DIV256   (2 << 2)
#define TIMER_CTRL_IE       (1 << 5)    /* Interrupt Enable (versatile only) */
#define TIMER_CTRL_PERIODIC (1 << 6)
#define TIMER_CTRL_ENABLE   (1 << 7)

#define TIMER_INTCLR        0x0c
#define TIMER_RIS       0x10
#define TIMER_MIS       0x14
#define TIMER_BGLOAD        0x18

#define TIMER_PERIOD_US   1

#endif /* __VE_RS1_TIMER_H__ */

