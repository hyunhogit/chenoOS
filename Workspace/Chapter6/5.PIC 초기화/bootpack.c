#include <stdio.h>
#include "bootpack.h"
#include "graphic.h"
#include "descript_table.h"
#include "int.h"

void HariMain(void)
{
	BOOTINFO* address_boot_info;
	char s[40];
	char cursor[256];
	int version=1;
	int x_position,y_position;
	init_pallette();			/*palletteÃÊ±âÈ­*/
	init_gdt();
	init_idt();
	init_pic();
	
	address_boot_info=(BOOTINFO*)0x0ff0;
	x_position=(address_boot_info->screen_xsize- 16) / 2;
	y_position=(address_boot_info->screen_ysize- 28 - 16) / 2; 
	sprintf(s,"HO Operating System:%d",version);	
	
	init_screen(address_boot_info->address_vram,address_boot_info->screen_xsize,address_boot_info->screen_ysize);
	init_mouse(cursor,COL8_008400);
	putString(address_boot_info->address_vram,address_boot_info->screen_xsize,140,10,COL8_000000,s);
	putMouse(address_boot_info->address_vram,address_boot_info->screen_xsize,x_position,y_position,cursor);
	for(;;){
		io_hlt();		
	}
}

