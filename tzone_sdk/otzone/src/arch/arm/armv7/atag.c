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
 * atag implementation
 */

#include <sw_types.h>
#include <atag.h>

/**
* @brief 
*
* @param value
*
* @return 
*/
static inline u32 convert_endianess(u32 value)
{
    u32 mod_value = 0;
    mod_value |= (0xFF000000 & value) >> 24;
    mod_value |= (0x00FF0000 & value) >> 8;
    mod_value |= (0x0000FF00 & value) << 8;
    mod_value |= (0x000000FF & value) << 24;

    return mod_value;
}

/**
* @brief 
*
* @param image_addr
* @param image_header
*/
void read_kernel_image_header(u32* image_addr,
                struct kernel_image_header* image_header)
{
    int image_name_index;

    struct kernel_image_header* image = (struct kernel_image_header*)

                                            image_addr;
    image_header->ker_img_magic =
                    image->ker_img_magic;
    image_header->ker_img_header_crc =
                    image->ker_img_header_crc;
    image_header->ker_img_create_time =
                    image->ker_img_create_time;
    image_header->ker_img_size =
                   convert_endianess(image->ker_img_size);
    image_header->ker_img_load_addr =
                   convert_endianess(image->ker_img_load_addr);
    image_header->ker_img_entry_point =
                   convert_endianess(image->ker_img_entry_point);
    image_header->ker_img_data_crc =
                    image->ker_img_data_crc;
    image_header->ker_img_os_name =
                    image->ker_img_os_name;
    image_header->ker_img_target_arch =
                    image->ker_img_target_arch;
    image_header->ker_img_type =
                    image->ker_img_type;
    image_header->ker_img_comp_type =
                    image->ker_img_comp_type;
    for(image_name_index = 0; image_name_index < KER_IMG_NAME_LEN;
                                                 image_name_index++)
    {
        image_header->ker_img_name[image_name_index] =
                    image->ker_img_name[image_name_index];

    }

    return;
}
struct tag * setup_start_tag (struct tag *params)
{
	params->hdr.tag = ATAG_CORE;
	params->hdr.size = tag_size (tag_core);

	params->u.core.flags = 0;
	params->u.core.pagesize = 0;
	params->u.core.rootdev = 0;

	params = tag_next (params);

	return params;
}

struct tag * setup_commandline_tag (struct tag *params, char *commandline)
{
	char *p;

	if (!commandline)
		return;

	/* eat leading white space */
	for (p = commandline; *p == ' '; p++);

	/* skip non-existent command lines so the kernel will still
	 * use its default command line.
	 */
	if (*p == '\0')
		return;

	params->hdr.tag = ATAG_CMDLINE;
	params->hdr.size =
		(sizeof (struct tag_header) + strlen (p) + 1 + 4) >> 2;

	strcpy (params->u.cmdline.cmdline, p);

	params = tag_next (params);

	return params;
}

struct tag * setup_initrd_tag (struct tag *params, u32 initrd_start, u32 initrd_end)
{
	params->hdr.tag = ATAG_INITRD2;
	params->hdr.size = tag_size (tag_initrd);

	params->u.initrd.start = initrd_start;
	params->u.initrd.size = initrd_end - initrd_start;

	params = tag_next (params);
	return params;
}

struct tag * setup_end_tag (struct tag *params)
{
	params->hdr.tag = ATAG_NONE;
	params->hdr.size = 0;
	return params;
}

struct tag * setup_memory_tags (u32 nr_dram_banks, u32 dram_start, u32 dram_bank_size, struct tag *params)
{
	int i;

	for (i = 0; i < nr_dram_banks; i++) {
		params->hdr.tag = ATAG_MEM;
		params->hdr.size = tag_size (tag_mem32);

		params->u.mem.start = dram_start + (i * dram_bank_size);	
		params->u.mem.size = dram_bank_size;

		params = tag_next (params);
	}
	return params;
}
