		ORG		0xc200
		
		MOV 		al,0x13			;���(0x13:320x200x8bit �÷�,0x03:16���ؽ�Ʈ 80x25)
		MOV		ah,0x00			;����BIOS���� ah�� 00����
		INT		0x10			;10���ͷ�Ʈ��ƾ=����BIOS

fin:
		HLT
		JMP		fin
