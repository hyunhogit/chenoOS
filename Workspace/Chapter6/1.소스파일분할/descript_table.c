typedef struct SEGMENT_DESCRIPTOR{
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
}SEGMENT_DESCRIPTOR;

typedef struct GATE_DESCRIPTOR{
	short offset_low, selector;
	char dw_count, access_right;
	short offset_high;
}GATE_DESCRIPTOR;

void init_gdt(void);
void set_segment_descriptor(SEGMENT_DESCRIPTOR* pSegDes,unsigned int limit,unsigned int base,unsigned int ar);
void load_gdtr(int limit,int address);
void init_idt(void);
void SetGateDescriptor(GATE_DESCRIPTOR* pGateDescriptor,int offset,int selector,int accessRight);
void load_idtr(int limit , int address);

void init_gdt(void){
	SEGMENT_DESCRIPTOR* gdt=(SEGMENT_DESCRIPTOR *) 0x00270000;     //임의의 메모리 빈공간
	int i;
	/*초기화*/
	for(i=0;i<8192;i++){
		set_segment_descriptor(gdt+i,0,0,0);
	}
	set_segment_descriptor(gdt+1,0xffffffff,0x00000000,0x4092);
	set_segment_descriptor(gdt+2,0x0007ffff,0x00280000,0x409a);
	load_gdtr(0xffff,0x00270000);
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
	GATE_DESCRIPTOR* idt=(GATE_DESCRIPTOR*)0x0026f800;
	int i=0;

	for (i=0;i<256;i++){
		SetGateDescriptor(idt+i,0,0,0);
	}
	load_idtr(0x7ff,0x0026f800);
	return;
}

void SetGateDescriptor(GATE_DESCRIPTOR* pGateDescriptor,int offset,int selector,int accessRight){
	pGateDescriptor->offset_low=offset&0xffff;
	pGateDescriptor->selector=selector;
	pGateDescriptor->dw_count=(accessRight>>8)&0xff;
	pGateDescriptor->access_right=accessRight&0xff;
	pGateDescriptor->offset_high=(offset>>16)&0xffff;
	return;
}
