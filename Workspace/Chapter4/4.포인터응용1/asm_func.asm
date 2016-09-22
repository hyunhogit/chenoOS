; naskfunc
; TAB=4

[FORMAT "WCOFF"]				; ������Ʈ ������ ����� ���
[INSTRSET "i486p"]				;486��ɱ��� ���.eax�� ���̺��̶� �����ϴ� 16bit�� �ƴ϶�, �������ͷ� �޾Ƶ��̰ڴٴ� �ǹ�
[BITS 32]					; 32 ��Ʈ ������ ��� �����


; ������Ʈ ������ ���� ����

[FILE "asm_func.asm"]				; ���� ���ϸ� ����

		GLOBAL	_io_hlt,_write_mem8	; �� ���α׷��� ���ԵǴ� �Լ���


; ���ϴ� ������ �Լ�

[SECTION .text]				; ������Ʈ ���Ͽ����� �̰��� ���� ���� ���α׷��� ����

_io_hlt:	; void io_hlt(void);
		HLT
		RET

_write_mem8:	;void write_mem8(int addr,int data); 
		;C��� �Լ� ȣ��� ���ڸ� Ư�� �������Ϳ� �־��.
		MOV		ECX,[ESP+4]	;[ESP+4]�� addr�� ��������Ƿ� �װ��� ECX�� read
		MOV		AL,[ESP+8]	;[ESP+8]�� data�� ��������Ƿ� �װ��� AL�� read
		MOV		[ECX],AL	;addr������ data Write
		RET