
[FORMAT "WCOFF"]				; ������Ʈ ������ ����� ���
[INSTRSET "i486p"]				;486��ɱ��� ���.eax�� ���̺��̶� �����ϴ� 16bit�� �ƴ϶�, �������ͷ� �޾Ƶ��̰ڴٴ� �ǹ�
[BITS 32]					; 32 ��Ʈ ������ ��� �����


[FILE "asm_func.asm"]				; ���� ���ϸ� ����

		GLOBAL 	_io_hlt,_io_cli,_io_sti,_io_stihlt
		GLOBAL	_io_in8, _io_in16, _io_in32
		GLOBAL	_io_out8,_io_out16,_io_out32
		GLOBAL	_io_load_eflags,_io_store_eflags
		GLOBAL  _load_gdtr,_load_idtr
		GLOBAL	_asm_inthandler21, _asm_inthandler2c
		EXTERN	_inthandler21, _inthandler2c


; ���ϴ� ������ �Լ�

[SECTION .text]				; ������Ʈ ���Ͽ����� �̰��� ���� ���� ���α׷��� ����

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
			MOV	EDX,[ESP+4]		;�Լ�ȣ��� �����ּҰ� ESP�������� ���ʴ�� �����ȴ�
			MOV	EAX,0
			IN	AL,DX			;8bit�� DX�������Ϳ��� AX�������ͷ�
			RET

_io_in16:		;void io_in16(int port);receive 16bit data from io
			MOV	EDX,[ESP+4]		
			MOV	EAX,0
			IN	AX,DX			;16bit��
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
			MOV	[ESP+6],AX		;address�� limit����
			LGDT	[ESP+6]			;address������ ���
			RET
			

_load_idtr:		;void load_idtr(int limit , int address);
			MOV	AX,[ESP+4]
			MOV	[ESP+6],AX
			LIDT	[ESP+6]
			RET	
		


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
