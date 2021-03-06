;C언어에서 없는 기능을 사용하기 위한 어셈블리 함수정의 공간, 링크시 C와 합쳐진다.
[FORMAT "WCOFF"]				; 오브젝트 파일을 만드는 모드
[INSTRSET "i486p"]				;486명령까지 사용.eax를 레이블이라 생각하는 16bit가 아니라, 레지스터로 받아들이겠다는 의미
[BITS 32]					; 32 비트 모드용의 기계어를 만든다


[FILE "asm_func.asm"]				; 원시 파일명 정보

		GLOBAL 	_io_hlt,_io_cli,_io_sti,_io_stihlt
		GLOBAL	_io_in8, _io_in16, _io_in32
		GLOBAL	_io_out8,_io_out16,_io_out32
		GLOBAL	_io_load_eflags,_io_store_eflags
		GLOBAL  _load_gdtr,_load_idtr
		GLOBAL	_load_cr0, _store_cr0
		GLOBAL	_asm_inthandler20 ,_asm_inthandler21, _asm_inthandler2c
		GLOBAL	_memtest_sub
		EXTERN	_inthandler20 ,_inthandler21, _inthandler2c
		GLOBAL	_load_tr
		GLOBAL	_SwitchGDT3
		GLOBAL	_SwitchGDT4
		GLOBAL  _Jump_Far


; 이하는 실제의 함수

[SECTION .text]				; 오브젝트 파일에서는 이것을 쓰고 나서 프로그램을 쓴다

_io_hlt:		; void io_hlt(void);
			HLT
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

_load_gdtr:		;void load_gdtr(int limit, int address); 
			MOV	AX,[ESP+4]		;limit
			MOV	[ESP+6],AX		;address 앞부분에 limit값을
			LGDT	[ESP+6]			;address값으로 등록
			RET
			

_load_idtr:		;void load_idtr(int limit , int address);
			MOV	AX,[ESP+4]
			MOV	[ESP+6],AX
			LIDT	[ESP+6]
			RET	
		
_load_cr0:		; int load_cr0(void);
		MOV		EAX,CR0
		RET

_store_cr0:		; void store_cr0(int cr0);
		MOV		EAX,[ESP+4]
		MOV		CR0,EAX
		RET

_asm_inthandler20:
			PUSH	ES
			PUSH	DS
			PUSHAD
			MOV		EAX,ESP
			PUSH	EAX
			MOV		AX,SS
			MOV		DS,AX
			MOV		ES,AX
			CALL	_inthandler20
			POP		EAX
			POPAD
			POP		DS
			POP		ES
			IRETD

_asm_inthandler21:
			PUSH	ES
			PUSH	DS
			PUSHAD
			MOV		EAX,ESP
			PUSH	EAX
			MOV		AX,SS
			MOV		DS,AX
			MOV		ES,AX
			CALL	_inthandler21
			POP		EAX
			POPAD
			POP		DS
			POP		ES
			IRETD

_asm_inthandler2c:
			PUSH	ES
			PUSH	DS
			PUSHAD
			MOV		EAX,ESP
			PUSH	EAX
			MOV		AX,SS
			MOV		DS,AX
			MOV		ES,AX
			CALL	_inthandler2c
			POP		EAX
			POPAD
			POP		DS
			POP		ES
			IRETD
_memtest_sub:	; unsigned int memtest_sub(unsigned int start, unsigned int end)
		PUSH	EDI						; (EBX, ESI, EDI 도 사용하고 싶기 때문에)
		PUSH	ESI
		PUSH	EBX
		MOV		ESI, 0xaa55aa55			; pat0 = 0xaa55aa55;
		MOV		EDI, 0x55aa55aa			; pat1 = 0x55aa55aa;
		MOV		EAX,[ESP+12+4]			; i = start;
mts_loop:
		MOV		EBX,EAX
		ADD		EBX, 0xffc				; p = i + 0xffc;
		MOV		EDX,[EBX]				; old = *p;
		MOV		[EBX], ESI				; *p = pat0;
		XOR		DWORD [EBX], 0xffffffff	; *p ^= 0xffffffff;
		CMP		EDI,[EBX]				; if (*p ! = pat1) goto fin;
		JNE		mts_fin
		XOR		DWORD [EBX], 0xffffffff	; *p ^= 0xffffffff;
		CMP		ESI,[EBX]				; if (*p ! = pat0) goto fin;
		JNE		mts_fin
		MOV		[EBX], EDX				; *p = old;
		ADD		EAX, 0x1000				; i += 0x1000;
		CMP		EAX,[ESP+12+8]			; if (i <= end) goto mts_loop;
		JBE		mts_loop
		POP		EBX
		POP		ESI
		POP		EDI
		RET
mts_fin:
		MOV		[EBX], EDX				; *p = old;
		POP		EBX
		POP		ESI
		POP		EDI
		RET

_load_tr:		; void load_tr(int tr);
		LTR		[ESP+4]			; tr
		RET

_SwitchGDT3:	
		JMP		3*8:0			; jump to 3th gdt
		RET
_SwitchGDT4:	
		JMP		4*8:0			; jump to 4th gdt
		RET

_Jump_Far:	; void Jump_Far( int eip , int cs )
		JMP	FAR [ESP+4]
		RET