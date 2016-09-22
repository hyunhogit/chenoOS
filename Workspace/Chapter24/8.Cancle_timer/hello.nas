[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "hello.nas"]

[SECTION .text]
_HariMain:
		MOV		ECX,msg
		MOV		EDX,1
putloop:
		MOV		AL,[CS:ECX]
		CMP		AL,0
		JE		fin
		INT		0x40
		ADD		ECX,1
		JMP		putloop
fin:
		MOV		EDX,4
		INT		0x40
		
[SECTION .data]

msg:
		DB	"hello",0
