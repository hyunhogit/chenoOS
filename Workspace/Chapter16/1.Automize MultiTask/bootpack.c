#include <stdio.h>
#include "bootpack.h"

void Run_TaskB( SheetManager* pSheetManager );

void HariMain(void)
{
	BOOTINFO* pBootInfo=(BOOTINFO*)ADR_BOOTINFO;
	char s[40]; 		/* Memory for Printing Message */
	int count = 0; 		/* CPU Performance Checker */
		
	FIFO timerFifo ;
	Timer* timer1, *timer2 , *timer3 ;
	char timerBuf[8] ;		/* Buffer for Timer*/
	
	int xPosition,yPosition;	/* Position of Mouse*/
	int cursorPosition = 8 ;	/* Position of Cursor*/
	int i;			/* Loop Index */
	char keyBuf[32];		/* Buffer for KeyBoard */
	char mouseBuf[128];	/* Buffer for Mouse */
	MOUSE_DATA mouseData;	/* DataStructure for Mouse */
	unsigned int totalMemory;	/* Tatal Memory Size*/
	MemoryManager* memoryManager = (MemoryManager *) MEMMAN_ADDR;
	SheetManager* pSheetManager;	
	Sheet* pSheetBackGround;	/*바탕화면을 위한 Sheet */
	Sheet* pSheetMouse;		/*마우스를 위한 Sheet*/
	Sheet* pSheetWindow2;
	unsigned char pMouseImage[256];	/* Image for Mouse */
	unsigned char* pBackGroundImage;	/* Image for BackGround*/
	unsigned char* pWindowImage2;	/* Image for Window2 */
	
	Task* pTaskB;

	static char keyTable[0x54] = {
		0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,   0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']',   0,   0,   'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`',   0,   '\\', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.'
	};
	
	init_gdt();			/*GDT와 IDT가 제대로 설정안하고 임시로 32비트 모드로 넘어왔기 때문에 가장먼저 실행해 주어야한다.*/
	init_idt();
	init_pic();
	io_sti(); 			/* IDT/PIC의 초기화가 끝났으므로 CPU의 인터럽트 금지를 해제 */
	InitKeyboard();
	FifoInit( &keyFifo , 32 , keyBuf) ;
	FifoInit( &mouseFifo, 128 , mouseBuf ) ;
	xPosition=(pBootInfo->xSize- 16) / 2;
	yPosition=(pBootInfo->ySize- 28 - 16) / 2;
	EnableMouse( &mouseData );
	Init_PIT();			/*타이머 매니저 초기화*/
	io_out8(PIC0_IMR, 0xf8); 	/* PIC1와 키보드를 허가(11111001) */
	io_out8(PIC1_IMR, 0xef); 	/* 마우스를 허가(11101111) */ 
	
	/* Dealing with Timer */ 
	timer1 = Alloc_Timer();
	timer2 = Alloc_Timer();
	timer3 = Alloc_Timer();
	
	FifoInit( &timerFifo , 8 , timerBuf );

	Init_Timer( timer1, &timerFifo , 10);
	Init_Timer( timer2, &timerFifo , 3);
	Init_Timer( timer3, &timerFifo , 1);	
	
	Set_Timer( timer1 , 1000 ) ;
	Set_Timer( timer2 , 300 ) ;
	Set_Timer( timer3 , 50 ) ;

	/* Dealing with Memory */	
	totalMemory = TestMemory(0x00400000, 0xbfffffff);
	Init_MemoryManager(memoryManager);
	Free_Memory(memoryManager, 0x00001000, 0x0009e000); /* 0x00001000 - 0x0009efff */
	Free_Memory(memoryManager, 0x00400000, totalMemory - 0x00400000);

	init_pallette();			/*pallette초기화*/
	pSheetManager= Init_SheetManager(memoryManager,pBootInfo->pVideo , pBootInfo->xSize,pBootInfo->ySize);
	
	/* Allocating Sheet to SheetManager */
	pSheetBackGround=Alloc_Sheet(pSheetManager);
	pSheetMouse=Alloc_Sheet(pSheetManager);
	pSheetWindow2=Alloc_Sheet(pSheetManager);
	
	/* Making space for image */
	pBackGroundImage=(unsigned char *) Alloc_MemoryBlock(memoryManager, (pBootInfo->xSize)*( pBootInfo->ySize));
	pWindowImage2=(unsigned char*)Alloc_MemoryBlock(memoryManager, 160*52);
	
	/* Making Image */ 	
	init_screen(pBackGroundImage,pBootInfo->xSize,pBootInfo->ySize);	
	init_mouse(pMouseImage,99);
	Make_Window(pWindowImage2,160,52,"Key Input");
	
	/* Setting Sheet image */
	Set_pImageOfSheet(pSheetBackGround, pBackGroundImage, pBootInfo->xSize, pBootInfo->ySize, -1); /* 투명색없음 */
	Set_pImageOfSheet(pSheetMouse, pMouseImage, 16, 16, 99);/*99:투명색 번호*/
	Set_pImageOfSheet(pSheetWindow2,pWindowImage2,160,52,-1);
	
	/* Positioning Sheet */
	Move_Sheet(pSheetManager,pSheetBackGround, 0,0);
	Move_Sheet(pSheetManager,pSheetMouse,xPosition,yPosition);
	Move_Sheet(pSheetManager,pSheetWindow2,450,100);
	
	/* Positioning Sheet in height */
	UpDown_Sheet( pSheetManager,  pSheetBackGround, 0);
	UpDown_Sheet(pSheetManager, pSheetWindow2, 2);
	UpDown_Sheet( pSheetManager,  pSheetMouse, 3);
	
	/* You can changing image */
	sprintf(s, "memory %dMB   free : %dKB", totalMemory / (1024 * 1024), Check_Size(memoryManager) / 1024);
	putString( pBackGroundImage , pBootInfo->xSize , 0, 48,COL8_FFFFFF, s); 
	Make_TextBox( pSheetWindow2, 8 , 28 , 152 , 44 , COL8_FFFFFF );
	
	/* Dealing with Tasks */
	Init_TaskManager( memoryManager ) ;	/* Harimain is first task */
	pTaskB=Alloc_Task();			/* Second task is taskB */
	pTaskB->taskStatus.esp = Alloc_MemoryBlock(memoryManager, 64 * 1024) + 64 * 1024-8;	/* Stack space for task B */
	pTaskB->taskStatus.eip = (int) &Run_TaskB;
	pTaskB->taskStatus.es = 1 * 8;
	pTaskB->taskStatus.cs = 2 * 8;
	pTaskB->taskStatus.ss = 1 * 8;
	pTaskB->taskStatus.ds = 1 * 8;
	pTaskB->taskStatus.fs = 1 * 8;
	pTaskB->taskStatus.gs = 1 * 8;
	
	/* Exporting SheetManager */
	*((int*) (pTaskB->taskStatus.esp+4))= (int)pSheetManager;		/* Argument : pSheetManager */
	
	Run_Task(pTaskB);

	/* Draw all Sheet */
	Draw_AllSheet(pSheetManager);
	
	for(;;){
		count++;
		io_cli();	/* 우선 인터럽트 막자 */
		if( FifoStatus( &keyFifo) + FifoStatus( &mouseFifo ) + FifoStatus( &timerFifo ) == 0 ){
			io_sti();
		}else{
			if( FifoStatus( &keyFifo) != 0 ){
				i=FifoGet( &keyFifo );
				io_sti();	/* 데이터만 빼고 다시 인터럽트 개방 */
				sprintf(s,"%02X",i);
				Draw_String( pSheetManager , pSheetBackGround , 0, 0 , COL8_FFFFFF , COL8_008484 , s , 2 );
				if( i < 0x54){
					if( i == 0x0E){	/* Backspace */
						Draw_String( pSheetManager , pSheetWindow2 , cursorPosition, 28 , COL8_000000  , COL8_FFFFFF , " " , 1 );	
						cursorPosition -= 8;
					}
					if( keyTable[i] != 0 ){
						s[0] = keyTable[i] ; 
						s[1] = 0;
						Draw_String( pSheetManager , pSheetWindow2 , cursorPosition, 28 , COL8_000000  , COL8_FFFFFF , s , 1 );
						cursorPosition += 8 ;
					}
					if( cursorPosition <8){
						cursorPosition = 8 ;
					}else if(cursorPosition >144){
						cursorPosition =144;
					}
				}
			}else if( FifoStatus( &mouseFifo) != 0 ){
				i=FifoGet( &mouseFifo );
				io_sti();	/* 데이터만 빼고 다시 인터럽트 개방 */
				if( DecodeMouse( &mouseData , i) ==1 ){
					/* 데이터가 3바이트 모였으므로 표시 */
					sprintf(s, "[lcr %4d %4d]", mouseData.x, mouseData.y);
					if ((mouseData.button & 0x01) != 0) {
						s[1] = 'L';
						Move_Sheet(pSheetManager,pSheetWindow2,xPosition,yPosition);
					}
					if ((mouseData.button & 0x02) != 0) {
						s[3] = 'R';
					}
					if ((mouseData.button & 0x04) != 0) {
						s[2] = 'C';
					}
			
					Draw_String( pSheetManager , pSheetBackGround , 0, 32 , COL8_FFFFFF , COL8_008484 , s , 15 );
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
					Draw_String( pSheetManager , pSheetBackGround , 0, 16 , COL8_FFFFFF , COL8_008484 , s , 10 );
					Move_Sheet(pSheetManager,pSheetMouse,xPosition,yPosition); /* 마우스 그린다 */		
				}
			}else if( FifoStatus( &timerFifo ) != 0 ){
				i= FifoGet(&timerFifo);	/*먼저읽어들인다. 비워두기 위해 */
				io_sti();
				if( i == 10 ){
					Draw_String( pSheetManager , pSheetBackGround , 0, 80 , COL8_FFFFFF , COL8_008400 , "Time Out 10" , 11 );
					sprintf(s, "%010d", count);
					Draw_String( pSheetManager , pSheetBackGround , 0, 96 , COL8_FFFFFF , COL8_008400 , s , 10 );
					
				}else if( i == 3 ){
					Draw_String( pSheetManager , pSheetBackGround , 0, 64 , COL8_FFFFFF , COL8_008400 , "Time Out 3" , 10 );
					count=0;
				}else{
					if(i!=0){ /* 깜빡임 처리 */
						Init_Timer( timer3 , &timerFifo , 0); /* 다음은 0으로 */
						boxfill8( pWindowImage2 ,pSheetWindow2->xSize , COL8_000000, cursorPosition,28, cursorPosition+8-1, 43);
					}else{
						Init_Timer( timer3 , &timerFifo , 1);
						boxfill8( pWindowImage2 ,pSheetWindow2->xSize , COL8_FFFFFF, cursorPosition,28, cursorPosition+8-1, 43);
					}
					Set_Timer(timer3, 50 );
					Refresh_Sheet(pSheetManager,pSheetWindow2,cursorPosition,28,cursorPosition+8,44);
				}
				
			}
		}	
	}
}

void Run_TaskB( SheetManager* pSheetManager ){	/* Don't Share variables with hariMain */
	Timer* screenTimer;
	FIFO fifo;
	char fifoBuf[128];
	char s[11];
	int i;
	int count = 0;
	Sheet* pSheetWindow;		/*Window Sheet */
	unsigned char* pWindowImage;		/* Window Image */	
	MemoryManager* memoryManager = (MemoryManager *) MEMMAN_ADDR;
	
	pSheetWindow=Alloc_Sheet(pSheetManager);
	pWindowImage=(unsigned char*)Alloc_MemoryBlock(memoryManager, 160*52);
	Make_Window(pWindowImage,160,52,"Counter");
	Set_pImageOfSheet(pSheetWindow,pWindowImage,160,52,-1);
	Move_Sheet(pSheetManager,pSheetWindow,450,35);
	UpDown_Sheet( pSheetManager,  pSheetWindow, 1);

	FifoInit( &fifo , 128 , fifoBuf );

	screenTimer = Alloc_Timer() ;
	Init_Timer( screenTimer , &fifo , 1) ;
	Set_Timer( screenTimer , 1 ) ;

	for( ; ; ){
		count++;
		io_cli();
		if( FifoStatus( &fifo ) == 0 ){
			io_stihlt();
		}else{
			i = FifoGet( &fifo ) ;
			io_sti();
			sprintf( s , "%11d", count );	
			Draw_String( pSheetManager , pSheetWindow, 40, 28 , COL8_000000 , COL8_C6C6C6 , s , 10 );
			Set_Timer( screenTimer , 1 ) ;
		
		}
	}
}
