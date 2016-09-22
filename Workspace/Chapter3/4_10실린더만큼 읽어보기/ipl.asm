;haribote-os
;TAB=4
CYLS		EQU		10
		ORG		0x7c00			;메모리 내 어디에 로딩 될 것인가.반드시 이 위치여야만 한다.메모리 아무데나 사용하면 안된다. 개인적으로 쓸수 있는 부분이 있는것.부트섹터는 0x7c00~0x7dff

;FAT12포맷 플로피디스크를 위한 서술
		JMP		entry
		DB		0x90
		DB		"HARIBOTE"		; boot sector이름을 자유롭게 써도 좋다(8바이트)
		DW		512			; 1섹터 크기(512로 해야 함)
		DB		1			; 클러스터 크기(1섹터로 해야 함)
		DW		1			; FAT가 어디에서 시작될까(보통 1섹터째부터)
		DB		2			; FAT 개수(2로 해야 함)
		DW		224			; 루트 디렉토리 영역의 크기(보통 224엔트리로 해야 한다)
		DW		2880			; 드라이브 크기(2880섹터로 해야 함)
		DB		0xf0			; 미디어 타입(0xf0로 해야 함)
		DW		9			; FAT영역 길이(9섹터로 해야 함)
		DW		18			; 1트럭에 몇 개의 섹터가 있을까(18로 해야 함)
		DW		2			; 헤드 수(2로 해야 함)
		DD		0			; 파티션을 사용하지 않기 때문에 여기는 반드시 0
		DD		2880			; 드라이브 크기를 한번 더 write
		DB		0,0,0x29		; 잘 모르지만 이 값으로 해 두면 좋은 것 같다
		DD		0xffffffff		; 아마, 볼륨 시리얼 번호
		DB		"HARIBOTE-OS  "		; 디스크 이름(11바이트)
		DB		"FAT12   "		; 포맷 이름(8바이트)
		RESB		18			; 우선 18바이트를 비어 둔다

;프로그램본체
entry:
		MOV		ax,0			;레지스터 초기화
		MOV		ss,ax
		MOV		sp,0x7c00
		MOV		ds,ax
		MOV		es,ax

;디스크를 읽는다

		MOV		ax,0x0820		;디스크에서 읽은 정보를 메모리 어디에 저장할 것인가= es:bx	
		MOV		es,ax			;es=0x8200 bx=0(밑에보면 있음) 8200번지에 디스크정보가 실린다.(0x8200~0x83ff) ->그냥 이렇게 했다(여기근처는 아무도 사용하지않는 부분). 8000~81ff까지는 부트섹터를 실을것
		MOV		ch,0			;interrupt 13번 호출시 실린더 0
		MOV		dh,0			;interrupt 13번 호출시 헤더0(위에서 읽으면0, 아래서 읽으면 1)
		MOV		cl,2			;interrupt 13번 호출시 섹터2(부트로더가 1번에 올라가고 2번에 운영체제 내용 올릴려고)
readloop:		
		MOV		si,0			;실패횟수를 count하는 register
retry:	
		MOV		ah,0x02			;interrupt 13번 호출시 디스크 read(2:read,3:write,4:검사,5:찾기)
		MOV		al,1			;1섹터 크기만큼 읽겠다.
		MOV 		bx,0			;interrupt 13번 호출시 버퍼어드레스(검사/찾기에 사용하므로 0으로 초기화)
		MOV		dl,0x00			;interrupt 13번 호출시 드라이브번호(0:a드라이브)
		INT		0x13			;interrupt 13번 호출(디스크 BIOS 호출)
		JNC		next			;error없을시
		ADD		si,1			;error시 si를 하나씩 카운트함
		CMP		si,5			;무한루프 도는 것을 방지하기 위해서 5번 실패하면 포기
		JAE		error			;si>=5 error로 점프
		MOV		ah,0x00			;interrupt 13번 호출시 ah:00,dl:00은 system reset을 의미
		MOV		DL,0x00			
		INT		0x13
		JMP		retry			;리셋후 다시 try
next:
		MOV		ax,es			;es값을 0x200만큼 더한다. 0x200(512바이트) 즉, 다음섹션으로
		ADD		ax,0x0020
		MOV		es,ax		
		ADD		cl,1			;cl에 1을 더한다(count한다.)
		CMP		cl,18			;cl값을 18과 비교한다.(18섹션이면 1실린더 읽은것)
		JBE		readloop		;cl이 18이하라면 readloop로 돌아간다.
		MOV		cl,1			;섹터 한바퀴 다 돌았으므로 초기화
		ADD		dh,1			;헤더를 세기 위한 레지스터 dh
		CMP		dh,2			;dh와 2를 비교
		JB		readloop		;dh 0~1이라면 readloop로
		MOV		dh,0			;dh 다 돌았으므로 초기화
		ADD		ch,1			;실린더 새기 위한 ch 레지스터
		CMP		ch,CYLS			;ch와 CYLS(=10)을 비교
		JB		readloop		;0~9라면 readloop로

;error 없을시
noerr:
		MOV		si,msg
		JMP		putloop

;error 있을시 
error:		
		MOV		ax,0
		MOV		es,ax
		MOV		si,errmsg

putloop:
		MOV		al,[si]			;si를 통해 msg 주소로부터 차례대로 내부의 값을 al에 저장
		ADD		si,1			;SI레지스터 1증가
		CMP		al,0			;al이 0인 비교해서
		JE		fin			;맞으면 fin으로 아니면 패스
		MOV		ah,0x0e			;한 글자 표시(비디오 함수 모드로서 0x0e일때 한글자를 출력한다.)
		MOV		bx,10			;칼라코드(interrupt 10번 걸때 bx값이 비디오에 출력될 글씨의 색상을 지정)
		INT		0x10			;인터럽트걸고 10번 bios함수(비디오 BIOS)호출,ah:0x0e,al:캐릭터코드, bh:컬러코드
		JMP		putloop			;루프 생성	

fin:
		HLT					;input이 있을때까지 cpu정지
		JMP		fin			;Endless loop

msg:
		DB		0x0a,0x0a		;개행 2개
		DB              "HO Operating System"	;메시지
		DB 		0x0a			;개행
		DB		0			;메시지의 끝을 알리기 위한 장치

;error 메시지
errmsg:
		DB		0x0a, 0x0a		; 개행을 2개
		DB		"load error"
		DB		0x0a			; 개행
		DB		0
		

		RESB		510-($-$$)		;0x7dfe(0x7c00부터 510번째)까지 0으로 채움
		DB		0x55,0xaa		;부트로더 인증표. 없으면 부트로더라고 인식 못함


