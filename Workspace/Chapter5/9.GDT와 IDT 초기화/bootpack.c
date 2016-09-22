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


void init_pallette(void){
	static unsigned char table_rgb[16*3]={/* static char ����� ������ �ۿ� ����� �� ������ DB��ɿ� ��� */
		0x00, 0x00, 0x00,	/*  0:�� */
		0xff, 0x00, 0x00,	/*  1:���� ���� */
		0x00, 0xff, 0x00,	/*  2:���� �ʷ� */
		0xff, 0xff, 0x00,	/*  3:���� Ȳ�� */
		0x00, 0x00, 0xff,	/*  4:���� �Ķ� */
		0xff, 0x00, 0xff,	/*  5:���� ����� */
		0x00, 0xff, 0xff,	/*  6:���� ���� */
		0xff, 0xff, 0xff,	/*  7:��� */
		0xc6, 0xc6, 0xc6,	/*  8:���� ȸ�� */
		0x84, 0x00, 0x00,	/*  9:��ο� ���� */
		0x00, 0x84, 0x00,	/* 10:��ο� �ʷ� */
		0x84, 0x84, 0x00,	/* 11:��ο� Ȳ�� */
		0x00, 0x00, 0x84,	/* 12:��ο� �Ķ� */
		0x84, 0x00, 0x84,	/* 13:��ο� ����� */
		0x00, 0x84, 0x84,	/* 14:��ο� ���� */
		0x84, 0x84, 0x84	/* 15:��ο� ȸ�� */
	};
	set_palette(0, 15, table_rgb);
	return;
}

void set_palette(int start, int end, unsigned char *rgb)
{
	int i, eflags;
	eflags = io_load_eflags();	/* ���ͷ�Ʈ �㰡 �÷����� ���� ����Ѵ� */
	io_cli(); 			/* �㰡 �÷��׸� 0���� �Ͽ� ���ͷ�Ʈ�� ������ �Ѵ� */
	io_out8(0x03c8, start);
	for (i = start; i <= end; i++) {
		io_out8(0x03c9, rgb[0] / 4);
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	io_store_eflags(eflags);	/* ���ͷ�Ʈ �㰡 �÷��׸� ������� �ǵ����� */
	return;
}

void boxfill8(unsigned char* vram, int xsize, unsigned char color, int x0, int y0, int x1, int y1){
	int x,y;
	
	for (y=y0;y<=y1;y++){
		for(x=x0;x<=x1;x++){
			*(vram+x+y*xsize)=color;
		}
	}
	return;
}

void init_screen(char* p_vram,int xsize,int ysize){
	boxfill8(p_vram,xsize,COL8_008400,0,0,xsize-1,ysize-15);
	boxfill8(p_vram,xsize,COL8_C6C6C6,0,ysize-14,xsize-1,ysize-1);
	
	boxfill8(p_vram,xsize,COL8_FFFFFF,0,ysize-15,xsize-1,ysize-15);
	
	boxfill8(p_vram,xsize,COL8_FFFFFF,1,ysize-13,30,ysize-13);
	boxfill8(p_vram,xsize,COL8_FFFFFF,1,ysize-13,1,ysize-2);
	boxfill8(p_vram,xsize,COL8_848484,1,ysize-2,30,ysize-2);
	boxfill8(p_vram,xsize,COL8_848484,30,ysize-13,30,ysize-2);
	
	boxfill8(p_vram,xsize,COL8_FFFFFF,xsize-30,ysize-2,xsize-2,ysize-2);
	boxfill8(p_vram,xsize,COL8_FFFFFF,xsize-2,ysize-13,xsize-2,ysize-2);
	boxfill8(p_vram,xsize,COL8_848484,xsize-30,ysize-13,xsize-2,ysize-13);
	boxfill8(p_vram,xsize,COL8_848484,xsize-30,ysize-13,xsize-30,ysize-2);
	return;
}

void putFont8(char* address_vram,int xsize,int x,int y,unsigned char color,char* font){	
	int i=0;
	char* address_pointer;
	for(i=0;i<16;i++){
		address_pointer=address_vram+(y+i)*xsize+x;
		if((font[i]&0x80)!=0){address_pointer[0]=color;}
		if((font[i]&0x40)!=0){address_pointer[1]=color;}
		if((font[i]&0x20)!=0){address_pointer[2]=color;}
		if((font[i]&0x10)!=0){address_pointer[3]=color;}
		if((font[i]&0x08)!=0){address_pointer[4]=color;}
		if((font[i]&0x04)!=0){address_pointer[5]=color;}
		if((font[i]&0x02)!=0){address_pointer[6]=color;}
		if((font[i]&0x01)!=0){address_pointer[7]=color;}
	}
	return;	
}

void putString(char* address_vram,int xsize,int x,int y,unsigned char color,char* address_string){
	extern char hankaku[4096];
	for(;*address_string!=0x00;address_string++){
		putFont8(address_vram,xsize,x,y,color,hankaku+(*address_string)* 16);
		x+=8;
	}		
	return;
}

void init_mouse(char* cursor, unsigned char background_color){
	
	static char mouse_cursor[16][16] = {
		"**************..",
		"*OOOOOOOOOOO*...",
		"*OOOOOOOOOO*....",
		"*OOOOOOOOO*.....",
		"*OOOOOOOO*......",
		"*OOOOOOO*.......",
		"*OOOOOOO*.......",
		"*OOOOOOOO*......",
		"*OOOO**OOO*.....",
		"*OOO*..*OOO*....",
		"*OO*....*OOO*...",
		"*O*......*OOO*..",
		"**........*OOO*.",
		"*..........*OOO*",
		"............*OO*",
		".............***"
	};
	int x,y;
	for(y=0;y<16;y++){
		for(x=0;x<16;x++){
			if(mouse_cursor[y][x]=='*')cursor[y*16+x]=COL8_000000;
			if(mouse_cursor[y][x]=='O')cursor[y*16+x]=COL8_FFFFFF;
			if(mouse_cursor[y][x]=='.')cursor[y*16+x]=background_color;
		}	
	}
	return;	
}

void putMouse(char* address_vram,int xsize,int x_position,int y_position,char* cursor){
	int x,y;
	for(y=0;y<16;y++){
		for(x=0;x<16;x++){
			address_vram[(y_position+y)*xsize+(x_position+x)]=cursor[y*16+x];
		}
	}
	return;
}

void init_gdt(void){
	SEGMENT_DESCRIPTOR* gdt=(SEGMENT_DESCRIPTOR *) 0x00270000;     //������ �޸� �����
	int i;
	/*�ʱ�ȭ*/
	for(i=0;i<8192;i++){
		set_segment_descriptor(gdt+i,0,0,0);
	}
	set_segment_descriptor(gdt+1,0xffffffff,0x00000000,0x4092);
	set_segment_descriptor(gdt+2,0x0007ffff,0x00280000,0x409a);
	load_gdtr(0xffff,0x00270000);
	return;
}

void set_segment_descriptor(SEGMENT_DESCRIPTOR* pSegmentDescriptor,unsigned int limit,unsigned int base,unsigned int accessRight){
	if(limit>0xffff){
		accessRight=accessRight|0x8000;		/*G bit=1��*/
		limit=limit/0x1000;
	}
	pSegmentDescriptor->limit_low= limit&0xffff; 	
	pSegmentDescriptor->base_low=base&0xffff;
	pSegmentDescriptor->base_mid=(base>>16)&0xff;
	pSegmentDescriptor->access_right=accessRight&0xff;
	pSegmentDescriptor->limit_high=((limit>>16)&0x0f)|((accessRight>>8)&0xf0);
	pSegmentDescriptor->base_high=(base>>24)&0xff;
	return;
}

void init_idt(void){
	GATE_DESCRIPTOR* idt=(GATE_DESCRIPTOR*)0x0026f800;
	int i=0;

	for (i=0;i<256;i++){
		SetGateDescriptor(idt+i,0,0,0);
	}
	load_idtr(0x7ff,0x0026f800);
	return;
}

void SetGateDescriptor(GATE_DESCRIPTOR* pGateDescriptor,int offset,int selector,int accessRight){
	pGateDescriptor->offset_low=offset&0xffff;
	pGateDescriptor->selector=selector;
	pGateDescriptor->dw_count=(accessRight>>8)&0xff;
	pGateDescriptor->access_right=accessRight&0xff;
	pGateDescriptor->offset_high=(offset>>16)&0xffff;
	return;
}
