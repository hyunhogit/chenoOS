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

#define COL8_000000		0
#define COL8_FF0000		1
#define COL8_00FF00		2
#define COL8_FFFF00		3
#define COL8_0000FF		4
#define COL8_FF00FF		5
#define COL8_00FFFF		6
#define COL8_FFFFFF		7
#define COL8_C6C6C6		8
#define COL8_840000		9
#define COL8_008400		10
#define COL8_848400		11
#define COL8_000084		12
#define COL8_840084		13
#define COL8_008484		14
#define COL8_848484		15

typedef struct BOOTINFO{
	char cyls;
	char leds;
	char videomode;
	char reserve;
	short screen_xsize;
	short screen_ysize;	
	char* address_vram;
}BOOTINFO;

void HariMain(void)
{
	BOOTINFO* address_boot_info;

	extern char hankaku[4096];

	init_pallette();			/*pallette�ʱ�ȭ*/
	
	address_boot_info=(BOOTINFO*)0x0ff0;
	
	
	init_screen(address_boot_info->address_vram,address_boot_info->screen_xsize,address_boot_info->screen_ysize);
	putFont8(address_boot_info->address_vram,address_boot_info->screen_xsize,10,10,COL8_000000,hankaku+0x41*16);
	putFont8(address_boot_info->address_vram,address_boot_info->screen_xsize,18,10,COL8_000000,hankaku+0x42*16);
	putFont8(address_boot_info->address_vram,address_boot_info->screen_xsize,26,10,COL8_000000,hankaku+'C'*16);

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
