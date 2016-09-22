void Init_MultiTask(){
	pMultiTaskTimer = Alloc_Timer();
	Set_Timer(pMultiTaskTimer , 2);
	taskNumber = 3* 8 ;
	return;
}

void Switch_Task(){
	if ( taskNumber == 3* 8) {
		taskNumber = 4*8 ;
	} else {
		taskNumber = 3*8;
	}	
	Set_Timer(pMultiTaskTimer , 2);
	Jump_Far( 0 , taskNumber );
}

