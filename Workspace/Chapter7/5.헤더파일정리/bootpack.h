
/* asm_func.asm */
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
void asm_inthandler21(void);
void asm_inthandler2c(void);

#define ADR_BOOTINFO	0x00000ff0

typedef struct BOOTINFO{
	char cyls;
	char leds;
	char videomode;
	char reserve;
	short screen_xsize;
	short screen_ysize;	
	char* address_vram;
}BOOTINFO;



/* descript_table.h */
#define ADR_IDT			0x0026f800
#define LIMIT_IDT		0x000007ff
#define ADR_GDT			0x00270000
#define LIMIT_GDT		0x0000ffff
#define ADR_BOTPAK		0x00280000
#define LIMIT_BOTPAK	0x0007ffff
#define AR_DATA32_RW	0x4092
#define AR_CODE32_ER	0x409a
#define AR_INTGATE32	0x008e

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

void init_gdt(void);
void set_segment_descriptor(SEGMENT_DESCRIPTOR* pSegDes,unsigned int limit,unsigned int base,unsigned int ar);
void load_gdtr(int limit,int address);
void init_idt(void);
void set_gate_descriptor(GATE_DESCRIPTOR* pGateDescriptor,int offset,int selector,int accessRight);
void load_idtr(int limit , int address);

/* graphic.h */
void init_pallette(void);
void set_palette(int start, int end, unsigned char *rgb);
void boxfill8(unsigned char* vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void init_screen(char* p_vram,int xsize,int ysize);
void putFont8(char* address_vram,int xsize,int x,int y,unsigned char color,char* font);
void putString(char* address_vram,int xsize,int x,int y,unsigned char color,char* address_string);
void init_mouse(char* cursor,unsigned char background_color);
void putMouse(char* address_vram,int xsize,int x_position,int y_position,char* cursor);


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

/* int.h */
void init_pic(void);
void inthandler21(int *esp);
void inthandler2c(int *esp);
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

/* fifo.h */
typedef struct fifo{
	unsigned char* buf;
	unsigned char nextRead ;
	unsigned char nextWrite ;
	unsigned char length ; 		/* 0:vacant 1:full */
	unsigned char free ; 		/* 버퍼에 비어있는 공간의 크기 */
	unsigned char flag ;		/* 버퍼가 넘쳤는지 확인하기 위함 */
}FIFO;
#define OVERRUN 0x0001 ;
void FifoInit( FIFO* fifo , int size , unsigned char* buf );
int FifoPut( FIFO* fifo, unsigned char data );
int FifoGet( FIFO* fifo );
int FifoStatus( FIFO* fifo);
