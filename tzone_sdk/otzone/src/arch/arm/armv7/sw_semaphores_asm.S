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
 * assembly routines for sempahore operations
 */

/*assembly routines for sempahore operations */

.global sw_spinlock_s
.type sw_spinlock_s, %function
.global sw_spinunlock_s
.type sw_spinunlock_s, %function
.global sw_trylock_s
.type sw_trylock_s, %function
.global sw_sem_wait_s
.type sw_sem_wait_s, %function
.global sw_sem_post_s
.type sw_sem_post_s, %function

.file "sw_semaphores_asm.s"

sw_spinlock_s:
    MOV r2, #1                      /*1 to lock it*/
    LDREX   r1, [r0]                /*get the current state of the locked*/
    TEQ     r1, #0                  /*Is it already locked */
    WFENE                           /*wait if yes */
    STREXEQ r1, r2, [r0]            /*try to lock it, if unlocked*/
    TEQEQ   r1, #0                  /*Did it fail ?*/
    BNE     sw_spinlock_s          /*loop if it failed.*/
    BX      lr


sw_spinunlock_s:
     MOV     r1, #0                   /*0 to unlock it*/
     STR     r1, [r0]                 /*release spin lock*/
     /*MCR     p15, 0, r1, c7, c10, 4   ; Drain Store Buffer (DSB)*/
     DSB
     SEV                              /*unlock any spinlock*/
     BX      lr

sw_trylock_s:
     MOV     r2, #1                   /*1 to lock it.*/
     LDREX   r1, [r0]                 /*check lock state.*/
     TEQ     r1, #0                   /*Is it locked ?*/
     STREXEQ r1, r2, [r0]             /*if not, try to lock it.*/
     MOV     r0, r1                   /*status of the lock in r0 for function return*/
     BX      lr

sw_sem_wait_s:
    LDREX   r1, [r0]               /*Retrieve semaphore's counter*/
    SUBS    r2, r1, #1             /*Decrement it*/
    WFEMI                          /*If the counter has become negative, wait here*/
    BMI     sw_sem_wait_s         /*On awakening, re-perform the decrement*/
    STREX   r1, r2, [r0]           /*Didn't lock ? Store the new counter*/
    TEQ     r1, #0                 /*Update successful?*/
    BNE     sw_sem_wait_s         /*Try again if not*/
    BX      lr

sw_sem_post_s:
    LDREX   r1, [r0]               /*load of counter*/
    ADD     r1, r1, #1             /*Increment*/
    STREX   r2, r1, [r0]           /*Attempt an update*/
    TEQ     r2, #0                 /*Are we Successful ?*/
    BNE     sw_sem_post_s         /*Jump back if not*/
    /*;MCR     p15, 0, r1, c7, c10, 4 ; Drain the Store Buffer (DSB)*/
    DSB
    SEV                            /*Unlock spinlock*/
    BX      lr
