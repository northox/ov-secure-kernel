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
 * Header for Helper functions for debugging
 *
 */

#ifndef __LIB__DEBUG_H__
#define __LIB__DEBUG_H__

#include <sw_types.h>

__attribute((noreturn)) void DIE_NOW(void * context, char* msg);

typedef char *va_list;

#define  ALIGNBND           (sizeof (signed int) - 1)
#define bnd(X, bnd)         (((sizeof (X)) + (bnd)) & (~(bnd)))
#define va_arg(ap, T) \
       (*(T *)(((ap) += (bnd (T, ALIGNBND))) - (bnd (T,ALIGNBND))))
#define va_end(ap)          (void) 0
#define va_start(ap, A)   \
  (void) ((ap) = (((char *) &(A)) + (bnd (A,ALIGNBND))))

#define LINE_FEED              0xA
#define CARRIAGE_RETURN        0xD
#define PRINTABLE_START        0x20
#define PRINTABLE_END          0x7E

/* output to serial */
/**
 * @brief 
 *
 * @param fmt
 * @param ...
 *
 * @return 
 */
u32 sw_printf(const char *fmt, ...);

/**
 * @brief 
 *
 * @param fmt
 * @param ...
 *
 * @return 
 */
u32 sw_printk(const char *fmt, ...);

/**
 * @brief 
 *
 * @param buf
 * @param fmt
 * @param args
 *
 * @return 
 */
u32 sw_vsprintf(char *buf, const char *fmt, va_list args);

/**
 * @brief 
 *
 * @param c
 *
 * @return 
 */
int printable_char(char c);

/**
 * @brief 
 *
 * @param context
 * @param msg
 */
void DIE_NOW(void* context, char* msg);

#endif
