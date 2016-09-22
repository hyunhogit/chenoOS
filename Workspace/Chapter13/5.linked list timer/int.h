/* ���ͷ�Ʈ ���� */

void init_pic(void)
/* PIC�� �ʱ�ȭ */
{
	io_out8(PIC0_IMR,  0xff  ); /* ��� ���ͷ�Ʈ�� �޾Ƶ����� �ʴ´� */
	io_out8(PIC1_IMR,  0xff  ); /* ��� ���ͷ�Ʈ�� �޾Ƶ����� �ʴ´� */

	io_out8(PIC0_ICW1, 0x11  ); /* edge trigger ��� */
	io_out8(PIC0_ICW2, 0x20  ); /* IRQ0-7�� INT20-27���� �޴´� */
	io_out8(PIC0_ICW3, 1 << 2); /* PIC1�� IRQ2���� ���� */
	io_out8(PIC0_ICW4, 0x01  ); /* non buffer��� */

	io_out8(PIC1_ICW1, 0x11  ); /* edge trigger ��� */
	io_out8(PIC1_ICW2, 0x28  ); /* IRQ8-15�� INT28-2f�� �޴´� */
	io_out8(PIC1_ICW3, 2     ); /* PIC1�� IRQ2���� ���� */
	io_out8(PIC1_ICW4, 0x01  ); /* non buffer��� */

	io_out8(PIC0_IMR,  0xfb  ); /* 11111011 PIC1 �ܴ̿� ��� ���� */
	io_out8(PIC1_IMR,  0xff  ); /* 11111111 ��� ���ͷ�Ʈ�� �޾Ƶ����� �ʴ´� */

	return;
}


void inthandler20(int* esp){
	int i; /* �ε����� */
	Timer* pTempTimer;	/* �ӽ� Ÿ�̸� */
	io_out8(PIC0_OCW2, 0x60);	/* IRQ-00 ���� �ϷḦ PIC1�� ���� */
	timerManager.count++;
	if( timerManager.timeOut <= timerManager.count ){	/* Ÿ�Ӿƿ��� Ÿ�̸� ����*/
		pTempTimer = timerManager.pTimer ;	/* ù Ÿ�̸� ���� �˻�*/
		for( i = 0 ; i < timerManager.usingTimer ; i++){
			if(timerManager.count>=pTempTimer->timeOut){	/*Ÿ�Ӿƿ��� Ÿ�̸�*/
				pTempTimer->flag=TIMER_FLAGS_ALLOC;	/*�÷��� ����*/
				FifoPut(pTempTimer->pFifo , pTempTimer->data );	/* Ÿ�Ӿƿ� �ñ׳� ���۷� */
			}else{
				break;
			}
			pTempTimer = pTempTimer->pNextTimer ;
		}
		timerManager.usingTimer-=i;	/* ���� ������ŭ �������ֱ� */
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
/* PS/2 Ű����κ����� ���ͷ�Ʈ */
{
	unsigned char data;
	io_out8(PIC0_OCW2, 0x61);	/* IRQ-01 ���� �ϷḦ PIC�� ����,�ٽ� �������� */
	data = io_in8(PORT_KEYDAT);	/* Ű�ڵ� ��� 8bit */
	FifoPut( &keyFifo, data );
	return;
}



void inthandler2c(int *esp)
/* PS/2 ���콺�κ����� ���ͷ�Ʈ */
{
	unsigned char data;
	io_out8(PIC1_OCW2, 0x64);	/* IRQ-12 ���� �ϷḦ PIC1�� ���� */
	io_out8(PIC0_OCW2, 0x62);	/* IRQ-02 ���� �ϷḦ PIC0�� ���� */
	data=io_in8(PORT_KEYDAT);
	FifoPut(&mouseFifo,data);
	return;
}

void WaitKBCSendReady(void)
{
	/* Ű���� ��Ʈ�ѷ��� ������ �۽��� �����ϰ� �Ǵ� ���� ��ٸ��� */
	for (;;) {
		if ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) {
			break;
		}
	}
	return;
}


void InitKeyboard(void)
{
	/* Ű���� ��Ʈ�ѷ��� �ʱ�ȭ */
	WaitKBCSendReady();
	io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	WaitKBCSendReady();
	io_out8(PORT_KEYDAT, KBC_MODE);
	return;
}

void EnableMouse(MOUSE_DATA* mouseData)
{
	/* ���콺 ��ȿ */
	WaitKBCSendReady();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	WaitKBCSendReady();
	io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
	return; /* �ߵǸ� ACK(0xfa)�� �۽ŵǾ� �´� */
	mouseData->phase = 0 ;
	return;
}

int DecodeMouse(MOUSE_DATA* mouseData , unsigned char data)
{
	if (mouseData->phase == 0) {
		/* ���콺�� 0 xfa�� ��ٸ��� �ִ� �ܰ� */
		if (data == 0xfa) {
			mouseData->phase = 1;
		}
		return 0;
	}
	if (mouseData->phase == 1) {
		/* ���콺�� 1����Ʈ°�� ��ٸ��� �ִ� �ܰ� */
		if((data &  0xc8 ) == 0x08){ /*ù����Ʈ�� ���� �߸� �� ��� ���, ù����Ʈ�� 0~3/8~f�������ߵ� */
			mouseData->data[0] = data;
			mouseData->phase = 2;
		}
		return 0;
	}
	if (mouseData->phase == 2) {
		/* ���콺�� 2����Ʈ°�� ��ٸ��� �ִ� �ܰ� */
		mouseData->data[1] = data;
		mouseData->phase = 3;
		return 0;
	}
	if (mouseData->phase == 3) {
		/* ���콺�� 3����Ʈ°�� ��ٸ��� �ִ� �ܰ� */
		mouseData->data[2] = data;
		mouseData->phase = 1;
		mouseData->button = mouseData->data[0] & 0x07; /* ��ư�� ���´� ù����Ʈ�� ���� 3��Ʈ�� ����Ǿ� ���� */
		mouseData->x = mouseData->data[1];
		mouseData->y = mouseData->data[2];
		if ((mouseData->data[0] & 0x10) != 0) { /* ù����Ʈ�� 5��° ��Ʈ�� 1�̸� */
			mouseData->x |= 0xffffff00;
		}
		if ((mouseData->data[0] & 0x20) != 0) {
			mouseData->y |= 0xffffff00;
		}
		mouseData->y = - mouseData->y; /* ���콺������ y������ ��ȣ�� ȭ��� �ݴ� */
		return 1;
	}
	return -1; /* ���⿡ �� ���� ���� �� */
}

