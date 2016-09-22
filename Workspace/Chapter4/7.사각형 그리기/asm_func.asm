; naskfunc
; TAB=4

[FORMAT "WCOFF"]				; 오브젝트 파일을 만드는 모드
[INSTRSET "i486p"]				;486명령까지 사용.eax를 레이블이라 생각하는 16bit가 아니라, 레지스터로 받아들이겠다는 의미
[BITS 32]					; 32 비트 모드용의 기계어를 만든다


; 오브젝트 파일을 위한 정보

[FILE "asm_func.asm"]				; 원시 파일명 정보

		GLOBAL	_write_mem8	; 이 프로그램에 포함되는 함수명
		GLOBAL 	_io_hlt,_io_cli,_io_sti,_io_stihlt
		GLOBAL	_io_in8, _io_in16, _io_in32
		GLOBAL	_io_out8,_io_out16,_io_out32
		GLOBAL	_io_load_eflags,_io_store_eflags

; 이하는 실제의 함수

[SECTION .text]				; 오브젝트 파일에서는 이것을 쓰고 나서 프로그램을 쓴다

_io_hlt:		; void io_hlt(void);
			HLT
			RET

_write_mem8:		;void write_mem8(int addr,int data); 
			;C언어 함수 호출시 인자를 특정 레지스터에 넣어둠.
			MOV	ECX,[ESP+4]	;[ESP+4]에 addr이 들어있으므로 그것을 ECX에 read
			MOV	AL,[ESP+8]	;[ESP+8]에 data가 들어있으므로 그것을 AL에 read
			MOV	[ECX],AL	;addr번지에 data Write
			RET

_io_cli:		;void io_cli(void)		:interrupt close
			CLI
			RET

_io_sti:		;void io_sti(void)		:interrupt open
			STI
			RET

_io_stihlt:		;void io_stihlt(void)		:interrupt open and halt
			STI
			HLT
			RET

_io_in8:		;void io_in8(int port)		:receive 8bit data from io
			MOV	EDX,[ESP+4]		;함수호출시 인자주소가 ESP기준으로 차례대로 결정된다
			MOV	EAX,0
			IN	AL,DX			;8bit만 DX레지스터에서 AX레지스터로
			RET

_io_in16:		;void io_in16(int port);receive 16bit data from io
			MOV	EDX,[ESP+4]		
			MOV	EAX,0
			IN	AX,DX			;16bit만
			RET

_io_in32:		;void io_in32(int port):receive 32bit data from io
			MOV	EDX,[ESP+4]
			IN	EAX,DX
			RET

_io_out8:		;void io_out8(int port,int data):write 8bit data to io
			MOV	EDX,[ESP+4]		;port
			MOV	EAX,[ESP+8]		;data
			OUT	DX,AL
			RET

_io_out16:		;void io_out16(int port,int data):write 16bit data to io
			MOV	EDX,[ESP+4]		;port
			MOV	EAX,[ESP+8]		;data
			OUT	DX,AX
			RET
	
_io_out32:		;void io_out16(int port,int data):write 32bit data to io
			MOV	EDX,[ESP+4]		;port
			MOV	EAX,[ESP+8]		;data
			OUT	DX,EAX
			RET

_io_load_eflags:	;int io_load_eflags(void)	:store flag at AX
			PUSHFD				;push flag double word
			POP	EAX			
			RET

_io_store_eflags:	;void io_store_eflags(int eflags);
			MOV	EAX,[ESP+4]
			PUSH 	EAX
			POPFD				;pop flag double word
			RET
					
		