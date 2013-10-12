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
 * ARITHMETIC API Tester implementation
 *
 */
#include <otz_tee_arith_api.h>
#include <otz_tee_mem_mgmt.h>
#include <sw_debug.h>
#include <sw_types.h>
#include <gp_internal_api_test.h>

/**
 * @brief 
 */
void arith_api_test(){
    TEE_BigInt a[] = { 0, 0, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104 };
    TEE_BigInt b[] = { 0, 0, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104, 305419896, 2596069104 };
    u32 i;
    TEE_BigInt *addResult = (TEE_BigInt*)TEE_Malloc(TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits) * sizeof(TEE_BigInt), 0);
    TEE_BigInt *subResult = (TEE_BigInt*)TEE_Malloc(TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits) * sizeof(TEE_BigInt), 0);
    TEE_BigInt *multResult = (TEE_BigInt*)TEE_Malloc(TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits) * sizeof(TEE_BigInt) * 2, 0);
    TEE_BigInt *div_q = (TEE_BigInt*)TEE_Malloc(TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits) * sizeof(TEE_BigInt), 0);
    TEE_BigInt *div_r = (TEE_BigInt*)TEE_Malloc(TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits) * sizeof(TEE_BigInt), 0);
    TEE_BigIntAdd(addResult,a,b);
    sw_printf("#######################ADD#######################\n");
    for(i = 0; i<TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits); i++){
        sw_printf("%08x | ",*(addResult+i));
    }
    sw_printf("Done doing 2048 bit Addition \n");
    sw_printf("#######################SUB#######################\n");
    TEE_BigIntSub(subResult,a,b);
    for(i = 0; i<TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits); i++){
        sw_printf("%08x | ",*(subResult+i));
    }
    sw_printf("Done doing 2048 bit Subtraction \n");
    sw_printf("#######################MULT#######################\n");
    TEE_BigIntMult(multResult,a,b);
    for(i = 0; i < TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits) * 2; i++){
        sw_printf("%08x | ",*(multResult+i));
    }
    sw_printf("Done doing 2048 bit Multiplication \n");
    sw_printf("#######################DIV#######################\n");
    TEE_BigIntDivide(div_q,div_r,a,b);
    sw_printf("Q:\n");
    for(i = 0; i < TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits); i++){
        sw_printf("%08x | ",*(div_q+i));
    }
    sw_printf("R:");
    for(i = 0; i < TEE_BigIntSizeInU32(TEE_BigIntNumberOfBits); i++){
        sw_printf("%08x | ",*(div_r+i));
    }
    sw_printf("Done doing 2048 bit Division \n");
    TEE_Free(addResult);
    TEE_Free(subResult);
    TEE_Free(multResult);
    TEE_Free(div_q);
    TEE_Free(div_r);
}
