C:\Tools\tolset\z_tools\nask.exe ipl.nas ipl.bin
C:\Tools\tolset\z_tools\edimg.exe   imgin:C:\Users\Ho\Tools\tolset\z_tools\fdimg0at.tek   wbinimg src:ipl.bin len:512 from:0 to:0   imgout:helloos.img
@set SDL_VIDEODRIVER=windib
@set QEMU_AUDIO_DRV=none
@set QEMU_AUDIO_LOG_TO_MONITOR=0
C:\tools\tolset\z_tools\qemu\qemu.exe -fda helloos.img
cmd.exe
