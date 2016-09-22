void io_hlt(void);
void write_mem8(int addr,int data);

void HariMain(void)
{
	int i;				/*32bit 정수형*/
	char* p;
	 
	p=(char*) 0xa0000;
	for(i=0;i<=0xffff;i++){		/*0xa0000~0x0affff:비디오메모리주소*/
		*(p+i)=i & 0x0f;	/*0:Black,7:Gray,15:White*/
	}
	
	for(;;){
		io_hlt();		
	}
}
