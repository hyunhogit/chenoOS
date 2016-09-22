Task* Init_TaskManager( MemoryManager* pMemoryManager ){
	int i ; 		/* for index */
	Task* pTask ;
	
	pTaskManager = (TaskManager*)Alloc_MemoryBlock( pMemoryManager , sizeof(TaskManager) )  ;
	
	pTaskManager->numRunning = 1 ;	
	pTaskManager->taskRunning = 0 ;	/* This is the first Task */
	for( i=0 ; i < MAX_TASKS ; i++ ){
		pTaskManager->tasks[ i ].selector = (TASK_POSITION + i ) * 8 ;
		pTaskManager->tasks[i].flag = 0 ;
		set_segment_descriptor( gdt + (TASK_POSITION + i ) , 103 , (int)&(pTaskManager->tasks[i].taskStatus) , AR_TSS32 );
	}
	pTask = Alloc_Task();		/* This is also one of task. So one task is allocating automatically */
	pTask->flag = 2 ; 
	pTaskManager->pTasks[0] = pTask ;	/* This is the first Task */
	
	load_tr( pTask->selector ) ;
	pSwitchTimer = Alloc_Timer() ;
	Set_Timer( pSwitchTimer , 2) ;
	return pTask;
}

Task* Alloc_Task(){
	int i ; 	/* index */
	Task* pTask ;
	for( i = 0 ; i < MAX_TASKS ; i++ ){	/* Finding the space */
		if( pTaskManager-> tasks[i].flag == 0 ){
			pTask =  &( pTaskManager-> tasks[i]) ;
			pTask->flag = 1 ;
			pTask->taskStatus.eflags = 0x00000202; /* IF = 1; */
			pTask->taskStatus.eax = 0; /* 우선 0으로 해 두기로 한다 */
			pTask->taskStatus.ecx = 0;
			pTask->taskStatus.edx = 0;
			pTask->taskStatus.ebx = 0;
			pTask->taskStatus.ebp = 0;
			pTask->taskStatus.esi = 0;
			pTask->taskStatus.edi = 0;
			pTask->taskStatus.es = 0;
			pTask->taskStatus.ds = 0;
			pTask->taskStatus.fs = 0;
			pTask->taskStatus.gs = 0;
			pTask->taskStatus.ldtr = 0;
			pTask->taskStatus.iomap = 0x40000000;
			return pTask;
		}
	}
	return 0 ; /* No space, already full! */
}

void Run_Task( Task* pTask ){
	pTask->flag = 2 ; 	
	pTaskManager->pTasks[ pTaskManager->numRunning ] = pTask;
	(pTaskManager->numRunning)++ ;
	return;
}

void Switch_Task(){
	Set_Timer( pSwitchTimer , 2) ;
	if( pTaskManager->numRunning >= 2 ){	/* switch needs minimum 2 tasks */
		pTaskManager->taskRunning ++ ;
		if( pTaskManager->taskRunning == pTaskManager->numRunning ){
			pTaskManager->taskRunning = 0;
		}
		Jump_Far( 0 , pTaskManager->pTasks[ pTaskManager->taskRunning ]->selector );
	}	
	return ;
}

void Sleep_Task( Task* pTask ){
	int i ; /* Index */
	char taskSwitch = 0 ;	
	if( pTask->flag == 2 ) {	/* Now running */
		if( pTask == pTaskManager->pTasks[ pTaskManager->taskRunning] ){ /* Sleeping by self */
			taskSwitch =1 ;	/* Switch task at the end of this function */
		}
		/* Finding index of task we want to sleep */
		for( i=0 ; i < pTaskManager->numRunning ; i++ ){	
			if ( pTaskManager->pTasks[i] == pTask ){
				break;
			}		
		}
		
		/* Deleting this task from task list*/
		pTaskManager->numRunning--;	/* Reduce total number of task running*/
		if( i < pTaskManager->taskRunning){		/* if task that is Running is back of the deleted task */
			pTaskManager->taskRunning-- ;	/* set taskNumber that is Running */
		}
		/* Delete and Pull other task that is in the back of the task */
		for( ; i < pTaskManager->numRunning ; i++ ){
			pTaskManager->pTasks[i]= pTaskManager->pTasks[ i+1 ] ;
		}
		/* Delete last task */
		//pTaskManager->pTasks[ pTaskManager->numRunning] = 0 ;
		
		pTask->flag = 1 ;			/* Not running, just Allocated */
		
		if( taskSwitch == 1){		/* Switching Task */
			if( pTaskManager->taskRunning >= pTaskManager->numRunning ){
				pTaskManager->taskRunning = 0 ;	
			}
			Jump_Far( 0 ,  pTaskManager->pTasks[ pTaskManager->taskRunning ]->selector );
		}
	}
	return;
}
