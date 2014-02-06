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
 * ARITH API IMPLEMENTATION 
 *
 */

#include "otz_tee_arith_api.h"
#include "sw_types.h"
#include "otz_tee_mem_mgmt.h"

/**
 * @brief 
 *
 * @param moduleSizeInBits
 *
 * @return 
 */
size_t TEE_BigIntFMMSizeInU32(size_t moduleSizeInBits){
    /*Not implemented*/
    return TEE_ERROR_NOT_IMPLEMENTED;
}

/**
 * @brief 
 *
 * @param ModuleSizeInBits
 *
 * @return 
 */
size_t TEE_BigIntFMMContextSizeInU32(size_t ModuleSizeInBits){
    /*Not implemented*/
    return TEE_ERROR_NOT_IMPLEMENTED;
}

/**
 * @brief 
 *
 * @param bigInt
 * @param len
 *
 * @return 
 */
TEE_Result TEE_BigIntInit(TEE_BigInt *bigInt, size_t len){
    TEE_MemFill((void *)bigInt, 0, (len * sizeof(TEE_BigInt)));
    return TEE_SUCCESS;
}

/**
 * @brief 
 *
 * @param context
 * @param len
 * @param modulus
 *
 * @return 
 */
TEE_Result TEE_BigIntFMMContextInit(TEE_BigIntFMMContext *context, size_t len, TEE_BigInt *modulus){
    /*Not implemented*/
    return TEE_ERROR_NOT_IMPLEMENTED;
}

/**
 * @brief 
 *
 * @param bigIntFMM
 * @param len
 *
 * @return 
 */
TEE_Result TEE_BigIntFMMInit(TEE_BigIntFMM* bigIntFMM, size_t len){
    TEE_MemFill((void *)bigIntFMM, 0, (len * sizeof(TEE_BigIntFMM)));
    return TEE_SUCCESS;
}

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
TEE_Result TEE_BigIntSetFromOctetString(TEE_BigInt *dest, u32 *buffer, size_t bufferLen, int sign){
    int iter = 0;
    u32 *ptr1, *ptr2;
    if((bufferLen/sizeof(TEE_BigInt)) > (TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits) - 1)){
        return TEE_ERROR_OVERFLOW;
    }
    ptr1 = (dest + TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits) - 1);
    ptr2 = (buffer + (bufferLen/sizeof(u32)));
    for(iter = bufferLen; iter > 0; iter--){
        ptr1[iter] = ptr2[iter];
    }
    return TEE_SUCCESS;
    if(!sign){
        *((u32*)dest) = *((u32*)dest) | (1<<((sizeof(u32) * 8) - 1));
    }
}

/**
 * @brief 
 *
 * @param buffer
 * @param bufferLen
 * @param bigInt
 *
 * @return 
 */
TEE_Result TEE_BigIntConvertToOctetString(void* buffer, size_t *bufferLen, TEE_BigInt* bigInt){
    int iter = 0;
    u32 *ptr1, *ptr2;
    if((*bufferLen/sizeof(TEE_BigInt)) < TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits)){
        return TEE_ERROR_SHORT_BUFFER;
    }
    ptr1 = buffer;
    ptr2 = bigInt;
    for(iter = 0; iter < *bufferLen; iter++){
        ptr1[iter] = ptr2[iter];
    }
    return TEE_SUCCESS;
}

/**
 * @brief 
 *
 * @param dest
 * @param shortVal
 */
void TEE_BigIntConvertFromS32(TEE_BigInt* dest, int shortVal){
    u32 *ptr;
    TEE_MemFill((void *)dest, 0, (sizeof(TEE_BigInt) * TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits)));
    ptr = ((u32*)dest + TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits) - 1 );
    *ptr = (u32) shortVal;
}

/**
 * @brief 
 *
 * @param dest
 * @param src
 *
 * @return 
 */
TEE_Result TEE_BigIntConvertToS32(int* dest, TEE_BigInt* src){
    u32 *ptr;
    ptr = ((u32*)dest + TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits) - 1 );
    *dest = (int)(*ptr);
    return TEE_SUCCESS;
}

/**
 * @brief 
 *
 * @param op1
 * @param shortVal
 *
 * @return 
 */
int TEE_BigIntCmpS32(TEE_BigInt* op1, int shortVal){
    u32 *ptr1, *ptr2;
    int len = TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits), i, ret_val = 0;
    bool stop = false;
    ptr1 = op1; ptr2 = &shortVal;
    for(i = 0; i < (len - 1 - (sizeof(int)/sizeof(TEE_BigInt))); i++){
        if(*(ptr1+i) != 0){
            stop = true;
            ret_val = 1;
        }
    }
    if(*(ptr1 + ++i) < *ptr2){
        ret_val = -1;
    }
    if(*(ptr1+i) > *ptr2){
        ret_val = 1;
    }
    return ret_val;
}

/**
 * @brief 
 *
 * @param dest
 * @param src
 * @param bits
 */
void TEE_BigIntRightShift(TEE_BigInt* dest, TEE_BigInt* src, size_t bits){
    u32 getBit, putBit, value;
    int len = TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits), i, j;
    putBit = 0;
    for(i = 0; i < len; i++){
        getBit = 0;
        for(j = bits - 1; j > 0 ; j--){
            value = *(((u32*)src) + i);
            getBit = (getBit<<1 | (value>>j & 1));
        }
        value = (value>>bits) | putBit;
        *(((u32*)dest)+i) = value;
        putBit = getBit<<(sizeof(TEE_BigInt)*8 - 1);
    }
}

/**
 * @brief 
 *
 * @param dest
 * @param op1
 * @param op2
 */
void TEE_BigIntAdd(TEE_BigInt* dest, TEE_BigInt* op1, TEE_BigInt* op2){
   int i;
   unsigned int sum = 0, carry = 0;
   for(i = TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits) - 1; i >= 0; i--){
       sum = *(op1+i) + *(op2+i) + carry;
       if(sum < *(op1+i) || sum < *(op2+i))
           carry = 1;
       else
           carry = 0;
       *(dest+i) = sum;
   }
}

/**
 * @brief 
 *
 * @param dest
 * @param src
 */
void TEE_BigIntNeg(TEE_BigInt* dest, TEE_BigInt* src){
    int i;
    u32 *ptr1, *ptr2;
    ptr1 = dest;
    ptr2 = src;
    *ptr1 = (*ptr2 | 1<<((sizeof(TEE_BigInt) * 8) - 1));
    for(i = 1; i < (TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits) - 1); i++){
        *(ptr1+i) = *(ptr2+i);
    }
}

/**
 * @brief 
 *
 * @param dest
 * @param op1
 * @param op2
 */
void TEE_BigIntAddMult(TEE_BigInt* dest, TEE_BigInt* op1, TEE_BigInt* op2){
   int i;
   unsigned int sum = 0, carry = 0;
   for(i = (TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits) * 2) - 1; i >= 0; i--){
       sum = *(op1+i) + *(op2+i) + carry;
       if(sum < *(op1+i) || sum < *(op2+i))
           carry = 1;
       else
           carry = 0;
       *(dest+i) = sum;
   }
}

/**
 * @brief 
 *
 * @param dest
 * @param op1
 * @param op2
 */
void TEE_BigIntSub(TEE_BigInt* dest, TEE_BigInt* op1, TEE_BigInt* op2){
    int i;
    unsigned int diff = 0, carry = 0;
   for(i = TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits) - 1; i >= 0; i--){
       diff = (*(op1+i) - carry) - *(op2+i);
       if(diff > *(op1+i))
           carry = 1;
       else
           carry = 0;
       *(dest+i) = diff;
   }

}

/**
 * @brief 
 *
 * @param src
 *
 * @return 
 */
u32 MaxBitPos(TEE_BigInt* src){
    u32 index = 0, val;
    int x = TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits), i = 0, j = 0;
    int found = 0;
    for(i = 0; (i < x) & (found == 0); i++){
        val = *(src+i);
        for(j = (sizeof(TEE_BigInt)*8)-1; (j >= 0) & (found == 0); j--){
            if((val>>j)&1){
                index = ((x - i -1) * sizeof(TEE_BigInt) * 8) + j;
                found = 1;
            }
        }
    }
    return index;
}

/**
 * @brief 
 *
 * @param src
 * @param bitIndex
 *
 * @return 
 */
int TEE_BigIntGetBit(TEE_BigInt* src, u32 bitIndex){
    int mod, bit;
    u32 value;
    mod = bitIndex/(sizeof(TEE_BigInt)*8);
    bit = bitIndex%(sizeof(TEE_BigInt)*8);
    mod = (TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits) - 1) - mod;
    value = *(src + mod);
    if((1<<bit) & value)
        return 1;
    else
        return 0;
}

/**
 * @brief 
 *
 * @param dest
 * @param op1
 * @param op2
 */
void TEE_BigIntMult(TEE_BigInt* dest, TEE_BigInt* op1, TEE_BigInt* op2){
    TEE_BigInt *temp = (TEE_BigInt*)TEE_Malloc(TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits) * 2 * sizeof(TEE_BigInt), 0);
    u32 maxBit = MaxBitPos(op2);
    u32 x = 0;
    TEE_MemMove((void*)(temp + TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits)), (void*)op1, TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits) * sizeof(TEE_BigInt));
    for(x = 0; x <= maxBit; x++){
        if(x)
            TEE_BigIntAddMult(temp, temp, temp);
        if(TEE_BigIntGetBit(op2,x)){
            TEE_BigIntAddMult(dest, dest, temp);
        }
    }
    TEE_Free(temp);
}

/**
 * @brief 
 *
 * @param dest
 * @param src
 */
void TEE_BigIntSquare(TEE_BigInt* dest, TEE_BigInt* src){
    TEE_BigIntMult(dest, src, src);
}

int TEE_BigIntCmp(TEE_BigInt* op1, TEE_BigInt* op2){
        u32 *ptr1, *ptr2;
        int len = TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits), i, ret_val = 0;
        ptr1 = (u32*)op1; ptr2 = (u32*)op2;
        if(*ptr1 == *ptr2){
            for(i = 0; i < len; i++){
                if(*(ptr1+i) < *(ptr2+i)){
                    ret_val = -1;
                    break;
                }
                if(*(ptr1+i) > *(ptr2+i)){
                    ret_val = 1;
                    break;
                }
            }
        }else if( (*((u32*)op1)) & ((1<<((sizeof(u32) * 8) - 1)) != 0)){
            ret_val = -1;
        }else{
            ret_val = 1;
        }
        return ret_val;
}

/**
 * @brief 
 *
 * @param dest
 * @param src
 * @param bits
 */
void TEE_BigIntLeftShift(TEE_BigInt* dest, TEE_BigInt* src, size_t bits){
    u32 getBit, putBit, value;
    int len = TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits), i, j;
    putBit = 0;
    if(bits >= (sizeof(u32) * 8)){
        TEE_MemMove(src, (src + (bits/(sizeof(u32) * 8))),(len - (bits/(sizeof(u32) * 8))) * sizeof(u32));
        TEE_MemFill((src+len-(bits/(sizeof(u32) * 8))), 0, (bits/(sizeof(u32) * 8 ) * sizeof(u32)));
        bits = bits % (sizeof(u32) * 8);
    }
    for(i = len - 1; i >= 0; i--){
        getBit = 0;
        value = *(((u32*)src) + i);
        for(j = 0; j < bits; j++){
            value = *(((u32*)src) + i);
            getBit = (getBit<<1 | (value>>((sizeof(u32) * 8) - j - 1) & 1));
        }
        value = (value<<bits) | putBit;
        *(((u32*)dest) + i) = value;
        putBit = getBit;
    }
}

/**
 * @brief 
 *
 * @param dest
 */
void incrementBigInt(TEE_BigInt* dest){
    int i;
    unsigned int sum, carry = 1;
    for(i = TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits) - 1; i >= 0 && carry == 1; i--){
        sum = *(dest+i)+ carry;
        if(sum < *(dest+i))
            carry = 1;
        else
            carry = 0;
        *(dest+i) = sum;
    }
}

/**
 * @brief 
 *
 * @param x
 * @param y
 *
 * @return 
 */
int XPowerY(int x, int y){
    int result = 1;
    while(y--){
        result = result * x;
    }
    return result;
}

/**
 * @brief 
 *
 * @param dest_q
 * @param dest_r
 * @param op1
 * @param op2
 */
void TEE_BigIntDivide(TEE_BigInt* dest_q, TEE_BigInt* dest_r, TEE_BigInt* op1, TEE_BigInt* op2){
    TEE_BigInt *tmp1, *shifted, *adder;
    int diffBit;
    tmp1 = (TEE_BigInt*)TEE_Malloc(TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits) * sizeof(TEE_BigInt), 0);
    shifted = (TEE_BigInt*)TEE_Malloc(TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits) * sizeof(TEE_BigInt), 0);
    TEE_MemMove(tmp1,op1,(TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits) * sizeof(TEE_BigInt)));
    while(TEE_BigIntCmp(tmp1,op2) >= 0){
        diffBit = MaxBitPos(tmp1) - MaxBitPos(op2);
        TEE_MemMove(shifted,op2,(TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits) * sizeof(TEE_BigInt)));
        if(diffBit){
            TEE_BigIntLeftShift(shifted, shifted, diffBit);
            adder = (TEE_BigInt*)TEE_Malloc(TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits) * sizeof(TEE_BigInt), 0);
            if(TEE_BigIntCmp(tmp1,shifted) >=0){
                *(adder+TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits) - 1 - (diffBit/32)) = XPowerY(2, diffBit%32);
                TEE_BigIntAdd(dest_q, dest_q, adder);
                TEE_BigIntSub(tmp1, tmp1, shifted);
            }else if(diffBit - 1){
                TEE_BigIntLeftShift(shifted, shifted, diffBit - 1);
                *(adder+TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits) - 1 - (diffBit/31)) = XPowerY(2, (diffBit%31) - 1);
                TEE_BigIntAdd(dest_q, dest_q, adder);
                TEE_BigIntSub(tmp1, tmp1, shifted);
            }else{
                TEE_BigIntSub(tmp1, tmp1, op2);
                incrementBigInt(dest_q);
            }
        }else{
            TEE_BigIntSub(tmp1, tmp1, op2);
            incrementBigInt(dest_q);
        }
    }
        TEE_MemMove(dest_r,tmp1,(TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits) * sizeof(TEE_BigInt)));
}

/**
 * @brief 
 *
 * @param dest
 * @param op
 * @param n
 */
void TEE_BigIntReduceMod(TEE_BigInt* dest, TEE_BigInt* op, TEE_BigInt* n){
    TEE_BigInt *temp = (TEE_BigInt*)TEE_Malloc(TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits) * sizeof(TEE_BigInt), 0);
    TEE_BigIntDivide(temp, dest, op, n);
}

/**
 * @brief 
 *
 * @param dest
 * @param op1
 * @param op2
 * @param n
 */
void TEE_BigIntAddMod(TEE_BigInt* dest, TEE_BigInt* op1, TEE_BigInt* op2, TEE_BigInt* n){
    TEE_BigInt *temp = (TEE_BigInt*)TEE_Malloc(TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits) * sizeof(TEE_BigInt), 0);
    TEE_BigIntAdd(temp, op1, op2);
    TEE_BigIntDivide(temp, dest, temp, n);
}

/**
 * @brief 
 *
 * @param dest
 * @param op1
 * @param op2
 * @param n
 */
void TEE_BigIntSubMod(TEE_BigInt* dest, TEE_BigInt* op1, TEE_BigInt* op2, TEE_BigInt* n){
    TEE_BigInt *temp = (TEE_BigInt*)TEE_Malloc(TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits) * sizeof(TEE_BigInt), 0);
    TEE_BigIntSub(temp, op1, op2);
    TEE_BigIntDivide(temp, dest, temp, n);
}

/**
 * @brief 
 *
 * @param dest
 * @param op1
 * @param src
 * @param n
 */
void TEE_BigIntMultMod(TEE_BigInt* dest, TEE_BigInt* op1, TEE_BigInt* src, TEE_BigInt* n){
    TEE_BigInt *temp = (TEE_BigInt*)TEE_Malloc(TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits) * sizeof(TEE_BigInt), 0);
    TEE_BigIntMult(temp, op1, src);
    TEE_BigIntDivide(temp, dest, temp, n);
}

/**
 * @brief 
 *
 * @param dest
 * @param src
 * @param n
 */
void TEE_BigIntSquareMod(TEE_BigInt* dest, TEE_BigInt* src, TEE_BigInt* n){
    TEE_BigInt *temp = (TEE_BigInt*)TEE_Malloc(TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits) * sizeof(TEE_BigInt), 0);
    TEE_BigIntSquare(temp, src);
    TEE_BigIntDivide(temp, dest, temp, n);
}

/**
 * @brief 
 *
 * @param dest
 * @param op
 * @param n
 */
void TEE_BigIntInvMod(TEE_BigInt* dest, TEE_BigInt* op, TEE_BigInt* n){
    //not implemented//
}

/**
 * @brief 
 *
 * @param op1
 * @param op2
 *
 * @return 
 */
int TEE_BigIntRelativePrime(TEE_BigInt* op1, TEE_BigInt* op2){
    //not implemented//
    return TEE_ERROR_NOT_IMPLEMENTED;
}

/**
 * @brief 
 *
 * @param gcd
 * @param u
 * @param v
 * @param op1
 * @param op2
 */
void TEE_BigIntExtendedGcd(TEE_BigInt* gcd, TEE_BigInt* u, TEE_BigInt* v, TEE_BigInt* op1, TEE_BigInt* op2){
    //not implemented//
}

/**
 * @brief 
 *
 * @param op
 * @param confidenceLevel
 *
 * @return 
 */
int TEE_BigIntIsProbablePrime(TEE_BigInt* op, u32 confidenceLevel){
    //not implemented//
    return TEE_ERROR_NOT_IMPLEMENTED;
}

/**
 * @brief 
 *
 * @param dest
 * @param src
 * @param n
 * @param context
 */
void TEE_BigInt2FMM(TEE_BigIntFMM* dest, TEE_BigInt* src, TEE_BigInt* n, TEE_BigIntFMMContext* context){
    //not supported//
}

/**
 * @brief 
 *
 * @param dest
 * @param src
 * @param n
 * @param context
 */
void TEE_BigIntFMM2BigInt(TEE_BigInt* dest, TEE_BigIntFMM* src, TEE_BigInt* n, TEE_BigIntFMMContext* context){
    //not supported//
}

/**
 * @brief 
 *
 * @param dest
 * @param op1
 * @param op2
 * @param n
 * @param context
 */
void TEE_BigIntFMMCompute(TEE_BigIntFMM* dest, TEE_BigIntFMM* op1, TEE_BigIntFMM* op2, TEE_BigInt* n, TEE_BigIntFMMContext* context){
    //not supported//
}
