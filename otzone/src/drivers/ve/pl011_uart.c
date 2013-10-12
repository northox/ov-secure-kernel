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
 * PL011 UART driver implementation
 */

#if defined(CONFIG_EB_BOARD)  || defined(CONFIG_VE_BOARD)

#include <uart.h>
#include <pl011_uart.h>

#include <sw_board.h>


/**
* @brief 
*
* @param id
*
* @return 
*/
static inline u32 get_uart_base_addr(u32 id);

/**
 * @brief 
 *
 * @param reg_offs
 * @param uartid
 *
 * @return 
 */
u32 read_uart(u32 reg_offs, u32 uartid)
{
    volatile u32 * reg_ptr = (u32*)(get_uart_base_addr(uartid) | reg_offs);
    return *reg_ptr;
}

/**
 * @brief 
 *
 * @param reg_offs
 * @param value
 * @param uartid
 */
void write_uart(u32 reg_offs, u32 value, u32 uartid)
{
    volatile u32 * reg_ptr = (u32*)(get_uart_base_addr(uartid) | reg_offs);
    *reg_ptr = value;
}


/**
 * @brief 
 *
 * @param id
 *
 * @return 
 */
static inline u32 get_uart_base_addr(u32 id)
{
    switch(id) {
        case 0:
            return UART0_ADDR;
        case 1:
            return UART1_ADDR;
        case 2:
            return UART2_ADDR;
        case 3:
            return UART3_ADDR;
        default:
            break;
    }
    return -1;
}

/**
 * @brief 
 *
 * @param c
 */
void serial_putc(char c)
{
    /* Wait until there is space in the FIFO */
    while (read_uart(UART_PL01x_FR, 0) & UART_PL01x_FR_TXFF);

    /* Send the character */
    write_uart(UART_PL01x_DR, c, 0);
}

/**
 * @brief 
 *
 * @param c
 */
void serial_puts(char * c)
{
    int index = 0;

    while (c[index] != '\0')
    {
        serial_putc(c[index]);
        index++;
    }
}

/**
 * @brief 
 *
 * @param uart_id
 */
void serial_init(u32 uart_id)
{
    int lcrh_reg;
    /* First, disable everything */
    write_uart(UART_PL011_CR, 0x0, uart_id);

    lcrh_reg = read_uart(UART_PL011_LCRH, uart_id);
    lcrh_reg &= ~UART_PL011_LCRH_FEN;
    write_uart(UART_PL011_LCRH, lcrh_reg, uart_id);

    /*
     * Set baud rate
     *
     baudrate = 115200
     UART clock = 24000000
     * IBRD = UART_CLK / (16 * BAUD_RATE)
     * FBRD = RND((64 * MOD(UART_CLK,(16 * BAUD_RATE))) 
     *    / (16 * BAUD_RATE))
     */

    write_uart(UART_PL011_IBRD, 13, uart_id);

    /* Set the UART to be 8 bits, 1 stop bit, 
     * no parity, fifo enabled 
     */
    write_uart(UART_PL011_LCRH, (UART_PL011_LCRH_WLEN_8 | UART_PL011_LCRH_FEN), uart_id);

    /* Finally, enable the UART */
    write_uart(UART_PL011_CR, (UART_PL011_CR_UARTEN | UART_PL011_CR_TXE), uart_id);

    return;
}

#endif
