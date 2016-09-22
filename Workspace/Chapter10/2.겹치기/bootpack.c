#include <stdio.h>
#include "bootpack.h"

void HariMain(void)
{
	BOOTINFO* pBootInfo=(BOOTINFO*)ADR_BOOTINFO;
	char s[40]; 			/*�޽��� ��¿� */
	
	int xPosition,yPosition;	/*���콺��ġ*/
	int i;				/*������*/
	char keyBuf[32];		/*Ű���嵥���͹���*/
	char mouseBuf[128];		/*���콺�����͹���*/
	MOUSE_DATA mouseData;		/*���콺������3���� ���� �ǹ��ִ� ����*/
	unsigned int totalMemory;	/*��ü �޸� ũ��*/
	MemoryManager* memoryManager = (MemoryManager *) MEMMAN_ADDR;
	SheetManager* pSheetManager;	
	Sheet* pSheetBackGround;	/*����ȭ���� ���� Sheet */
	Sheet* pSheetMouse;		/*���콺�� ���� Sheet*/
	unsigned char pMouseImage[256];		/*���콺�̹���*/
	unsigned char* pBackGroundImage;

	init_gdt();
	init_idt();
	init_pic();
	io_sti(); /* IDT/PIC�� �ʱ�ȭ�� �������Ƿ� CPU�� ���ͷ�Ʈ ������ ���� */
	InitKeyboard();
	FifoInit( &keyFifo , 32 , keyBuf) ;
	FifoInit( &mouseFifo, 128 , mouseBuf ) ; 
	io_out8(PIC0_IMR, 0xf9); /* PIC1�� Ű���带 �㰡(11111001) */
	io_out8(PIC1_IMR, 0xef); /* ���콺�� �㰡(11101111) */
	EnableMouse( &mouseData );
	xPosition=(pBootInfo->xSize- 16) / 2;
	yPosition=(pBootInfo->ySize- 28 - 16) / 2; 

	
	totalMemory = TestMemory(0x00400000, 0xbfffffff);
	Init_MemoryManager(memoryManager);
	Free_Memory(memoryManager, 0x00001000, 0x0009e000); /* 0x00001000 - 0x0009efff */
	Free_Memory(memoryManager, 0x00400000, totalMemory - 0x00400000);

	init_pallette();			/*pallette�ʱ�ȭ*/
	pSheetManager= Init_SheetManager(memoryManager,pBootInfo->pVideo , pBootInfo->xSize,pBootInfo->ySize);
	pSheetBackGround=Alloc_Sheet(pSheetManager);
	pSheetMouse=Alloc_Sheet(pSheetManager);
	pBackGroundImage=(unsigned char *) Alloc_MemoryBlock(memoryManager, (pBootInfo->xSize)*( pBootInfo->ySize));	
	init_screen(pBackGroundImage,pBootInfo->xSize,pBootInfo->ySize);	
	init_mouse(pMouseImage,99);
	Set_pImageOfSheet(pSheetBackGround, pBackGroundImage, pBootInfo->xSize, pBootInfo->ySize, -1); /* ��������� */
	Set_pImageOfSheet(pSheetMouse, pMouseImage, 16, 16, 99);/*99:����� ��ȣ*/
	Move_Sheet(pSheetManager,pSheetBackGround, 0,0);
	Move_Sheet(pSheetManager,pSheetMouse,xPosition,yPosition);
	UpDown_Sheet( pSheetManager,  pSheetBackGround, 0);
	UpDown_Sheet( pSheetManager,  pSheetMouse, 1);
	
	sprintf(s, "memory %dMB   free : %dKB", totalMemory / (1024 * 1024), Check_Size(memoryManager) / 1024);
	putString( pBackGroundImage , pBootInfo->xSize , 0, 48,COL8_FFFFFF, s); 
	Draw_AllSheet(pSheetManager);
	
	for(;;){
		io_cli();	/* �켱 ���ͷ�Ʈ ���� */
		if( FifoStatus( &keyFifo) + FifoStatus( &mouseFifo ) == 0 ){
			io_stihlt();
		}else{
			if( FifoStatus( &keyFifo) != 0 ){
				i=FifoGet( &keyFifo );
				io_sti();	/* �����͸� ���� �ٽ� ���ͷ�Ʈ ���� */
				sprintf(s,"%02X",i);
				boxfill8(pBackGroundImage,pBootInfo->xSize,COL8_008484,0,0,15,15);
				putString(pBackGroundImage,pBootInfo->xSize,0,0,COL8_FFFFFF,s);
				Draw_AllSheet(pSheetManager);
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
					boxfill8(pBackGroundImage,pBootInfo->xSize, COL8_008484, 0, 32, 15 * 8 - 1, 16+31);
					putString(pBackGroundImage,pBootInfo->xSize, 0, 32, COL8_FFFFFF, s);
					/* ���콺 Ŀ���� �̵� */
					xPosition += mouseData.x;
					yPosition += mouseData.y;
					if (xPosition  < 0) {
						xPosition = 0;
					}
					if (yPosition< 0) {
						yPosition = 0;
					}
					if (xPosition  > pBootInfo->xSize - 16) {
						xPosition = pBootInfo->xSize - 16;
					}
					if (yPosition > pBootInfo->ySize - 16) {
						yPosition =pBootInfo->ySize- 16;
					}
					sprintf(s, "(%3d, %3d)",xPosition, yPosition);
					boxfill8( pBackGroundImage ,pBootInfo->xSize , COL8_008484, 0,16, 79, 31); /* ��ǥ ����� */
					putString(pBackGroundImage , pBootInfo->xSize , 0, 16, COL8_FFFFFF, s); /* ��ǥ ���� */
					Move_Sheet(pSheetManager,pSheetMouse,xPosition,yPosition); /* ���콺 �׸��� */		
				}
			}
		}	
	}
	return;
}

