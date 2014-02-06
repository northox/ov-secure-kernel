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
 * PL011 UART declarations
 */

#ifndef _PL011_BOARD_H__
#define _PL011_BOARD_H__

#define UART_SIZE             0x00010000

/*
 *  PL011 definitions
 *
 */
#define UART_PL011_IBRD                 0x24
#define UART_PL011_FBRD                 0x28
#define UART_PL011_LCRH                 0x2C
#define UART_PL011_CR                   0x30
#define UART_PL011_IMSC                 0x38
#define UART_PL011_MIS                  0x40
#define UART_PL011_ICR                  0x44
#define UART_PL011_PERIPH_ID0           0xFE0

#define UART_PL011_LCRH_SPS             (1 << 7)
#define UART_PL011_LCRH_WLEN_8          (3 << 5)
#define UART_PL011_LCRH_WLEN_7          (2 << 5)
#define UART_PL011_LCRH_WLEN_6          (1 << 5)
#define UART_PL011_LCRH_WLEN_5          (0 << 5)
#define UART_PL011_LCRH_FEN             (1 << 4)
#define UART_PL011_LCRH_STP2            (1 << 3)
#define UART_PL011_LCRH_EPS             (1 << 2)
#define UART_PL011_LCRH_PEN             (1 << 1)
#define UART_PL011_LCRH_BRK             (1 << 0)

#define UART_PL011_CR_CTSEN             (1 << 15)
#define UART_PL011_CR_RTSEN             (1 << 14)
#define UART_PL011_CR_OUT2              (1 << 13)
#define UART_PL011_CR_OUT1              (1 << 12)
#define UART_PL011_CR_RTS               (1 << 11)
#define UART_PL011_CR_DTR               (1 << 10)
#define UART_PL011_CR_RXE               (1 << 9)
#define UART_PL011_CR_TXE               (1 << 8)
#define UART_PL011_CR_LPE               (1 << 7)
#define UART_PL011_CR_IIRLP             (1 << 2)
#define UART_PL011_CR_SIREN             (1 << 1)
#define UART_PL011_CR_UARTEN            (1 << 0)

#define UART_PL011_IMSC_OEIM            (1 << 10)
#define UART_PL011_IMSC_BEIM            (1 << 9)
#define UART_PL011_IMSC_PEIM            (1 << 8)
#define UART_PL011_IMSC_FEIM            (1 << 7)
#define UART_PL011_IMSC_RTIM            (1 << 6)
#define UART_PL011_IMSC_TXIM            (1 << 5)
#define UART_PL011_IMSC_RXIM            (1 << 4)
#define UART_PL011_IMSC_DSRMIM          (1 << 3)
#define UART_PL011_IMSC_DCDMIM          (1 << 2)
#define UART_PL011_IMSC_CTSMIM          (1 << 1)
#define UART_PL011_IMSC_RIMIM           (1 << 0)

#define UART_PL011_MIS_OEMIS        (1 << 10)
#define UART_PL011_MIS_BEMIS        (1 << 9)
#define UART_PL011_MIS_PEMIS        (1 << 8)
#define UART_PL011_MIS_FEMIS        (1 << 7)
#define UART_PL011_MIS_RTMIS        (1 << 6)
#define UART_PL011_MIS_TXMIS        (1 << 5)
#define UART_PL011_MIS_RXMIS        (1 << 4)
#define UART_PL011_MIS_DSRMMIS      (1 << 3)
#define UART_PL011_MIS_DCDMMIS      (1 << 2)
#define UART_PL011_MIS_CTSMMIS      (1 << 1)
#define UART_PL011_MIS_RIMMIS       (1 << 0)

#define UART_PL011_ICR_OEIC     (1 << 10)
#define UART_PL011_ICR_BEIC     (1 << 9)
#define UART_PL011_ICR_PEIC     (1 << 8)
#define UART_PL011_ICR_FEIC     (1 << 7)
#define UART_PL011_ICR_RTIC     (1 << 6)
#define UART_PL011_ICR_TXIC     (1 << 5)
#define UART_PL011_ICR_RXIC     (1 << 4)
#define UART_PL011_ICR_DSRMIC       (1 << 3)
#define UART_PL011_ICR_DCDMIC       (1 << 2)
#define UART_PL011_ICR_CTSMIC       (1 << 1)
#define UART_PL011_ICR_RIMIC        (1 << 0)

#define UART_PL01x_DR                   0x00     /*  Data read or written from the interface. */
#define UART_PL01x_RSR                  0x04     /*  Receive status register (Read). */
#define UART_PL01x_ECR                  0x04     /*  Error clear register (Write). */
#define UART_PL01x_FR                   0x18     /*  Flag register (Read only). */

#define UART_PL01x_RSR_OE               0x08
#define UART_PL01x_RSR_BE               0x04
#define UART_PL01x_RSR_PE               0x02
#define UART_PL01x_RSR_FE               0x01

#define UART_PL01x_FR_TXFE              0x80
#define UART_PL01x_FR_RXFF              0x40
#define UART_PL01x_FR_TXFF              0x20
#define UART_PL01x_FR_RXFE              0x10
#define UART_PL01x_FR_BUSY              0x08
#define UART_PL01x_FR_TMSK              (UART_PL01x_FR_TXFF + UART_PL01x_FR_BUSY)
#endif

