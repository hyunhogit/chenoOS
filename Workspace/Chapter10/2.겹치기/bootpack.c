#include <stdio.h>
#include "bootpack.h"

void HariMain(void)
{
	BOOTINFO* pBootInfo=(BOOTINFO*)ADR_BOOTINFO;
	char s[40]; 			/*메시지 출력용 */
	
	int xPosition,yPosition;	/*마우스위치*/
	int i;				/*루프용*/
	char keyBuf[32];		/*키보드데이터버퍼*/
	char mouseBuf[128];		/*마우스데이터버퍼*/
	MOUSE_DATA mouseData;		/*마우스데이터3개씩 묶은 의미있는 정보*/
	unsigned int totalMemory;	/*전체 메모리 크기*/
	MemoryManager* memoryManager = (MemoryManager *) MEMMAN_ADDR;
	SheetManager* pSheetManager;	
	Sheet* pSheetBackGround;	/*바탕화면을 위한 Sheet */
	Sheet* pSheetMouse;		/*마우스를 위한 Sheet*/
	unsigned char pMouseImage[256];		/*마우스이미지*/
	unsigned char* pBackGroundImage;

	init_gdt();
	init_idt();
	init_pic();
	io_sti(); /* IDT/PIC의 초기화가 끝났으므로 CPU의 인터럽트 금지를 해제 */
	InitKeyboard();
	FifoInit( &keyFifo , 32 , keyBuf) ;
	FifoInit( &mouseFifo, 128 , mouseBuf ) ; 
	io_out8(PIC0_IMR, 0xf9); /* PIC1와 키보드를 허가(11111001) */
	io_out8(PIC1_IMR, 0xef); /* 마우스를 허가(11101111) */
	EnableMouse( &mouseData );
	xPosition=(pBootInfo->xSize- 16) / 2;
	yPosition=(pBootInfo->ySize- 28 - 16) / 2; 

	
	totalMemory = TestMemory(0x00400000, 0xbfffffff);
	Init_MemoryManager(memoryManager);
	Free_Memory(memoryManager, 0x00001000, 0x0009e000); /* 0x00001000 - 0x0009efff */
	Free_Memory(memoryManager, 0x00400000, totalMemory - 0x00400000);

	init_pallette();			/*pallette초기화*/
	pSheetManager= Init_SheetManager(memoryManager,pBootInfo->pVideo , pBootInfo->xSize,pBootInfo->ySize);
	pSheetBackGround=Alloc_Sheet(pSheetManager);
	pSheetMouse=Alloc_Sheet(pSheetManager);
	pBackGroundImage=(unsigned char *) Alloc_MemoryBlock(memoryManager, (pBootInfo->xSize)*( pBootInfo->ySize));	
	init_screen(pBackGroundImage,pBootInfo->xSize,pBootInfo->ySize);	
	init_mouse(pMouseImage,99);
	Set_pImageOfSheet(pSheetBackGround, pBackGroundImage, pBootInfo->xSize, pBootInfo->ySize, -1); /* 투명색없음 */
	Set_pImageOfSheet(pSheetMouse, pMouseImage, 16, 16, 99);/*99:투명색 번호*/
	Move_Sheet(pSheetManager,pSheetBackGround, 0,0);
	Move_Sheet(pSheetManager,pSheetMouse,xPosition,yPosition);
	UpDown_Sheet( pSheetManager,  pSheetBackGround, 0);
	UpDown_Sheet( pSheetManager,  pSheetMouse, 1);
	
	sprintf(s, "memory %dMB   free : %dKB", totalMemory / (1024 * 1024), Check_Size(memoryManager) / 1024);
	putString( pBackGroundImage , pBootInfo->xSize , 0, 48,COL8_FFFFFF, s); 
	Draw_AllSheet(pSheetManager);
	
	for(;;){
		io_cli();	/* 우선 인터럽트 막자 */
		if( FifoStatus( &keyFifo) + FifoStatus( &mouseFifo ) == 0 ){
			io_stihlt();
		}else{
			if( FifoStatus( &keyFifo) != 0 ){
				i=FifoGet( &keyFifo );
				io_sti();	/* 데이터만 빼고 다시 인터럽트 개방 */
				sprintf(s,"%02X",i);
				boxfill8(pBackGroundImage,pBootInfo->xSize,COL8_008484,0,0,15,15);
				putString(pBackGroundImage,pBootInfo->xSize,0,0,COL8_FFFFFF,s);
				Draw_AllSheet(pSheetManager);
			}else if( FifoStatus( &mouseFifo) != 0 ){
				i=FifoGet( &mouseFifo );
				io_sti();	/* 데이터만 빼고 다시 인터럽트 개방 */
				if( DecodeMouse( &mouseData , i) ==1 ){
					/* 데이터가 3바이트 모였으므로 표시 */
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
					/* 마우스 커서의 이동 */
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
					boxfill8( pBackGroundImage ,pBootInfo->xSize , COL8_008484, 0,16, 79, 31); /* 좌표 지운다 */
					putString(pBackGroundImage , pBootInfo->xSize , 0, 16, COL8_FFFFFF, s); /* 좌표 쓴다 */
					Move_Sheet(pSheetManager,pSheetMouse,xPosition,yPosition); /* 마우스 그린다 */		
				}
			}
		}	
	}
	return;
}

