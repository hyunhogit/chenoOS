
void Init_PIT(void){	/*Ÿ�̸� �Ŵ��� �ʱ�ȭ*/
	int i;
	io_out8(PIT_CTRL,0x34);
	io_out8(PIT_CNT0,0x9c);
	io_out8(PIT_CNT0,0x2e);
	timerManager.count=0;
	timerManager.timeOut=0xffffffff;
	timerManager.usingTimer = 0 ;
	for( i = 0 ; i < MAX_TIMER ; i++ ){
		timerManager.timer[ i ].flag =  TIMER_FLAG_UNUSING ;
	}
	return;
}

Timer* Alloc_Timer(){		/* Ÿ�Ӿƿ��� ������� Ÿ�̸Ӹ� ��� */
	int i=0;
	for ( i =0 ; i< MAX_TIMER ; i++ ){
		if( timerManager.timer[i].flag == 0){
			timerManager.timer[i].flag = TIMER_FLAGS_ALLOC ;
			return &timerManager.timer[i];
		}
	}
	return 0;
}

void Free_Timer( Timer* pTimer){	/* Ÿ�̸Ӹ� ��������� �ʵ��� */
	pTimer->flag = TIMER_FLAG_UNUSING ;
	return;
}

void Init_Timer( Timer* pTimer , FIFO* pFifo , unsigned char data ){	/* Ÿ�̸��� �ʱ�ȭ */
	pTimer->data = data ; 
	pTimer->pFifo= pFifo ;
	return ;
}

void Set_Timer( Timer* pTimer, unsigned int timeOut ){	/* ���ο� Ÿ�̸Ӹ� �߰��ϸ� Ÿ�Ӿƿ� ������ ���� */
	int eflags ;
	Timer* pTempTimer ;					
	pTimer->timeOut = timerManager.count + timeOut ;	/*����ð����� ȯ��*/
	pTimer->flag = TIMER_FLAGS_USING;			
	eflags= io_load_eflags();
	io_cli();	/*���ͷ�Ʈ ����*/
	timerManager.usingTimer++ ;		/*���ο� Ÿ�̸Ӱ� �þ���Ƿ� ������� ���� �ø�*/

	if( timerManager.usingTimer ==1 ){	/* ��ϵ� ù��° Ÿ�̸��� ��� */
		timerManager.pTimer = pTimer ; /* ù Ÿ�̸ӷ� ��� */
		timerManager.timeOut = pTimer->timeOut;	/* Ÿ�Ӿƿ� ����*/
		timerManager.pTimer->pNextTimer = 0 ; /* ���� Ÿ�̸Ӵ� ����*/
		
	}else{	/* ó���� �ƴ϶��..*/
		if( timerManager.timeOut >= pTimer->timeOut ){	/* ���� ���� �︮�� �Ǵ� ��� */
			pTempTimer = timerManager.pTimer;	/* ���� ����Ÿ�̸Ӹ� �ӽ÷� ���� */
			timerManager.pTimer = pTimer;		/* ���ο� Ÿ�̸Ӹ� ����Ÿ�̸ӷ� ��� */
			timerManager.timeOut = timerManager.pTimer->timeOut; 	/* ���ο� Ÿ�̸��� Ÿ�Ӿƿ����� �Ŵ����� ����*/
			timerManager.pTimer->pNextTimer = pTempTimer;		/* ���� �︱ Ÿ�̸Ӹ� ���� ������ */
		}else{
			pTempTimer = timerManager.pTimer;		/*ù��° Ÿ�̸Ӹ� �ӽ÷� ����*/
			for(;;){	
				if(pTempTimer->pNextTimer->timeOut > pTimer->timeOut){	/* ���� Ÿ�Ӿƿ��� ���ο� Ÿ�Ӿƿ��� �� */
					pTimer->pNextTimer = pTempTimer->pNextTimer;
					pTempTimer->pNextTimer = pTimer ;
					break;					
				}
				pTempTimer = pTempTimer->pNextTimer;	/* �ӽ�����Ÿ�̸Ӹ� �������� �ϳ��� �������� �ѱ� */
				if( pTempTimer->pNextTimer == 0 ){	/* �� �������� ���� �Ǵ� ��� */
					pTempTimer->pNextTimer = pTimer ;
					pTimer->pNextTimer = 0 ;
					break;
				}
			}
		}
	}
	io_store_eflags(eflags);
	return;
} 

