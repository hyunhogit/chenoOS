TOOLPATH = C:\Users\Ho\Tools\tolset/z_tools/
INCPATH  = C:\Users\Ho\Tools\tolset/z_tools/haribote/

MAKE     = $(TOOLPATH)make.exe -r
NASK     = $(TOOLPATH)nask.exe
CC1      = $(TOOLPATH)cc1.exe -I$(INCPATH) -Os -Wall -quiet
GAS2NASK = $(TOOLPATH)gas2nask.exe -a
OBJ2BIM  = $(TOOLPATH)obj2bim.exe
BIM2HRB  = $(TOOLPATH)bim2hrb.exe
RULEFILE = $(TOOLPATH)haribote/haribote.rul
EDIMG    = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)imgtol.com
MAKEFONT = $(TOOLPATH)makefont.exe
BIN2OBJ  = $(TOOLPATH)bin2obj.exe
COPY     = copy
DEL      = del

# 디폴트 동작

default :
	$(MAKE) run

# 파일 생성 규칙

boot.bin : boot.asm Makefile
	$(NASK) boot.asm boot.bin

hoos.bin : hoos.asm Makefile
	$(NASK) hoos.asm hoos.bin 

asm_func.obj : asm_func.asm Makefile
	$(NASK) asm_func.asm asm_func.obj 

hankaku.bin : hankaku.txt Makefile
	$(MAKEFONT) hankaku.txt hankaku.bin

hankaku.obj : hankaku.bin Makefile
	$(BIN2OBJ) hankaku.bin hankaku.obj _hankaku

bootpack.bim : bootpack.obj asm_func.obj  hankaku.obj  Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bootpack.bim stack:3136k map:bootpack.map \
		bootpack.obj asm_func.obj hankaku.obj  
# 3MB+64KB=3136KB

bootpack.hrb : bootpack.bim Makefile
	$(BIM2HRB) bootpack.bim bootpack.hrb 0

haribote.sys : hoos.bin bootpack.hrb Makefile
	copy /B hoos.bin+bootpack.hrb haribote.sys

haribote.bin : boot.bin haribote.sys Makefile
	$(EDIMG)   imgin:C:\Users\Ho\Tools\tolset/z_tools/fdimg0at.tek \
		wbinimg src:boot.bin len:512 from:0 to:0 \
		copy from:haribote.sys to:@: \
		imgout:haribote.bin

# 일반 규칙(생성규칙이 우선시)

%.gas : %.c Makefile
	$(CC1) -o $*.gas $*.c

%.asm : %.gas Makefile
	$(GAS2NASK) $*.gas $*.asm

%.obj : %.asm Makefile
	$(NASK) $*.asm $*.obj

# 커맨드

img :
	$(MAKE) haribote.bin

run :
	$(MAKE) img
	C:\Users\Ho\Tools\Qemu\qemu.exe  -m 32 -fda haribote.bin

install :
	$(MAKE) img
	$(IMGTOL) w a: haribote.bin

clean :
	-$(DEL) *.bin
	-$(DEL) *.gas
	-$(DEL) *.obj
	-$(DEL) bootpack.asm
	-$(DEL) bootpack.map
	-$(DEL) bootpack.bim
	-$(DEL) bootpack.hrb
	-$(DEL) haribote.sys
	-$(DEL)	haribote.bin

src_only :
	$(MAKE) clean
	-$(DEL) haribote.img
