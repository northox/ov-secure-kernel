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
 * ARITHMETIC API implementation
 *
 */
#ifndef OTZ_TEE_ARITH_API_H
#define OTZ_TEE_ARITH_API_H

#include "sw_types.h"
#include "otz_tee_common.h"

#define TEE_BigIntNumberOfBits 2048
#define TEE_BigIntSizeInU32(n) (((n+31)/32)+2)


typedef enum __TEE_Result TEE_Result;

//typedef unsigned int u32;
//Arithmetic Data Types
typedef u32 TEE_BigInt;
typedef u32 TEE_BigIntFMM;
typedef u32 TEE_BigIntFMMContext;

//Memory allocations and sizeof() functions

/**
* @brief 
*
* @param ModuleSizeInBits
*
* @return 
*/
size_t TEE_BigIntFMMContextSizeInU32(size_t ModuleSizeInBits);

/**
* @brief 
*
* @param bigInt
* @param len
*
* @return 
*/
TEE_Result TEE_BigIntInit(TEE_BigInt *bigInt, size_t len);

/**
* @brief 
*
* @param context
* @param len
* @param modulus
*
* @return 
*/
TEE_Result TEE_BigIntFMMContextInit(TEE_BigIntFMMContext *context, size_t len, TEE_BigInt *modulus);

/**
* @brief 
*
* @param bigIntFMM
* @param len
*
* @return 
*/
TEE_Result TEE_BigIntFMMInit(TEE_BigIntFMM* bigIntFMM, size_t len);
//Convertion Functions

/**
* @brief 
*
* @param dest
* @param buffer
* @param bufferLen
* @param sign
*
* @return 
*/
TEE_Result TEE_BigIntSetFromOctetString(TEE_BigInt *dest, u32 *buffer, size_t bufferLen, int sign);

/**
* @brief 
*
* @param buffer
* @param bufferLen
* @param bigInt
*
* @return 
*/
TEE_Result TEE_BigIntConvertToOctetString(void* buffer, size_t *bufferLen, TEE_BigInt* bigInt);

/**
* @brief 
*
* @param dest
* @param shortVal
*/
void TEE_BigIntConvertFromS32(TEE_BigInt* dest, int shortVal);

/**
* @brief 
*
* @param dest
* @param src
*
* @return 
*/
TEE_Result TEE_BigIntConvertToS32(int* dest, TEE_BigInt* src);

/**
* @brief 
*
* @param op1
* @param op2
*
* @return 
*/
int TEE_BigIntCmp(TEE_BigInt* op1, TEE_BigInt* op2);

/**
* @brief 
*
* @param op1
* @param shortVal
*
* @return 
*/
int TEE_BigIntCmpS32(TEE_BigInt* op1, int shortVal);

/**
* @brief 
*
* @param dest
* @param src
* @param bits
*/
void TEE_BigIntRightShift(TEE_BigInt* dest, TEE_BigInt* src, size_t bits);

/**
* @brief 
*
* @param src
* @param bitIndex
*
* @return 
*/
int TEE_BigIntGetBit(TEE_BigInt* src, u32 bitIndex);

/**
* @brief 
*
* @param dest
* @param op1
* @param op2
*/
void TEE_BigIntAdd(TEE_BigInt* dest, TEE_BigInt* op1, TEE_BigInt* op2);

/**
* @brief 
*
* @param dest
* @param op1
* @param op2
*/
void TEE_BigIntSub(TEE_BigInt* dest, TEE_BigInt* op1, TEE_BigInt* op2);

/**
* @brief 
*
* @param dest
* @param src
*/
void TEE_BigIntNeg(TEE_BigInt* dest, TEE_BigInt* src);

/**
* @brief 
*
* @param dest
* @param op1
* @param src
*/
void TEE_BigIntMult(TEE_BigInt* dest, TEE_BigInt* op1, TEE_BigInt* src);

/**
* @brief 
*
* @param dest
* @param src
*/
void TEE_BigIntSquare(TEE_BigInt* dest, TEE_BigInt* src);

/**
* @brief 
*
* @param dest_q
* @param dest_r
* @param op1
* @param op2
*/
void TEE_BigIntDivide(TEE_BigInt* dest_q, TEE_BigInt* dest_r, TEE_BigInt* op1, TEE_BigInt* op2);

/**
* @brief 
*
* @param dest
* @param op
* @param n
*/
void TEE_BigIntReduceMod(TEE_BigInt* dest, TEE_BigInt* op, TEE_BigInt* n);

/**
* @brief 
*
* @param dest
* @param op1
* @param op2
* @param n
*/
void TEE_BigIntAddMod(TEE_BigInt* dest, TEE_BigInt* op1, TEE_BigInt* op2, TEE_BigInt* n);

/**
* @brief 
*
* @param dest
* @param op1
* @param op2
* @param n
*/
void TEE_BigIntSubMod(TEE_BigInt* dest, TEE_BigInt* op1, TEE_BigInt* op2, TEE_BigInt* n);

/**
* @brief 
*
* @param dest
* @param op1
* @param src
* @param n
*/
void TEE_BigIntMultMod(TEE_BigInt* dest, TEE_BigInt* op1, TEE_BigInt* src, TEE_BigInt* n);

/**
* @brief 
*
* @param dest
* @param src
* @param n
*/
void TEE_BigIntSquareMod(TEE_BigInt* dest, TEE_BigInt* src, TEE_BigInt* n);

/**
* @brief 
*
* @param dest
* @param op
* @param n
*/
void TEE_BigIntInvMod(TEE_BigInt* dest, TEE_BigInt* op, TEE_BigInt* n);

/**
* @brief 
*
* @param op1
* @param op2
*
* @return 
*/
int TEE_BigIntRelativePrime(TEE_BigInt* op1, TEE_BigInt* op2);

/**
* @brief 
*
* @param gcd
* @param u
* @param v
* @param op1
* @param op2
*/
void TEE_BigIntExtendedGcd(TEE_BigInt* gcd, TEE_BigInt* u, TEE_BigInt* v, TEE_BigInt* op1, TEE_BigInt* op2);

/**
* @brief 
*
* @param op
* @param confidenceLevel
*
* @return 
*/
int TEE_BigIntIsProbablePrime(TEE_BigInt* op, u32 confidenceLevel);

/**
* @brief 
*
* @param dest
* @param src
* @param n
* @param context
*/
void TEE_BigInt2FMM(TEE_BigIntFMM* dest, TEE_BigInt* src, TEE_BigInt* n, TEE_BigIntFMMContext* context);

/**
* @brief 
*
* @param dest
* @param src
* @param n
* @param context
*/
void TEE_BigIntFMM2BigInt(TEE_BigInt* dest, TEE_BigIntFMM* src, TEE_BigInt* n, TEE_BigIntFMMContext* context);

/**
* @brief 
*
* @param dest
* @param op1
* @param op2
* @param n
* @param context
*/
void TEE_BigIntFMMCompute(TEE_BigIntFMM* dest, TEE_BigIntFMM* op1, TEE_BigIntFMM* op2, TEE_BigInt* n, TEE_BigIntFMMContext* context);

#endif
