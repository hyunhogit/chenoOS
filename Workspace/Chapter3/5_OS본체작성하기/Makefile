#변수설정
TOOLPATH = C:\Users\Ho\Tools\tolset\z_tools\

#인자 없는 경우
default:
	$(TOOLPATH)make.exe run

#파일 생성규칙

ipl.bin:ipl.nas Makefile
	$(TOOLPATH)nask.exe ipl.nas ipl.bin

helloos.img:ipl.bin Makefile
	$(TOOLPATH)edimg.exe imgin:$(TOOLPATH)fdimg0at.tek wbinimg src:ipl.bin len:512 from:0 \
		to:0 imgout:helloos.img

#커맨드:

img:
	$(TOOLPATH)make.exe -r helloos.img

run:
	$(TOOLPATH)make.exe img
	$(TOOLPATH)qemu.exe helloos.img

clean:
	-del ipl.bin
	-del helloos.img

