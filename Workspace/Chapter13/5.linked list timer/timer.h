
void Init_PIT(void){	/*타이머 매니저 초기화*/
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

Timer* Alloc_Timer(){		/* 타임아웃과 관계없이 타이머를 등록 */
	int i=0;
	for ( i =0 ; i< MAX_TIMER ; i++ ){
		if( timerManager.timer[i].flag == 0){
			timerManager.timer[i].flag = TIMER_FLAGS_ALLOC ;
			return &timerManager.timer[i];
		}
	}
	return 0;
}

void Free_Timer( Timer* pTimer){	/* 타이머를 사용중이지 않도록 */
	pTimer->flag = TIMER_FLAG_UNUSING ;
	return;
}

void Init_Timer( Timer* pTimer , FIFO* pFifo , unsigned char data ){	/* 타이머의 초기화 */
	pTimer->data = data ; 
	pTimer->pFifo= pFifo ;
	return ;
}

void Set_Timer( Timer* pTimer, unsigned int timeOut ){	/* 새로운 타이머를 추가하며 타임아웃 순으로 정리 */
	int eflags ;
	Timer* pTempTimer ;					
	pTimer->timeOut = timerManager.count + timeOut ;	/*절대시간으로 환산*/
	pTimer->flag = TIMER_FLAGS_USING;			
	eflags= io_load_eflags();
	io_cli();	/*인터럽트 금지*/
	timerManager.usingTimer++ ;		/*새로운 타이머가 늘어났으므로 사용중인 개수 늘림*/

	if( timerManager.usingTimer ==1 ){	/* 등록된 첫번째 타이머인 경우 */
		timerManager.pTimer = pTimer ; /* 첫 타이머로 등록 */
		timerManager.timeOut = pTimer->timeOut;	/* 타임아웃 갱신*/
		timerManager.pTimer->pNextTimer = 0 ; /* 다음 타이머는 없음*/
		
	}else{	/* 처음이 아니라면..*/
		if( timerManager.timeOut >= pTimer->timeOut ){	/* 가장 먼저 울리게 되는 경우 */
			pTempTimer = timerManager.pTimer;	/* 이전 최초타이머를 임시로 저장 */
			timerManager.pTimer = pTimer;		/* 새로운 타이머를 최초타이머로 등록 */
			timerManager.timeOut = timerManager.pTimer->timeOut; 	/* 새로운 타이머의 타임아웃으로 매니저도 갱신*/
			timerManager.pTimer->pNextTimer = pTempTimer;		/* 다음 울릴 타이머를 연결 시켜줌 */
		}else{
			pTempTimer = timerManager.pTimer;		/*첫번째 타이머를 임시로 저장*/
			for(;;){	
				if(pTempTimer->pNextTimer->timeOut > pTimer->timeOut){	/* 다음 타임아웃과 새로운 타임아웃을 비교 */
					pTimer->pNextTimer = pTempTimer->pNextTimer;
					pTempTimer->pNextTimer = pTimer ;
					break;					
				}
				pTempTimer = pTempTimer->pNextTimer;	/* 임시저장타이머를 루프마다 하나씩 다음으로 넘김 */
				if( pTempTimer->pNextTimer == 0 ){	/* 맨 마지막에 오게 되는 경우 */
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

