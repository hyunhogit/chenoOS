

#인자 없는 경우
default:
	C:\Users\Ho\Tools\tolset\z_tools\make.exe run

#파일 생성규칙

ipl.bin:ipl.nas Makefile
	C:\Users\Ho\Tools\tolset\z_tools\nask.exe ipl.nas ipl.bin

helloos.bin:ipl.bin Makefile
	C:\Users\Ho\Tools\tolset\z_tools\edimg.exe imgin:C:\Users\Ho\Tools\tolset\z_tools\fdimg0at.tek wbinimg src:ipl.bin len:512 from:0 \
		to:0 imgout:helloos.bin

#커맨드:

img:
	C:\Users\Ho\Tools\tolset\z_tools\make.exe -r helloos.bin

run:
	C:\Users\Ho\Tools\tolset\z_tools\make.exe img
	C:\Users\Ho\Tools\tolset\z_tools\qemu\qemu.exe -L . -m 32 -localtime -std-vga -fda helloos.img

clean:
	-del ipl.bin
	-del helloos.img

