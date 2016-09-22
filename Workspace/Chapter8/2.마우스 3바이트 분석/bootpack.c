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
	char mouseBuf[128];
	MOUSE_DATA mouseData;

	
	init_pallette();			/*pallette�ʱ�ȭ*/
	init_gdt();
	init_idt();
	init_pic();
	InitKeyboard();
	FifoInit( &keyFifo , 32 , keyBuf) ;
	FifoInit( &mouseFifo, 128 , mouseBuf ) ; 
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
	
	EnableMouse( &mouseData );

	for(;;){
		io_cli();	/* �켱 ���ͷ�Ʈ ���� */
		if( FifoStatus( &keyFifo) + FifoStatus( &mouseFifo ) == 0 ){
			io_stihlt();
		}else{
			if( FifoStatus( &keyFifo) != 0 ){
				i=FifoGet( &keyFifo );
				io_sti();	/* �����͸� ���� �ٽ� ���ͷ�Ʈ ���� */
				sprintf(s,"%02X",i);
				boxfill8(address_boot_info->address_vram,address_boot_info->screen_xsize,COL8_008484,0,16,15,31);
				putString(address_boot_info->address_vram,address_boot_info->screen_xsize,0,16,COL8_FFFFFF,s);
			}else if( FifoStatus( &mouseFifo) != 0 ){
				i=FifoGet( &mouseFifo );
				io_sti();	/* �����͸� ���� �ٽ� ���ͷ�Ʈ ���� */
				if( DecodeMouse( &mouseData , i) ==1 ){
					/* �����Ͱ� 3����Ʈ �����Ƿ� ǥ�� */
					sprintf(s, "[lcr %4d %4d]", mouseData.x, mouseData.y);
					if ((mouseData.button & 0x01) != 0) {
						s[1] = 'L';
					}
					if ((mouseData.button & 0x02) != 0) {
						s[3] = 'R';
					}
					if ((mouseData.button & 0x04) != 0) {
						s[2] = 'C';
					}
					boxfill8(address_boot_info->address_vram,address_boot_info->screen_xsize, COL8_008484, 32, 16, 32 + 15 * 8 - 1, 31);
					putString(address_boot_info->address_vram,address_boot_info->screen_xsize, 32, 16, COL8_FFFFFF, s);
				}
			}
		}	
	}
	return;
}

