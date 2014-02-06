/* Linker script */

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH("arm")

ENTRY(_start)

PHYS_OFFSET = 0x90400000;

SECTIONS
{
    . = PHYS_OFFSET;
    _SW_CODE_START = .;

    .text : { 
	  *(.text) 
	  *(.rodata .rodata.*) 
	}
    _SW_KSYMTAB = .;
	__ksymtab : {
						*(__ksymtab)
				}
	. = ALIGN(0x4);
	_SW_KSYMTAB_END = .;
    _SW_KSYMTAB_STRINGS = .;
    	__ksymtab_strings : {
					    *(__ksymtab_strings)
				    }
	. = ALIGN(0x4);
	_SW_FS_START = .;
	fs_image : {
		*(fs_image);
	}
	. = ALIGN(0x100000);
    _SW_TEXT_END = .;
    _text_size = _SW_TEXT_END - _SW_CODE_START;

    .init : {* (.init) }
    _MOD_INIT_SECTION_START = .;
    .mod_init : {* (.mod_init) }
    _MOD_INIT_SECTION_END = .;
    . = ALIGN(0x100000);
    _MOD_INIT_PADDING_END = .;
    _init_size = _MOD_INIT_PADDING_END - _SW_TEXT_END;

    .data : { *(.data) }
    . = ALIGN(0x100000);
    _SW_DATA_END = .;
    _data_size = _SW_DATA_END - _MOD_INIT_PADDING_END;

/* 
    PROVIDE(_NSADMIN_SECTION_START = .);
        .nsadmin : {
		* (.nsadmin);
		}
    PROVIDE(_NSADMIN_SECTION_END = .);
*/
	normal_image : {
		*(normal_image);
	}
    . = ALIGN(0x100000);

	normal_image_2 : {
		*(normal_image_2);
	}
    . = ALIGN(0x100000);

	initrd_image : {
		*(initrd_image);
	}
    . = ALIGN(0x100000);


    _SW_BSS_START = .;
   .bss : { *(.bss) *(COMMON)}
   . = ALIGN(0x100000);
   _SW_CODE_END = .;
   _bss_size = _SW_CODE_END - _SW_BSS_START;
	PROVIDE (end = .);
}

