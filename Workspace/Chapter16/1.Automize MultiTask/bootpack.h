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

SEGMENT_DESCRIPTOR* gdt=(SEGMENT_DESCRIPTOR *) ADR_GDT;     //������ �޸� �����
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
	unsigned char free ; 		/* ���ۿ� ����ִ� ������ ũ�� */
	unsigned char flag ;		/* ���۰� ���ƴ��� Ȯ���ϱ� ���� */
}FIFO;

void FifoInit( FIFO* fifo , int size , unsigned char* buf );
int FifoPut( FIFO* fifo, unsigned char data );
int FifoGet( FIFO* fifo );
int FifoStatus( FIFO* fifo);
#include "fifo.h"

/**************************************************************************************************************************/
#define EFLAGS_AC_BIT		0x00040000
#define CR0_CACHE_DISABLE	0x60000000 /* ĳ���� ���� ���� */

#define MEMMAN_FREES		4090	/* �� �޸� �ִ밳��, �̰����� �� 32KB */
#define MEMMAN_ADDR		0x003c0000

typedef struct freememory {	/* �� ���� */
	unsigned int addr, size;
}FreeMemory;

typedef struct memorymanager {		/* �޸� �޴�����Ʈ */
	int frees;		/* ���� ������� ���� */
	int maxfrees;		/* ��Ȳ������. frees�� ���� �ִ밪 */
	int lostsize;		/* ����� ������ �ʰ��� �Ҿ���� �뷮 */
	int losts;		/* ����� ������ �ʰ��� �Ҿ���� Ƚ�� */
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
#define TIMER_FLAGS_ALLOC		1	/* Ȯ���� ���� */
#define TIMER_FLAGS_USING		2	/* Ÿ�̸� �۵��� */

typedef struct timer Timer;

struct timer{
	unsigned int timeOut;	/* time out�Ǵ� Ÿ�̸� ���α��� ����ð� */
	FIFO* pFifo;		/* Ÿ�Ӿƿ��� �����͸� ���� ���� */
	unsigned char data;		/* Ÿ�Ӿƿ��� ���� ������ */
	unsigned int flag; 		/* ������� Ÿ�̸����� �ƴ��� */
	Timer* pNextTimer;		/* �� ���� �︱ Ÿ�̸Ӹ� ����Ŵ */			
};

typedef struct timerManager{
	unsigned int count ;		/* �Ŵ����� ��� �ִ� �ð� */
	unsigned int timeOut ;	/* �ٷ� �������� time out ����ð� */
	Timer* pTimer ;		/* �ٷ� ������ �︱ Ÿ�̸Ӹ� ����Ŵ */
	Timer timer[MAX_TIMER] ;	/* ��� Ÿ�̸ӵ��� �����صΰ� ���� */
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

typedef struct Sheet {        		/*�ϳ��� ���� */
	unsigned char* pImage; 	/* �׸��� �ִ� �� */
	int xSize;            		/* �̹��� x size */
	int ySize;			/* �̹��� y size */
	int xPosition;		/* �̹��� �»�� x ��ġ */
	int yPosition;		/* �̹��� �»�� y ��ġ */	
	int col_inv;
	int height;			/* ������ ���� ���� */
	int flag;			/* ���� ��� ���� UNUSING_SHEET:�̻��,USING_SHEET:��� */
}Sheet;

#define	MAX_SHEET 256		/* ������ �ִ� ������ �ִ�� */

typedef struct SheetManager{
	unsigned char* pVideo;	/* ���� �޸� */
	int xSize;			/* ȭ���� xSize */
	int ySize;			/* ȭ���� ySize */
	int maxHeight;		/* sheet�� �ְ� ���� */
	Sheet* pSHEETs[MAX_SHEET];	/* sheet�� ������� �����ص� �迭*/
	Sheet sheets[MAX_SHEET];	/* ������ sheet�� ������ */
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
#define COL8_000000		0		/*  0:�� */
#define COL8_FF0000		1		/*  1:���� ���� */
#define COL8_00FF00		2		/*  2:���� �ʷ� */
#define COL8_FFFF00		3		/*  3:���� Ȳ�� */
#define COL8_0000FF		4		/*  4:���� �Ķ� */
#define COL8_FF00FF		5		/*  5:���� ����� */
#define COL8_00FFFF		6		/*  6:���� ���� */
#define COL8_FFFFFF		7		/*  7:��� */
#define COL8_C6C6C6		8		/*  8:���� ȸ�� */
#define COL8_840000		9		/*  9:��ο� ���� */
#define COL8_008400		10		/* 10:��ο� �ʷ� */
#define COL8_848400		11		/* 11:��ο� Ȳ�� */
#define COL8_000084		12		/* 12:��ο� �Ķ� */
#define COL8_840084		13		/* 13:��ο� ����� */
#define COL8_008484		14		/* 14:��ο� ���� */
#define COL8_848484		15		/* 15:��ο� ȸ�� */

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
