/* 마우스나 윈도우의 중첩 처리 */

SheetManager* Init_SheetManager(MemoryManager* pMemoryManager, unsigned char* pVideo, int xSize, int ySize){
	SheetManager* pSheetManager;
	int i;
	pSheetManager = (SheetManager*) Alloc_MemoryBlock(pMemoryManager, sizeof(SheetManager));
	if (pSheetManager == 0) {
		goto err;
	}
	pSheetManager->map=(unsigned char*)Alloc_MemoryBlock(pMemoryManager,xSize*ySize);
	if(pSheetManager->map==0){	
		Free_MemoryBlock(pMemoryManager,(int)pSheetManager,sizeof(SheetManager));
		goto err;
	}
	pSheetManager->pVideo = pVideo;
	pSheetManager->xSize = xSize;
	pSheetManager->ySize = ySize;
	pSheetManager->maxHeight = -1; /* 시트는 한 장도 없다 */
	for (i = 0; i < MAX_SHEET; i++) {
		pSheetManager->sheets[i].flag = UNUSING_SHEET; /* 미사용 마크 */
	}
err:
	return pSheetManager;
}

Sheet* Alloc_Sheet(SheetManager* pSheetManager){
	Sheet* pSheet;
	int i;
	for (i = 0; i < MAX_SHEET ; i++) {
		if ( pSheetManager->sheets[i].flag == 0) {
			pSheet = &(pSheetManager->sheets[i]);
			pSheet->flag = USING_SHEET; /* 사용중 마크 */
			pSheet->height = -1; /* 비표시중 */
			return pSheet;
		}
	}
	return 0;	/* 모든 시트가 사용중이었다 */
}


void Set_pImageOfSheet(Sheet* pSheet, unsigned char* pImage, int xSize, int ySize, int col_inv){
	pSheet->pImage = pImage;
	pSheet->xSize = xSize;
	pSheet->ySize = ySize;
	pSheet->col_inv = col_inv;
	return;
}

void UpDown_Sheet(SheetManager* pSheetManager, Sheet* pSheet, int height){
	int h;
	int oldHeight = pSheet->height; /* 설정 전의 높이를 기억한다 */

	/* 지정이 너무 낮거나 너무 높으면 수정한다 */
	if (height > pSheetManager->maxHeight + 1) { /*-1부터 시작하므로*/
		height =  pSheetManager->maxHeight + 1;
	}
	if (height < -1) {
		height = -1;	/* minimum value is -1 */
	}
	pSheet->height = height; /* 높이를 설정 */

	/* 이하는 주로 sheets[]가 늘어놓고 대체 */
	if (oldHeight > height) {	/* 이전보다 낮아진다 */
		if (height >= 0) {
			/* 사이의 것을 끌어올린다 */
			for (h = oldHeight; h > height; h--) {
				pSheetManager->pSHEETs[h] = pSheetManager->pSHEETs[h - 1];
				pSheetManager->pSHEETs[h]->height = h;
			}
			pSheetManager->pSHEETs[height] = pSheet;
			Write_Map(pSheetManager,pSheet->xPosition,pSheet->yPosition,pSheet->xPosition+pSheet->xSize,pSheet->yPosition+pSheet->ySize,height+1); 
			Draw_Region(pSheetManager,pSheet->xPosition,pSheet->yPosition,pSheet->xPosition+pSheet->xSize,pSheet->yPosition+pSheet->ySize,height+1,oldHeight); /* 새로운 레이어의 정보에 따라 화면을 다시 그린다 */
		} else {	/* 비표시화 */
			if ( pSheetManager->maxHeight > oldHeight) {
				/* 위로 되어있는 것을 내린다 */
				for (h = oldHeight; h <  pSheetManager->maxHeight; h++) {
					pSheetManager->pSHEETs[h] = pSheetManager->pSHEETs[h + 1];
					pSheetManager->pSHEETs[h]->height = h;
				}
			}
			pSheetManager->maxHeight--; /* 표시중의 레이어가 1개 줄어들므로 맨 위의 높이가 줄어든다 */
			Write_Map(pSheetManager,pSheet->xPosition,pSheet->yPosition,pSheet->xPosition+pSheet->xSize,pSheet->yPosition+pSheet->ySize,0); 
			Draw_Region(pSheetManager,pSheet->xPosition,pSheet->yPosition,pSheet->xPosition+pSheet->xSize,pSheet->yPosition+pSheet->ySize,0,oldHeight-1); /* 새로운 레이어의 정보에 따라 화면을 다시 그린다 */
		}
	} else if (oldHeight < height) {	/* 이전보다 높아진다 */
		if (oldHeight >= 0) {
			/* 사이의 것을 눌러 내린다 */
			for (h = oldHeight; h < height; h++) {
				pSheetManager->pSHEETs[h] = pSheetManager->pSHEETs[h + 1];
				pSheetManager->pSHEETs[h]->height = h;
			}
			pSheetManager->pSHEETs[height] = pSheet;
		} else {	/* 비표시 상태에서 표시 상태로 */
			/* 위로 되어있는 것을 들어 올린다 */
			for (h =pSheetManager->maxHeight; h >= height; h--) {
				pSheetManager->pSHEETs[h + 1] = pSheetManager->pSHEETs[h];
				pSheetManager->pSHEETs[h + 1]->height = h + 1;
			}
			pSheetManager->pSHEETs[height] = pSheet;
			pSheetManager->maxHeight++; /* 표시중의 레이어가 1개 증가하므로 맨 위의 높이가 증가한다 */
		}
		Write_Map(pSheetManager,pSheet->xPosition,pSheet->yPosition,pSheet->xPosition+pSheet->xSize,pSheet->yPosition+pSheet->ySize,height); 
		Draw_Region(pSheetManager,pSheet->xPosition,pSheet->yPosition,pSheet->xPosition+pSheet->xSize,pSheet->yPosition+pSheet->ySize,height,height); /* 새로운 레이어의 정보에 따라 화면을 다시 그린다 */
	}
	return;
}

void Draw_Region(SheetManager*  pSheetManager, int xStart, int yStart, int xEnd, int yEnd,int startHeight,int endHeight){
	int h;
	int x;
	int y;/*index*/
	int xVideo;/*비디오 좌표*/
	int yVideo;
	int regionStartX;
	int regionStartY;
	int regionEndX;
	int regionEndY;
	unsigned char* pImage;
	unsigned char* pVideo=pSheetManager->pVideo;
	Sheet* sheet;
	unsigned char sheetID;
	unsigned char* map=pSheetManager->map;
	
	if(xStart<0){xStart=0;}
	if(yStart<0){yStart=0;}
	if(xEnd>pSheetManager->xSize){xEnd=pSheetManager->xSize;}
	if(yEnd>pSheetManager->ySize){yEnd=pSheetManager->ySize;}	

	for(h=startHeight;h<=endHeight;h++){
		sheet=pSheetManager->pSHEETs[h];
		sheetID=sheet-(pSheetManager->sheets);
		pImage=sheet->pImage;

		/*yVideo의 범위를 y의범위로 바꾸기 위함*/
		regionStartX=xStart-sheet->xPosition;
		regionStartY=yStart-sheet->yPosition;
		regionEndX=xEnd-sheet->xPosition;
		regionEndY=yEnd-sheet->yPosition;
		if(regionStartX<0){regionStartX=0;}
		if(regionStartY<0){regionStartY=0;}
		if(regionEndX>sheet->xSize){regionEndX=sheet->xSize;}
		if(regionEndY>sheet->ySize){regionEndY=sheet->ySize;}
		for(y=regionStartY;y<regionEndY;y++){
			yVideo=sheet->yPosition+y;
			for(x=regionStartX;x<regionEndX;x++){
				xVideo=sheet->xPosition+x;	
				if(map[yVideo*(pSheetManager->xSize)+xVideo]==sheetID){
					pVideo[yVideo*(pSheetManager->xSize)+xVideo]=pImage[y*(sheet->xSize)+x];
				}
			}
		}
	}
	return;	
}

void Draw_AllSheet(SheetManager*  pSheetManager){
	int h;
	int x;
	int y;/*index*/
	int xVideo;/*비디오 좌표*/
	int yVideo;
	unsigned char* pImage;
	unsigned char c;
	unsigned char* pVideo=pSheetManager->pVideo;
	Sheet* sheet;
	
	for(h=0;h<=pSheetManager->maxHeight;h++){
		sheet=pSheetManager->pSHEETs[h];
		pImage=sheet->pImage;
		for(y=0;y<sheet->ySize;y++){
			yVideo=sheet->yPosition+y;
			for(x=0;x<sheet->xSize;x++){
				xVideo=sheet->xPosition+x;
				c=pImage[y*(sheet->xSize)+x];
				if(c!=sheet->col_inv){		/* visible */
					pVideo[yVideo*(pSheetManager->xSize)+xVideo]=c;
				}
			}
		}
	}
	return;
}

void Refresh_Sheet(SheetManager* pSheetManager,Sheet* pSheet,int xStart, int yStart, int xEnd, int yEnd){
	if (pSheet->height >= 0) { /* 만약 표시중이라면, 새로운 레이어의 정보에 따라 화면을 다시 그린다 */
		Draw_Region(pSheetManager , pSheet->xPosition+xStart, pSheet->yPosition+yStart , pSheet->xPosition+xEnd , pSheet->yPosition+yEnd , pSheet->height,pSheet->height);
	}
	return;
}

void Move_Sheet(SheetManager* pSheetManager, Sheet* pSheet, int xPosition, int yPosition){
	int oldX=pSheet->xPosition;
	int oldY=pSheet->yPosition;
	pSheet->xPosition = xPosition;
	pSheet->yPosition = yPosition;/*변경후 새로운 위치에 다시 그리기*/	
	if (pSheet->height >= 0) { /* 만약 표시중이라면 */
		Write_Map(pSheetManager,oldX,oldY,oldX+pSheet->xSize,oldY+pSheet->ySize,0);
		Write_Map(pSheetManager,xPosition,yPosition,xPosition+pSheet->xSize,yPosition+pSheet->ySize,pSheet->height);
		Draw_Region(pSheetManager,oldX,oldY,oldX+pSheet->xSize,oldY+pSheet->ySize,0,pSheet->height-1);
		Draw_Region(pSheetManager,xPosition,yPosition,xPosition+pSheet->xSize,yPosition+pSheet->ySize,pSheet->height,pSheet->height);
	}
	return;
}

void Free_Sheet(SheetManager* pSheetManager, Sheet* pSheet){
	if (pSheet->height >= 0) {
		UpDown_Sheet(pSheetManager, pSheet, -1); /* 표시중이라면 우선 비표시로 한다 */
	}
	pSheet->flag = 0; /* 미사용 마크 */
	return;
}


void Write_Map(SheetManager*  pSheetManager, int xStart, int yStart, int xEnd, int yEnd,int height){
	int h;
	int x;
	int y;/*index*/
	int xVideo;/*비디오 좌표*/
	int yVideo;
	int regionStartX;
	int regionStartY;
	int regionEndX;
	int regionEndY;
	unsigned char sheetID;
	unsigned char* pImage;
	unsigned char* map=pSheetManager->map;
	Sheet* sheet;
	
	if(xStart<0)  {xStart=0;}
	if(yStart<0)  {yStart=0;}
	if(xEnd>pSheetManager->xSize)  {xEnd=pSheetManager->xSize;}
	if(yEnd>pSheetManager->ySize)  {yEnd=pSheetManager->ySize;}	

	for(h=height;h<=pSheetManager->maxHeight;h++){
		sheet=pSheetManager->pSHEETs[h];
		sheetID=sheet-(pSheetManager->sheets);
		pImage=sheet->pImage;

		/*yVideo의 범위를 y의범위로 바꾸기 위함*/
		regionStartX=xStart-sheet->xPosition;
		regionStartY=yStart-sheet->yPosition;
		regionEndX=xEnd-sheet->xPosition;
		regionEndY=yEnd-sheet->yPosition;
		if(regionStartX<0){regionStartX=0;}
		if(regionStartY<0){regionStartY=0;}
		if(regionEndX>sheet->xSize){regionEndX=sheet->xSize;}
		if(regionEndY>sheet->ySize){regionEndY=sheet->ySize;}
		for(y=regionStartY;y<regionEndY;y++){
			yVideo=sheet->yPosition+y;
			for(x=regionStartX;x<regionEndX;x++){
				xVideo=sheet->xPosition+x;	
				if(pImage[y*(sheet->xSize)+x] != sheet->col_inv){
					map[yVideo*(pSheetManager->xSize)+xVideo] = sheetID;
				}
			}
		}
	}
	return;	
	
}
