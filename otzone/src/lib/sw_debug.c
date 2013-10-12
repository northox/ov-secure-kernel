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
 * Helper functions for debugging
 */
#include <elf_loader.h>
#include "sw_debug.h"
#include "cpu.h"
#include "sw_string_functions.h"
#include "uart.h"
#include "sw_modinit.h"
#include "sw_types.h"







/**
 * @brief 
 *
 * @param fmt
 * @param ...
 *
 * @return 
 */
u32 sw_printf(const char *fmt, ...)
{
#if _OTZ_NO_LIBC_BUILD
	va_list args;
  	u32 i;
  	char print_buffer[256];
  	va_start(args, fmt);

  	i = sw_vsprintf(print_buffer, fmt, args);
  	va_end(args);

  	/* Print the string */
  	sw_write("UART",print_buffer);
  	return i;
#else
  	return(0);
#endif
}
EXPORT_SYMBOL(sw_printf);
/**
 * @brief Writes directly to device, with no permission check
 *
 * @param fmt
 * @param ...
 *
 * @return 
 */
u32 sw_printk(const char *fmt, ...)
{
  	va_list args;
  	u32 i;
  	char print_buffer[256];
  	va_start(args, fmt);

	i = sw_vsprintf(print_buffer, fmt, args);
	va_end(args);

  	/* Print the string */
  	serial_puts(print_buffer);
  	return i;
}

/**
 * @brief 
 *
 * @param buf
 * @param fmt
 * @param args
 *
 * @return 
 */
u32 sw_vsprintf(char *buf, const char *fmt, va_list args)
{
#if _OTZ_NO_LIBC_BUILD
  	char *str;
  	str = buf;

  	for(; *fmt ; ++fmt){
    	if((*fmt != '%') && (*fmt != '\n') && (*fmt != '\t')){
      		*str++ = *fmt;
      		continue;
    	}

    	if(*fmt == '%'){
      		/* skip % */
			++fmt;
      		u32 is_unsigned = 0;
      		u32 zero_padding = 1;
			
			if(*fmt == '0'){
        		/* zero padding!*/
        		/* skip 0 */
        		++fmt;
          		zero_padding = *fmt++;
          		if((zero_padding < 0x31) || (zero_padding > 0x38)){
            		serial_puts("invalid padding bits.\0");
          		}
          		zero_padding -= 0x30;
			}

	  		switch(*fmt){
				case 'l':
				{
					++fmt;
					break;
				}
	  		}
      		
			switch(*fmt){		
        		case 'x':
				{
          			u32 number = va_arg(args, int);
	          		int length = 8;
    	      		int length_in_bits = 32;
        	  		int byte = 0;
          			int i = 0;
					bool keep_zeros = FALSE;
          		
					for(i = 0; i < length; i++){
            			byte = number >> (length_in_bits - ((i+1) * 4));
            			byte = byte & 0xF;
            			if(byte != 0){
              				keep_zeros = TRUE;
            			}
            			if(keep_zeros || i >= (7-(zero_padding-1))){
              				if((byte >= 0) && (byte <= 9)){
                				byte = byte + 0x30;
              				}
              				else{
                				switch(byte){
                  					case 0xa:
                    					byte = 0x61;
            					        break;
                  					case 0xb:
										byte = 0x62;
      						            break;
                  					case 0xc:
                    					byte = 0x63;
                    					break;
                  					case 0xd:
                    					byte = 0x64;
                    					break;
                  					case 0xe:
                    					byte = 0x65;
                   	 					break;
                  					case 0xf:
                    					byte = 0x66;
                    					break;
                				} /* switch ends */
              				} /* else ends */
              				*str++ = byte;
            			}
          			} /* for ends - whole number is now done */
					break;
				}
				case 'u':
					is_unsigned = 1;
				case 'i':
	    		case 'd':
				{
					u32 i,j,max_num_zeros,num_of_digits_u32,number_u32,
						divisor_value_u32,new_div_val = 1,sw_quotient_value = 0;
		          	bool keep_zeros = FALSE;
					 
					if(!is_unsigned){
						int signed_num_32 = va_arg(args,int);
						if(signed_num_32 < 0){
							*str++ = 0x2d;
							signed_num_32 = -(signed_num_32);
						}
						number_u32 = (u32)signed_num_32;
					}
					else{
						u32 unsigned_value_32 = va_arg(args,unsigned int);
						number_u32 = unsigned_value_32;
					}
			
            		divisor_value_u32 = 1000000000;
					num_of_digits_u32 = 10;
					max_num_zeros = num_of_digits_u32 - 1;

					for(i = 0; i < max_num_zeros; i++){
           				while(number_u32 >= divisor_value_u32){
							number_u32 -= divisor_value_u32;
							++sw_quotient_value;
						}
						if(sw_quotient_value != 0)
							keep_zeros = TRUE;
						if(keep_zeros || i > ((max_num_zeros-1)-(zero_padding-1))){
							sw_quotient_value += 0x30;
				 			*str++ = sw_quotient_value;
						}
						j = i;
						while(j < (max_num_zeros-1)){
							new_div_val *= 10;
							j++;
						}
						sw_quotient_value = 0;
						divisor_value_u32 = new_div_val;
						new_div_val = 1;
					}
					*str++ = (number_u32 + 0x30);
					break;
				}
				case 'o':
				{
					u32 number,length = 10,length_in_bits = 32,byte = 0,i = 0;
					bool keep_zeros = FALSE;

					number = va_arg(args, int);
		    		byte = number >> 30;
					byte &= 0x3;
					if(byte != 0){
						keep_zeros = TRUE;
					}
					if(keep_zeros || zero_padding > length){
                		byte = byte + 0x30;
              			*str++ = byte;
            		}

		  			number <<= 2;
          			for(i = 0; i < length; i++){
         				byte = number >> (length_in_bits - ((i+1) * 3));
            			byte &= 0x7;              	
						if(byte != 0){
              				keep_zeros = TRUE;
              			}
              			if(keep_zeros || i >= (9-(zero_padding-1))){
                			byte = byte + 0x30;
              				*str++ = byte;
            			}
					}
					break;
				}
        		case 's':
				{
          			char *arg_string = va_arg(args, char *);
          			while((*str = *arg_string++)){
            			++str;
          			}	
					break;
				}
        		case 'c':
				{
          			char character = va_arg(args, char);
          			*str++ = character;
          			break;
				}
				case '%':
				{
					*str++ = *fmt;
					break;
				}
				case '\t':
				{
					*str++ = '%';
					*str++ = *fmt;
					break;
				}
				case '\n':
				{
					*str++ = '%';
					*str++ = '\r';
		      		*str++ = '\n';
					break;
				}
        		default:
				{
          			serial_puts("option after %: ");
          			serial_putc(*fmt);
          			serial_puts("\r\n");
          			serial_puts("Unknown option after %\0");
        		}
      		} /* switch ends             */
    	} /* if % character found      */

    	if(*fmt == '\n'){
      		*str++ = '\r';
      		*str++ = '\n';
    	}
		if(*fmt == '\t')
			*str++ = *fmt;
	} /* for ends */
  	*str = '\0';
  	return sw_strlen(str);
#else
	return(0);
#endif
}

/**
 * @brief 
 *
 * @param c
 *
 * @return 
 */
int printable_char(char c)
{
	if((c == LINE_FEED) || (c == CARRIAGE_RETURN)){
    	return 1;
	}
  	else if((c >= PRINTABLE_START) && (c <= PRINTABLE_END)){
    	return 1;
  	}
  	else{
    	return 0;
  	}
}

/**
 * @brief 
 *
 * @param context
 * @param msg
 */
void DIE_NOW(void* context, char* msg)
{
#if _OTZ_NO_LIBC_BUILD
	serial_puts(msg);
	serial_puts("\r\n");
#endif
	infinite_idle_loop();
}

