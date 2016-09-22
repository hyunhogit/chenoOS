
void Init_PIT(void){
	int i;
	io_out8(PIT_CTRL,0x34);
	io_out8(PIT_CNT0,0x9c);
	io_out8(PIT_CNT0,0x2e);
	timerManager.count=0;
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
	pTimer->timeOut = timeOut;
	pTimer->flag = TIMER_FLAGS_USING;
	return;
} 

