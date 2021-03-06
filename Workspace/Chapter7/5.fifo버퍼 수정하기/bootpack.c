#include <stdio.h>
#include "bootpack.h"

void HariMain(void)
{
	BOOTINFO* address_boot_info;
	char s[40];
	char cursor[256];
	int version=1;
	int x_position,y_position;
	int i;
	char keyBuf[32];
	
	
	init_pallette();			/*pallette초기화*/
	init_gdt();
	init_idt();
	init_pic();
	FifoInit( &keyFifo , 32 , keyBuf) ;
	io_sti(); /* IDT/PIC의 초기화가 끝났으므로 CPU의 인터럽트 금지를 해제 */

	
	address_boot_info=(BOOTINFO*)ADR_BOOTINFO;
	x_position=(address_boot_info->screen_xsize- 16) / 2;
	y_position=(address_boot_info->screen_ysize- 28 - 16) / 2; 
	sprintf(s,"HO Operating System:%d",version);	//메모리에 글씨저장
	
	init_screen(address_boot_info->address_vram,address_boot_info->screen_xsize,address_boot_info->screen_ysize);
	init_mouse(cursor,COL8_008400);
	putString(address_boot_info->address_vram,address_boot_info->screen_xsize,140,10,COL8_000000,s);//저장한 메모리로 부터 그래픽에 글씨 출력
	putMouse(address_boot_info->address_vram,address_boot_info->screen_xsize,x_position,y_position,cursor);
	
	
	io_out8(PIC0_IMR, 0xf9); /* PIC1와 키보드를 허가(11111001) */
	io_out8(PIC1_IMR, 0xef); /* 마우스를 허가(11101111) */

	for(;;){
		io_cli();	/* 우선 인터럽트 막자 */
		if( FifoStatus( &keyFifo) == 0 ){
			io_stihlt();
		}else{
			i=FifoGet( &keyFifo );
			io_sti();	/* 데이터만 빼고 다시 인터럽트 개방 */
			sprintf(s,"%02X",i);
			boxfill8(address_boot_info->address_vram,address_boot_info->screen_xsize,COL8_008484,0,16,15,31);
			putString(address_boot_info->address_vram,address_boot_info->screen_xsize,0,16,COL8_FFFFFF,s);
		}	
	}
	return;
}

