;haribote-os
;TAB=4
CYLS		EQU		10
		ORG		0x7c00			;�޸� �� ��� �ε� �� ���ΰ�.�ݵ�� �� ��ġ���߸� �Ѵ�.�޸� �ƹ����� ����ϸ� �ȵȴ�. ���������� ���� �ִ� �κ��� �ִ°�.��Ʈ���ʹ� 0x7c00~0x7dff

;FAT12���� �÷��ǵ�ũ�� ���� ����
		JMP		entry
		DB		0x90
		DB		"HARIBOTE"		; boot sector�̸��� �����Ӱ� �ᵵ ����(8����Ʈ)
		DW		512			; 1���� ũ��(512�� �ؾ� ��)
		DB		1			; Ŭ������ ũ��(1���ͷ� �ؾ� ��)
		DW		1			; FAT�� ��𿡼� ���۵ɱ�(���� 1����°����)
		DB		2			; FAT ����(2�� �ؾ� ��)
		DW		224			; ��Ʈ ���丮 ������ ũ��(���� 224��Ʈ���� �ؾ� �Ѵ�)
		DW		2880			; ����̺� ũ��(2880���ͷ� �ؾ� ��)
		DB		0xf0			; �̵�� Ÿ��(0xf0�� �ؾ� ��)
		DW		9			; FAT���� ����(9���ͷ� �ؾ� ��)
		DW		18			; 1Ʈ���� �� ���� ���Ͱ� ������(18�� �ؾ� ��)
		DW		2			; ��� ��(2�� �ؾ� ��)
		DD		0			; ��Ƽ���� ������� �ʱ� ������ ����� �ݵ�� 0
		DD		2880			; ����̺� ũ�⸦ �ѹ� �� write
		DB		0,0,0x29		; �� ������ �� ������ �� �θ� ���� �� ����
		DD		0xffffffff		; �Ƹ�, ���� �ø��� ��ȣ
		DB		"HARIBOTE-OS  "		; ��ũ �̸�(11����Ʈ)
		DB		"FAT12   "		; ���� �̸�(8����Ʈ)
		RESB		18			; �켱 18����Ʈ�� ��� �д�

;���α׷���ü
entry:
		MOV		ax,0			;�������� �ʱ�ȭ
		MOV		ss,ax
		MOV		sp,0x7c00
		MOV		ds,ax
		MOV		es,ax

;��ũ�� �д´�

		MOV		ax,0x0820		;��ũ���� ���� ������ �޸� ��� ������ ���ΰ�= es:bx	
		MOV		es,ax			;es=0x8200 bx=0(�ؿ����� ����) 8200������ ��ũ������ �Ǹ���.(0x8200~0x83ff) ->�׳� �̷��� �ߴ�(�����ó�� �ƹ��� ��������ʴ� �κ�). 8000~81ff������ ��Ʈ���͸� ������
		MOV		ch,0			;interrupt 13�� ȣ��� �Ǹ��� 0
		MOV		dh,0			;interrupt 13�� ȣ��� ���0(������ ������0, �Ʒ��� ������ 1)
		MOV		cl,2			;interrupt 13�� ȣ��� ����2(��Ʈ�δ��� 1���� �ö󰡰� 2���� �ü�� ���� �ø�����)
readloop:		
		MOV		si,0			;����Ƚ���� count�ϴ� register
retry:	
		MOV		ah,0x02			;interrupt 13�� ȣ��� ��ũ read(2:read,3:write,4:�˻�,5:ã��)
		MOV		al,1			;1���� ũ�⸸ŭ �аڴ�.
		MOV 		bx,0			;interrupt 13�� ȣ��� ���۾�巹��(�˻�/ã�⿡ ����ϹǷ� 0���� �ʱ�ȭ)
		MOV		dl,0x00			;interrupt 13�� ȣ��� ����̺��ȣ(0:a����̺�)
		INT		0x13			;interrupt 13�� ȣ��(��ũ BIOS ȣ��)
		JNC		next			;error������
		ADD		si,1			;error�� si�� �ϳ��� ī��Ʈ��
		CMP		si,5			;���ѷ��� ���� ���� �����ϱ� ���ؼ� 5�� �����ϸ� ����
		JAE		error			;si>=5 error�� ����
		MOV		ah,0x00			;interrupt 13�� ȣ��� ah:00,dl:00�� system reset�� �ǹ�
		MOV		DL,0x00			
		INT		0x13
		JMP		retry			;������ �ٽ� try
next:
		MOV		ax,es			;es���� 0x200��ŭ ���Ѵ�. 0x200(512����Ʈ) ��, ������������
		ADD		ax,0x0020
		MOV		es,ax		
		ADD		cl,1			;cl�� 1�� ���Ѵ�(count�Ѵ�.)
		CMP		cl,18			;cl���� 18�� ���Ѵ�.(18�����̸� 1�Ǹ��� ������)
		JBE		readloop		;cl�� 18���϶�� readloop�� ���ư���.
		MOV		cl,1			;���� �ѹ��� �� �������Ƿ� �ʱ�ȭ
		ADD		dh,1			;����� ���� ���� �������� dh
		CMP		dh,2			;dh�� 2�� ��
		JB		readloop		;dh 0~1�̶�� readloop��
		MOV		dh,0			;dh �� �������Ƿ� �ʱ�ȭ
		ADD		ch,1			;�Ǹ��� ���� ���� ch ��������
		CMP		ch,CYLS			;ch�� CYLS(=10)�� ��
		JB		readloop		;0~9��� readloop��

;error ������
noerr:
		MOV		si,msg
		JMP		putloop

;error ������ 
error:		
		MOV		ax,0
		MOV		es,ax
		MOV		si,errmsg

putloop:
		MOV		al,[si]			;si�� ���� msg �ּҷκ��� ���ʴ�� ������ ���� al�� ����
		ADD		si,1			;SI�������� 1����
		CMP		al,0			;al�� 0�� ���ؼ�
		JE		fin			;������ fin���� �ƴϸ� �н�
		MOV		ah,0x0e			;�� ���� ǥ��(���� �Լ� ���μ� 0x0e�϶� �ѱ��ڸ� ����Ѵ�.)
		MOV		bx,10			;Į���ڵ�(interrupt 10�� �ɶ� bx���� ������ ��µ� �۾��� ������ ����)
		INT		0x10			;���ͷ�Ʈ�ɰ� 10�� bios�Լ�(���� BIOS)ȣ��,ah:0x0e,al:ĳ�����ڵ�, bh:�÷��ڵ�
		JMP		putloop			;���� ����	

fin:
		HLT					;input�� ���������� cpu����
		JMP		fin			;Endless loop

msg:
		DB		0x0a,0x0a		;���� 2��
		DB              "HO Operating System"	;�޽���
		DB 		0x0a			;����
		DB		0			;�޽����� ���� �˸��� ���� ��ġ

;error �޽���
errmsg:
		DB		0x0a, 0x0a		; ������ 2��
		DB		"load error"
		DB		0x0a			; ����
		DB		0
		

		RESB		510-($-$$)		;0x7dfe(0x7c00���� 510��°)���� 0���� ä��
		DB		0x55,0xaa		;��Ʈ�δ� ����ǥ. ������ ��Ʈ�δ���� �ν� ����


