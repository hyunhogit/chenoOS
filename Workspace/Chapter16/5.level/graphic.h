void init_pallette(void){	/*우리가 사용가능한 물감의 개수는 0xFFFFFF개 이중 자주 사용할 255개만 8bit에정해두는것. 물감이 100개있어도, 자주쓰는 10개만 팔레트에 담아 두는것처럼*/
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

void set_palette(int start, int end, unsigned char *rgb)	/* 지정한 색들을 팔레트에 등록. 비디오 DA 컨버터라는 장치에 등록*/
{
	int i, eflags;
	eflags = io_load_eflags();	/* 인터럽트 허가 플래그의 값을 기록한다 */
	io_cli(); 			/* 허가 플래그를 0으로 하여 인터럽트를 금지로 한다 */
	io_out8(0x03c8, start);		/* 0x03c8에 쓰고 싶은 팔레트 번호를 적음,읽고 싶을때는 0x03c7 */
	for (i = start; i <= end; i++) {/* 그 다음 원하는 0x03c9에 순서대로 rgb값을 적어내야한다*/
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
	char s[40];
	boxfill8(p_vram,xsize,COL8_008484,0,0,xsize-1,ysize-15);
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
	sprintf(s,"HO Operating System");	//메모리에 글씨저장
	putString(p_vram,xsize,400,10,COL8_000000,s);//저장한 메모리로 부터 그래픽에 글씨 출력
	
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
	boxfill8( pSheet->pImage , pSheet->xSize , backGroundColor , xPosition , yPosition , xPosition + length*8 -1, yPosition + 15 );	/*상대주소*/
	putString( pSheet->pImage , pSheet->xSize , xPosition , yPosition , color , string );	/*상대주소*/
	Refresh_Sheet( pSheetManager , pSheet , xPosition  , yPosition , xPosition + length*8 , yPosition + 16 );
	return ;
}

void Make_Window(unsigned char* pImage, int xSize, int ySize, char* title , char act ){
	static char pCloseButton[14][16] = {
		"OOOOOOOOOOOOOOO@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQQQ@@QQQQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"O$$$$$$$$$$$$$$@",
		"@@@@@@@@@@@@@@@@"
	};
	int x, y;	/* index */
	char c;
	char tc, tbc;
	if (act != 0) {
		tc = COL8_FFFFFF;
		tbc = COL8_000084;
	} else {
		tc = COL8_C6C6C6;
		tbc = COL8_848484;
	}
	boxfill8(pImage, xSize, COL8_C6C6C6, 0,         0,         xSize - 1, 0        );
	boxfill8(pImage, xSize, COL8_FFFFFF, 1,         1,         xSize - 2, 1        );
	boxfill8(pImage, xSize, COL8_C6C6C6, 0,         0,         0,         ySize - 1);
	boxfill8(pImage, xSize, COL8_FFFFFF, 1,         1,         1,         ySize - 2);
	boxfill8(pImage, xSize, COL8_848484, xSize - 2, 1,         xSize - 2, ySize - 2);
	boxfill8(pImage, xSize, COL8_000000, xSize - 1, 0,         xSize - 1, ySize - 1);
	boxfill8(pImage, xSize, COL8_C6C6C6, 2,         2,         xSize - 3, ySize - 3);
	boxfill8(pImage, xSize, tbc , 3,         3,         xSize - 4, 20       );
	boxfill8(pImage, xSize, COL8_848484, 1,         ySize - 2, xSize - 2, ySize - 2);
	boxfill8(pImage, xSize, COL8_000000, 0,         ySize - 1, xSize - 1, ySize - 1);
	putString(pImage, xSize, 24, 4, tc, title);
	for (y = 0; y < 14; y++) {
		for (x = 0; x < 16; x++) {
			c = pCloseButton[y][x];
			if (c == '@') {
				c = COL8_000000;
			} else if (c == '$') {
				c = COL8_848484;
			} else if (c == 'Q') {
				c = COL8_C6C6C6;
			} else {
				c = COL8_FFFFFF;
			}
			pImage[(5 + y) * xSize + (xSize - 21 + x)] = c;
		}
	}
	return;
}


void Make_TextBox(Sheet* pSheet, int xStart, int yStart, int xEnd, int yEnd, int color){
	boxfill8( pSheet->pImage , pSheet->xSize , COL8_848484 , xStart-2 , yStart-3 , xEnd+1   , yStart-3 );
	boxfill8( pSheet->pImage , pSheet->xSize , COL8_848484 , xStart-3 , yStart-3 , xStart-3 , yEnd+1   );
	boxfill8( pSheet->pImage , pSheet->xSize , COL8_FFFFFF   , xStart-3 , yEnd+2   , xEnd+1   , yEnd+2  );
	boxfill8( pSheet->pImage , pSheet->xSize , COL8_FFFFFF   , xEnd+2   , yStart-3 , xEnd+2   , yEnd+2  );
	boxfill8( pSheet->pImage , pSheet->xSize , COL8_000000 , xStart-1 , yStart-2 , xEnd       , yEnd-2  );
	boxfill8( pSheet->pImage , pSheet->xSize , COL8_000000 , xStart-2 , yStart-2 , xStart-2  , yEnd     );
	boxfill8( pSheet->pImage , pSheet->xSize , COL8_C6C6C6 , xStart-2 , yEnd+1   , xEnd       , yEnd+1  );
	boxfill8( pSheet->pImage , pSheet->xSize , COL8_C6C6C6 , xEnd+1   , yStart-2 , xEnd+1    , yEnd+1  );
	boxfill8( pSheet->pImage , pSheet->xSize , color              , xStart-1  , yStart-1 , xEnd       , yEnd     );
	return;
}

