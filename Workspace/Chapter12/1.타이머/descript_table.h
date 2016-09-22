

void init_gdt(void){
	SEGMENT_DESCRIPTOR* gdt=(SEGMENT_DESCRIPTOR *) ADR_GDT;     //임의의 메모리 빈공간
	int i;
	/*초기화*/
	for(i=0;i<LIMIT_GDT/8;i++){
		set_segment_descriptor(gdt+i,0,0,0);
	}
	set_segment_descriptor(gdt+1,0xffffffff,0x00000000,AR_DATA32_RW);
	set_segment_descriptor(gdt+2,0x0007ffff,0x00280000,AR_CODE32_ER);
	load_gdtr(LIMIT_GDT,ADR_GDT);
	return;
}

void set_segment_descriptor(SEGMENT_DESCRIPTOR* pSegmentDescriptor,unsigned int limit,unsigned int base,unsigned int accessRight){
	if(limit>0xffff){
		accessRight=accessRight|0x8000;		/*G bit=1로*/
		limit=limit/0x1000;
	}
	pSegmentDescriptor->limit_low= limit&0xffff; 	
	pSegmentDescriptor->base_low=base&0xffff;
	pSegmentDescriptor->base_mid=(base>>16)&0xff;
	pSegmentDescriptor->access_right=accessRight&0xff;
	pSegmentDescriptor->limit_high=((limit>>16)&0x0f)|((accessRight>>8)&0xf0);
	pSegmentDescriptor->base_high=(base>>24)&0xff;
	return;
}

void init_idt(void){
	GATE_DESCRIPTOR* idt=(GATE_DESCRIPTOR*) ADR_IDT ;
	int i=0;

	for (i=0; i< LIMIT_IDT/8 ;i++){
		set_gate_descriptor(idt+i,0,0,0);
	}
	load_idtr(LIMIT_IDT,ADR_IDT);
	
	/* IDT의 설정 */
	set_gate_descriptor(idt + 0x20, (int) asm_inthandler20, 2 * 8, AR_INTGATE32);
	set_gate_descriptor(idt + 0x21, (int) asm_inthandler21, 2 * 8, AR_INTGATE32);
	set_gate_descriptor(idt + 0x2c, (int) asm_inthandler2c, 2 * 8, AR_INTGATE32);
	
	return;
}

void set_gate_descriptor(GATE_DESCRIPTOR* pGateDescriptor,int offset,int selector,int accessRight){
	pGateDescriptor->offset_low=offset&0xffff;
	pGateDescriptor->selector=selector;
	pGateDescriptor->dw_count=(accessRight>>8)&0xff;
	pGateDescriptor->access_right=accessRight&0xff;
	pGateDescriptor->offset_high=(offset>>16)&0xffff;
	return;
}
