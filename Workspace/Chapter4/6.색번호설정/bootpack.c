void write_mem8(void);
void io_hlt(void);
void io_cli(void);
void io_sti(void);
void io_stihlt(void);
void io_in8(int port,int data);
void io_in16(int port,int data);
void io_in32(int port,int data);
void io_out8(int port, int data);
void io_in16(int port,int data);
void io_in32(int port,int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);
/* 실은 같은 원시 파일에 써 있어도 정의하기 전에 사용한다면,역시 선언해 두지 않으면 안 된다. */
void init_pallette(void);
void set_pallette(int start, int end, unsigned char *rgb);

void HariMain(void)
{
	int i;				/*32bit 정수형*/
	char* p;	 
	p=(char*) 0xa0000;		/*p가 VRAM의 주소를 갖게됨*/

	init_pallette();		/*pallette초기화*/
	for(i=0;i<=0xffff;i++){		/*0xa0000~0x0affff:비디오메모리주소*/
		*(p+i)=i & 0x0f;	/*0:Black,7:Gray,15:White*/
	}
	
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
	set_pallette(0, 15, table_rgb);
	return;
}

void set_pallette(int start, int end, unsigned char *rgb)
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
