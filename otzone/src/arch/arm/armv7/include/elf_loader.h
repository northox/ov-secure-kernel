#ifndef _ELF_LOADER_H
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
 * Elf loader implementation
 *
 */

#define  _ELF_LOADER_H
#include <task.h>
#define ELF_FLAG (0x7245)
#define ELF_FLAG_MAP (0x5859)
/**
 * @brief the main elf loader function
 *
 * The main loader function which will be called by the __elf_load()
 * syscall, has functionality for handling both relocatable files and executable
 * files, although executable file support is not required  as of now, so
 * control doesn't go to the executable file part of the function.
 *
 * @param conf : configuration parameter for the task 
 *
 * @return 0 on success, -1 on failure
 */
int elf_load( sa_config_t *conf);

/**
 * @brief cleans up the reserved secure memory region
 *
 * @return 
 */
int elf_loader_cleanup(void);

/**
 * @brief 
 */
struct kernel_symbol
{
	unsigned long addr;
	const char *symbol;
};

/* For every exported symbol, place a struct in the __ksymtab section */
#define __EXPORT_SYMBOL(sym, sec)                               \
	 extern typeof(sym) sym;                                 \
	static const char __kstrtab_##sym[]                     \
	__attribute__((section("__ksymtab_strings"), aligned(1))) \
	=  #sym;                            \
	static const struct kernel_symbol __ksymtab_##sym       \
	__attribute__((section("__ksymtab" sec), unused))       \
	= { (unsigned long)&sym, __kstrtab_##sym }

#define EXPORT_SYMBOL(sym)                                      \
	__EXPORT_SYMBOL(sym, "")

#endif
