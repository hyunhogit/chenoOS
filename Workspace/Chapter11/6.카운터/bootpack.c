#include <stdio.h>
#include "bootpack.h"

void Make_Window(unsigned char* pImage, int xSize, int ySize, char* title);

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
	Sheet* pSheetWindow;
	unsigned char pMouseImage[256];		/*마우스이미지*/
	unsigned char* pBackGroundImage;	/*배경이미지*/
	unsigned char* pWindowImage;
	unsigned int counter=0;		/*카운터전용 변수*/

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
	pSheetWindow=Alloc_Sheet(pSheetManager);
	pBackGroundImage=(unsigned char *) Alloc_MemoryBlock(memoryManager, (pBootInfo->xSize)*( pBootInfo->ySize));
	pWindowImage=(unsigned char*)Alloc_MemoryBlock(memoryManager, 160*52);	
	init_screen(pBackGroundImage,pBootInfo->xSize,pBootInfo->ySize);	
	init_mouse(pMouseImage,99);
	Make_Window(pWindowImage,160,52,"Counter");
	Set_pImageOfSheet(pSheetBackGround, pBackGroundImage, pBootInfo->xSize, pBootInfo->ySize, -1); /* 투명색없음 */
	Set_pImageOfSheet(pSheetMouse, pMouseImage, 16, 16, 99);/*99:투명색 번호*/
	Set_pImageOfSheet(pSheetWindow,pWindowImage,160,52,-1);
	Move_Sheet(pSheetManager,pSheetBackGround, 0,0);
	Move_Sheet(pSheetManager,pSheetMouse,xPosition,yPosition);
	Move_Sheet(pSheetManager,pSheetWindow,80,72);
	UpDown_Sheet( pSheetManager,  pSheetBackGround, 0);
	UpDown_Sheet( pSheetManager,  pSheetWindow, 1);
	UpDown_Sheet( pSheetManager,  pSheetMouse, 2);
	
	sprintf(s, "memory %dMB   free : %dKB", totalMemory / (1024 * 1024), Check_Size(memoryManager) / 1024);
	putString( pBackGroundImage , pBootInfo->xSize , 0, 48,COL8_FFFFFF, s); 
	Draw_AllSheet(pSheetManager);
	
	
	for(;;){
		counter++;			/*카운트*/
		sprintf(s,"%010d",counter);	
		boxfill8(pWindowImage,160,COL8_C6C6C6,40,28,119,43);	/*상대주소*/
		putString(pWindowImage,160,40,28,COL8_000000,s);	/*상대주소*/
		Refresh_Sheet(pSheetManager,pSheetWindow,40,28,120,44); 	/*절대주소*/
		io_cli();	/* 우선 인터럽트 막자 */
		if( FifoStatus( &keyFifo) + FifoStatus( &mouseFifo ) == 0 ){
			io_sti();
		}else{
			if( FifoStatus( &keyFifo) != 0 ){
				i=FifoGet( &keyFifo );
				io_sti();	/* 데이터만 빼고 다시 인터럽트 개방 */
				sprintf(s,"%02X",i);
				boxfill8(pBackGroundImage,pBootInfo->xSize,COL8_008484,0,0,15,15);
				putString(pBackGroundImage,pBootInfo->xSize,0,0,COL8_FFFFFF,s);
				Refresh_Sheet(pSheetManager,pSheetBackGround,0,0,16,16);
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
					Refresh_Sheet(pSheetManager,pSheetBackGround,0,32,120,48);
					/* 마우스 커서의 이동 */
					xPosition += mouseData.x;
					yPosition += mouseData.y;
					if (xPosition  < 0) {
						xPosition = 0;
					}
					if (yPosition< 0) {
						yPosition = 0;
					}
					if (xPosition  > pBootInfo->xSize - 1) {
						xPosition = pBootInfo->xSize - 1;
					}
					if (yPosition > pBootInfo->ySize - 1) {
						yPosition =pBootInfo->ySize- 1;
					}
					sprintf(s, "(%3d, %3d)",xPosition, yPosition);
					boxfill8( pBackGroundImage ,pBootInfo->xSize , COL8_008484, 0,16, 79, 31); /* 좌표 지운다 */
					putString(pBackGroundImage , pBootInfo->xSize , 0, 16, COL8_FFFFFF, s); /* 좌표 쓴다 */
					Refresh_Sheet(pSheetManager,pSheetBackGround,0,16,80,32);
					Move_Sheet(pSheetManager,pSheetMouse,xPosition,yPosition); /* 마우스 그린다 */		
				}
			}
		}	
	}
	return;
}


void Make_Window(unsigned char* pImage, int xSize, int ySize, char* title){
	static char pCloseButton[14][16] = {
		"OOOOOOOOOOOOOOO@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQQQ@@QQQQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"O$$$$$$$$$$$$$$@",
		"@@@@@@@@@@@@@@@@"
	};
	int x, y;
	char c;
	boxfill8(pImage, xSize, COL8_C6C6C6, 0,         0,         xSize - 1, 0        );
	boxfill8(pImage, xSize, COL8_FFFFFF, 1,         1,         xSize - 2, 1        );
	boxfill8(pImage, xSize, COL8_C6C6C6, 0,         0,         0,         ySize - 1);
	boxfill8(pImage, xSize, COL8_FFFFFF, 1,         1,         1,         ySize - 2);
	boxfill8(pImage, xSize, COL8_848484, xSize - 2, 1,         xSize - 2, ySize - 2);
	boxfill8(pImage, xSize, COL8_000000, xSize - 1, 0,         xSize - 1, ySize - 1);
	boxfill8(pImage, xSize, COL8_C6C6C6, 2,         2,         xSize - 3, ySize - 3);
	boxfill8(pImage, xSize, COL8_000084, 3,         3,         xSize - 4, 20       );
	boxfill8(pImage, xSize, COL8_848484, 1,         ySize - 2, xSize - 2, ySize - 2);
	boxfill8(pImage, xSize, COL8_000000, 0,         ySize - 1, xSize - 1, ySize - 1);
	putString(pImage, xSize, 24, 4, COL8_FFFFFF, title);
	for (y = 0; y < 14; y++) {
		for (x = 0; x < 16; x++) {
			c = pCloseButton[y][x];
			if (c == '@') {
				c = COL8_000000;
			} else if (c == '$') {
				c = COL8_848484;
			} else if (c == 'Q') {
				c = COL8_C6C6C6;
			} else {
				c = COL8_FFFFFF;
			}
			pImage[(5 + y) * xSize + (xSize - 21 + x)] = c;
		}
	}
	return;
}


