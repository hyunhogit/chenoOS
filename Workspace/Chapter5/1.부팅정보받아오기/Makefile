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
COPY     = copy
DEL      = del

# 디폴트 동작

default :
	$(MAKE) img

# 파일 생성 규칙

boot.bin : boot.asm Makefile
	$(NASK) boot.asm boot.bin

hoos.bin : hoos.asm Makefile
	$(NASK) hoos.asm hoos.bin 

bootpack.gas : bootpack.c Makefile
	$(CC1) -o bootpack.gas bootpack.c

bootpack.asm : bootpack.gas Makefile
	$(GAS2NASK) bootpack.gas bootpack.asm

bootpack.obj : bootpack.asm Makefile
	$(NASK) bootpack.asm bootpack.obj

asm_func.obj : asm_func.asm Makefile
	$(NASK) asm_func.asm asm_func.obj 

bootpack.bim : bootpack.obj asm_func.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bootpack.bim stack:3136k map:bootpack.map \
		bootpack.obj asm_func.obj
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

# 커맨드

img :
	$(MAKE) haribote.bin

run :
	$(MAKE) img
	C:\Users\Ho\Tools\Qemu\qemu.exe -fda haribote.bin

install :
	$(MAKE) img
	$(IMGTOL) w a: haribote.bin

clean :
	-$(DEL) *.bin
	-$(DEL) *.lst
	-$(DEL) *.gas
	-$(DEL) *.obj
	-$(DEL) bootpack.nas
	-$(DEL) bootpack.map
	-$(DEL) bootpack.bim
	-$(DEL) bootpack.hrb
	-$(DEL) haribote.sys
	-$(DEL)	haribote.bin

src_only :
	$(MAKE) clean
	-$(DEL) haribote.img
