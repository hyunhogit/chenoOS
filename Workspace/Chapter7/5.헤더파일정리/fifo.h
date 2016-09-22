#include "bootpack.h"
void FifoInit( FIFO* fifo , int size , unsigned char* buf ){
	fifo -> length 		= size ;
	fifo -> buf 		= buf;
	fifo -> free 		= size ;
	fifo -> flag		= 0 ;
	fifo -> nextWrite	= 0 ;
	fifo -> nextRead 	= 0 ;	
	return ;
} 

int FifoPut( FIFO* fifo, unsigned char data ){
	if( fifo -> free == 0){ /*가득 차 있다면 */
		fifo -> flag	= OVERRUN ;
		return -1;
	}
	fifo -> buf[ fifo -> nextWrite ] = data ;
	fifo -> nextWrite ++ ;
	if( fifo -> nextWrite == fifo -> length){
		fifo -> nextWrite = 0 ;
	}
	fifo -> free -- ;
	return 0 ;
}

int FifoGet( FIFO* fifo ){
	int data ;
	if( fifo -> free == fifo -> length ){ /*비어있다면 */
		return -1 ;
	}
	data = fifo -> buf[ fifo -> nextRead ] ;
	fifo -> nextRead ++;
	if( fifo -> nextRead == fifo -> length ){ /* 마지막 번호에 도착하면 */
		fifo -> nextRead = 0;	
	}
	fifo -> free ++;
	return data ;
}

int FifoStatus( FIFO* fifo){
	return fifo -> length - fifo -> free ;
}	
