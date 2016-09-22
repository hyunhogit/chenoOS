#define ADR_IDT			0x0026f800
#define LIMIT_IDT		0x000007ff
#define ADR_GDT			0x00270000
#define LIMIT_GDT		0x0000ffff
#define ADR_BOTPAK		0x00280000
#define LIMIT_BOTPAK	0x0007ffff
#define AR_DATA32_RW	0x4092
#define AR_CODE32_ER	0x409a
#define AR_INTGATE32	0x008e

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
void set_gate_descriptor(GATE_DESCRIPTOR* pGateDescriptor,int offset,int selector,int accessRight);
void load_idtr(int limit , int address);
void asm_inthandler21(void);
void asm_inthandler2c(void);

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
