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
	
	init_pallette();			/*pallette�ʱ�ȭ*/
	init_gdt();
	init_idt();
	init_pic();
	io_sti(); /* IDT/PIC�� �ʱ�ȭ�� �������Ƿ� CPU�� ���ͷ�Ʈ ������ ���� */

	
	address_boot_info=(BOOTINFO*)ADR_BOOTINFO;
	x_position=(address_boot_info->screen_xsize- 16) / 2;
	y_position=(address_boot_info->screen_ysize- 28 - 16) / 2; 
	sprintf(s,"HO Operating System:%d",version);	//�޸𸮿� �۾�����
	
	init_screen(address_boot_info->address_vram,address_boot_info->screen_xsize,address_boot_info->screen_ysize);
	init_mouse(cursor,COL8_008400);
	putString(address_boot_info->address_vram,address_boot_info->screen_xsize,140,10,COL8_000000,s);//������ �޸𸮷� ���� �׷��ȿ� �۾� ���
	putMouse(address_boot_info->address_vram,address_boot_info->screen_xsize,x_position,y_position,cursor);
	
	
	io_out8(PIC0_IMR, 0xf9); /* PIC1�� Ű���带 �㰡(11111001) */
	io_out8(PIC1_IMR, 0xef); /* ���콺�� �㰡(11101111) */

	for(;;){
		io_hlt();		
	}
}

