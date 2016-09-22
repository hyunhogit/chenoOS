Task* Init_TaskManager( MemoryManager* pMemoryManager ){
	int i ; 		/* for index */
	Task* pTask ;
	
	pTaskManager = (TaskManager*)Alloc_MemoryBlock( pMemoryManager , sizeof(TaskManager) )  ;
	pTaskManager->levelSwitch = 0;
	pTaskManager->levelRunning = 0;
	for( i=0 ; i < MAX_TASKS ; i++ ){
		pTaskManager->tasks[i].selector = (TASK_POSITION + i ) * 8 ;
		pTaskManager->tasks[i].flag = 0 ;
		set_segment_descriptor( gdt + (TASK_POSITION + i ) , 103 , (int)&(pTaskManager->tasks[i].taskStatus) , AR_TSS32 );
	}
	for( i=0 ; i < NUM_LEVEL ; i++ ){
		pTaskManager->priorityLevel[i].numRunning = 0 ;
		pTaskManager->priorityLevel[i].taskRunning = 0;
	}
	pTask = Alloc_Task();		/* This is also one of task. So one task is allocating automatically */
	pTask->flag = 2 ; 
	pTask->priority = 10;
	pTask->level = 0 ;
	pTaskManager->priorityLevel[0].numRunning = 1 ;	
	pTaskManager->priorityLevel[0].taskRunning = 0 ;	/* This is the first Task */
	pTaskManager->priorityLevel[0].pTasks[0] = pTask ;	/* This is the first Task */
	
	load_tr( pTask->selector ) ;
	pSwitchTimer = Alloc_Timer() ;
	Set_Timer( pSwitchTimer , pTask->priority) ;

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
			pTask->taskStatus.eax = 0;	 /* 우선 0으로 해 두기로 한다 */
			pTask->taskStatus.ecx = 0;
			pTask->taskStatus.edx = 0;
			pTask->taskStatus.ebx = 0;
			pTask->taskStatus.ebp = 0;
			pTask->taskStatus.esi  = 0;
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

void Run_Task( Task* pTask , int level , int priority ){
	if( priority > 0){	/* Error check  for priority */
		pTask->priority = priority ;
	}
	if(level < 0 ){	/* Error check  for level */
		level = pTaskManager->levelRunning;
	}
	if( pTask->flag != 2 ){
		pTask->flag = 2 ;  	/* Mark for running */
		pTaskManager->priorityLevel[level].pTasks[ pTaskManager->priorityLevel[level].numRunning ] = pTask;
		(pTaskManager->priorityLevel[level].numRunning)++ ;
	}
	if( level < pTaskManager->levelRunning ){
		pTaskManager->levelRunning = level ;
		pTaskManager->levelSwitch = 1 ;
	}
	return;
}	
	
void Switch_Task(){
	if(pTaskManager->levelSwitch == 0 ){	/* switch needs minimum 2 tasks */
		if( pTaskManager->priorityLevel[pTaskManager->levelRunning].numRunning>=2){
			pTaskManager->priorityLevel[pTaskManager->levelRunning].taskRunning ++ ;
			if( pTaskManager->priorityLevel[pTaskManager->levelRunning].taskRunning == pTaskManager->priorityLevel[pTaskManager->levelRunning].numRunning ){
				pTaskManager->priorityLevel[pTaskManager->levelRunning].taskRunning = 0;
			}
			Set_Timer( pSwitchTimer , pTaskManager->priorityLevel[pTaskManager->levelRunning].pTasks[ pTaskManager->priorityLevel[pTaskManager->levelRunning].taskRunning ]->priority );
			Jump_Far( 0 , pTaskManager->priorityLevel[pTaskManager->levelRunning].pTasks[ pTaskManager->priorityLevel[pTaskManager->levelRunning].taskRunning ]->selector );
		}else{
			Set_Timer( pSwitchTimer , pTaskManager->priorityLevel[pTaskManager->levelRunning].pTasks[ pTaskManager->priorityLevel[pTaskManager->levelRunning].taskRunning ]->priority );
		}
	}else if(  pTaskManager->levelSwitch ==1 ){
		pTaskManager->levelSwitch = 0;
		pTaskManager->priorityLevel[pTaskManager->levelRunning].taskRunning = 0;
		Set_Timer( pSwitchTimer , pTaskManager->priorityLevel[pTaskManager->levelRunning].pTasks[ pTaskManager->priorityLevel[pTaskManager->levelRunning].taskRunning ]->priority );
		Jump_Far( 0 , pTaskManager->priorityLevel[pTaskManager->levelRunning].pTasks[ pTaskManager->priorityLevel[pTaskManager->levelRunning].taskRunning ]->selector );
	}
	return ;
}	
	
void Sleep_Task( Task* pTask ){	
	int i ; /* Index */
	char taskSwitch = 0 ;
	if( pTask->flag == 2 ) {	/* Now running */
		if( pTask == pTaskManager->priorityLevel[pTaskManager->levelRunning].pTasks[ pTaskManager->priorityLevel[pTaskManager->levelRunning].taskRunning] ){ /* Sleeping by self */
			taskSwitch =1 ;	/* Switch task at the end of this function */
		}
		/* Finding index of task we want to sleep */
		for( i=0 ; i < pTaskManager->priorityLevel[pTaskManager->levelRunning].numRunning ; i++ ){	
			if ( pTaskManager->priorityLevel[pTaskManager->levelRunning].pTasks[i] == pTask ){
				break;
			}		
		}
		
		/* Deleting this task from task list*/
		pTaskManager->priorityLevel[pTaskManager->levelRunning].numRunning--;	/* Reduce total number of task running*/
		
		if( i < pTaskManager->priorityLevel[pTaskManager->levelRunning].taskRunning){		/* if task that is Running is back of the deleted task */
			pTaskManager->priorityLevel[pTaskManager->levelRunning].taskRunning-- ;	/* set taskNumber that is Running */
		}
		/* Delete and Pull other task that is in the back of the task */
		for( ; i < pTaskManager->priorityLevel[pTaskManager->levelRunning].numRunning ; i++ ){
			pTaskManager->priorityLevel[pTaskManager->levelRunning].pTasks[i]= pTaskManager->priorityLevel[pTaskManager->levelRunning].pTasks[ i+1 ] ;
		}
		pTask->flag = 1 ;			/* Not running, just Allocated */
		
		if( taskSwitch != 0){		/* Switching Task */
			if( pTaskManager->priorityLevel[pTaskManager->levelRunning].taskRunning >= pTaskManager->priorityLevel[pTaskManager->levelRunning].numRunning ){
				pTaskManager->priorityLevel[ pTaskManager->levelRunning ].taskRunning = 0 ;	
			}
			if( pTaskManager->priorityLevel[pTaskManager->levelRunning].numRunning == 0 ){	/* There is no task in this level */
				for( ; pTaskManager->levelRunning < NUM_LEVEL ; ( pTaskManager->levelRunning )++){
					if( pTaskManager->priorityLevel[ pTaskManager->levelRunning ].numRunning>0){
						break;
					}
				}			
				pTaskManager->levelSwitch = 1;	
			}
		}
	}
	return;
}
