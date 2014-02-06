/*
 * boot.S - simple register setup code for stand-alone Linux booting
 *
 * Copyright (C) 2009 ARM Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
TARGET(binary)

INPUT(../../../linux-otz/arch/arm/boot/Image)
INPUT(./boot.o)

PHYS_OFFSET = 0x70000000;

SECTIONS
{
    . = PHYS_OFFSET;
    .text : { boot.o }

    . = PHYS_OFFSET + 0x8000;
    kernel = .;
      .kernel : { ../../../linux-otz/arch/arm/boot/Image }

    .data : { *(.data) }
    .bss : { *(.bss) }
}
