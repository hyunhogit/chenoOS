
void Init_PIT(void){
	int i;
	io_out8(PIT_CTRL,0x34);
	io_out8(PIT_CNT0,0x9c);
	io_out8(PIT_CNT0,0x2e);
	timerManager.count=0;
	timerManager.next=0xffffffff;
	timerManager.usingTimer = 0 ;
	for( i = 0 ; i < MAX_TIMER ; i++ ){
		timerManager.timer[ i ].flag =  TIMER_FLAG_UNUSING ;
	}
	return;
}

Timer* Alloc_Timer(){
	int i=0;
	for ( i =0 ; i< MAX_TIMER ; i++ ){
		if( timerManager.timer[i].flag == 0){
			timerManager.timer[i].flag = TIMER_FLAGS_ALLOC ;
			return &timerManager.timer[i];
		}
	}
	return 0;
}

void Free_Timer( Timer* pTimer){
	pTimer->flag = TIMER_FLAG_UNUSING ;
	return;
}

void Init_Timer( Timer* pTimer , FIFO* pFifo , unsigned char data ){
	pTimer->data = data ; 
	pTimer->pFifo= pFifo ;
	return ;
}

void Set_Timer( Timer* pTimer, unsigned int timeOut ){
	int eflags ;
	int i,j ;
	pTimer->timeOut = timerManager.count + timeOut ;
	pTimer->flag = TIMER_FLAGS_USING;
	eflags= io_load_eflags();
	io_cli();
	/* 새로운 타이머는 몇번째쯤 울리게 되는지 검색 */
	for( i=0 ; i< timerManager.usingTimer ; i++ ){
		if( timerManager.pTimer[i]->timeOut >= pTimer->timeOut ){
			break;
		}
	}
	/* 그 이후의 것들은 뒤로 한칸씩 옮겨놔 */
	for( j = timerManager.usingTimer ; j>i ; j--){
		timerManager.pTimer[j]=timerManager.pTimer[j-1];
	}
	timerManager.usingTimer++ ;
	timerManager.pTimer[i] = pTimer;
	timerManager.next = timerManager.pTimer[0]->timeOut;
	io_store_eflags(eflags);
	return;
} 

