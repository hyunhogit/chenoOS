; naskfunc
; TAB=4

[FORMAT "WCOFF"]				; 오브젝트 파일을 만드는 모드
[INSTRSET "i486p"]				;486명령까지 사용.eax를 레이블이라 생각하는 16bit가 아니라, 레지스터로 받아들이겠다는 의미
[BITS 32]					; 32 비트 모드용의 기계어를 만든다


; 오브젝트 파일을 위한 정보

[FILE "asm_func.asm"]				; 원시 파일명 정보

		GLOBAL	_io_hlt,_write_mem8	; 이 프로그램에 포함되는 함수명


; 이하는 실제의 함수

[SECTION .text]				; 오브젝트 파일에서는 이것을 쓰고 나서 프로그램을 쓴다

_io_hlt:	; void io_hlt(void);
		HLT
		RET

_write_mem8:	;void write_mem8(int addr,int data); 
		;C언어 함수 호출시 인자를 특정 레지스터에 넣어둠.
		MOV		ECX,[ESP+4]	;[ESP+4]에 addr이 들어있으므로 그것을 ECX에 read
		MOV		AL,[ESP+8]	;[ESP+8]에 data가 들어있으므로 그것을 AL에 read
		MOV		[ECX],AL	;addr번지에 data Write
		RET