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
 * Secure Frame_base buffer info initialization
 */

#include <sw_board.h>
#include <sw_fb.h>
#include <mem_mng.h>
#include <fat32.h>
#include <elf_loader.h>

u32 *g_framebuff;
s32int fb_open = 0;
s32int xres = 1024;
s32int yres = 600;
s32int bpp = 16;
u8 rgb444 = 0;

void sw_fb_init()
{
	rgb444 = 0;
	init_lcd(1024, 600);
}

void sw_fb_exit()
{
	/* To be done later */
}

void sw_fb_write(s32int offset, u32 color)
{
	if(offset <= (yres * xres + 
				xres) && (offset >= 0) && (fb_open == 1)) {
		u16 RGB565=((((color >> 16) & 0xF8) << 8) | 
				(((color >> 8) & 0xFC) << 3) | 
				((color & 0xF8) >> 3));

		u16 *tmp = (u16*)g_framebuff;
		tmp[offset] = RGB565;
	}
}

s32int get_sw_window_info(s32int info_num)
{
	if(fb_open) {
		switch(info_num) {
			case 1:
				return bpp;
			case 2:
				return xres;
			case 3:
				return yres;
		}
	}
}

void sw_flush_cache()
{
#ifdef TOUCH_SUPPORT
	flush_cache_all();
#ifdef CONFIG_CACHE_L2X0
	l2x0_flush_range(VE_FRAME_BASE, VE_FRAME_SIZE);
#endif
#endif
}

void init_pl111( u32 clcd_base,
		s32int           width,
		s32int           height,
		u32			     frame_base )
{
	volatile u32*  clcd  = (volatile u32*) clcd_base;

	/* PL111 register offsets (32-bit words) */
	const s32int PL111_TIM0 = (0x00/4);
	const s32int PL111_TIM1 = (0x04/4);
	const s32int PL111_TIM2 = (0x08/4);
	const s32int PL111_TIM3 = (0x0C/4);
	const s32int PL111_UBAS = (0x10/4);
	const s32int PL111_LBAS = (0x14/4);
	const s32int PL111_CNTL = (0x18/4);
	const s32int PL111_IENB = (0x1C/4);

	/*	Timing number for an 8.4" LCD screen for use on a VGA screen */
	u32 TIM0_VAL = 
		( (((width/16)-1)<<2) | (63<<8) | (31<<16) | (63<<8) );
	u32 TIM1_VAL = ( (height - 1) | (24<<10) | (11<<16) | (9<<24) );
	u32 TIM2_VAL = ( (0x7<<11) | ((width - 1)<<16) | (1<<26) );

	/* Statically allocate memory for screen buffer */
	g_framebuff = (u32*) frame_base;

	sw_printf("FRAME_BASE:%x\n", g_framebuff);

	/* Program the CLCD controller registers and start the CLCD */
	clcd[ PL111_TIM0 ] = TIM0_VAL;
	clcd[ PL111_TIM1 ] = TIM1_VAL;
	clcd[ PL111_TIM2 ] = TIM2_VAL;
	clcd[ PL111_TIM3 ] = 0; 
	clcd[ PL111_UBAS ] = frame_base;
	clcd[ PL111_LBAS ] = 0;
	clcd[ PL111_IENB ] = 0;

	/* Set the control register: 16BPP, Power OFF */
	if(!rgb444) {
		/* RGB 565 mode */
		clcd[ PL111_CNTL ] = (1<<0) | (4<<0) | (1<<5) | (0<<8) | (4 << 1);
	}
	else {
		/* RGB 444 mode */
		clcd[ PL111_CNTL ] = (1<<0) | (1<<5) | (4<<1);
	}

	/* Power ON */
	clcd[ PL111_CNTL ] |= (1<<11);
	fb_open = 1;
}


s32int init_lcd(s32int width, s32int height)
{
	const u32 dvi_mux = 0;

	/* VE System Register 32-bit word offsets */
	const s32int VE_SYS_ID       = (0x00/4);
	const s32int VE_SYS_CFG_DATA = (0xA0/4);
	const s32int VE_SYS_CFG_CTRL = (0xA4/4);
	const s32int VE_SYS_ID_HBI = 0x225;

	volatile u32*  ve_sysreg = (volatile u32*) VE_SYSTEM_REGS;

	/* Set CLCD clock
	 *   SYS_CFG_DATA sets oscillator rate value as 5.4MHz
	 *   SYS_CFG_CTRL
	 *      ( start=1 | write=1 | function=1 | site=0 | position=0 | device=1 )
	 */
	ve_sysreg[ VE_SYS_CFG_DATA ] = 5400000;
	ve_sysreg[ VE_SYS_CFG_CTRL ] = 0x80000000 | 
		(1<<30) | (1<<20) | (0<<16) | (0<<12) | (1<<0);

	/* 	Set DVI mux for correct MMB
		SYS_CFG_CTRL ( start=1 | write=1 | 
		function=7 | site=0 | position=0 | device=0 )
	 */
	ve_sysreg[ VE_SYS_CFG_DATA ] = dvi_mux;
	ve_sysreg[ VE_SYS_CFG_CTRL ] = 0x80000000 | 
		(1<<30) | (7<<20) | (0<<16) | (0<<12) | (0<<0);

	init_pl111(VE_CLCD_BASE, width, height, VE_FRAME_BASE);
	return 1;
}

void screen_clear(int width, int height)
{
    int x, y;
    for (y=0; y<height; ++y) {
        for (x=0; x<width; x++) {
            int offset = (y*width+x)*2;
            g_framebuff[offset] = 0;
            g_framebuff[offset+1] = 0;
        }
    }
}


#ifdef CONFIG_FILESYSTEM_SUPPORT
	#define FB_WIDTH  (352)
	#define FB_HEIGHT   (288)
	#define NUM_FRAMES  (2) /* our hard coded image contains exactly 2 frames*/
	#define RGB_BUFF_SIZE ((FB_WIDTH*FB_HEIGHT)*2)
u8 rgb_buff[RGB_BUFF_SIZE];
#endif
void write_media_player_data(void)
{
    
#ifdef CONFIG_FILESYSTEM_SUPPORT
	int fd;
    u32 retVal,curr_size = 0,img_index=0,bytes_read;
    s32 img_size = 0,offset=0,x=0,y=0;
    u16 red,blue,green,pixel;
    u32 i, j, read_done;
    unsigned short *fb_ptr = (unsigned short*)(&g_framebuff);

	rgb444 = 1;
    init_lcd(FB_WIDTH, FB_HEIGHT);
    screen_clear(FB_WIDTH,FB_HEIGHT);
        for(i = 0; i < 30; i++) {
        fd = file_open("/lib/test.rgb", 0);
        if(fd < 0) {
            sw_printf("Unable to open file \n");
            return;
        }
        bytes_read = 0;
        while(1) {
            retVal = file_read(fd, (void*)(rgb_buff+bytes_read), (RGB_BUFF_SIZE-bytes_read));
			if(retVal <= 0) {
                break;
            }
			bytes_read += retVal;
			if(bytes_read == RGB_BUFF_SIZE)
				break;
        }
        sw_memcpy(g_framebuff,rgb_buff,bytes_read);
        sw_printf("Going to sleep now \n");
        sw_usleep(200000);
        sw_printf("Finished sleeping now \n");
        sw_printf("Now displaying the second frame \n");
        bytes_read = 0;
        while(1) {
            retVal = file_read(fd, (void*)(rgb_buff+bytes_read), (RGB_BUFF_SIZE-bytes_read));
			if(retVal <= 0) {
                break;
            }
			bytes_read += retVal;
			if(bytes_read == RGB_BUFF_SIZE)
				break;
        }
        sw_memcpy(g_framebuff,rgb_buff,bytes_read);
        file_close(fd);
        sw_printf("Going to sleep now \n");
        sw_usleep(200000);
        sw_printf("Finished sleeping now \n");
        }
#endif
}
EXPORT_SYMBOL(write_media_player_data);
