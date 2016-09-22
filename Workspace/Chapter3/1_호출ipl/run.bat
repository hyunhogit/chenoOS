C:\Users\Ho\Tools\tolset\z_tools\nask.exe ipl.nas ipl.bin
C:\Users\Ho\Tools\tolset\z_tools\edimg.exe   imgin:C:\Users\Ho\Tools\tolset\z_tools\fdimg0at.tek   wbinimg src:ipl.bin len:512 from:0 to:0   imgout:helloos.img
REM Start qemu on windows.
@ECHO OFF

REM SDL_VIDEODRIVER=directx is faster than windib. But keyboard cannot work well.
SET SDL_VIDEODRIVER=windib

REM SDL_AUDIODRIVER=waveout or dsound can be used. Only if QEMU_AUDIO_DRV=sdl.
SET SDL_AUDIODRIVER=dsound

REM QEMU_AUDIO_DRV=dsound or fmod or sdl or none can be used. See qemu -audio-help.
SET QEMU_AUDIO_DRV=dsound

REM QEMU_AUDIO_LOG_TO_MONITOR=1 displays log messages in QEMU monitor.
SET QEMU_AUDIO_LOG_TO_MONITOR=0

REM PCI-based PC(default): -M pc 
REM ISA-based PC         : -M isapc
REM -M isapc is added for NE2000 ISA card.
@ECHO ON
C:\Users\Ho\Tools\Qemu\qemu.exe -fda helloos.img
