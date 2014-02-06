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
 * fat32 file system implementation
 */
#include<sw_types.h>
#include<fat32.h>


/**
* @brief 
*	Mount the FAT file system and retrieve its basic information
* @param dev
*	Pointer to the start of the file system image
* @return 0:File system is FAT32 and the mount process is successful
*         -1:Mount is unsuccessful 
*/
int mount_file_system(u8 *dev)
{
	bool is_fat32;
	boot_sector *bs;
	u8 *ptr = dev;  
	is_fat32 = chk_fat32_file_system(ptr + FILE_SYS_TYPE_OFF);
	if(is_fat32){ 
		bs = (boot_sector*)ptr;
		cal_region_offsets(bs,dev);
		file_head = NULL;
		fdes = 2;
		return 0;
	}
        else
		return -1;
}

/**
* @brief 
*	 Verify the type of FAT file system 
* @param file_sys_type
*	 Pointer to the location where the type of file system is stored
* @return true:indicates FAT32 file system
*	  false:indicates not FAT32 file system
*/
bool chk_fat32_file_system(u8 *file_sys_type)
{
	int i;
	char fs_type[FILE_SYS_TYPE_LENGTH] = "FAT32   ";
	u8 *type = file_sys_type;
	for(i=0;i<FILE_SYS_TYPE_LENGTH;i++){
		if(*type != fs_type[i] ){
			return false;
		}
		else
			type++;
	}
	return true;
}

/**
* @brief 
*	Function to calculate all the region offsets
* @param bs
*	Pointer to the boot sector structure which holds the information
*	of the boot sector of the file system 
* @param dev
*	Pointer to the start of the file system
*/
void cal_region_offsets(boot_sector *bs, u8 *dev)
{
	u32 next_clus;
	int clus_cnt = 0;
	sec_size = bs->bytes_per_sec;
	cluster_size = (bs->sec_per_clus)*sec_size;
	bs_sector = dev;
	fs_infosector = bs_sector + (bs->fs_info_sector * sec_size);
	backup_boot_sector = bs_sector + (bs->backup_boot_sector * sec_size);
	first_fat = bs_sector + (bs->reserved_sec_cnt * sec_size);
	if(bs->fat_cnt == MAX_FAT_CNT)
		second_fat = (bs->sectors_per_fat * sec_size) + first_fat;
	root_directory = bs_sector + ((bs->reserved_sec_cnt 
						+ (bs->fat_cnt * bs->sectors_per_fat) 
                        + (bs->root_dir_strt_cluster-2))*sec_size) ;   
	next_clus = bs->root_dir_strt_cluster;
	while(next_clus != END_OF_CLUSTER && next_clus != END_OF_ROOT_CLUSTER){
		next_clus = get_fat_table_entry(next_clus);
		clus_cnt++;
	}
	rt_dir_strt_clus = bs->root_dir_strt_cluster;
	root_dir_size = cluster_size * clus_cnt;
	rt_dir_entry_cnt = root_dir_size / DIR_ENTRY_LEN;
	nxt_free_cluster = *(u32*)(fs_infosector + NXT_FREE_CLUS_OFF);
}


/**
* @brief 
*	Function to open a specific file mentioned in the path 
*	based on the incoming modes
* @param file_path
*	Pointer to the location of the file path string
* @param flags
*	Flags indicating the modes in which the file is to be opened
*
* @return Non-zero:File descriptor of the opened file
	  Zero    :File open is unsuccessful
*/
int file_open(const char *file_path,int flags)
{
	const char *path = file_path;
	const char *temp_path,*delim_strt;
	char shrt_file_name[SHRT_FILE_NAME_LEN];
	char long_file_name[LONG_FILE_NAME_LEN];
	int len = 0,fl_des = 0,crt_flag,i;
	int delim_cnt = 0;
	int mode;
        int extn_len_cnt = 0;
	int seq_num = 1;
	bool is_file_found;
	dir_entry *entry = NULL;
	file_info *info;
	u8 *pwd = root_directory;
	u32 strt_cluster = rt_dir_strt_clus;
	bool is_long_file_name = false;	

	sw_memset(long_file_name,SPACE_VAL,LONG_FILE_NAME_LEN);
	delim_cnt = find_depth(file_path);
	
	path = file_path;
	for(i=0;i<delim_cnt;i++){
		if(*path == DELIMITER){
			delim_strt = path;
			path++;
		}
		while((*path != EXTN_DELIMITER) && (*path != '\0') 
			&& (*path != DELIMITER) && (len < LONG_FILE_NAME_LEN)){
			long_file_name[len] = *path; 
			path++; 
			len++;
		}
		temp_path = path;
		if(*temp_path == EXTN_DELIMITER){
			temp_path++;
			while(*temp_path != DELIMITER && *temp_path != '\0'){
				extn_len_cnt++;
				temp_path++;
			}
		}
		if(len > FILE_NAME_SHRT_LEN || extn_len_cnt > FILE_NAME_EXTN_LEN)
			is_long_file_name = true;

		if(is_long_file_name){
			path = delim_strt;
			len = 0;
			if(*path == DELIMITER)
				path++;
			while(len < LONG_FILE_NAME_LEN  && *path != '\0'
                              && *path != DELIMITER){
                             	long_file_name[len] = *path;
                                path++;
                                len++;
                        }
			long_file_name[len] = '\0';
			if(entry){
				sw_free(entry);
				entry = NULL;
			}
			is_file_found = get_dir_entry(long_file_name,&entry,
										  pwd,strt_cluster,true);				
		}
		else{
			len = FILE_NAME_SHRT_LEN;
			while(len < SHRT_FILE_NAME_LEN  && *path != '\0' 
			      && *path != DELIMITER){ 
				if(*path == EXTN_DELIMITER)
					path++;
				long_file_name[len] = *path;
				path++;
				len++;
			}
			convert_to_uppercase(long_file_name); 
			if(entry){
				sw_free(entry);
				entry = NULL;
			}
			is_file_found = get_dir_entry(long_file_name,&entry,
										  pwd,strt_cluster,false);
		}
		if((is_file_found) & (i != delim_cnt - 1)){ 
			strt_cluster = (entry->strt_clus_hword)<<16 | 
				       (entry->strt_clus_lword);
			pwd = cluster_to_memory_addr(strt_cluster);
			len = 0;
			extn_len_cnt = 0;
			sw_memset(shrt_file_name,SPACE_VAL,SHRT_FILE_NAME_LEN);
			sw_memset(long_file_name,SPACE_VAL,LONG_FILE_NAME_LEN);
			is_long_file_name = false;
		}		
	}
	if(is_file_found){
		if(flags & FILE_WRITE){
			if(chk_file_lock(file_path) == -1)
				flags = FILE_READ;				
			if(entry->attr & ATTR_READ){
				sw_printf("Cannot open the file in write mode\n");
				return -1;
			}
		}
		info = (file_info*)sw_malloc(sizeof(file_info));
                fl_des = retrieve_file_info(info,entry,flags,
											dir_file_offset,file_path);
	}  
	else{
              	if((flags & FILE_CREATE_NEW) || (flags & FILE_CREATE_ALWAYS)
                   || (flags & FILE_WRITE)){
                	if(is_long_file_name){
				get_short_file_name(long_file_name,shrt_file_name,
									(char)seq_num);
				if(get_dir_entry(shrt_file_name,NULL,
								 pwd,strt_cluster,false) == true){
					while(get_dir_entry(shrt_file_name,NULL,
										pwd,strt_cluster,false)){
						seq_num++;
						get_short_file_name(long_file_name,
											shrt_file_name,'seq_num');
					}
				}
				convert_to_uppercase(shrt_file_name);
				crt_flag = create_file(long_file_name,
									   shrt_file_name,strt_cluster,&entry);
			}
			else
				crt_flag = create_file(NULL,long_file_name,strt_cluster,&entry);
                        if(crt_flag == 0)
				sw_printf("File creation success\n");
			info = (file_info*)sw_malloc(sizeof(file_info));
			fl_des = retrieve_file_info(info,entry,flags,
										dir_file_offset,file_path);
                }
	  	else
			return -1;
        }
	return fl_des;  
}

/**
* @brief 
*	Function to check whether a given file name exists in the file system
*	and if so retrieve the corresponding short file name directory entry
* @param fname
*	Pointer to the start of the file name which needs to be searched
* @param entry
*	Pointer to a pointer of the directory entry structure in which the 
*	short file name directory entry content is to be stored
* @param strt
*	Pointer to the start of the region where the file/directory needs 
*	to be searched
* @param strt_clus
*	Starting cluster of the directory in which the file is assumed 
*	to be present 
* @param is_lfn
*	Boolean value which indicates whether the file is short or long file
* @return 
*  	true: if the file/dir is present	
*	false:if it is not present in the respected path
*/
bool get_dir_entry(char *fname,dir_entry **entry,u8 *strt,
				   u32 strt_clus,bool is_lfn)
{
	u8 *dir_strt = strt;
	dir_entry *dir_ptr;
	u8 chk_sum;
	bool file_found = false;
	int i = 0;
	int tmp_cnt = 0;
	int lfn_len = 0;
	char temp_lfn[LONG_FILE_NAME_LEN];
        sw_memset(temp_lfn,SPACE_VAL,LONG_FILE_NAME_LEN);
	u32 next_clus = strt_clus;
	
	while(!((next_clus == END_OF_CLUSTER) && (i == MAX_DIR_ENTRY_CNT + 2) && 
			next_clus != END_OF_ROOT_CLUSTER)){
		if((i - MAX_DIR_ENTRY_CNT == 1) && (next_clus != END_OF_CLUSTER) &&
			next_clus != END_OF_ROOT_CLUSTER){
                        next_clus = get_fat_table_entry(next_clus);
                        if(next_clus == END_OF_CLUSTER || 
						   next_clus == END_OF_ROOT_CLUSTER)
								break;
						dir_strt = cluster_to_memory_addr(next_clus);
                        if(next_clus != END_OF_CLUSTER && next_clus != 
										END_OF_ROOT_CLUSTER)
                                i = 0;
                }
		if(is_lfn){
			if(*(dir_strt + ATTR_OFF) == ATTR_LONG_FNAME){
				lfn_len = get_long_file_name(temp_lfn,&dir_strt,&tmp_cnt);
				if(sw_memcmp(temp_lfn,fname,lfn_len) == 0)
					file_found = true;
			}
		}
		else{
			if(sw_memcmp(fname,dir_strt,SHRT_FILE_NAME_LEN) == 0){
				file_found = true;
			}
		}
		if(file_found)
			break;
		dir_strt += DIR_ENTRY_LEN;
		//TODO
		if(tmp_cnt != 0)
			i += tmp_cnt;
		else
			i++;
                //i= (tmp_cnt != 0) ? i += tmp_cnt : i++;
		tmp_cnt = 0;
	}
	if(file_found){
		if(entry != NULL){
			dir_ptr = (dir_entry*)sw_malloc(sizeof(dir_entry));
			sw_memcpy(dir_ptr,dir_strt,DIR_ENTRY_LEN);
			*entry = dir_ptr;
			dir_file_offset = dir_strt;
		}
		return true;
	}
	else
		return false;
}

/**
* @brief 
*	Function to retrieve the long file name given its
*	end of the long file name entry
* @param fname
*	Pointer to a location where the retrieved long file name starts
* @param dir_strt
*	Pointer to the memory location where the last component of the long 
*	file name 
*	resides
* @param count
*	Count representing the number of directory entries needed for the 
*	long file name
* @return 
* 	Length of the long file name
*/
int get_long_file_name(char *fname,u8 **dir_strt,int *count)
{
	lfn_entry *lf_ptr;
	int index = 0;
	int counter = 0;
	u8 chk_sum;

	lf_ptr = (lfn_entry *)(*dir_strt);
	counter = lf_ptr->ord_field & LONG_FNAME_MASK;
	*count = counter;
	while((lf_ptr->ord_field > 1) && 
	     ((lf_ptr->ord_field & LONG_FNAME_MASK) > 1))
		lf_ptr++;               
	*dir_strt = (u8 *)lf_ptr + DIR_ENTRY_LEN;
	chk_sum = calc_check_sum(*dir_strt);
	while(counter--){
		if(*((u8 *)lf_ptr + CHECK_SUM_OFF) == chk_sum){
			if(lfn_entry_to_name(lf_ptr,fname,&index) == 1)
				break;  
			lf_ptr--;
		}
	}
	return (index - 1);
}

/**
* @brief 
*	Function to get the long file name correspoding to a directory slot
* @param entry
*	Pointer to the slot where a long file component resides
* @param lfname
*	Pointer to the start of the long file name
* @param index
*	Pointer to the offset of the long file name where the retrieved name 
*	from the given slot is going to be stored
* @return 
*	0:if the file name doesnt end in the given slot
*	1:if the file name ends in the given slot
*/
int lfn_entry_to_name(lfn_entry *entry,char *lfname,int *index)
{
	int i;
	for(i = 0; i <= 8; i += 2){
		if(entry->fname0_4[i] == 0)
			return 1;
		lfname[*index] = entry->fname0_4[i];
		(*index)++;
	}
	for(i = 0; i <= 10; i += 2){
		if(entry->fname6_11[i] == 0)
                        return 1;
		lfname[*index] = entry->fname6_11[i];
		(*index)++;
	}
	for(i = 0 ; i <= 2; i += 2){
		if(entry->fname12_13[i] == 0)
                        return 1;
		lfname[*index] = entry->fname12_13[i];
		(*index)++;
	}
	return 0;
}


/**
* @brief 
*	Function to assign a file descriptor and necessary information to a file
*	structure inorder to pursue subsequent operations
* @param finfo
*	Pointer to a file structure where the file information is stored
* @param entry
*	Pointer to the directory entry of the file/directory 
* @param mode
*	Mode in which the file is requested to be opened
* @return 
*	File descriptor of the file
*/
int retrieve_file_info(file_info *finfo,dir_entry *entry,u8 mode,
					   u8 *dir_offset,const char *path)
{
	file_info *temp;
	if(file_head == NULL)
		file_head = finfo;
	else{
		temp = file_head;
                while(temp->next != NULL)
                        temp = temp->next;
                temp->next = finfo;

	}
	finfo->fd = fdes = fdes + 1;
	finfo->file_path = sw_malloc(sizeof(char) * (sw_strlen(path) + 1));
	sw_strcpy(finfo->file_path,path);
	finfo->entry = entry;
	finfo->strt_cluster = (entry->strt_clus_hword)<<16 | 
			      (entry->strt_clus_lword);
	finfo->cur_cluster = finfo->strt_cluster;
	finfo->cur_offset = 0;
	finfo->file_size = entry->size;
	finfo->mode = mode;
	finfo->bytes_traversed = 0;
	finfo->dir_entry_offset = dir_offset;
	finfo->next = NULL;
	
	return(finfo->fd);
}

/**
* @brief 
*	Function to read the contents of a file
* @param fd
*	File descriptor of the file in which read operation needs to be performed
* @param buffer
*	Pointer to the start of the buffer where the contents read are stored
* @param bytes_to_read
*	Number of bytes to be read from the file
* @return 
*	Number of bytes successfully read
*/
int file_read(int fd,char * buffer,int bytes_to_read)
{
	file_info *temp = file_head;
	u8 *file_cont;
	int bytes_to_copy;
	int tmp_offset;
	int bytes_read = 0;
	int tmp_bytes_cnt = 0;

	while(temp != NULL){
		if(temp->fd == fd)
			break;
		temp = temp->next;
	}
	if(temp->entry->attr != ATTR_DIR){
		tmp_offset = bytes_to_read > (temp->file_size - temp->bytes_traversed) ?
		     	     temp->file_size - temp->bytes_traversed : bytes_to_read; 
		tmp_bytes_cnt = tmp_offset;
	}
	else
		tmp_offset = tmp_bytes_cnt = bytes_to_read;
			
	while(tmp_offset > 0){
		if(temp->cur_cluster != END_OF_CLUSTER && 
		   temp->cur_cluster != END_OF_ROOT_CLUSTER){  
			if(cluster_size - temp->cur_offset != 0){
				file_cont = cluster_to_memory_addr(temp->cur_cluster) 
					    + temp->cur_offset;					
				bytes_to_copy = cluster_size - (tmp_bytes_cnt - bytes_read);
				bytes_to_copy =  bytes_to_copy > 0 ? 
						 tmp_bytes_cnt - bytes_read: 
						 cluster_size;
				bytes_to_copy = cluster_size 
								- temp->cur_offset > bytes_to_copy ?
								bytes_to_copy : cluster_size - temp->cur_offset;
				sw_memcpy(buffer + bytes_read,file_cont,bytes_to_copy);
				tmp_offset -= bytes_to_copy;
				temp->cur_offset += bytes_to_copy;
				bytes_read += bytes_to_copy;
			}
			else{
				temp->cur_cluster = get_fat_table_entry(temp->cur_cluster);
				temp->cur_offset = 0;
			}
		
		}	
		else{
			break;
		}
	}	
	temp->bytes_traversed += bytes_read;
	return bytes_read;
}

/**
* @brief 
*	Function to write into the given file with the contents given
* @param fd
*	File descriptor of the file 
* @param buffer
*	Pointer to the start of the contents which needs to be written into
*	the file
* @param bytes_to_write
*	Indicates the number of bytes which needs to be written into the file
* @return 
*	Integer representing number of bytes successfully written into the file
*/
int file_write(int fd, const char *buffer,int bytes_to_write)
{
	file_info *temp = file_head;
	u8 *file_cont;
	int tmp_offset = bytes_to_write;
	int alloc_cluster_cnt = 0;
	int bytes_to_copy;
	int bytes_written = 0;
	u32 new_cluster;
	u32 prev_cluster;	

	while(temp != NULL){
		if(temp->fd == fd)
			break;
		temp = temp->next;	
	}
	if(temp->mode == FILE_READ){
		sw_printf("File opened in read only mode\n");
		return 0;
	}
	while(tmp_offset > 0){
		if((temp->cur_cluster != 0) && (temp->cur_cluster != END_OF_CLUSTER) 
			&& (temp->cur_cluster != END_OF_ROOT_CLUSTER) && 
			(cluster_size - temp->cur_offset != 0)) {
			file_cont = cluster_to_memory_addr(temp->cur_cluster) 
				    + temp->cur_offset;
			bytes_to_copy = cluster_size - (bytes_to_write - bytes_written);
			bytes_to_copy = bytes_to_copy > 0 ?  
					bytes_to_write - bytes_written: 
					cluster_size;
			bytes_to_copy = cluster_size - temp->cur_offset > bytes_to_copy ?
					bytes_to_copy : cluster_size - temp->cur_offset;
			sw_memcpy(file_cont,buffer + bytes_written,bytes_to_copy);
			bytes_written += bytes_to_copy;
			temp->cur_offset += bytes_to_copy;
			tmp_offset -= bytes_to_copy; 
			prev_cluster = temp->cur_cluster;
			if(temp->cur_offset == cluster_size)
				temp->cur_cluster = get_fat_table_entry(temp->cur_cluster);   
			if(temp->cur_cluster != 0 && temp->cur_cluster != END_OF_CLUSTER
			   && temp->cur_cluster != END_OF_ROOT_CLUSTER
			   && tmp_offset != 0)
				temp->cur_offset = 0;
		}
		else{
			new_cluster = allocate_cluster();
			if(alloc_cluster_cnt == 0 && temp->cur_cluster == 0 
			   && temp->strt_cluster == 0){
				temp->strt_cluster = new_cluster;
				*(u16*)(dir_file_offset + STRT_CLUS_LOW_OFF) = 
					temp->strt_cluster & LOW_CLUSWORD_MASK;
				*(u16*)(dir_file_offset + STRT_CLUS_HIGH_OFF) = 
					(temp->strt_cluster & HIGH_CLUSWORD_MASK) >> 16;

			}
			else 
				write_fat_table(prev_cluster,new_cluster);
			temp->cur_cluster = new_cluster;
			temp->cur_offset = 0;
			alloc_cluster_cnt++;       
		}    
	}  
	temp->bytes_traversed += bytes_written; 
	if(temp->file_size < bytes_written){
		*(dir_file_offset + FILE_SIZE_OFF) = (u32)(bytes_written);
		temp->file_size = (u32)(bytes_written);
	}	
	if(alloc_cluster_cnt > 0)
		write_fat_table(new_cluster,END_OF_CLUSTER);

	return bytes_written;   
}

/**
* @brief 
*	Close the opened file based on the file descriptor and 
*	free the file structure pointer
* @param fd
*	File descriptor of the file which needs to be closed
* @return 
*	Zero on successful closing of the file
*/
int file_close(int fd)
{
	file_info *temp = file_head,*prev;
	while(temp != NULL){
		if(temp->fd == fd)
			break;
		prev = temp; 
		temp = temp->next;
	}
	if(temp == NULL) 
                return -1;
	if(temp == file_head){
		file_head = temp->next;
		sw_malloc_free(temp->file_path);
		sw_malloc_free(temp->entry);
//		sw_malloc_free(temp->dir_entry_offset);
		sw_malloc_free(temp);
	}
	else{
		prev->next = temp->next;
		sw_malloc_free(temp->file_path);
		sw_malloc_free(temp->entry);
		sw_malloc_free(temp->dir_entry_offset);
		sw_malloc_free(temp);   
	}
	return 0;
}
 
/**
* @brief 
*	Function to create a file if the file doesnt exists and the create 
*	flag is set
* @param lfname
*	Pointer to the start of the long file. NULL in case of short file
*	name creation 
* @param shrt_name
*	Pointer to the start of the short file name. In case of long file names
*	the corresponding short file name is found out.
* @param clus
*	Starting cluster of the directory in which the file is going to be
*	created
* @param entry
*	Pointer to a pointer of the directory entry structure in which the 
*       short file name directory entry content is to be stored
* @return 
*	On successful creation returns zero
*/
int create_file(char *lfname,char *shrt_name,u32 clus,dir_entry **entry)
{
	u8 *cur_dir_entry;
	u32 next_clus = clus;
	u32 new_clus;
	u32 prev_clus;
	int new_clus_cnt = 0;
	int i = 0,j = 0,k = 0;
	int req_ent_cnt = 0;
	int temp_cnt = 0;
	char *lfptr = lfname;
	bool is_gap_found = false;
	u8 chk_sum;
	dir_entry *dir_ptr;
	
	cur_dir_entry = cluster_to_memory_addr(next_clus);
	while(!is_gap_found){
        	if(k - MAX_DIR_ENTRY_CNT == 1){
			prev_clus = next_clus;
			next_clus = get_fat_table_entry(next_clus);
			cur_dir_entry = cluster_to_memory_addr(next_clus);
			if(next_clus == END_OF_CLUSTER || next_clus == END_OF_ROOT_CLUSTER){
				new_clus = allocate_cluster();
				write_fat_table(prev_clus,new_clus);
				new_clus_cnt++;
				k = 0;
			}
		}
		if(lfname != NULL){
			chk_sum = calc_check_sum(shrt_name);
			while(*lfptr != '\0'){
				lfptr++;
				i++;
			}
			req_ent_cnt = cal_req_dir_cnt(i);
			temp_cnt = req_ent_cnt;
			while(req_ent_cnt--){
				if(k - MAX_DIR_ENTRY_CNT == 1){
					req_ent_cnt = temp_cnt;
					break;
				}			
				if(*cur_dir_entry == FREE_DIR_ENTRY ||
				   *cur_dir_entry == DEL_DIR_ENTRY){
					cur_dir_entry += DIR_ENTRY_LEN;
					j++;
					k++;
				}
				else
					k++;				
			}
			if(j == temp_cnt){
				is_gap_found = true;
				fill_lng_fname_entries(lfname,cur_dir_entry,chk_sum,temp_cnt);
			}
			req_ent_cnt = temp_cnt;
			j = 0;
		}
		else{
			if(*cur_dir_entry == FREE_DIR_ENTRY ||
                           *cur_dir_entry == DEL_DIR_ENTRY)
				is_gap_found = true;
			k++;
		}
		if(is_gap_found){
			sw_memset(cur_dir_entry,0,DIR_ENTRY_LEN);
			sw_memcpy(cur_dir_entry,shrt_name,SHRT_FILE_NAME_LEN);
			*(cur_dir_entry + ATTR_OFF) = ATTR_ARCHIVE;
			dir_ptr = (dir_entry*)sw_malloc(sizeof(dir_entry));
			sw_memcpy(dir_ptr,cur_dir_entry,DIR_ENTRY_LEN);
			*entry = dir_ptr;
			dir_file_offset = cur_dir_entry;
		}
		if(new_clus_cnt > 0)
			write_fat_table(new_clus,END_OF_CLUSTER);	
		cur_dir_entry += DIR_ENTRY_LEN;
	}
	return 0;
}

/**
* @brief 
*	Function to put the long file name entries into the respective allocated
*	region
* @param fname
*	Pointer to the start of the long file name 
* @param entry
*	Starting memory location to the short file name directory entry
* @param chk_sum
*	Calculated check sum of the short file name corresponding to the
*	long file name
* @param count
*	Counter indicating the number of 32 byte entries needed for the whole 
*	file name to get accommodated
*/
void fill_lng_fname_entries(char *fname,u8 *entry,u8 chk_sum,int count)
{
	lfn_entry *lfn_ent = (lfn_entry *)entry;
	char *file_name = fname;
	u8 *lf_entry;
	bool end_of_name = false;
	int i,j;
	int max_ord_cnt;
	int ord_num = 1;
	lfn_ent = (u8 *)lfn_ent - DIR_ENTRY_LEN;
	while(count--){
		lf_entry = (u8 *)lfn_ent;
		sw_memset(lf_entry,0,DIR_ENTRY_LEN);
		*lf_entry = count - 1 == 0 ? ord_num : ord_num | LAST_ORD_FIELD_SEQ;
		*(lf_entry + ATTR_OFF) = ATTR_LONG_FNAME;
		*(lf_entry + CHECK_SUM_OFF) = chk_sum; 
		for(j = 0;j < 3;j++){
			switch(j){
				case 0:
					max_ord_cnt = LFN_FIRST_SET_CNT;
					lf_entry = (u8 *)lfn_ent + LFN_FIRST_OFF;
					break;
				case 1:
					max_ord_cnt = LFN_SEC_SET_CNT;
					lf_entry = (u8 *) lfn_ent + LFN_SIXTH_OFF;			
					break;
				case 2:
					max_ord_cnt = LFN_THIRD_SET_CNT;
					lf_entry = (u8 *) lfn_ent + LFN_TWELVETH_OFF;
					break;
			}	
			for(i = 0; i < max_ord_cnt; i++){
				if(*file_name == '\0'){
					if(!end_of_name){
						*(u16 *)lf_entry = LFN_TERM_MARK;
						end_of_name = true;
					}
					else
						*(u16 *)lf_entry = LFN_END_MARK;
				}
				else{			
					*(lf_entry) = *file_name;
					file_name++;
				}
				lf_entry += 2;			
			}
		}
		ord_num++;
                lfn_ent--;	
	}
}

/**
* @brief 
*	Function to seek the opened file pointer to the desired location
*	so that subsequent read and write operations are performed from 
*	the new value
* @param fd
*	File descriptor of the file in which the seek operation needs to be 
*	performed
* @param offset
*	Offset bytes indicating the number of bytes which needs to be moved
* @param whence
*	Flag denoting the position from which the seek operation needs to be
*	performed
* @return 
*	Number of bytes traversed from the beginning of the file to the current 
*	position
*/
int file_seek(int fd,int offset,int whence)
{
	int seek_mode = whence;
 	file_info *temp = file_head;
	int off_bytes;
	int tmp_off = offset;
	u32 cur_cluster;
	u32 strt_cluster;	
	int bytes_traversed = 0;
	int bytes_to_copy;
	int clus_cnt = 0;     	

	while (temp != NULL){
		if(temp->fd == fd)
			break;
		temp = temp->next;
	}		
	if(offset < 0){
		if(seek_mode == FILE_SEEK_CUR){
			tmp_off = temp->bytes_traversed + offset;
			seek_mode = FILE_SEEK_SET;
		}
		else if(seek_mode == FILE_SEEK_END){
			tmp_off = temp->file_size + offset;
		        seek_mode = FILE_SEEK_SET;
		}
		else
			return -1;
	}
		
	switch(seek_mode){
		case FILE_SEEK_SET:
			strt_cluster = temp->strt_cluster;
			temp->cur_offset = 0;
			off_bytes = get_seek_off(temp,strt_cluster,tmp_off);
			break;
		case FILE_SEEK_CUR:
			strt_cluster = temp->cur_cluster;
			off_bytes = (temp->bytes_traversed - temp->cur_offset) +
			            get_seek_off(temp,strt_cluster,tmp_off);
			break;
		case FILE_SEEK_END:
			cur_cluster = temp->strt_cluster;
                        while(cur_cluster != END_OF_CLUSTER && 
							  cur_cluster != END_OF_ROOT_CLUSTER){
                                temp->cur_cluster = cur_cluster;
                                cur_cluster = get_fat_table_entry(cur_cluster);
                                clus_cnt++;
                        }
                	temp->cur_offset = temp->file_size - 
					   (cluster_size * (clus_cnt - 1));
			off_bytes = get_seek_off(temp,temp->cur_cluster,tmp_off) + 
				    (cluster_size * (clus_cnt - 1));
			break;
	}
	temp->bytes_traversed = off_bytes;
	return off_bytes;
}

/**
* @brief 
*	Function to rename a file/directory
* @param old
*	Constant pointer to the start of the path where the file which 
*	needs to be renamed is located
* @param new
*	Constant pointer to the start of the path where the new name of the 
*	file/directory is situated
* @return 
*	zero on successful renaming
*	-1 if the new name already exists 
*/
int file_rename(const char *old,const char *new)
{
	int fd_old;
	int fd_new;
	file_info *temp_old;
	file_info *temp_new;

	fd_new = file_open(new,FILE_READ);
	if(fd_new == -1){
		fd_old = file_open(old,FILE_READ);
		fd_new = 0;
		fd_new = file_open(new,FILE_CREATE_NEW);
		temp_old = temp_new = file_head;
		while(temp_old->fd != fd_old)
			temp_old = temp_old->next;
		while(temp_new->fd != fd_new)
			temp_new = temp_new->next;
		sw_memcpy(temp_new->dir_entry_offset + ATTR_OFF,
			  temp_old->dir_entry_offset + ATTR_OFF,FILE_STAT_LEN);
		*(temp_old->dir_entry_offset) = DEL_DIR_ENTRY;
		file_close(fd_old);
		file_close(fd_new);
		return 0;
	}
	else{
		sw_printf("Already exists\n");
		return -1;
	}

}

/**
* @brief 
*	Function to create a directory
* @param path
*	Pointer to the start of the path which represents the directory hierarchy
* @return 
*	On successful creation:File descriptor of the directory 
*	-1:If the creation failed or if it already exists
*/
int create_dir(const char *path)
{
	int fd;
	fd = file_open(path,FILE_READ);
	if(fd != -1)
		fd = file_open(path,FILE_CREATE_NEW);
	else
		return -1;
	return fd;
}

/**
* @brief 
*	Function to open a directory 
* @param path
*	Pointer to the start of the path representing the directory hierarchy
* @return 
*	file descriptor assigned to the directory
*/
int open_dir(const char *path)
{
	int fd_dir;	
	fd_dir = file_open(path,FILE_READ);
	return fd_dir;		
}

/**
* @brief 
*	Function to list the directory contents 
* @param path
*	Pointer to the start of the path representing the directory hierarchy
* @return 
*	Number of directories/files present in the given directory
*/
int list_dir(const char *path)
{
	int fd_dir;
	file_stat *finfo;
	int file_count = 0;
	fd_dir = open_dir(path);
	
	finfo = read_dir(fd_dir);
	if(finfo != NULL){
		file_count++;
		//sw_printf("%s\n",(finfo->lname) != SPACE_VAL ? finfo->lname : 
		//		  finfo->name);
		sw_printf("%s\n",finfo->name);
		while(finfo != NULL){
			finfo = read_dir(fd_dir);
			if(finfo == NULL)
				break;
			sw_printf("%s\n",finfo->name);
			file_count++;
		}
	}
	return file_count;
}

/**
* @brief 
*
* @param fd
*
* @return 
*/
file_stat * read_dir(int fd)
{
        file_info *temp;
	file_stat *stat;
	dir_entry *entry;
	
        char lfn[LONG_FILE_NAME_LEN];
        char sfn[FULL_SHRT_NAME_LEN];
        int lfn_len = 0;
        int ent_cnt = 0;
        int lent_cnt = 0;
        int index = 0;
        int index1 = 0;
	int read_count;
        u8 *cur_entry;
	u8 *strt_entry;
        u32 cur_clus;
        bool set_extn_lim = false;
        temp = file_head;
	
	while(temp != NULL){
		if(temp->fd == fd)
			break;
		temp = temp->next;
	}		
    	cur_entry = cluster_to_memory_addr(temp->cur_cluster) 
											+ temp->cur_offset;	
	
	entry = sw_malloc(sizeof(dir_entry));
	sw_memset(entry,SPACE_VAL,DIR_ENTRY_LEN);
	read_count = file_read(fd,(char*)entry,DIR_ENTRY_LEN);
        if(read_count == 0)
		return NULL;
	while(*(entry->name) == FREE_DIR_ENTRY || *(entry->name) == DEL_DIR_ENTRY
	      || *(entry->name) == DOT_DIR_ENTRY){
		cur_entry = cluster_to_memory_addr(temp->cur_cluster) 
											+ temp->cur_offset;
		read_count = file_read(fd,(char*)entry,DIR_ENTRY_LEN);
        	if(read_count == 0)
                	return NULL;
	}
	if(entry->attr == ATTR_LONG_FNAME){
		lfn_len = get_long_file_name(lfn,&cur_entry,&lent_cnt);
		lfn[lfn_len + 1] = '\0';
		sw_memset(entry,SPACE_VAL,DIR_ENTRY_LEN);
		temp->cur_offset += (lent_cnt - 1) * DIR_ENTRY_LEN;			
		read_count = file_read(fd,entry,DIR_ENTRY_LEN);
	}
	
	index = index1 = 0;
	set_extn_lim = false;
	while(index1 < SHRT_FILE_NAME_LEN){
		if((index1 == FILE_NAME_SHRT_LEN ||
		   (*(u8 *)(entry + index1) == SPACE_VAL)) &&
		   (*(u8*)(entry + FILE_NAME_SHRT_LEN) != SPACE_VAL)){
			if(!set_extn_lim){
				sfn[index] = EXTN_DELIMITER;
				set_extn_lim = true;
				index++;
			}
		}
		while(*(u8 *)(entry + index1) == SPACE_VAL)
			index1++;
		sfn[index] = *(u8 *)(entry + index1);
		index1++;
		index++;
	}
	*(sfn + (index + 1)) = '\0';
	stat = sw_malloc(sizeof(file_stat));
	if(*lfn != SPACE_VAL){
		stat->lname = sw_malloc((lfn_len + 1) * sizeof(char));
		sw_strcpy(stat->lname,lfn);
		sw_strcpy(stat->name,entry->name);
	}
	else{
		sw_strcpy(stat->name,sfn);
		stat->lname = SPACE_VAL;
	}
	stat->size = entry->size;
	stat->attr = entry->attr;
	sw_memset(sfn,SPACE_VAL,SHRT_FILE_NAME_LEN);
	sw_memset(lfn,SPACE_VAL,LONG_FILE_NAME_LEN);
	sw_malloc_free(entry);
	return stat;
}

/**
* @brief 
*	Function to find an unused cluster from the FAT table
* @return
*	Newly allocated cluster for the file
*/
u32 allocate_cluster()
{
	u32 new_cluster;
	u8 *fat_offset;
	u8 empty_cluster[BYTES_PER_CLUSTER_ENTRY];
	sw_memset(empty_cluster,0,BYTES_PER_CLUSTER_ENTRY);
	fat_offset = first_fat + ((nxt_free_cluster + 1)* BYTES_PER_CLUSTER_ENTRY);
	while(!(sw_memcmp(fat_offset,empty_cluster,BYTES_PER_CLUSTER_ENTRY) == 0 )){
		fat_offset += BYTES_PER_CLUSTER_ENTRY;
	}
	new_cluster = (fat_offset - first_fat)/BYTES_PER_CLUSTER_ENTRY;
	nxt_free_cluster = new_cluster;
	*(u32 *)(fs_infosector + NXT_FREE_CLUS_OFF) = new_cluster;
	return new_cluster;
}

/**
* @brief 
*	Function to get the corresponding physical address from the given cluster
* @param cluster
*	Cluster value for which the address is to be obtained
* @return 
*	Memory address of the incoming cluster
*/
u8 * cluster_to_memory_addr(u32 cluster)
{
	u8 *clus_addr;
	clus_addr = root_directory + sec_size + ((cluster - 3) * sec_size);
	return clus_addr;
}

/**
* @brief 
*	Function to retrieve the next cluster value in a cluster chain from 
*	the FAT table
* @param cluster
*	Cluster whose next cluster value is to be obtained 
* @return 
*	Corresponding FAT table entry for the given cluster
*/
u32 get_fat_table_entry(u32 cluster)
{
	u32 fat_entry;
	fat_entry = *(u32*)(first_fat + (cluster * 4));
	return fat_entry;
}

/**
* @brief 
*	Function to write into the FAT table after assigning an empty cluster 
*	to a file
* @param cluster
*	Cluster number for which the entry has to be made
* @param value
*	Next cluster value of the cluster number provided
*/
void write_fat_table(u32 cluster,u32 value)
{
	u8 *offset;
	offset = first_fat + (cluster * 4);
	*(u32*)(offset) = value;
}

/**
* @brief 
*	Function to convert the incoming short file name to its corresponding upper 
*	case letters 
* @param file_name
*	File name whose values are to be converted	
*/
void convert_to_uppercase(char * file_name)
{
	char *name = file_name;
	int count = 0;
	while(*name != '\0' && count++ < SHRT_FILE_NAME_LEN){
		*name = (*name >= 'a' && *name <= 'z') ? *name - ASCII_DIFF : *name;
		name++; 
	}
}

/**
* @brief 
*	Function to calculate the number of iterations needed to reach the file from
* 	the given starting path
* @param file_path
*	Pointer to the starting of the file path
* @return 
*	Count of directories which resides before the file/directory
*	from the given path
*/
int find_depth(const char *file_path)
{
	char *path = file_path;
	int count = 0;
	while(*path != '\0'){
		if(*path == DELIMITER)
			count++;
		path++;
	}
	return count;
}

/**
* @brief 
*	Function to move the current cluster and its offset value to 
*	the value specified in the offset from the starting cluster
	provided
* @param file_ptr
*	File structure pointer pointing to the opened file
* @param strt_cluster
*	Starting cluster value from which the seek operation has to 
*	be performed
* @param offset
*	Offset bytes for the seek operation
* @return 
*	Number of bytes traversed from the beginning of the file
*/
int get_seek_off(file_info * file_ptr,u32 strt_cluster,int offset)
{ 
	file_info *temp = file_ptr;
	u32 cur_cluster = strt_cluster;
	int tmp_off = offset;
	int off_bytes;
	u32 prev_cluster;
	int clus_cnt = 0;
	int alloc_clus_cnt = 0;
	int bytes_traversed = 0;
	int bytes_to_copy;

	if((offset + temp->cur_offset < cluster_size) && strt_cluster != 0){
		temp->cur_offset += offset;
		temp->cur_cluster = strt_cluster;
		return temp->cur_offset;
	}
	else{
		while(tmp_off > 0){
			if(cluster_size - temp->cur_offset != 0 && 
			   cur_cluster != 0){
				temp->cur_cluster = cur_cluster;
				prev_cluster = cur_cluster;
				bytes_to_copy = cluster_size -
						(offset - bytes_traversed);
				bytes_to_copy = bytes_to_copy > 0 ?
						offset - bytes_traversed : 
						cluster_size;
				bytes_to_copy = cluster_size - temp->cur_offset >
 						bytes_to_copy ? bytes_to_copy :
						cluster_size - temp->cur_offset;				
				bytes_traversed += bytes_to_copy;
				tmp_off -= bytes_to_copy;
				temp->cur_offset += bytes_to_copy;
				if(temp->cur_offset == cluster_size)
					cur_cluster = get_fat_table_entry(cur_cluster);
				if(cur_cluster != 0 && cur_cluster != END_OF_CLUSTER 
				   && cur_cluster != END_OF_ROOT_CLUSTER
				   && tmp_off != 0)
					temp->cur_offset = 0;
				clus_cnt++;                     
			}
			else{ 
				if(cur_cluster == END_OF_CLUSTER || 
				   cur_cluster == 0 ||
				   cur_cluster == END_OF_ROOT_CLUSTER){
					cur_cluster = allocate_cluster();           
					if(alloc_clus_cnt != 0)
						write_fat_table(prev_cluster,cur_cluster);
					alloc_clus_cnt++;
				}
				temp->cur_offset = 0;
			}
		}
		if(alloc_clus_cnt > 0)
			write_fat_table(temp->cur_cluster,END_OF_CLUSTER);
		off_bytes = ((clus_cnt -1)*cluster_size) + temp->cur_offset;
	}
	return off_bytes;
}

/**
* @brief 
*	Function to extract the short file name given its long file name
* @param lfname
*	Pointer to the start of the long file name string	
* @param sfname
*	Pointer to the start of the short file name in which this function 
*	extracts and stored the short file name value
*/
void get_short_file_name(char * lfname,char *sfname,char seq_num)
{
	char *start = lfname;
	char *end = lfname;
	int len = 0;
	int extn_cnt = 0;
	sw_memset(sfname,SPACE_VAL,SHRT_FILE_NAME_LEN);
	while(*end != '\0')
		end++;
	while(*end != EXTN_DELIMITER && end != start)
                end--;
	while(*start != '\0' && len != 6){
		if(*start == EXTN_DELIMITER){ 
			if(start != end)
				start++;
			else
				break;
		}
		sfname[len] = *start;
		start++;
		len++;		
	}
	sfname[FNAME_EXTN_SEP_OFF] = TILDE;
	sfname[FNAME_SEQ_NUM_OFF] = seq_num; 
	len = FILE_NAME_SHRT_LEN;
	while(*end != DELIMITER && *end != '\0' && extn_cnt != FILE_NAME_EXTN_LEN
	      && end != lfname){
		if(*end == EXTN_DELIMITER)
			end++;
		sfname[len] = *end;
		len++;
		end++;
		extn_cnt++;
	}
}

/**
 * @brief 
 *
 * @param path
 *
 * @return 
 */
int chk_file_lock(const char *path)
{
	file_info *temp; 
	temp = file_head;
	while(temp != NULL){
		if(sw_strcmp(temp->file_path,path) == 0 &&
		   (temp->mode & FILE_WRITE)){
			sw_printf("File already opened in write mode\n");
			return -1;
		}
		temp = temp->next;
	}
	return 0;
}


/**
* @brief 
*	Function to calculate the checksum for the long file name 
*	based on its short name entry
* @param dir_entry
*	Pointer to the start of the short file name corresponding
*	to a long file name
* @return 
*	checksum value of the short file name
*/
u8 calc_check_sum(u8 * dir_entry)
{
	int i;
	u8 sum = 0;
	for(i=0;i < SHRT_FILE_NAME_LEN; i++){
		sum = ((sum & 1) << 7) + (sum >> 1) + dir_entry[i];
	}
	return sum;
}

/**
* @brief 
*
* @param file_name_len
*
* @return 
*/
int cal_req_dir_cnt(int file_name_len)
{
	int req_cnt = 0;
	int len = file_name_len;
	while(len >= LFN_LEN_PER_ENTRY){
		len -= LFN_LEN_PER_ENTRY;
		req_cnt++;
	}
	req_cnt = len != 0 ? req_cnt++ : req_cnt;
	return req_cnt;
}

/**
* @brief
* file_truncate - function cause the regular file named by path 
*            	  to be truncated to a size of precisely length bytes
*
* @param tr_pf -  truncates the file whose name is specified in filename
* @param tr_lgh - The desired truncate size of the file in bytes
*
* @return - If the file contents is successfully truncated, a zero value is 
*           returned.On failure, non-Zero is returned
*/
int file_truncate(const char* tr_pf,int tr_lgh)
{
	u32 new_clstr,prev_clstr,*tr_addr;
	u8 *cur_addr;
	int sk_ted,end_size,tmp_tsize,wt_zo=0,zo_cnt=0,i=1;
	char  buff[1024]={'\0'};
	
    file_info *tinfo= file_head;;

    if(tinfo == NULL) {
        return -1;
    }

	while((sw_strcmp(tinfo->file_path,tr_pf)) && tinfo != NULL) {
        tinfo = tinfo->next;
    }
	
    tr_addr=(u32)(tinfo->dir_entry_offset+28);

	if(*tr_addr==tinfo->file_size) {
    *tr_addr=(u32)tr_lgh;
	
	if(tr_lgh>tinfo->file_size) {
		sk_ted=file_seek(tinfo->fd,0x00000000,FILE_SEEK_END);
		
		if(sk_ted!=-1) {

			cur_addr=cluster_to_memory_addr(tinfo->cur_cluster)+
									   (u32)tinfo->cur_offset;
        	end_size=tr_lgh-(int)(tinfo->file_size);
			tmp_tsize=end_size;
			while(end_size){
        	wt_zo+=file_write(tinfo->fd,(const char*)&buff,end_size);
			--end_size;
			++zo_cnt;
			}
			if(zo_cnt!=tmp_tsize && wt_zo==0) {
				sw_printf("null bytes is not appended \n");
				return -1;
			}
   		}
		else {
			return -1;
		}
	}

    tinfo->file_size=(u32)tr_lgh;
    prev_clstr=tinfo->strt_cluster;

    if(tinfo->file_size==0) {
        tinfo->cur_offset=(u16)(tinfo->file_size);
        write_fat_table(prev_clstr,0x00000000);
    }

    else if(tinfo->file_size>cluster_size){
		tinfo->cur_offset=(u16)(tinfo->file_size);
    while(tinfo->cur_offset>cluster_size) {
        new_clstr=allocate_cluster();
		tinfo->cur_offset=(u16)(tinfo->file_size-cluster_size);
		i=+1;
        write_fat_table(prev_clstr,new_clstr);
        prev_clstr=new_clstr;
    }
        write_fat_table(new_clstr,END_OF_CLUSTER);
    }

    else {
        tinfo->cur_offset=(u16)(tinfo->file_size);
        write_fat_table(prev_clstr,END_OF_CLUSTER);
    }

	tinfo->file_size=(u32)tr_lgh;
	tinfo->entry->size=(u32)tr_lgh;
    return 0;
}
	return -1;
}

/**
 * @brief 
 * file_remove - Deletes the file whose name is specified in filename
 *
 * @param fl_name - string containing the name of the file to be deleted
 *
 * @return - if the file is successfully deleted, a zero value is returned.
 *           On failure, a non-zero value is returned
 */
int file_remove(const char *fl_name)
{
	int fl_rem;
	fl_rem=file_unlink(fl_name);
	if(fl_rem!=0) {
		return -1;
	}
	return 0;
}

/**
 * @brief 
 * file_unlink - removes the file from the file system
 *
 * @param ul_name - string containing the name of the file to be deleted
 *
 * @return - if the file is successfully deleted, a zero value is returned.
 *           On failure, a non-zero value is returned
 */
int file_unlink(const char *ul_name)
{
	u8 *rm_addr;
    int fl_cls;
    file_info *rinfo= file_head;
    if(rinfo == NULL) {
        return -1;
    }
   while((sw_strcmp(rinfo->file_path,ul_name)) && rinfo != NULL) {
        rinfo = rinfo->next;
    }
    rm_addr=rinfo->dir_entry_offset;
    *rm_addr = DEL_DIR_ENTRY;
    fl_cls=file_close(rinfo->fd);
    if(fl_cls!=0) {
		sw_printf("the file is not removed from the filesystem \n");
        return -1;
    }
    return 0;
}
