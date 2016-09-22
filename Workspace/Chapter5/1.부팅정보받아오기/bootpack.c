void io_hlt(void);
void io_cli(void);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);
/* 실은 같은 원시 파일에 써 있어도 정의하기 전에 사용한다면,역시 선언해 두지 않으면 안 된다. */
void init_pallette(void);
void set_palette(int start, int end, unsigned char *rgb);
void boxfill8(unsigned char* vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void init_screen(char* p_vram,int xsize,int ysize);

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

void HariMain(void)
{
	int xsize,ysize;
	char* p_vram;
	short* p_boot_info_screen_size_x;
	short* p_boot_info_screen_size_y;
	int* p_boot_info_vram;
	
	init_pallette();			/*pallette초기화*/
	
	p_boot_info_screen_size_x=(short*)0x0ff4;
	p_boot_info_screen_size_y=(short*)0x0ff6;
	p_boot_info_vram=(int*)0x0ff8;
	
	xsize=*p_boot_info_screen_size_x;
	ysize=*p_boot_info_screen_size_y;	 
	p_vram=(char*)*p_boot_info_vram;		/*p가 VRAM의 주소를 갖게됨*/
	
	init_screen(p_vram,xsize,ysize);

	for(;;){
		io_hlt();		
	}
}


void init_pallette(void){
	static unsigned char table_rgb[16*3]={/* static char 명령은 데이터 밖에 사용할 수 없지만 DB명령에 상당 */
		0x00, 0x00, 0x00,	/*  0:흑 */
		0xff, 0x00, 0x00,	/*  1:밝은 빨강 */
		0x00, 0xff, 0x00,	/*  2:밝은 초록 */
		0xff, 0xff, 0x00,	/*  3:밝은 황색 */
		0x00, 0x00, 0xff,	/*  4:밝은 파랑 */
		0xff, 0x00, 0xff,	/*  5:밝은 보라색 */
		0x00, 0xff, 0xff,	/*  6:밝은 물색 */
		0xff, 0xff, 0xff,	/*  7:흰색 */
		0xc6, 0xc6, 0xc6,	/*  8:밝은 회색 */
		0x84, 0x00, 0x00,	/*  9:어두운 빨강 */
		0x00, 0x84, 0x00,	/* 10:어두운 초록 */
		0x84, 0x84, 0x00,	/* 11:어두운 황색 */
		0x00, 0x00, 0x84,	/* 12:어두운 파랑 */
		0x84, 0x00, 0x84,	/* 13:어두운 보라색 */
		0x00, 0x84, 0x84,	/* 14:어두운 물색 */
		0x84, 0x84, 0x84	/* 15:어두운 회색 */
	};
	set_palette(0, 15, table_rgb);
	return;
}

void set_palette(int start, int end, unsigned char *rgb)
{
	int i, eflags;
	eflags = io_load_eflags();	/* 인터럽트 허가 플래그의 값을 기록한다 */
	io_cli(); 			/* 허가 플래그를 0으로 하여 인터럽트를 금지로 한다 */
	io_out8(0x03c8, start);
	for (i = start; i <= end; i++) {
		io_out8(0x03c9, rgb[0] / 4);
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	io_store_eflags(eflags);	/* 인터럽트 허가 플래그를 원래대로 되돌린다 */
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
