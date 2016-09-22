#include <stdio.h>

void io_hlt(void);
void io_cli(void);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);
/* ���� ���� ���� ���Ͽ� �� �־ �����ϱ� ���� ����Ѵٸ�,���� ������ ���� ������ �� �ȴ�. */
void init_pallette(void);
void set_palette(int start, int end, unsigned char *rgb);
void boxfill8(unsigned char* vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void init_screen(char* p_vram,int xsize,int ysize);
void putFont8(char* address_vram,int xsize,int x,int y,unsigned char color,char* font);
void putString(char* address_vram,int xsize,int x,int y,unsigned char color,char* address_string);
void init_mouse(char* cursor,unsigned char background_color);
void putMouse(char* address_vram,int xsize,int x_position,int y_position,char* cursor);


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

typedef struct BOOTINFO{
	char cyls;
	char leds;
	char videomode;
	char reserve;
	short screen_xsize;
	short screen_ysize;	
	char* address_vram;
}BOOTINFO;

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
void SetGateDescriptor(GATE_DESCRIPTOR* pGateDescriptor,int offset,int selector,int accessRight);
void load_idtr(int limit , int address);

void HariMain(void)
{
	BOOTINFO* address_boot_info;
	char s[40];
	char cursor[256];
	int version=1;
	int x_position,y_position;
	init_pallette();			/*pallette�ʱ�ȭ*/
	init_gdt();
	init_idt();
	
	
	address_boot_info=(BOOTINFO*)0x0ff0;
	x_position=(address_boot_info->screen_xsize- 16) / 2;
	y_position=(address_boot_info->screen_ysize- 28 - 16) / 2; 
	sprintf(s,"HO Operating System:%d",version);	
	
	init_screen(address_boot_info->address_vram,address_boot_info->screen_xsize,address_boot_info->screen_ysize);
	init_mouse(cursor,COL8_008400);
	putString(address_boot_info->address_vram,address_boot_info->screen_xsize,140,10,COL8_000000,s);
	putMouse(address_boot_info->address_vram,address_boot_info->screen_xsize,x_position,y_position,cursor);
	for(;;){
		io_hlt();		
	}
}




