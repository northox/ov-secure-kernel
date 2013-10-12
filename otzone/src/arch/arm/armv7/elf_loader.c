
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
#ifndef CONFIG_FILESYSTEM_SUPPORT
#error "Filesystem support required for Elf loader"
#endif
#include <elf.h>
#include <task.h>
#include <secure_api.h>
#include <sw_debug.h>
#include <otz_id.h>
#include <fat32.h>
#include <page_table.h>
#include <dispatcher_task.h>
#include <sw_string_functions.h>
#include <sw_mem_functions.h>
#include <mem_mng.h>
#include <sw_list.h>
#include <cpu_data.h>
#include <sw_types.h>
#include <task.h>
#include <elf_loader_app.h>
#include <elf_loader.h>
#include <sw_board.h>

#define __KSYMTAB_ADDR ((va_t *)&_SW_KSYMTAB)

static int init_map_loader(sa_config_t *);

static int fix_process_func(Elf32_Shdr *sec_header , Elf32_Sym *sym , char
		*entry_func, u32 symindex, const char *strtab,
		sa_config_t *psa_config)
{
	Elf32_Shdr *tmp_shdr=sec_header;
	int index;
	u32 n = ((tmp_shdr[symindex].sh_size) / (sizeof(Elf32_Sym))), i;

	for(i=1; i<n ; i++){
		index = sym[i].st_name;
		if(sw_strcmp((char *)(strtab + index), entry_func) == 0){

			/* BASE_LOAD_ADDRESS is where  the .text section 
			 *  gets mapped..so
			 * all we need is an offset from BASE_LOAD_ADDRESS */
			psa_config->process = BASE_LOAD_ADDRESS 
				+ sym[i].st_value; 
			return 0;
		}

	}
	return -1;
}


/**
 * @brief clean up function to remove the mappings
 *
 * @param psa_config
 *
 * @return 0 on success , -1 on failure
 */
int elf_loader_cleanup(void)
{
	int status = 0;
	/* 0 indicates that the mapping is successful, so do the cleanup */ 
	status = unmap_secure_memory((va_t)BASE_LOAD_ADDRESS, 
			SECTION_SIZE);
	if (status == -1){
		sw_printf("Unable to unmap secure memory of elf loader\n");
		return -1;
	}

	return status;

}

/**
 * @brief : Loads the sections marked with SHF_ALLOC flags into memory
 *
 * @param file_map : memory map of the entire relocatable object file
 * @param sec_header : pointer to the section header table
 *
 * @return 0 on success , -1 on failure
 */
static int load_into_memory(void *file_map , Elf32_Shdr * sec_header)
{
	Elf32_Ehdr *elf_header = (Elf32_Ehdr *)file_map;
	Elf32_Shdr *tmp_shdr = sec_header;
	void *status = NULL;
	int shnum = elf_header->e_shnum,i;
	u32 prev_addr = BASE_LOAD_ADDRESS;

	for(i = 1; i < shnum ; i++){
		if(tmp_shdr[i].sh_flags & SHF_ALLOC){
			status = sw_memcpy((void *)prev_addr, 
					(void*)tmp_shdr[i].sh_addr,
					tmp_shdr[i].sh_size);
			if(status == NULL){
				sw_printf("load failed\n");
				return -1;
			}
			prev_addr += tmp_shdr[i].sh_size;
		}



	}
	return 0;
}


/**
 * @brief: Get address of undefined symbol
 *
 * If our relocatable object file has an undefined symbol ie it st_shndx
 * will be SHN_UNDEF, then we need to lookup the kernel symbol table (__ksymtab)
 * for the absolute address of the function in memory.
 *
 * @param file_map : memory map of the entire relocatable object file
 * @param sec_header : pointer to the section header table
 * @param symname : name of the symbol that is undefined and needs to looked up
 * in the kernel symbol table
 * 
 *
 * @return : the address of the function on success, -1 on failure
 */
static u32 get_undefined_symbol_addr(struct kernel_symbol *sym_tab,
		const char *symname, int n)
{

	int i, ret=-1;

	for(i=0; i<n ;i++){
		if(sw_strcmp(symname, sym_tab->symbol) == 0){

			ret = (u32)sym_tab->addr;
			break;
		}
		sym_tab+=1;
	}
	return ret;

}



/**
 * @brief :  fix entry point of relocatable file
 *
 * In a relocatable file, the 'e_entry' field will be set to zero, fix
 * it to the appropriate entry point, note that we specify the entry function's
 * name in the sa_config_t structure, this function will get the relative offset
 * of the function and adds it to the BASE_LOAD_ADDRESS, taking advantage of the
 * fact that the .text section will be the first section to be loaded.
 *
 * @param sec_header
 * @param sym  : pointer to the symbol table
 * @param entry_func : name of the entry function
 * @param symindex : symbol table index
 * @param strtab : symbol string table index
 * @param psa_config : configuration structure for the task
 *
 * @return : 0 on success, -1 on failure
 */
static int fix_entry_point(Elf32_Shdr *sec_header , Elf32_Sym *sym , char
		*entry_func, u32 symindex, const char *strtab,
		sa_config_t *psa_config)
{
	Elf32_Shdr *tmp_shdr=sec_header;
	int index;
	u32 n = ((tmp_shdr[symindex].sh_size) / (sizeof(Elf32_Sym))), i;

	for(i=1; i<n ; i++){
		index = sym[i].st_name;
		if(sw_strcmp((char *)(strtab + index), entry_func) == 0){

			/* BASE_LOAD_ADDRESS is where  the .text section 
			 *  gets mapped..so
			 * all we need is an offset from BASE_LOAD_ADDRESS */
			psa_config->entry_point = BASE_LOAD_ADDRESS 
				+ sym[i].st_value; 
			return 0;
		}

	}
	return -1;



}


/**
 * @brief : Apply relocations to the relocatable object file
 *
 *This function does the actual relocations, for a function it will be
 * (S - A) + P where S is the actual address of the symbol, A is the  address of
 * the relocation and P is the addend value (Refer the ELF documentation on more
 * details) and for a data object we can lookup its value in st_value field of
 * the symbol table and add it to the value at the relocation offset
 *
 * @param file_map
 * @param sec_header
 * @param str_tab : pointer to the symbol string table
 * @param symindex
 * @param relindex  : relocatable section index
 * @param conf
 * @param fl  : fix_entry_point() function needs to be called only once, this
 * will take care of it.
 *
 * @return 0 on success, -1 on failure
 */

static int apply_relocations(void *file_map , Elf32_Shdr *sec_header , const
		char *str_tab, u32 symindex, u32 relindex, sa_config_t *conf,
		int *fl)
{
	Elf32_Shdr *sym_sec = sec_header + symindex;
	Elf32_Shdr *rel_sec = sec_header + relindex;
	Elf32_Rel *rel = (Elf32_Rel *)rel_sec->sh_addr;
	Elf32_Sym *tmp_sym = NULL;
	Elf32_Sym *sym = (Elf32_Sym *)sym_sec->sh_addr;
	u32 i, status;
	char *entry_func= conf->entry_func;
	u32 symbol_addr,sym_index = symindex;
	s32 offset,loc_to_rel;
	Elf32_Shdr *dst_sec = sec_header + rel_sec->sh_info;
	Elf32_Ehdr *elf_header = NULL;
	const char *symname;
	Elf32_Shdr *tmp_shdr = sec_header;

	va_t * sym_tab_start = (va_t *)&_SW_KSYMTAB;
	va_t * sym_tab_end = (va_t *)&_SW_KSYMTAB_END;
	u32 n = ((u32)sym_tab_end - 
			(u32)sym_tab_start) / sizeof(struct kernel_symbol);
	struct kernel_symbol *sym_tab = NULL;

	sym_tab=(struct kernel_symbol *)sym_tab_start;

	tmp_sym = sym;
	for(i=0; i < (rel_sec->sh_size)/(sizeof(Elf32_Rel)); i++){
		/* Get the index inside the sym_tab */
		offset = ELF32_R_SYM(rel[i].r_info);
		if(offset < 0 || offset > 
				((sym_sec->sh_size ) / (sizeof(Elf32_Sym)))){
			sw_printf("Invalid offset inside the symbol table\n");
			return -1;
		}

		/* Index of the symbol in the symbol table */
		sym = (Elf32_Sym *)sym + offset;	

		/* Note that st_name is a index into the symbol string table */
		symname = str_tab + sym->st_name;



		if( (rel[i].r_offset < 0 || rel[i].r_offset > 
					(dst_sec->sh_size - sizeof(u32)))){
			sw_printf("Invalid offset of %s\n", symname);
			return -1;
		}


		switch(ELF32_R_TYPE(rel[i].r_info))
		{
			case R_ARM_NONE:
				/* ignore it..*/
				break;
			case R_ARM_ABS32:
				/* Right now we are just exporting functions,
				 * add relocation support when we export global
				 * variable too */
				*(u32 *)loc_to_rel += sym->st_value;
				break;
			case R_ARM_CALL:
			case R_ARM_JUMP24:
			case R_ARM_PC24:
/* Calculate relocations differently here, because there is a strong possibility
 * that the reloc offset will exceed 32MB , so assuming that we load @ address
 * closer to the kernel, use that  address instead of dst_sec->sh_addr..Also we
 * are taking advantage of the fact that the .text section will be the first to 
 * be mapped */
				if(sym->st_shndx == SHN_UNDEF){

					symbol_addr = get_undefined_symbol_addr(
							sym_tab,symname,n);
					if(symbol_addr == -1){
						sw_printf("Unable to find" 
								"symbol\n");
						return -1;
					}
					loc_to_rel = (u32)(dst_sec->sh_addr + 
							rel[i].r_offset);
					offset = (*(u32 *) loc_to_rel &
							0x00ffffff) << 2;
					if(offset & 0x02000000)
						offset -= 0x04000000;
/* We know that the .text section will be the first section to be mapped.. so 
 * take advantage of it */
					offset += symbol_addr - 
						((u32) BASE_LOAD_ADDRESS +
						 rel[i].r_offset) ;


					if (offset & 3 ||
							offset <=
							(s32)0xfe000000 ||
							offset >= 
							(s32)0x02000000) {

						sw_printf("Invalid \
								relocation\n");
						return -1;
					}
					offset >>= 2;

					/* we need only the last 24 bits */
					*(u32 *)loc_to_rel &= 0xff000000;
					*(u32 *)loc_to_rel 
						|= offset & 0x00ffffff;


				}
				else{

					/* Get the offset to relocate */
					loc_to_rel = (u32)(dst_sec->sh_addr 
							+ rel[i].r_offset);
					offset = (*(u32 *) loc_to_rel &
							0x00ffffff) << 2;

					if(offset & 0x02000000)
						offset -= 0x04000000;
					offset += 
						((u32)tmp_shdr[sym->st_shndx].sh_addr
						 + sym->st_value) - loc_to_rel;
					if (offset & 3 ||
							offset <= 0xfe000000 ||
							offset >= 0x02000000) {

						sw_printf("Invalid" 
								"relocation\n");
						return -1;
					}
					offset >>= 2;

					/* we need only the last 24 bits */
					*(u32 *)loc_to_rel &= 0xff000000; 
					*(u32 *)loc_to_rel |= offset & 
						0x00ffffff;
				}
				break;

		default:
				break;


		}
		sym = tmp_sym;
	}
	/* We are calling the fix_entry_point() since all the params it requires
	 * are present in this function.. 'fl ' will
	 make sure that we call the function only once...*/
	if(*fl == 0){	

		status = fix_entry_point(sec_header, tmp_sym ,entry_func  ,
				sym_index, str_tab, conf);
		if(status == -1){
			sw_printf("Unable to fix entry_point\n");
			return -1;
		}

		status = fix_process_func(sec_header, tmp_sym
				,conf->process_name  ,
				sym_index, str_tab, conf);

		if(status == -1){
			sw_printf("Unable to fix process_func\n");
			return -1;
		}
		elf_header = file_map;
		elf_header->e_entry = conf->entry_point;
		*fl = 1;
	}

	return 0;

}



/**
 * @brief : the main function to handle relocations and map the object file
 *
 *This function is more like the  main function to handle  relocatable
 * object file, it first fixes the sh_addr field of the section headers,gets the
 * index of the SYMTAB and STRTAB and then calls
 * the appropriate function to do the relocations, fix the entry point
 * and map it  to memory
 *
 * @param fp : file pointer to the relocatable object file
 * @param elf_header : pointer to the ELF header
 * @param conf
 *
 * @return 
 */
static int relocate_elf_loader(int fp, Elf32_Ehdr *elf_header, 
		sa_config_t *conf)
{
	void *file_map = NULL, *tmp = NULL;
	u32 offset, len, num_bytes,sh_off,sh_num,i,symindex,strindex,relindex;
	char *str_tab = NULL;
	int status,ret = OTZ_OK,fl = 0;
	Elf32_Shdr *sec_header , *tmp_shdr;


	/* Get the number of bytes in the file */
	offset = file_seek(fp,0x0,FILE_SEEK_END);
	if(offset == -1){
		sw_printf("file seek failed\n");
		ret = -1;
		goto out;
	}

	len = offset;
	offset = file_seek(fp,0x0,FILE_SEEK_SET);
	file_map = (void *)sw_vir_page_alloc(len);
	if(!file_map){
		sw_printf("Insufficient  memory\n");
		ret = -1;
		goto out;
	}

	/* Map the file to memory */
	tmp = file_map;

	do{
		num_bytes = file_read(fp,(char *)tmp,len);
		if(num_bytes == 0){
			sw_printf("Read failed\n");
			ret = -1;
			goto out;
		}
		tmp = (char *)tmp + num_bytes;
		len -= num_bytes;
	}
	while(len > 0);

	tmp = file_map;
	sw_memcpy((char *)elf_header,(char *)tmp,sizeof(Elf32_Ehdr));
	sh_off = elf_header->e_shoff;
	sh_num = elf_header->e_shnum;

	sec_header = (Elf32_Shdr *)((char *)tmp + sh_off);
	tmp_shdr = sec_header;

/* Get the symtab index, strtab index and reloc index.. 
 * and also fix sh_addr with file_map as the base address for easy referral  */
	for(i = 1; i < sh_num ; i++){
		if(tmp_shdr[i].sh_type == SHT_SYMTAB){
			symindex = i;
		}
		if(tmp_shdr[i].sh_type == SHT_STRTAB){
			strindex = i;
		}

		tmp_shdr[i].sh_addr = (Elf32_Addr)((char *)tmp + 
				tmp_shdr[i].sh_offset);

	}
	/* Note that offset if from the beginning of the file */
	str_tab = ((char *)tmp  + tmp_shdr[strindex].sh_offset);

/* Note that there will be multiple relocation sections..Apply for everything */
	for(i = 1;i < sh_num ; i++){
		if(tmp_shdr[i].sh_type == SHT_REL){
			relindex = i;
			status = apply_relocations(file_map, sec_header ,
					str_tab, symindex, relindex, conf,&fl);
			if(status == -1){
				sw_printf("Relocation failed\n");
				ret = -1;
				goto out;
			}
		}
	}

	status = load_into_memory(file_map,sec_header);
	if(status == -1){
		sw_printf("Unable to load into memory\n");
		ret = -1;
		goto out;
	}

	/* Note that we have a separate cleanup label for both executable 
	 * and relocatable files */
out:
	if(file_map)
		sw_vir_addr_free((u32) file_map,len);
	if(fp != -1)
		status = file_close(fp);
	if(status == -1)
		sw_printf("Unable to close file");
	return ret;
}

/**
 * @brief Maps a region of 1MB to the Virtual address which will be used
 * eventually as the module load address
 *
 * @return 0 on success , -1 on failure
 */
static int init_map_loader(sa_config_t *psa_config)
{

	if(map_secure_memory(BASE_LOAD_ADDRESS,(u32)ELF_LOADER_START_ADDRESS,
				SECTION_SIZE,PTF_EXEC | PTF_PROT_URW)
			!= OTZ_OK){
		sw_printf("Unable to map VA to PA\n");
		return -1;
	}
	psa_config->elf_flag = ELF_FLAG_MAP;
	return 0;
}



/**
 * @brief Validate the elf header 
 *
 * @param fp
 * @param elf_header
 *
 * @return 0 if it is a executable file, 1 if the file is relocatable object
 * file and -1 on failure
 */
static int validate_elf_header(int fp , Elf32_Ehdr *elf_header) 
{
	int num_bytes,rel = 0;
	num_bytes = file_read(fp,(char *)elf_header,sizeof(Elf32_Ehdr));
	if(num_bytes != sizeof(Elf32_Ehdr)){
		sw_printf("Read failed\n");
		return -1;

	}
	if(!(elf_header->e_ident[EI_MAG0] == ELFMAG0 &&
				elf_header->e_ident[EI_MAG1] == ELFMAG1 &&
				elf_header->e_ident[EI_MAG2] == ELFMAG2 &&
				elf_header->e_ident[EI_MAG3] == ELFMAG3)){
		sw_printf("Invalid ELF file\n");
		return -1;

	}
	/* We support ARM specific ELF with 32 bit LSB support only */
	if(elf_header->e_ident[EI_CLASS] != ELFCLASS32){
		sw_printf("Not a 32-bit executable\n");
		return -1;

	}
	if(elf_header->e_ident[EI_DATA] != ELFDATA2LSB){
		sw_printf("Not in Little Endian format\n");
		return -1;

	}
	/* Check if the version is current ie EV_CURRENT */
	if(elf_header->e_ident[EI_VERSION] != EV_CURRENT){
		sw_printf("Not using the current version of ELF\n");
		return -1;

	}
	/*Check if the ELF file is a executable file ie of type ET_EXEC */
	if(elf_header->e_type != ET_EXEC){
		/*sw_printf("Not a executable file \n");*/
		if(elf_header->e_type == ET_REL)
		{
			rel = 1;
		}

	}
	/* Check if the architecture is ARM */
	if(elf_header->e_machine != EM_ARM){
		sw_printf("Unsupported process architecture\n");
		return -1;

	}
/* Check if the version is current (Not sure if the check here is mandatory)*/
	if(elf_header->e_version != EV_CURRENT){
		sw_printf("Not using the current version of the ELF file\n");
		return -1;

	}


	return rel;
}

/**
 * @brief Gets the number of loadable segments, ie segments with type PT_LOAD
 *
 * @param fp
 * @param elf_header
 * @param phdr_tab : pointer to the program header table
 *
 * @return number of loadable segments  on success, -1 on failure
 */
int get_number_of_loadable_segments(int fp, Elf32_Ehdr *elf_header, 
		Elf32_Phdr **phdr_tab)
{
	int ph_num,num_bytes,num_of_segments,num_of_loadable_segments = 0;
	Elf32_Phdr *phdr_table, *tmp_phdr;
	ph_num = elf_header->e_phnum;

	phdr_table = (Elf32_Phdr *)sw_malloc(ph_num * sizeof( Elf32_Phdr));

	if(phdr_table == NULL){
		sw_printf("No memory available\n");
		return -1;
	}
	num_bytes = 0;
	num_bytes = file_read(fp,(char *)phdr_table,
			(ph_num * sizeof(Elf32_Phdr)));
	if (num_bytes != (ph_num * sizeof( Elf32_Phdr))){
		sw_printf("Reading Programheader table from ELF file failed\n");
		return -1;
	} 
	/* Get the number of PT_LOAD segments */
	tmp_phdr = phdr_table;

	for(num_of_segments = 0;num_of_segments < elf_header->e_phnum;
			num_of_segments++)	{
		if(tmp_phdr->p_type == PT_LOAD)
			num_of_loadable_segments++;
		tmp_phdr = ( Elf32_Phdr *)((char *)tmp_phdr + 
				elf_header->e_phentsize);

	}
	*phdr_tab = phdr_table;
	return num_of_loadable_segments;

}


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
int elf_load( struct sa_config_t *conf)
{
	Elf32_Ehdr elf_header;
	Elf32_Ehdr *tmp_ehdr;
	u32 rel = 0;
	u32 num_bytes = 0,va_offset,status;
	int num_of_segments,fp = -1,ret = OTZ_OK,mode = FILE_READ;
	u32 *flag = NULL,*base_va = NULL;
	u32 offset,leftover_size, num_of_loadable_segments = 0;
	Elf32_Phdr *phdr_table = NULL, *tmp_phdr = NULL;
	char *leftover_data = NULL;

	conf->elf_flag = -1;
	ret = init_map_loader(conf);
	if(ret != 0)
	{
		sw_printf("Mapping PA to VA failed\n");
		ret = -1;
		goto out;
	}
	fp = file_open(conf->file_path,mode);
	if(fp == -1){
		sw_printf("unable to open file\n");
		ret =- 1;
		goto out;
	}

	rel = validate_elf_header(fp,&elf_header);
	tmp_ehdr = &elf_header;
	if(rel == 1){
		status = relocate_elf_loader(fp,tmp_ehdr,conf);
/* We dont care about whatever be the status here, because we need to clean up 
* the elf_loader task irrespective of the result */
		ret = status;
		if(status == -1 ||  status == 0)

			goto out;
	}
	if(rel == -1 ){
		ret = -1;
		goto out;
	}
	/* Here we are dealing with a executable...right now the executable
	 * support is not required.. */
	goto out;
	conf->entry_point = elf_header.e_entry;

	num_of_loadable_segments = get_number_of_loadable_segments(fp,
			&elf_header,&phdr_table);
	if(num_of_loadable_segments == -1){
		ret =- 1;
		goto out;
	}
	tmp_phdr = phdr_table;
/* array of u32 entries whose total number equals number_of_loadable_segments */
	flag = sw_malloc(num_of_loadable_segments * sizeof(u32) );
	if(flag  == NULL){
		sw_printf("No memory available\n");
		ret =- 1;
		goto out;
	}
	sw_memset(flag,0x0,num_of_loadable_segments * sizeof(u32));
	for(num_of_segments = 0;num_of_segments < elf_header.e_phnum;
			num_of_segments++){
		if(tmp_phdr->p_type == PT_LOAD)
		{
/* we need to check if the segment contains the .bss section 
 *  in which p_filesz < p_memsz */
			if(tmp_phdr->p_filesz == tmp_phdr->p_memsz)
			{


				va_offset = tmp_phdr->p_vaddr; 	

				offset = tmp_phdr->p_offset; 			
				file_seek(fp,0,FILE_SEEK_SET); 
				file_seek(fp,offset,FILE_SEEK_SET);
				num_bytes = 0;
				num_bytes = file_read(fp,((char *)base_va
							+ va_offset ),
						tmp_phdr->p_filesz);	
				if(num_bytes != tmp_phdr->p_filesz){
					sw_printf("Reading segment failed\n");
					ret = -1;
					goto out;
				}



			}
			else if (tmp_phdr->p_filesz < tmp_phdr->p_memsz){

				va_offset = tmp_phdr->p_vaddr;
				offset = tmp_phdr->p_offset; 
				file_seek(fp,0,FILE_SEEK_SET); 
				file_seek(fp,offset,FILE_SEEK_SET);
				num_bytes = 0;
				num_bytes = file_read(fp,((char *)base_va +
							va_offset),
						tmp_phdr->p_filesz);		
				if(num_bytes != tmp_phdr->p_filesz){
					sw_printf("Reading segment failed\n");
					ret = -1;
					goto out;
				}
				leftover_size = (tmp_phdr->p_memsz - 
						tmp_phdr->p_filesz + 1);
				leftover_data = (char *)sw_malloc(
						tmp_phdr->p_memsz -
						tmp_phdr->p_filesz+ 1);
				sw_memset(leftover_data,0x0,(tmp_phdr->p_memsz -
							tmp_phdr->p_filesz + 1));
				sw_memcpy(base_va,leftover_data,leftover_size);	
				sw_malloc_free(leftover_data);		



			}
		}

		tmp_phdr = ( Elf32_Phdr *)((char *)tmp_phdr + 
				elf_header.e_phentsize);
	}	


	/* TODO: 
	   1) Also, Add support for dynamic linking (Not needed for now )
	 */


out:
	/* Free the allocated memory and resources here */ 

	if(phdr_table){
		tmp_phdr = phdr_table;
		for(num_of_segments = 0,num_of_loadable_segments = 0;
				num_of_segments < elf_header.e_phnum;
				num_of_segments++){
			if(tmp_phdr->p_type == PT_LOAD){
				if(!flag[num_of_loadable_segments]){
					sw_vir_addr_free(((u32)base_va + 
								tmp_phdr->p_vaddr),
							tmp_phdr->p_memsz );

				}
				num_of_loadable_segments++;
			}
			tmp_phdr = ( Elf32_Phdr *)((char *)tmp_phdr + 
					elf_header.e_phentsize);

		}

		sw_malloc_free(phdr_table);
	}
	if(fp != -1){

		file_close(fp);

	}


	return ret;
}

