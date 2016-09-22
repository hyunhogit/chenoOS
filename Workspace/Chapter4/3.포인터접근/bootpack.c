void io_hlt(void);
void write_mem8(int addr,int data);

void HariMain(void)
{
	int i;				/*32bit 정수형*/
	char* p;	
	
	for(i=0xa0000;i<=0xaffff;i++){	/*0xa0000~0x0affff:비디오메모리주소*/
		p=(char*)i;
		*p=i&0x0f;	
	}
	
	for(;;){
		io_hlt();		/*0:Black,7:Gray,15:White*/
	}
}
