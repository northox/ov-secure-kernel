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
 * User Management implementation functions
 *
 */

#include <global.h>
#include <sw_buddy.h>
#include <task.h>
#include <sw_user_mgmt.h>
#include <sw_debug.h>
#include<otz_id.h>

/**
 * @brief 
 *
 * @param d_id
 *
 * @return 
 */
int getPermission(int d_id){
  struct acl_device *acld;
  struct acl_group *aclg;
  struct acl_user *aclu;
  int found = -1;
  struct sw_task *exec_task;
  if(global_val.exec_mode == 1){
      exec_task = get_current_task();
      list_for_each_entry(acld, &global_val.device_acl_list, head){
          if(acld->did == d_id){
              list_for_each_entry(aclg, &acld->group_head, head){
                  if(exec_task->acl.gid == aclg->gid){
                      list_for_each_entry(aclu, &aclg->user_head, head){
                          if(exec_task->acl.uid == aclu->uid){
                              found = 1;
                              break;
                          }
                      }
                      break;
                  }
              }
              break;
          }
      }
  }else{
      found = 1;
  }
  return found;
}

int setPermission(struct sw_task  *new_task){
    new_task->acl.uid = new_task->service_id;
/*
	sw_printf("SW: SERVICE_ID: %x\n", new_task->service_id);
*/
    if(new_task->service_id == OTZ_SVC_GLOBAL){
        new_task->acl.username = "OTZ_GLOBAL";
        new_task->acl.gid = 1;
        updateGlobalACL(OTZ_SVC_GLOBAL, 1, new_task->service_id);
        updateGlobalACL(OTZ_SVC_ECHO, 1, new_task->service_id);
        updateGlobalACL(OTZ_SVC_DRM, 1, new_task->service_id);
        updateGlobalACL(OTZ_SVC_CRYPT, 1, new_task->service_id);
        updateGlobalACL(OTZ_SVC_MUTEX_TEST, 1, new_task->service_id);
        updateGlobalACL(OTZ_SVC_VIRTUAL_KEYBOARD, 1, new_task->service_id);
        return 1;
    }else if(new_task->service_id == OTZ_SVC_ECHO){
        new_task->acl.username = "OTZ_ECHO";
        new_task->acl.gid = 1;
        return 1;
	}else if(new_task->service_id == OTZ_SVC_DRM){
        new_task->acl.username = "OTZ_DRM";
        new_task->acl.gid = 1;
        updateGlobalACL(2, 1, new_task->service_id);
        return 1;
    }else if(new_task->service_id == OTZ_SVC_CRYPT){
        new_task->acl.username = "OTZ_CRYPT";
        new_task->acl.gid = 1;
        return 1;
    }else if(new_task->service_id == OTZ_SVC_MUTEX_TEST){
        new_task->acl.username = "OTZ_MUTEX";
        new_task->acl.gid = 1;
        updateGlobalACL(2, 1, new_task->service_id);
        return 1;
    }else if(new_task->service_id == OTZ_SVC_VIRTUAL_KEYBOARD){
        new_task->acl.username = "OTZ_KEYBOARD";
        new_task->acl.gid = 1;
        updateGlobalACL(2, 1, new_task->service_id);
        return 1;
    }else{
        new_task->acl.username = "UN-DEF";
        new_task->acl.gid = 0;
        return -1;
    }
}

/**
 * @brief 
 *
 * @param d_id
 * @param g_id
 * @param u_id
 */
void updateGlobalACL(int d_id, int g_id, int u_id){  
    acl_device *acld;
    acl_group *aclg;
    acl_user *aclu;
    int found = -1;
    list_for_each_entry(acld, &global_val.device_acl_list, head){
      if(acld->did == d_id){
          found = 1;
          list_for_each_entry(aclg, &acld->group_head, head){
              if(g_id == aclg->gid){
                  found = 2;
                  list_for_each_entry(aclu, &aclg->user_head, head){
                      if(u_id == aclu->uid){
                          found = 3;
                          break;
                      }
                  }
                  break;
              }
          }
          break;
      }
    }
    if(found == 3){
/*
        sw_printf("ACL with DEVICE: %x USER: %x GROUP: %x already exist.\n", d_id, u_id, g_id);
*/
        return;
    }
    if(found < 3){
        aclu = (acl_user *) sw_malloc_private(COMMON_HEAP_ID,(sizeof(acl_user)));
        if(aclu == NULL){
            DIE_NOW(0,"Heap allocation failed while creating user Global ACL\n");
        }
        INIT_LIST_HEAD(&aclu->head);
        aclu->uid = u_id;
    }
    if(found == 2){
        list_add_tail(&aclg->user_head, &aclu->head);
        sw_printf("USER: %x added to GROUP: %x DEVICE: %x\n", u_id, g_id, d_id);
    }
    if(found < 2){
        aclg = (acl_group *) sw_malloc_private(COMMON_HEAP_ID,(sizeof(acl_group)));
        if(aclg == NULL){
            DIE_NOW(0,"Heap allocation failed while creating group Global ACL\n");
        }
        INIT_LIST_HEAD(&aclg->head);
        INIT_LIST_HEAD(&aclg->user_head);
        aclg->gid = g_id;
        list_add_tail(&aclg->user_head, &aclu->head);
    }
    if(found == 1){
        list_add_tail(&acld->group_head, &aclg->head);
        sw_printf("USER: %x GROUP:%x added to DEVICE: %x\n", u_id, g_id, d_id);
    }
    if(found < 1){
        acld = (acl_device *) sw_malloc_private(COMMON_HEAP_ID,(sizeof(acl_device)));
        if(acld == NULL){
            DIE_NOW(0,"Heap allocation failed while creating device Global ACL\n");
        }
        INIT_LIST_HEAD(&acld->head);
        INIT_LIST_HEAD(&acld->group_head);
        acld->did = d_id;
        list_add_tail(&acld->group_head, &aclg->head);
        list_add_tail(&global_val.device_acl_list, &acld->head);
        sw_printf("Created GlobalACL for USER: %x GROUP:%x to access DEVICE: %x\n", u_id, g_id, d_id);
    }
}
