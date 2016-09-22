void init_pallette(void){	/*�츮�� ��밡���� ������ ������ 0xFFFFFF�� ���� ���� ����� 255���� 8bit�����صδ°�. ������ 100���־, ���־��� 10���� �ȷ�Ʈ�� ��� �δ°�ó��*/
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

void set_palette(int start, int end, unsigned char *rgb)	/* ������ ������ �ȷ�Ʈ�� ���. ���� DA �����Ͷ�� ��ġ�� ���*/
{
	int i, eflags;
	eflags = io_load_eflags();	/* ���ͷ�Ʈ �㰡 �÷����� ���� ����Ѵ� */
	io_cli(); 			/* �㰡 �÷��׸� 0���� �Ͽ� ���ͷ�Ʈ�� ������ �Ѵ� */
	io_out8(0x03c8, start);		/* 0x03c8�� ���� ���� �ȷ�Ʈ ��ȣ�� ����,�а� �������� 0x03c7 */
	for (i = start; i <= end; i++) {/* �� ���� ���ϴ� 0x03c9�� ������� rgb���� ������Ѵ�*/
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
	char s[40];
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
	sprintf(s,"HO Operating System");	//�޸𸮿� �۾�����
	putString(p_vram,xsize,140,10,COL8_000000,s);//������ �޸𸮷� ���� �׷��ȿ� �۾� ���
	
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


void Draw_String( SheetManager* pSheetManager , Sheet* pSheet , int xPosition , int yPosition , unsigned char color , unsigned char backGroundColor , unsigned char* string , unsigned int length ){
	boxfill8( pSheet->pImage , pSheet->xSize , backGroundColor , xPosition , yPosition , xPosition + length*8 -1, yPosition + 15 );	/*����ּ�*/
	putString( pSheet->pImage , pSheet->xSize , xPosition , yPosition , color , string );	/*����ּ�*/
	Refresh_Sheet( pSheetManager , pSheet , xPosition  , yPosition , xPosition + length*8 , yPosition + 16 );
	return ;
}
