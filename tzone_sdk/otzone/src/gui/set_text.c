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
 * Set text implementation
 */

#include <gui_manager.h>

s32int countt = -1;

/*Setting text based on the object type*/
/**
* @brief 
*
* @param str
* @param id[]
*/
void set_text(u8* str, u8 id[])
{
	countt = obj_val.obj_count-1;
	while(countt!=-1) {
		if(sw_strcmp(obj_val.obj_id[countt], id) == 0) {
			break;
		}
		countt--;
	}

	if(countt != -1) {
		s32int width = obj_val.obj_coord[countt][2] - 
			obj_val.obj_coord[countt][0];
		s32int height = obj_val.obj_coord[countt][3] - 
			obj_val.obj_coord[countt][1];

		if(sw_strcmp(obj_val.obj_type[countt], "textfield\0") == 0) {
			fill_rect(obj_val.obj_coord[countt][0], obj_val.obj_coord[countt][1]
					,obj_val.obj_coord[countt][2], obj_val.obj_coord[countt][3],
					obj_val.obj_color[countt]);
			put_string(obj_val.obj_coord[countt][0]+10, 
					obj_val.obj_coord[countt][1]+(height-8)/2, str, 
					sw_strlen(str),
					calc_contrast_color(obj_val.obj_color[countt]), NO_TEXT_BG);
			sw_strcpy(obj_val.obj_str[countt], str);
		}

		if(sw_strcmp(obj_val.obj_type[countt], "password\0") == 0) {
			fill_rect(obj_val.obj_coord[countt][0],obj_val.obj_coord[countt][1],
					obj_val.obj_coord[countt][2], obj_val.obj_coord[countt][3],
					obj_val.obj_color[countt]);

			s32int i = 0;
			if(sw_strcmp(str, "\0") != 0) {
				u8 temp_str[500];
				sw_strcpy(temp_str, str);
				while(str[i] != '\0') {
					str[i++] = '*';
				}

				put_string(obj_val.obj_coord[countt][0]+10, 
						obj_val.obj_coord[countt][1] + (height-8)/2, str, 
						sw_strlen(str),
						calc_contrast_color(obj_val.obj_color[countt]), 
						NO_TEXT_BG);
				
				sw_strcpy(obj_val.obj_str[countt], temp_str);
				sw_strcpy(str, temp_str);
			}
			else {
				put_string(obj_val.obj_coord[countt][0]+10, 
						obj_val.obj_coord[countt][1] + (height-8)/2, str, 
						sw_strlen(str),
						calc_contrast_color(obj_val.obj_color[countt]), 
						NO_TEXT_BG);
				
				sw_strcpy(obj_val.obj_str[countt], str);
			}
		}
	}
}

