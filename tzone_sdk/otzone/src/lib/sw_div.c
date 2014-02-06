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
 *  Div routines
 */

#include "sw_debug.h"
#include "sw_types.h"

/** These division/modulus routines are NOT to be called directly, instead
   they are here to allow the use of the / and % C operators cleanly in the 
   rest of the codebase. */

/** Support functions for division routines */

/**
 * @brief 
 * Check if a uint is a power of two.
 * If so, return the power value.
 * If not a power, returns -1 
 *
 * @param n
 *
 * @return 
 */
static int is_power_of2(u32 n)
{
  return (n && !(n & (n - 1)));
}

/**
 * @brief 
 * Assuming n is a power of 2, find the power 
 *
 * @param n
 *
 * @return 
 */
static int get_power_of2(u32 n)
{
  int i;
  for (i = 0; i < sizeof(u32)*8; i++)
  {
    if (n & 0x1)
    {
      return i;
    }
    else
    {
      n = n >> 1;
    }
  }

  //if we get here, then n was 0. return error
  return -1;
}

/* End support functions */

/**
 * @brief 
 *
 * @param dividend
 * @param divisor
 * @param acc
 *
 * @return 
 */
static u32 __aeabi_uidiv_recursive(u32 dividend, u32 divisor, u32 acc);

/**
 * @brief 
 *
 * @param dividend
 * @param divisor
 * @param acc
 *
 * @return 
 */
static u64 __aeabi_uldivmod_recursive(u64 dividend, u64 divisor, u64 acc);

/**
 * @brief 
 *
 * @param dividend
 * @param divisor
 * @param acc
 *
 * @return 
 */
static u32 __aeabi_idivmod_recursive(u32 dividend, u32 divisor, u32 acc);

/** ARMEABI defined division routines */

/**
 * @brief 
 * Simple unsigned int division with minimal optimizations.
 * For best results, use a tail-call optimizing seasoning.
 *
 * @param dividend
 * @param divisor
 *
 * @return 
 */
u32 __aeabi_uidiv(u32 dividend, u32 divisor)
{
  if (divisor == 0)
  {
    DIE_NOW(0, "Divide by 0!");
  }
  
  if (dividend < divisor)
  {
    return 0;
  }
  
  /* check if divisor is a power of 2 */
  if (is_power_of2(divisor))
  {
    return dividend >> get_power_of2(divisor);
  }
  return __aeabi_uidiv_recursive(dividend, divisor, 0);
}

/**
 * @brief 
 * Recursive case for uidiv 
 *
 * @param dividend
 * @param divisor
 * @param acc
 *
 * @return 
 */
static u32 __aeabi_uidiv_recursive(u32 dividend, u32 divisor, u32 acc)
{
  if (dividend < divisor)
  {
    return acc;
  }

  u32 res = 1, tmp = divisor;
  
  while (dividend > tmp)
  {
    /* we need to be careful here that we don't shift past the last bit */
    if (tmp & (1 << (sizeof(u32)*8-1)))
    {
      break;
    }

    tmp = tmp << 1;
    if (tmp > dividend)
    {
      tmp = tmp >> 1; //shift it back for later
      break;
    }
    res *= 2;
  }

  return __aeabi_uidiv_recursive(dividend - tmp, divisor, acc + res);
}

/**
 * @brief 
 * The layout of this structure must not be altered. 
 * The return type and member order is defined by
 * the ARM eabi. Quotient is returned in r0-r1 and remainder in r2-r3. 
 */
typedef struct
{
  u64 quot;
  u64 rem;
} ulldiv_t;

/**
 * @brief 
 * Does 64 bit unsigned int division, returning a struct
 * containing the quotient and remainder 
 *
 * @param dividend
 * @param divisor
 *
 * @return 
 */
ulldiv_t __aeabi_uldivmod(u64 dividend, u64 divisor)
{
  ulldiv_t ret;

  if (divisor == 0)
  {
    DIE_NOW(0, "Divide by 0!");
  }

  if (dividend < divisor)
  {
    ret.quot = 0;
    ret.rem = dividend;
    return ret;
  }

  /* check if divisor is a power of 2 */
  if (is_power_of2(divisor))
  {
    // this func call just returns a number of bits, no need for ull
    ret.quot = dividend >> get_power_of2(divisor);
    ret.rem = dividend - ret.quot * divisor;
    return ret;
  }

  ret.quot = __aeabi_uldivmod_recursive(dividend, divisor, 0);
  ret.rem = dividend - ret.quot * divisor;
  return ret;
}

/**
 * @brief 
 * recursive case returning just the quotient 
 *
 * @param dividend
 * @param divisor
 * @param acc
 *
 * @return 
 */
static u64 __aeabi_uldivmod_recursive(u64 dividend, u64 divisor, u64 acc)
{
  if (dividend < divisor)
  {
    return acc;
  }

  u64 res = 1, tmp = divisor;

  while (dividend > tmp)
  {
    /* we need to be careful here that we don't shift past the last bit */
    if (tmp & (1ULL << (sizeof(u64)*8-1)))
    {
      break;
    }

    tmp = tmp << 1;
    if (tmp > dividend)
    {
      tmp = tmp >> 1; //shift it back for later
      break;
    }
    res *= 2;
  }

  return __aeabi_uldivmod_recursive(dividend - tmp, divisor, acc + res);
}


/**
* @brief 
*
* @param dividend
* @param divisor
* @param acc
*
* @return 
*/
static u32 __aeabi_idivmod_recursive(u32 dividend, u32 divisor, u32 acc)
{
    u32 res,tmp;
    if (dividend < divisor) {
        return(dividend);
    } 
    res = 1;
    tmp = divisor;
    while (dividend > tmp) {
        /* we need to be careful here that we don't shift past the last bit */
        if (tmp & (1 << (sizeof(u32)*8-1))) {
            break;
        } 
        tmp = tmp << 1;
        if (tmp > dividend) {
            tmp = tmp >> 1; //shift it back for later
            break;
        }
        res *= 2;
    } 
    return __aeabi_idivmod_recursive(dividend - tmp, divisor, acc + res);
}

/**
* @brief 
*
* @param dividend
* @param divisor
*
* @return 
*/
u32 __aeabi_idivmod(u32 dividend, u32 divisor)
{
    if (divisor == 0) {
        DIE_NOW(0, "Divide by 0!");
    }
    if(dividend < divisor) {
        return(dividend);
    }
    if(is_power_of2(divisor)) {
        return(dividend & (divisor - 1));
    }
    return(__aeabi_idivmod_recursive(dividend,divisor,0));
}

