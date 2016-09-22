/******************************************************************************************************************/
#define ADR_BOOTINFO	0x00000ff0

typedef struct BOOTINFO{
	char cyls;
	char leds;
	char videomode;
	char reserve;
	short xSize;
	short ySize;	
	char* pVideo;
}BOOTINFO;

void io_hlt(void);
void io_cli(void);
void io_sti(void);
void io_stihlt(void);
int io_in8(int port);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
void asm_inthandler20(void);
void asm_inthandler21(void);
void asm_inthandler2c(void);
int load_cr0(void);
void store_cr0(int cr0);
unsigned int memtest_sub(unsigned int start, unsigned int end);
void load_tr(int tr);
void SwitchGDT3(void);
void SwitchGDT4(void);
void Jump_Far( int eip , int cs );

/***************************************************************************************************************/
#define ADR_IDT			0x0026f800
#define LIMIT_IDT		0x000007ff
#define ADR_GDT			0x00270000
#define LIMIT_GDT		0x0000ffff
#define ADR_BOTPAK		0x00280000
#define LIMIT_BOTPAK	0x0007ffff
#define AR_DATA32_RW	0x4092
#define AR_CODE32_ER	0x409a
#define AR_INTGATE32	0x008e
#define AR_TSS32		0x0089

typedef struct SEGMENT_DESCRIPTOR{
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
}SEGMENT_DESCRIPTOR;

typedef struct GATE_DESCRIPTOR{
	short offset_low, selector;
	char dw_count, access_right;
	short offset_high;
}GATE_DESCRIPTOR;

SEGMENT_DESCRIPTOR* gdt=(SEGMENT_DESCRIPTOR *) ADR_GDT;     //임의의 메모리 빈공간
GATE_DESCRIPTOR* idt=(GATE_DESCRIPTOR*) ADR_IDT ;

void init_gdt(void);
void set_segment_descriptor(SEGMENT_DESCRIPTOR* pSegDes,unsigned int limit,unsigned int base,unsigned int ar);
void load_gdtr(int limit,int address);
void init_idt(void);
void set_gate_descriptor(GATE_DESCRIPTOR* pGateDescriptor,int offset,int selector,int accessRight);
void load_idtr(int limit , int address);
#include "descript_table.h"

/************************************************************************************************************************/
#define OVERRUN 0x0001 ;

typedef struct fifo{
	unsigned char* buf;
	unsigned char nextRead ;
	unsigned char nextWrite ;
	unsigned char length ; 		/* 0:vacant 1:full */
	unsigned char free ; 		/* 버퍼에 비어있는 공간의 크기 */
	unsigned char flag ;		/* 버퍼가 넘쳤는지 확인하기 위함 */
}FIFO;

void FifoInit( FIFO* fifo , int size , unsigned char* buf );
int FifoPut( FIFO* fifo, unsigned char data );
int FifoGet( FIFO* fifo );
int FifoStatus( FIFO* fifo);
#include "fifo.h"

/**************************************************************************************************************************/
#define EFLAGS_AC_BIT		0x00040000
#define CR0_CACHE_DISABLE	0x60000000 /* 캐쉬를 끄기 위함 */

#define MEMMAN_FREES		4090	/* 빈 메모리 최대개수, 이것으로 약 32KB */
#define MEMMAN_ADDR		0x003c0000

typedef struct freememory {	/* 빈 정보 */
	unsigned int addr, size;
}FreeMemory;

typedef struct memorymanager {		/* 메모리 메니지먼트 */
	int frees;		/* 현재 빈공간의 개수 */
	int maxfrees;		/* 상황관찰용. frees의 역대 최대값 */
	int lostsize;		/* 빈공간 개수를 초과해 잃어버린 용량 */
	int losts;		/* 빈공간 개수를 초과해 잃어버린 횟수 */
	FreeMemory free[MEMMAN_FREES];
}MemoryManager;

unsigned int TestMemory(unsigned int start, unsigned int end);
void Init_MemoryManager(MemoryManager *man);
unsigned int Check_Size(MemoryManager *man);
unsigned int Alloc_Memory(MemoryManager *man, unsigned int size);
int Free_Memory(MemoryManager *man, unsigned int addr, unsigned int size);
unsigned int Alloc_MemoryBlock(MemoryManager *man, unsigned int size);
int Free_MemoryBlock(MemoryManager *man, unsigned int addr, unsigned int size);
#include "memory.h"

/***************************************************************************************************************************/
#define PIT_CTRL 0x0043
#define PIT_CNT0 0x0040
#define MAX_TIMER 500
#define TIMER_FLAG_UNUSING		0
#define TIMER_FLAGS_ALLOC		1	/* 확보한 상태 */
#define TIMER_FLAGS_USING		2	/* 타이머 작동중 */

typedef struct timer Timer;

struct timer{
	unsigned int timeOut;	/* time out되는 타이머 본인기준 절대시간 */
	FIFO* pFifo;		/* 타임아웃시 데이터를 보낼 버퍼 */
	unsigned char data;		/* 타임아웃시 보낼 데이터 */
	unsigned int flag; 		/* 사용중인 타이머인지 아닌지 */
	Timer* pNextTimer;		/* 나 다음 울릴 타이머를 가르킴 */			
};

typedef struct timerManager{
	unsigned int count ;		/* 매니저가 재고 있는 시간 */
	unsigned int timeOut ;	/* 바로 다음에올 time out 절대시간 */
	Timer* pTimer ;		/* 바로 다음에 울릴 타이머를 가르킴 */
	Timer timer[MAX_TIMER] ;	/* 모든 타이머들을 저장해두고 있음 */
}TimerManager;

TimerManager timerManager;

void Init_PIT(void) ;
Timer* Alloc_Timer() ;
void Free_Timer( Timer* pTimer) ;
void Init_Timer( Timer* pTimer , FIFO* pFifo , unsigned char data ) ;
void Set_Timer( Timer* pTimer, unsigned int timeOut ) ;
#include "timer.h"

/*************************************************************************************************************************/
#define MAX_TASKS		 1000
#define TASK_POSITION	 3

typedef struct TaskStatus{
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;				
}TaskStatus;

typedef struct TASK{
	int selector ; 			/* The number of GDT */
	int flag;			/* 0: unusing , 1: using , 2: running */
	TaskStatus taskStatus;	/* TaskStatusState*/
}Task;

typedef struct TASKMANAGER{
	int numRunning;	/* The number of running task */
	int taskRunning;	/* The task that is running */
	Task tasks[ MAX_TASKS ];
	Task* pTasks[ MAX_TASKS ];	/* For ordering */
}TaskManager;

int taskNumber;
TaskManager*  pTaskManager;
Timer* pSwitchTimer;

Task* Init_TaskManager( MemoryManager* pMemoryManager );
Task* Alloc_Task();
void Run_Task( Task* pTask );
void Switch_Task();

#include "multiTask.h"

/*************************************************************************************************************************/
#define PIC0_ICW1		0x0020
#define PIC0_OCW2		0x0020
#define PIC0_IMR		0x0021
#define PIC0_ICW2		0x0021
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC1_ICW1		0x00a0
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1
#define PIC1_ICW2		0x00a1
#define PIC1_ICW3		0x00a1
#define PIC1_ICW4		0x00a1

#define PORT_KEYDAT		0x0060

#define PORT_KEYSTA		0x0064
#define PORT_KEYCMD		0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE	0x60
#define KBC_MODE			0x47


#define KEYCMD_SENDTO_MOUSE		0xd4
#define MOUSECMD_ENABLE			0xf4

FIFO keyFifo;
FIFO mouseFifo;

typedef struct mouse_data {
	unsigned char data[3];
	unsigned char phase;
	int x , y ;
	int button ;
}MOUSE_DATA;


void init_pic(void);
void inthandler20(int* esp);
void inthandler21(int *esp);
void inthandler2c(int *esp);
void WaitKBCSendReady(void);
void InitKeyboard(void);
void EnableMouse(MOUSE_DATA* mouseData);
int DecodeMouse(MOUSE_DATA* mouseData , unsigned char data);
#include "int.h"

/**************************************************************************************************************************/
#define USING_SHEET 1
#define UNUSING_SHEET 0 

typedef struct Sheet {        		/*하나의 종이 */
	unsigned char* pImage; 	/* 그림이 있는 곳 */
	int xSize;            		/* 이미지 x size */
	int ySize;			/* 이미지 y size */
	int xPosition;		/* 이미지 좌상단 x 위치 */
	int yPosition;		/* 이미지 좌상단 y 위치 */	
	int col_inv;
	int height;			/* 종이의 높이 순서 */
	int flag;			/* 종이 사용 여부 UNUSING_SHEET:미사용,USING_SHEET:사용 */
}Sheet;

#define	MAX_SHEET 256		/* 가질수 있는 종이의 최대수 */

typedef struct SheetManager{
	unsigned char* pVideo;	/* 비디오 메모리 */
	int xSize;			/* 화면의 xSize */
	int ySize;			/* 화면의 ySize */
	int maxHeight;		/* sheet의 최고 높이 */
	Sheet* pSHEETs[MAX_SHEET];	/* sheet를 순서대로 정리해둔 배열*/
	Sheet sheets[MAX_SHEET];	/* 각각의 sheet의 정보들 */
	unsigned char* map;
}SheetManager;

SheetManager* Init_SheetManager(MemoryManager* pMemoryManager, unsigned char* pVideo, int xSize, int ySize);
Sheet* Alloc_Sheet(SheetManager* sheetManager);
void Set_pImageOfSheet(Sheet* pSheet, unsigned char* pImage, int xSize, int ySize, int col_inv);
void UpDown_Sheet(SheetManager* pSheetManager, Sheet* pSheet, int height);
void Draw_Region(SheetManager*  pSheetManager, int xStart, int yStart, int xEnd, int yEnd,int startHeight,int endHeight);
void Draw_AllSheet(SheetManager*  pSheetManager);
void Refresh_Sheet(SheetManager* pSheetManager,Sheet* pSheet,int xStart, int yStart, int xEnd, int yEnd);
void Move_Sheet(SheetManager* pSheetManager, Sheet* pSheet, int xPosition, int yPosition);
void Free_Sheet(SheetManager* pSheetManager, Sheet* pSheet);
void Write_Map(SheetManager*  pSheetManager, int xStart, int yStart, int xEnd, int yEnd, int height);
#include "sheet.h"

/***********************************************************************************************************************/
#define COL8_000000		0		/*  0:흑 */
#define COL8_FF0000		1		/*  1:밝은 빨강 */
#define COL8_00FF00		2		/*  2:밝은 초록 */
#define COL8_FFFF00		3		/*  3:밝은 황색 */
#define COL8_0000FF		4		/*  4:밝은 파랑 */
#define COL8_FF00FF		5		/*  5:밝은 보라색 */
#define COL8_00FFFF		6		/*  6:밝은 물색 */
#define COL8_FFFFFF		7		/*  7:흰색 */
#define COL8_C6C6C6		8		/*  8:밝은 회색 */
#define COL8_840000		9		/*  9:어두운 빨강 */
#define COL8_008400		10		/* 10:어두운 초록 */
#define COL8_848400		11		/* 11:어두운 황색 */
#define COL8_000084		12		/* 12:어두운 파랑 */
#define COL8_840084		13		/* 13:어두운 보라색 */
#define COL8_008484		14		/* 14:어두운 물색 */
#define COL8_848484		15		/* 15:어두운 회색 */

void init_pallette(void);
void set_palette(int start, int end, unsigned char *rgb);
void boxfill8(unsigned char* vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void init_screen(char* p_vram,int xsize,int ysize);
void putFont8(char* address_vram,int xsize,int x,int y,unsigned char color,char* font);
void putString(char* address_vram,int xsize,int x,int y,unsigned char color,char* address_string);
void init_mouse(char* cursor,unsigned char background_color);
void putMouse(char* address_vram,int xsize,int x_position,int y_position,char* cursor);
void Draw_String( SheetManager* pSheetManager , Sheet* pSheet , int xPosition , int yPosition , unsigned char color , unsigned char backGroundColor , unsigned char* string , unsigned int length );
void Make_Window(unsigned char* pImage, int xSize, int ySize, char* title);
void Make_TextBox(Sheet* pSheet, int xStart, int yStart, int xEnd, int yEnd, int color);
#include "graphic.h"
/***************************************************************************************************************************/
