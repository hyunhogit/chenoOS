/* 인터럽트 관계 */

void init_pic(void)
/* PIC의 초기화 */
{
	io_out8(PIC0_IMR,  0xff  ); /* 모든 인터럽트를 받아들이지 않는다 */
	io_out8(PIC1_IMR,  0xff  ); /* 모든 인터럽트를 받아들이지 않는다 */

	io_out8(PIC0_ICW1, 0x11  ); /* edge trigger 모드 */
	io_out8(PIC0_ICW2, 0x20  ); /* IRQ0-7은 INT20-27으로 받는다 */
	io_out8(PIC0_ICW3, 1 << 2); /* PIC1는 IRQ2에서 접속 */
	io_out8(PIC0_ICW4, 0x01  ); /* non buffer모드 */

	io_out8(PIC1_ICW1, 0x11  ); /* edge trigger 모드 */
	io_out8(PIC1_ICW2, 0x28  ); /* IRQ8-15는 INT28-2f로 받는다 */
	io_out8(PIC1_ICW3, 2     ); /* PIC1는 IRQ2에서 접속 */
	io_out8(PIC1_ICW4, 0x01  ); /* non buffer모드 */

	io_out8(PIC0_IMR,  0xfb  ); /* 11111011 PIC1 이외는 모두 금지 */
	io_out8(PIC1_IMR,  0xff  ); /* 11111111 모든 인터럽트를 받아들이지 않는다 */

	return;
}


void inthandler20(int* esp){
	int i; /* 인덱스용 */
	Timer* pTempTimer;	/* 임시 타이머 */
	io_out8(PIC0_OCW2, 0x60);	/* IRQ-00 접수 완료를 PIC1에 통지 */
	timerManager.count++;
	if( timerManager.timeOut <= timerManager.count ){	/* 타임아웃된 타이머 존재*/
		pTempTimer = timerManager.pTimer ;	/* 첫 타이머 부터 검사*/
		for( i = 0 ; i < timerManager.usingTimer ; i++){
			if(timerManager.count>=pTempTimer->timeOut){	/*타임아웃된 타이머*/
				pTempTimer->flag=TIMER_FLAGS_ALLOC;	/*플래그 갱신*/
				FifoPut(pTempTimer->pFifo , pTempTimer->data );	/* 타임아웃 시그널 버퍼로 */
			}else{
				break;
			}
			pTempTimer = pTempTimer->pNextTimer ;
		}
		timerManager.usingTimer-=i;	/* 빠진 개수만큼 제거해주기 */
		timerManager.pTimer=pTempTimer;
		if(timerManager.usingTimer>0){
			timerManager.timeOut=timerManager.pTimer->timeOut;
		}else{
			timerManager.timeOut=0xffffffff;
		}
	}
	return;
}


void inthandler21(int *esp)
/* PS/2 키보드로부터의 인터럽트 */
{
	unsigned char data;
	io_out8(PIC0_OCW2, 0x61);	/* IRQ-01 접수 완료를 PIC에 통지,다시 감시해줘 */
	data = io_in8(PORT_KEYDAT);	/* 키코드 취득 8bit */
	FifoPut( &keyFifo, data );
	return;
}



void inthandler2c(int *esp)
/* PS/2 마우스로부터의 인터럽트 */
{
	unsigned char data;
	io_out8(PIC1_OCW2, 0x64);	/* IRQ-12 접수 완료를 PIC1에 통지 */
	io_out8(PIC0_OCW2, 0x62);	/* IRQ-02 접수 완료를 PIC0에 통지 */
	data=io_in8(PORT_KEYDAT);
	FifoPut(&mouseFifo,data);
	return;
}

void WaitKBCSendReady(void)
{
	/* 키보드 콘트롤러가 데이터 송신이 가능하게 되는 것을 기다린다 */
	for (;;) {
		if ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) {
			break;
		}
	}
	return;
}


void InitKeyboard(void)
{
	/* 키보드 콘트롤러의 초기화 */
	WaitKBCSendReady();
	io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	WaitKBCSendReady();
	io_out8(PORT_KEYDAT, KBC_MODE);
	return;
}

void EnableMouse(MOUSE_DATA* mouseData)
{
	/* 마우스 유효 */
	WaitKBCSendReady();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	WaitKBCSendReady();
	io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
	return; /* 잘되면 ACK(0xfa)가 송신되어 온다 */
	mouseData->phase = 0 ;
	return;
}

int DecodeMouse(MOUSE_DATA* mouseData , unsigned char data)
{
	if (mouseData->phase == 0) {
		/* 마우스의 0 xfa를 기다리고 있는 단계 */
		if (data == 0xfa) {
			mouseData->phase = 1;
		}
		return 0;
	}
	if (mouseData->phase == 1) {
		/* 마우스의 1바이트째를 기다리고 있는 단계 */
		if((data &  0xc8 ) == 0x08){ /*첫바이트가 만약 잘못 온 경우 대비, 첫바이트는 0~3/8~f범위여야되 */
			mouseData->data[0] = data;
			mouseData->phase = 2;
		}
		return 0;
	}
	if (mouseData->phase == 2) {
		/* 마우스의 2바이트째를 기다리고 있는 단계 */
		mouseData->data[1] = data;
		mouseData->phase = 3;
		return 0;
	}
	if (mouseData->phase == 3) {
		/* 마우스의 3바이트째를 기다리고 있는 단계 */
		mouseData->data[2] = data;
		mouseData->phase = 1;
		mouseData->button = mouseData->data[0] & 0x07; /* 버튼의 상태는 첫바이트의 하위 3비트에 저장되어 있음 */
		mouseData->x = mouseData->data[1];
		mouseData->y = mouseData->data[2];
		if ((mouseData->data[0] & 0x10) != 0) { /* 첫바이트의 5번째 비트가 1이면 */
			mouseData->x |= 0xffffff00;
		}
		if ((mouseData->data[0] & 0x20) != 0) {
			mouseData->y |= 0xffffff00;
		}
		mouseData->y = - mouseData->y; /* 마우스에서는 y방향의 부호가 화면과 반대 */
		return 1;
	}
	return -1; /* 여기에 올 일은 없을 것 */
}

