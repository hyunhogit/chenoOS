#include <stdio.h>
#include "bootpack.h"

Task* pTaskA ;
Task* pTaskB[3];
void Run_TaskB( SheetManager* pSheetManager , int j);

void HariMain(void)
{
	BOOTINFO* pBootInfo=(BOOTINFO*)ADR_BOOTINFO;
	char s[40]; 		/* Memory for Printing Message */
	int count = 0; 		/* CPU Performance Checker */
	
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
	
	Sheet* pSheetBackGround;	/*����ȭ���� ���� Sheet */
	Sheet* pSheetMouse;		/*���콺�� ���� Sheet*/
	Sheet* pSheetWindow2;
	unsigned char pMouseImage[256];	/* Image for Mouse */
	unsigned char* pBackGroundImage;	/* Image for BackGround*/
	unsigned char* pWindowImage2;	/* Image for Window2 */
	
	static char keyTable[0x54] = {
		0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,   0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']',   0,   0,   'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`',   0,   '\\', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.'
	};
	
	init_gdt();			/*GDT�� IDT�� ����� �������ϰ� �ӽ÷� 32��Ʈ ���� �Ѿ�Ա� ������ ������� ������ �־���Ѵ�.*/
	init_idt();
	init_pic();
	io_sti(); 			/* IDT/PIC�� �ʱ�ȭ�� �������Ƿ� CPU�� ���ͷ�Ʈ ������ ���� */
	InitKeyboard();
	FifoInit( &keyFifo , 32 , keyBuf , 0 ) ;
	FifoInit( &mouseFifo, 128 , mouseBuf , 0 ) ;
	xPosition=(pBootInfo->xSize- 16) / 2;
	yPosition=(pBootInfo->ySize- 28 - 16) / 2;
	EnableMouse( &mouseData );
	Init_PIT();			/*Ÿ�̸� �Ŵ��� �ʱ�ȭ*/
	io_out8(PIC0_IMR, 0xf8); 	/* PIC1�� Ű���带 �㰡(11111001) */
	io_out8(PIC1_IMR, 0xef); 	/* ���콺�� �㰡(11101111) */ 
	
	/* Dealing with Timer */ 
	timer1 = Alloc_Timer();
	timer2 = Alloc_Timer();
	timer3 = Alloc_Timer();
	
	FifoInit( &timerFifo , 8 , timerBuf , 0);

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

	init_pallette();			/*pallette�ʱ�ȭ*/
	pSheetManager= Init_SheetManager(memoryManager,pBootInfo->pVideo , pBootInfo->xSize,pBootInfo->ySize);
	
	/* Dealing with Sheet */
	/* BackGround */
	pSheetBackGround=Alloc_Sheet(pSheetManager);	/* Allocating Sheet to SheetManager */
	pBackGroundImage=(unsigned char *) Alloc_MemoryBlock(memoryManager, (pBootInfo->xSize)*( pBootInfo->ySize));	/* Making space for image */
	init_screen(pBackGroundImage,pBootInfo->xSize,pBootInfo->ySize);                                                                                                                 	/* Making Image */ 		
	Set_pImageOfSheet(pSheetBackGround, pBackGroundImage, pBootInfo->xSize, pBootInfo->ySize, -1); /* ��������� */	/* Setting Sheet image */
	sprintf(s, "memory %dMB   free : %dKB", totalMemory / (1024 * 1024), Check_Size(memoryManager) / 1024);	/* You can changing image */	
	putString( pBackGroundImage , pBootInfo->xSize , 400, 30,COL8_FFFFFF, s); 
	Move_Sheet(pSheetManager,pSheetBackGround, 0,0);		/* Positioning Sheet */
	UpDown_Sheet( pSheetManager,  pSheetBackGround, 0);		/* Positioning Sheet in height */
	
	/* Dealing with Tasks */
	pTaskA = Init_TaskManager( memoryManager ) ;	/* Harimain is first task */
	keyFifo.pTask = pTaskA;
	mouseFifo.pTask = pTaskA;
	timerFifo.pTask = pTaskA;
	
	for( i = 0 ; i <3 ; i++ ){
		pTaskB[i] = Alloc_Task();			/* Second task is taskB */
		pTaskB[i]->taskStatus.esp = Alloc_MemoryBlock(memoryManager, 64 * 1024) + 64 * 1024-9;	/* Stack space for task B */
		pTaskB[i]->taskStatus.eip = (int) &Run_TaskB;		
		pTaskB[i]->taskStatus.es = 1 * 8;
		pTaskB[i]->taskStatus.cs = 2 * 8;
		pTaskB[i]->taskStatus.ss = 1 * 8;
		pTaskB[i]->taskStatus.ds = 1 * 8;
		pTaskB[i]->taskStatus.fs = 1 * 8;
		pTaskB[i]->taskStatus.gs = 1 * 8;
		/* Exporting SheetManager */
		*((int*) (pTaskB[i]->taskStatus.esp+4))= (int)pSheetManager;		/* Argument : pSheetManager */
		*((int*) (pTaskB[i]->taskStatus.esp+8))= i ;
		Run_Task(pTaskB[i],  i+1 );		
	}

	/* window2 */
	pSheetWindow2=Alloc_Sheet(pSheetManager);	/* Allocating Sheet to SheetManager */	
	pWindowImage2=(unsigned char*)Alloc_MemoryBlock(memoryManager, 160*52);	/* Making space for image */
	Make_Window(pWindowImage2,160,52,"Key Input",1);	/* Making Image */ 
	Set_pImageOfSheet(pSheetWindow2,pWindowImage2,160,52,-1);	/* Setting Sheet image */
	Move_Sheet(pSheetManager,pSheetWindow2,470,400);		/* Positioning Sheet */
	UpDown_Sheet(pSheetManager, pSheetWindow2, 4);		/* Positioning Sheet in height */
	Make_TextBox( pSheetWindow2, 8 , 28 , 152 , 44 , COL8_FFFFFF );	/* You can changing image */
	
	/* Mouse */
	pSheetMouse=Alloc_Sheet(pSheetManager);			/* Allocating Sheet to SheetManager */
	init_mouse(pMouseImage,99);					/* Making Image */ 
	Set_pImageOfSheet(pSheetMouse, pMouseImage, 16, 16, 99);	/*99:����� ��ȣ*/	/* Setting Sheet image */
	Move_Sheet(pSheetManager,pSheetMouse,xPosition,yPosition);		/* Positioning Sheet */
	UpDown_Sheet( pSheetManager,  pSheetMouse, 5);			/* Positioning Sheet in height */
	Draw_AllSheet(pSheetManager);
	
	for(;;){
		count++;
		io_cli();	/* �켱 ���ͷ�Ʈ ���� */
		if( FifoStatus( &keyFifo) + FifoStatus( &mouseFifo ) + FifoStatus( &timerFifo ) == 0 ){
			Sleep_Task( pTaskA );
			io_sti();
		}else{
			if( FifoStatus( &keyFifo) != 0 ){
				i=FifoGet( &keyFifo );
				io_sti();	/* �����͸� ���� �ٽ� ���ͷ�Ʈ ���� */
				sprintf(s,"%02X",i);
				Draw_String( pSheetManager , pSheetBackGround , 610, 46 , COL8_FFFFFF , COL8_008484 , s , 2 );
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
				io_sti();	/* �����͸� ���� �ٽ� ���ͷ�Ʈ ���� */
				if( DecodeMouse( &mouseData , i) ==1 ){
					/* �����Ͱ� 3����Ʈ �����Ƿ� ǥ�� */
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
			
					Draw_String( pSheetManager , pSheetBackGround , 400, 46 , COL8_FFFFFF , COL8_008484 , s , 15 );
					/* ���콺 Ŀ���� �̵� */
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
					sprintf(s, "(%3d, %3d)", xPosition, yPosition );
					Draw_String( pSheetManager , pSheetBackGround , 520, 46 , COL8_FFFFFF , COL8_008484 , s , 10 );
					Move_Sheet(pSheetManager,pSheetMouse,xPosition,yPosition); /* ���콺 �׸��� */		
				}
			}else if( FifoStatus( &timerFifo ) != 0 ){
				i= FifoGet(&timerFifo);	/*�����о���δ�. ����α� ���� */
				io_sti();
				if( i == 10 ){
					sprintf(s, "%010d", count);
					Draw_String( pSheetManager , pSheetBackGround , 400, 62 , COL8_FFFFFF , COL8_008484 , s , 10 );
				}else if( i == 3 ){
					count=0;
				}else{
					if(i!=0){ /* ������ ó�� */
						Init_Timer( timer3 , &timerFifo , 0); /* ������ 0���� */
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

void Run_TaskB(SheetManager* pSheetManager , int j ){	/* Don't Share variables with hariMain */
	Timer* screenTimer;
	FIFO fifo;
	char fifoBuf[128];
	char s[11];
	int i;
	int count = 0;
	int countStart = 0;
	Sheet* pSheetWindow;		/*Window Sheet */
	unsigned char* pWindowImage;		/* Window Image */	
	MemoryManager* memoryManager = (MemoryManager *) MEMMAN_ADDR;
	
	Draw_AllSheet(pSheetManager);
	
	pSheetWindow=Alloc_Sheet(pSheetManager);
	pWindowImage=(unsigned char*)Alloc_MemoryBlock(memoryManager, 140*52);
	sprintf(s, "counter%d",j);
	Make_Window(pWindowImage,140,52,s,0);
	Set_pImageOfSheet(pSheetWindow,pWindowImage,140,52,-1);
	Move_Sheet(pSheetManager,pSheetWindow,20,20+(60*j));
	UpDown_Sheet( pSheetManager,  pSheetWindow, j+1);
	
	FifoInit( &fifo , 128 , fifoBuf , 0 );

	screenTimer = Alloc_Timer() ;
	Init_Timer( screenTimer , &fifo , 1) ;
	Set_Timer( screenTimer , 100 ) ;
	
	for( ; ; ){
		count++;
		io_cli();
		if( FifoStatus( &fifo ) == 0 ){
			io_sti();
		}else{
			i = FifoGet( &fifo ) ;
			io_sti();
			sprintf( s , "%11d", count-countStart );
			countStart = count ;	
			Draw_String( pSheetManager , pSheetWindow, 40, 28 , COL8_000000 , COL8_C6C6C6 , s , 11 );
			Set_Timer( screenTimer , 100 ) ;
		}
	}
}
