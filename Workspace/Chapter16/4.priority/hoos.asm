; haribote-os boot asm
; TAB=4

BOTPAK	EQU		0x00280000		; bootpack�� �ε� ���
DSKCAC	EQU		0x00100000		; ��ũ ĳ�� ���α׷��� ���
DSKCAC0	EQU		0x00008000		; ��ũ ĳ�� ���α׷��� ���(������)

; BOOT_INFO ����
CYLS	EQU		0x0ff0			; boot sector�� �����Ѵ�
LEDS	EQU		0x0ff1
VMODE	EQU		0x0ff2			; �� �������� ���� ����.� ��Ʈ Į���ΰ�?
SCRNX	EQU		0x0ff4			; �ػ��� X
SCRNY	EQU		0x0ff6			; �ػ��� Y
VRAM	EQU		0x0ff8			; Video Ram

		ORG		0xc200		; �� ���α׷��� ��� Read�Ǵ°�

; ȭ�� ��带 ����
		
		MOV		BX , 0x4101	;VBE Screen mode, 640 x 480 x 8bit
		MOV		AX , 0x4f02	;Using VBE Mode 
		INT		0x10
		MOV		BYTE [VMODE], 8	; ȭ�� ��带 write�Ѵ�(C�� �����Ѵ�)
		MOV		WORD [SCRNX],640
		MOV		WORD [SCRNY],480
		MOV		DWORD [VRAM], 0xE0000000

; Ű������ LED���¸� BIOS�� �˷��ش�

		MOV		AH,0x02
		INT		0x16 			; keyboard BIOS
		MOV		[LEDS],AL

; PIC�� ���� ���ͷ�Ʈ�� �޾Ƶ����� �ʰ� �Ѵ�
;	ATȣȯ���� ��翡���� PIC�� �ʱ�ȭ�� �Ѵٸ�,
;	�̰͵��� CLI�տ� �� ���� ������ �̵��� ��������.
;	PIC�� �ʱ�ȭ�� ���߿� �Ѵ�

		MOV		AL,0xff
		OUT		0x21,AL
		NOP					; OUT ������ �����ϸ� �� ���� �ʴ� ������ �ִ� �� ���� ������
		OUT		0xa1,AL

		CLI					; CPU���������� ���ͷ�Ʈ ����

; CPU�κ��� 1MB�̻��� �޸𸮿� �׼��� �� �� �ֵ��� A20GATE�� ����
; GDT�� ����Ʈ���������� ����ϴ� ����� �ٷ�ٸ�, A20GATE�� �ϵ���������� �����ϴ� ���� �ǹ��Ѵ�.
; A20����Ʈ�� ����������, Ȧ�������ۿ� ������ ���ϰ� �ȴ�. ��, ���־�߸� �Ѵ�

		CALL	waitkbdout
		MOV		AL,0xd1
		OUT		0x64,AL
		CALL	waitkbdout
		MOV		AL, 0xdf		; enable A20
		OUT		0x60,AL
		CALL	waitkbdout

; ������Ʈ ��� ����

[INSTRSET "i486p"]					; 486���ɱ��� ����ϰ� �ʹٰ� �ϴ� ���

		LGDT	[GDTR0]				; GDTR ������  ���
		MOV		EAX,CR0
		AND		EAX, 0x7fffffff		; bit31�� 0���� �Ѵ�(����¡ ������ ����)
		OR		EAX, 0x00000001		; bit0�� 1���� �Ѵ�(������Ʈ ��� �����̹Ƿ�)
		MOV		CR0,EAX			; 32��Ʈ ��ȣ��� ����ġ ON
		JMP		pipelineflush		; ��ȣ��带 Ű������ �ٷ� ������ �����ϴ� ���� �ִ�.

pipelineflush:						; �������������� ó���߿� �̸� ���� ������ �����Դ� ����
							; ��尡 �ٲ������ �ٽ� ó�� �ؾ� �Ѵ�.
		MOV		AX,1*8			;  �а� ���� ���� ���׸�Ʈ(segment)�� ����Ŵ
		MOV		DS,AX			; ���׸�Ʈ ���������� �ǹ̰� �޶������Ƿ�, ��ü ���׸�Ʈ�� ���� 0���� 8�� �ٲ�. 8�� gdt+1�� ���׸�Ʈ�� ����Ų��.
		MOV		ES,AX
		MOV		FS,AX
		MOV		GS,AX
		MOV		SS,AX

; bootpack�� ����(������ bootpack���̺� �ڿ� c������ �������� �ǹǷ�) �� �κ��� �ű�

		MOV		ESI, bootpack		; ���ۿ�
		MOV		EDI, BOTPAK		; ����ó 0x00280000������
		MOV		ECX,512*1024/4		; ����ũ��(�������� �����̹Ƿ� 4�� ��������� ����Ʈ�� ��)
		CALL	memcpy				; ����!

; �ϴ� �迡 ��ũ �����͵� ������ ��ġ�� ����

; �켱�� boot sector���� 0x7c00���� 1MB���ķ� ����

		MOV		ESI, 0x7c00		; ���ۿ�
		MOV		EDI, DSKCAC		; ����ó 0x00100000 
		MOV		ECX,512/4
		CALL	memcpy

; ������ ���� (��Ʈ���� ���� ����)�� �� �ڷ� �ű�

		MOV		ESI, DSKCAC0+512	; ���ۿ�
		MOV		EDI, DSKCAC+512		; ����ó
		MOV		ECX,0
		MOV		CL,BYTE [CYLS]
		IMUL		ECX,512*18*2/4		; �Ǹ������κ��� ����Ʈ��/4�� ��ȯ
		SUB		ECX,512/4		; IPL�и�ŭ �����Ѵ�
		CALL	memcpy

; asmhead�� ���� ������ �� �Ǵ� ���� ���� �� �����Ƿ�,
;	�׸����� bootpack�� �ñ��

; bootpack�� �⵿

		MOV		EBX,BOTPAK
		MOV		ECX,[EBX+16]
		ADD		ECX, 3			; ECX += 3;
		SHR		ECX, 2			; ECX /= 4;
		JZ		skip			; ���� �ؾ� �� ���� ����
		MOV		ESI,[EBX+20]		; ���ۿ�
		ADD		ESI,EBX
		MOV		EDI,[EBX+12]		; ����ó
		CALL	memcpy
skip:
		MOV		ESP,[EBX+12]		; ���� �ʱ�ġ
		JMP		DWORD 2*8:0x0000001b

waitkbdout:
		IN		 AL,0x64
		AND		 AL,0x02
		IN		 AL, 0x60 		; �� ������ Read(���� ���۰� �������� ���ϰ�)
		JNZ		waitkbdout		; AND�� ����� 0�� �ƴϸ� waitkbdout��
		RET

memcpy:
		MOV		EAX,[ESI]
		ADD		ESI,4
		MOV		[EDI],EAX
		ADD		EDI,4
		SUB		ECX,1
		JNZ		memcpy			; ���� �� ����� 0�� �ƴϸ� memcpy��
		RET
; memcpy�� �ּ� ������ prefix �ִ� ���� ���� ������ string ���ɿ����� �� �� �ִ�

		ALIGNB	16
GDT0:		;gdt
		RESB	8				; null descriptor �ϴ°��� ������ �ʼ�.
		DW		0xffff, 0x0000, 0x9200, 0x00cf	; read/write ���� ���׸�Ʈ(segment) 32bit
		DW		0xffff, 0x0000, 0x9a28, 0x0047	; ���� ���� ���׸�Ʈ(segment) 32 bit(bootpack��)

		DW		0
GDTR0:		;gdt�� ũ��� �����ּ�
		DW		8*3-1
		DD		GDT0

		ALIGNB	16
bootpack: