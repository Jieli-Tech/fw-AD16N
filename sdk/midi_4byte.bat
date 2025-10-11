@echo off
setlocal enabledelayedexpansion

set "input_file=AD16N_mbox_flash.cbp"
set "temp_file=%temp%\%~nx0.tmp"

set "search1=apps/include_lib/liba/ARCH/pi32v2_lto_r1/lib_midi_synth.a"
set "replace1=apps/include_lib/liba/ARCH/pi32v2_lto_r1/lib_midi_synth_pro.a"

set "search2=midi_2byte"
set "replace2=midi_4byte"

if not exist "%input_file%" (
    echo 错误：找不到文件 %input_file%
    pause
    exit /b 1
)

(for /f "tokens=1* delims=:" %%a in ('findstr /n "^" "%input_file%"') do (
    set "line=%%b"
    if defined line (
        set "line=!line:%search1%=%replace1%!"
        set "line=!line:%search2%=%replace2%!"
        echo(!line!
    ) else echo(
)) > "%temp_file%"

move /y "%temp_file%" "%input_file%" >nul

MIDI_VER_SELECT.bat 4BYTE
echo 完成！！！！！！
echo midi音色库4byte地址版本驱动替换完成，能支持立体声！
pause
