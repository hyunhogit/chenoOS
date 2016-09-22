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
/* ���� ���� ���� ���Ͽ� �� �־ �����ϱ� ���� ����Ѵٸ�,���� ������ ���� ������ �� �ȴ�. */
void init_pallette(void);
void set_pallette(int start, int end, unsigned char *rgb);

void HariMain(void)
{
	int i;				/*32bit ������*/
	char* p;	 
	p=(char*) 0xa0000;		/*p�� VRAM�� �ּҸ� ���Ե�*/

	init_pallette();		/*pallette�ʱ�ȭ*/
	for(i=0;i<=0xffff;i++){		/*0xa0000~0x0affff:�����޸��ּ�*/
		*(p+i)=i & 0x0f;	/*0:Black,7:Gray,15:White*/
	}
	
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
	set_pallette(0, 15, table_rgb);
	return;
}

void set_pallette(int start, int end, unsigned char *rgb)
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
