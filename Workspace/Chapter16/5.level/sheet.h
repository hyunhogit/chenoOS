/* ���콺�� �������� ��ø ó�� */

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
	pSheetManager->maxHeight = -1; /* ��Ʈ�� �� �嵵 ���� */
	for (i = 0; i < MAX_SHEET; i++) {
		pSheetManager->sheets[i].flag = UNUSING_SHEET; /* �̻�� ��ũ */
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
			pSheet->flag = USING_SHEET; /* ����� ��ũ */
			pSheet->height = -1; /* ��ǥ���� */
			return pSheet;
		}
	}
	return 0;	/* ��� ��Ʈ�� ������̾��� */
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
	int oldHeight = pSheet->height; /* ���� ���� ���̸� ����Ѵ� */

	/* ������ �ʹ� ���ų� �ʹ� ������ �����Ѵ� */
	if (height > pSheetManager->maxHeight + 1) { /*-1���� �����ϹǷ�*/
		height =  pSheetManager->maxHeight + 1;
	}
	if (height < -1) {
		height = -1;	/* minimum value is -1 */
	}
	pSheet->height = height; /* ���̸� ���� */

	/* ���ϴ� �ַ� sheets[]�� �þ���� ��ü */
	if (oldHeight > height) {	/* �������� �������� */
		if (height >= 0) {
			/* ������ ���� ����ø��� */
			for (h = oldHeight; h > height; h--) {
				pSheetManager->pSHEETs[h] = pSheetManager->pSHEETs[h - 1];
				pSheetManager->pSHEETs[h]->height = h;
			}
			pSheetManager->pSHEETs[height] = pSheet;
			Write_Map(pSheetManager,pSheet->xPosition,pSheet->yPosition,pSheet->xPosition+pSheet->xSize,pSheet->yPosition+pSheet->ySize,height+1); 
			Draw_Region(pSheetManager,pSheet->xPosition,pSheet->yPosition,pSheet->xPosition+pSheet->xSize,pSheet->yPosition+pSheet->ySize,height+1,oldHeight); /* ���ο� ���̾��� ������ ���� ȭ���� �ٽ� �׸��� */
		} else {	/* ��ǥ��ȭ */
			if ( pSheetManager->maxHeight > oldHeight) {
				/* ���� �Ǿ��ִ� ���� ������ */
				for (h = oldHeight; h <  pSheetManager->maxHeight; h++) {
					pSheetManager->pSHEETs[h] = pSheetManager->pSHEETs[h + 1];
					pSheetManager->pSHEETs[h]->height = h;
				}
			}
			pSheetManager->maxHeight--; /* ǥ������ ���̾ 1�� �پ��Ƿ� �� ���� ���̰� �پ��� */
			Write_Map(pSheetManager,pSheet->xPosition,pSheet->yPosition,pSheet->xPosition+pSheet->xSize,pSheet->yPosition+pSheet->ySize,0); 
			Draw_Region(pSheetManager,pSheet->xPosition,pSheet->yPosition,pSheet->xPosition+pSheet->xSize,pSheet->yPosition+pSheet->ySize,0,oldHeight-1); /* ���ο� ���̾��� ������ ���� ȭ���� �ٽ� �׸��� */
		}
	} else if (oldHeight < height) {	/* �������� �������� */
		if (oldHeight >= 0) {
			/* ������ ���� ���� ������ */
			for (h = oldHeight; h < height; h++) {
				pSheetManager->pSHEETs[h] = pSheetManager->pSHEETs[h + 1];
				pSheetManager->pSHEETs[h]->height = h;
			}
			pSheetManager->pSHEETs[height] = pSheet;
		} else {	/* ��ǥ�� ���¿��� ǥ�� ���·� */
			/* ���� �Ǿ��ִ� ���� ��� �ø��� */
			for (h =pSheetManager->maxHeight; h >= height; h--) {
				pSheetManager->pSHEETs[h + 1] = pSheetManager->pSHEETs[h];
				pSheetManager->pSHEETs[h + 1]->height = h + 1;
			}
			pSheetManager->pSHEETs[height] = pSheet;
			pSheetManager->maxHeight++; /* ǥ������ ���̾ 1�� �����ϹǷ� �� ���� ���̰� �����Ѵ� */
		}
		Write_Map(pSheetManager,pSheet->xPosition,pSheet->yPosition,pSheet->xPosition+pSheet->xSize,pSheet->yPosition+pSheet->ySize,height); 
		Draw_Region(pSheetManager,pSheet->xPosition,pSheet->yPosition,pSheet->xPosition+pSheet->xSize,pSheet->yPosition+pSheet->ySize,height,height); /* ���ο� ���̾��� ������ ���� ȭ���� �ٽ� �׸��� */
	}
	return;
}

void Draw_Region(SheetManager*  pSheetManager, int xStart, int yStart, int xEnd, int yEnd,int startHeight,int endHeight){
	int h;
	int x;
	int y;/*index*/
	int xVideo;/*���� ��ǥ*/
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

		/*yVideo�� ������ y�ǹ����� �ٲٱ� ����*/
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
	int xVideo;/*���� ��ǥ*/
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
	if (pSheet->height >= 0) { /* ���� ǥ�����̶��, ���ο� ���̾��� ������ ���� ȭ���� �ٽ� �׸��� */
		Draw_Region(pSheetManager , pSheet->xPosition+xStart, pSheet->yPosition+yStart , pSheet->xPosition+xEnd , pSheet->yPosition+yEnd , pSheet->height,pSheet->height);
	}
	return;
}

void Move_Sheet(SheetManager* pSheetManager, Sheet* pSheet, int xPosition, int yPosition){
	int oldX=pSheet->xPosition;
	int oldY=pSheet->yPosition;
	pSheet->xPosition = xPosition;
	pSheet->yPosition = yPosition;/*������ ���ο� ��ġ�� �ٽ� �׸���*/	
	if (pSheet->height >= 0) { /* ���� ǥ�����̶�� */
		Write_Map(pSheetManager,oldX,oldY,oldX+pSheet->xSize,oldY+pSheet->ySize,0);
		Write_Map(pSheetManager,xPosition,yPosition,xPosition+pSheet->xSize,yPosition+pSheet->ySize,pSheet->height);
		Draw_Region(pSheetManager,oldX,oldY,oldX+pSheet->xSize,oldY+pSheet->ySize,0,pSheet->height-1);
		Draw_Region(pSheetManager,xPosition,yPosition,xPosition+pSheet->xSize,yPosition+pSheet->ySize,pSheet->height,pSheet->height);
	}
	return;
}

void Free_Sheet(SheetManager* pSheetManager, Sheet* pSheet){
	if (pSheet->height >= 0) {
		UpDown_Sheet(pSheetManager, pSheet, -1); /* ǥ�����̶�� �켱 ��ǥ�÷� �Ѵ� */
	}
	pSheet->flag = 0; /* �̻�� ��ũ */
	return;
}


void Write_Map(SheetManager*  pSheetManager, int xStart, int yStart, int xEnd, int yEnd,int height){
	int h;
	int x;
	int y;/*index*/
	int xVideo;/*���� ��ǥ*/
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

		/*yVideo�� ������ y�ǹ����� �ٲٱ� ����*/
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
