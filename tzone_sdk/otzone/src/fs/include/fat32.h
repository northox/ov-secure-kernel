#ifndef __FAT32_H_
#define __FAT32_H_

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
 * Header file for fat32 filesystem
 */
#include<sw_types.h>

#define END_OF_CLUSTER	         0x0FFFFFFF
#define END_OF_ROOT_CLUSTER      0X0FFFFFF8
#define MAX_DIR_ENTRY_CNT	 16
#define FILE_SYS_TYPE_OFF    	 82
#define BYTES_PER_SEC_OFF        11
#define SEC_PER_CLUS_OFF         13
#define RES_SEC_CNT_OFF          14
#define FAT_CNT_OFF              16
#define TOT_SEC_CNT_OFF          32
#define SEC_PER_FAT              36
#define ROOT_DIR_STRT_CLUS_OFF   44
#define FS_INFOSECTOR_OFF        48
#define BACKUP_BOOT_SEC_OFF      50
#define NXT_FREE_CLUS_OFF        492
#define FILE_SYS_TYPE_LENGTH     8 
#define SHRT_FILE_NAME_LEN       11
#define STRT_CLUS_LOW_OFF        26
#define STRT_CLUS_HIGH_OFF       20
#define FILE_SIZE_OFF	         28
#define ATTR_OFF                 11
#define FILE_STAT_LEN            21
#define CHECK_SUM_OFF	         13
#define FILE_NAME_SHRT_LEN       8
#define FILE_NAME_EXTN_LEN       3
#define LONG_FILE_NAME_LEN       255
#define LOW_CLUSWORD_MASK        0x0000FFFF
#define HIGH_CLUSWORD_MASK       0xFFFF0000
#define LONG_FNAME_MASK		 0x0F
#define LAST_ORD_FIELD_SEQ	 0x40
#define LFN_END_MARK		 0xFFFF
#define LFN_TERM_MARK		 0x0000
#define LFN_FIRST_OFF		 0x01
#define LFN_SIXTH_OFF		 0x0E
#define LFN_TWELVETH_OFF	 0x1C
#define LFN_FIRST_SET_CNT	 5
#define LFN_SEC_SET_CNT		 6
#define LFN_THIRD_SET_CNT        2        
#define LFN_FIRST_SET_LEN	 10
#define LFN_SEC_SET_LEN          12
#define LFN_THIRD_SET_LEN        4
#define LFN_EMPTY_LEN	         2 
#define LFN_LEN_PER_ENTRY        13
#define FNAME_EXTN_SEP_OFF       6
#define FNAME_SEQ_NUM_OFF        7            
#define BYTES_PER_CLUSTER_ENTRY  4
#define DIR_ENTRY_LEN            32
#define VOL_ID_LEN               4
#define VOL_LABEL_LEN            11
#define RESERV_LEN               12
#define FS_VER_LEN               2 
#define OEM_NAME_LEN             8
#define JUMP_INS_LEN             3
#define MAX_FAT_CNT              2 
#define SPACE_VAL                32 
#define FILE_READ                0x01
#define FILE_WRITE               0X02
#define FILE_CREATE_NEW          0x04
#define FILE_CREATE_ALWAYS       0x08
#define FILE_APPEND              0x10              
#define ATTR_READ                0x01
#define ATTR_HIDDEN              0x02
#define ATTR_SYSTEM              0x04
#define ATTR_VOL_LABEL           0x08
#define ATTR_DIR                 0x10
#define ATTR_ARCHIVE             0x20
#define ATTR_LONG_FNAME		 0x0F
#define FREE_DIR_ENTRY           0x00
#define DEL_DIR_ENTRY            0xE5
#define DOT_DIR_ENTRY		 0x2E
#define ASCII_DIFF               32
#define FILE_SEEK_SET            0
#define FILE_SEEK_CUR            1
#define FILE_SEEK_END            2
#define DELIMITER	         '/'
#define EXTN_DELIMITER           '.'
#define TILDE	                 '~'
#define FULL_SHRT_NAME_LEN       13
 
/*Region Offsets*/     
u8 *bs_sector;
u8 *fs_infosector;
u8 *backup_boot_sector;
u8 *first_fat;
u8 *second_fat;
u8 *root_directory;
u8 *data;
/*Size and max entries of various regions*/
u16 sec_size;
u32 cluster_size;
u16 root_dir_size;
u16 rt_dir_entry_cnt;
u32 rt_dir_strt_clus;
u32 nxt_free_cluster;
u8 *dir_file_offset;

/**
* @brief 	
*	Structure of the boot sector
*/
typedef struct __attribute__((__packed__))
{
	u8   jump[JUMP_INS_LEN];
	u8   OEM_name[OEM_NAME_LEN];
	u16  bytes_per_sec;
	u8   sec_per_clus;
	u16  reserved_sec_cnt;
	u8   fat_cnt;
	u16  root_dir_max_cnt;
	u16  tot_sectors;
	u8   media_desc;
	u16  sec_per_fat_fat16;
	u16  sec_per_track;
	u16  number_of_heads;
	u32  hidden_sec_cnt;  
	u32  tol_sector_cnt;
	u32  sectors_per_fat;
	u16  ext_flags;
	u8   fs_version[FS_VER_LEN];   
	u32  root_dir_strt_cluster;
	u16  fs_info_sector;
	u16  backup_boot_sector;
	u8   reserved[RESERV_LEN];
	u8   drive_number;
	u8   reserved1;
	u8   boot_sig;
	u8   volume_id[VOL_ID_LEN];
	u8   volume_label[VOL_LABEL_LEN];
	u8   file_system_type[FILE_SYS_TYPE_LENGTH];  
} boot_sector;


/**
* @brief 
*	Structure of the 32 byte directory entry
*/
typedef struct __attribute__((__packed__))
{
	u8  name[FILE_NAME_SHRT_LEN];
	u8  extn[FILE_NAME_EXTN_LEN];
	u8  attr;
	u8  reserved;
	u8  crt_time_tenth;
	u16 crt_time;
	u16 crt_date;
	u16 lst_access_date;
	u16 strt_clus_hword;
	u16 lst_mod_time;
	u16 lst_mod_date;
	u16 strt_clus_lword;
	u32 size;
} dir_entry;

/**
* @brief 
*	Structure of the long file name slot
*/
typedef struct
{
	u8 ord_field;
	u8 fname0_4[LFN_FIRST_SET_LEN];
	u8 flag;
	u8 reserved;
	u8 chksum;
	u8 fname6_11[LFN_SEC_SET_LEN];
	u8 empty[LFN_EMPTY_LEN];
	u8 fname12_13[LFN_THIRD_SET_LEN];	
} lfn_entry;


/**
* @brief 
*	Structure of a file containing its basic information
*	required for many operations
*/
typedef struct
{
	int fd;
	dir_entry *entry;
	const char *file_path;
	u32 strt_cluster;
	u32 cur_cluster;
	u16 cur_offset;
	u32 file_size;
	u8  mode;
	u32 bytes_traversed;
	u8 *dir_entry_offset;
	struct file_info *next;
} file_info;


/**
* @brief 
*	Structure containing the basic details of a file/directory
*/
typedef struct 
{
	char name[FULL_SHRT_NAME_LEN];
	const char *lname;
	u32 size;
	u16 time;
	u16 date;
	u8 attr;	
} file_stat;

file_info *file_head;
int fdes; 

/**
* @brief 
*
* @param 
*
* @return 
*/
int mount_file_system(u8 *);

/**
* @brief 
*
* @param 
*
* @return 
*/
bool chk_fat32_file_system(u8 *);

/**
* @brief 
*
* @param 
* @param 
*/
void cal_region_offsets(boot_sector *,u8 *);

/**
* @brief 
*
* @param 
* @param int
*
* @return 
*/
int file_open(const char *,int);

/**
* @brief 
*
* @param 
* @param 
* @param 
* @param u32
* @param bool
*
* @return 
*/
bool get_dir_entry(char *,dir_entry **,u8 *,u32,bool);

/**
* @brief 
*
* @param 
* @param 
* @param 
*
* @return 
*/
int lfn_entry_to_name(lfn_entry *,char *,int *);

/**
* @brief 
*
* @param u32
*
* @return 
*/
u8 * cluster_to_memory_addr(u32);

/**
* @brief 
*
* @param 
* @param 
* @param u8
* @param 
* @param 
*
* @return 
*/
int retrieve_file_info(file_info *,dir_entry *,u8,u8 *,const char *);

/**
* @brief 
*
* @param u32
*
* @return 
*/
u32 get_fat_table_entry(u32);

/**
* @brief 
*
* @param 
*/
void convert_to_uppercase(char *);

/**
* @brief 
*
* @param 
*
* @return 
*/
int find_depth(const char *);

/**
* @brief 
*
* @param 
* @param 
* @param u32
* @param 
*
* @return 
*/
int create_file(char *,char *,u32,dir_entry **);

/**
* @brief 
*
* @param int
* @param 
* @param int
*
* @return 
*/
int file_read(int,char *,int);

/**
* @brief 
*
* @param int
* @param 
* @param int
*
* @return 
*/
int file_write(int,const char *,int);

/**
* @brief 
*
* @return 
*/
u32 allocate_cluster();

/**
* @brief 
*
* @param u32
* @param u32
*/
void write_fat_table(u32,u32);
/**
* @brief 
*
* @param 
* @param 
* @param char
*/
void get_short_file_name(char *,char *,char);

/**
* @brief 
*
* @param 
* @param 
* @param u8
* @param int
*/
void fill_lng_fname_entries(char *,u8 *,u8,int);

/**
* @brief 
*
* @param 
*
* @return 
*/
u8 calc_check_sum(u8 *);

/**
* @brief 
*
* @param int
*
* @return 
*/
int file_close(int);

/**
* @brief 
*
* @param int
* @param int
* @param int
*
* @return 
*/
int file_seek(int,int,int);

/**
* @brief 
*
* @param 
* @param u32
* @param int
*
* @return 
*/
int get_seek_off(file_info *,u32,int);

/**
* @brief 
*
* @param 
* @param 
* @param 
*
* @return 
*/
int get_long_file_name(char *,u8 **,int *);

/**
* @brief 
*
* @param 
* @param 
*
* @return 
*/
int file_rename(const char *, const char *);

/**
* @brief 
*
* @param 
*
* @return 
*/
int create_dir(const char *);

/**
* @brief 
*
* @param 
*
* @return 
*/
int chk_file_lock(const char *);

/**
* @brief 
*
* @param int
*
* @return 
*/
file_stat * read_dir(int);

/**
* @brief 
*
* @param 
*
* @return 
*/
int open_dir(const char *);

/**
* @brief 
*
* @param int
*
* @return 
*/
int cal_req_dir_cnt(int );

/**
* @brief
* file_truncate - function cause the regular file named by path 
*                 to be truncated to a size of precisely length bytes
*
* @param tr_pf -  truncates the file whose name is specified in filename
* @param tr_lgh - The desired truncate size of the file in bytes
*
* @return - If the file contents is successfully truncated, a zero value is 
*           returned.On failure, non-Zero is returned
*/
int file_truncate(const char* tr_pf,int tr_lgh);

/**
 * @brief 
 * file_remove - Deletes the file whose name is specified in filename
 *
 * @param fl_name - string containing the name of the file to be deleted
 *
 * @return - if the file is successfully deleted, a zero value is returned.
 *           On failure, a non-zero value is returned
 */
int file_remove(const char *fl_name);

/**
 * @brief 
 * file_remove - Deletes the file whose name is specified in filename
 *
 * @param fl_name - string containing the name of the file to be deleted
 *
 * @return - if the file is successfully deleted, a zero value is returned.
 *           On failure, a non-zero value is returned
 */
int file_unlink(const char *ul_name);

#endif
