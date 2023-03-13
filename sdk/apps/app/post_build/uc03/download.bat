

set ELF_NAME=%1%
cd /d %~dp0

set OBJDUMP=C:\JL\pi32\bin\llvm-objdump.exe
set OBJCOPY=C:\JL\pi32\bin\llvm-objcopy.exe

%OBJDUMP% -d -print-imm-hex -print-dbg %ELF_NAME%.elf > %ELF_NAME%.lst
%OBJCOPY% -O binary -j .app_code %ELF_NAME%.elf %ELF_NAME%.bin
%OBJCOPY% -O binary -j .data %ELF_NAME%.elf data.bin
%OBJDUMP% -section-headers %ELF_NAME%.elf
copy /b %ELF_NAME%.bin+data.bin app.bin

@echo *******************************************************************************************************
@echo UC03 flash
@echo *******************************************************************************************************
@echo % date %
cd / d % ~dp0

isd_download.exe -tonorflash -dev uc03 -boot 0x101600 -div8 -wait 300 -otp usbboot.bin -uboot uboot.boot -app app.bin 0x2f000 -res dir_a dir_song dir_eng dir_poetry dir_story dir_bin_f1x dir_notice dir_midi midi_cfg -output-fw output_flash.fw

::-ex_flash
::-format all
@REM 常用命令说明
@rem - format vm
@rem - format all
@rem - reboot 500

copy /y /b output_flash.fw jl_isd.fw
fw_add.exe -noenc -fw jl_isd.fw -add ota.bin -type 100 -out jl_isd.fw
ufw_maker.exe -fw_to_ufw jl_isd.fw
copy jl_isd.ufw update.ufw
del jl_isd.ufw

ping / n 2 127.1 > null
IF EXIST null del null
