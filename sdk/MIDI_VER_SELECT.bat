@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

:: 设置目标文件路径
set "target_file=apps\app\src\mbox_flash\uc03\app_modules.h"
set "temp_file=%temp%\%~nx0.tmp"

:: 检查参数
if "%~1"=="" (
    echo Usage: %~nx0 [2BYTE^|4BYTE]
    echo Example: %~nx0 2BYTE
    echo Example: %~nx0 4BYTE
    pause
    exit /b 1
)

:: 将参数转换为大写
set "param=%~1"
for %%i in (A B C D E F G H I J K L M N O P Q R S T U V W X Y Z) do (
    set "param=!param:%%i=%%i!"
)

:: 验证参数
if not "!param!"=="2BYTE" if not "!param!"=="4BYTE" (
    echo Error: Parameter must be 2BYTE or 4BYTE
    pause
    exit /b 1
)

:: 检查文件是否存在
if not exist "%target_file%" (
    echo Error: Cannot find file %target_file%
    echo Please make sure you're running this script from the correct directory
    dir "apps\app\src\mbox_flash\uc03\" 2>nul || echo The directory structure does not exist
    pause
    exit /b 1
)

:: 设置要替换的内容
set "replace_line=#define MIDI_VER_SELECT MIDI_VER_!param!"

:: 处理文件
echo Processing file: %target_file%
(for /f "usebackq tokens=1* delims=:" %%a in (`findstr /n "^" "%target_file%"`) do (
    set "line=%%b"
    if defined line (
        :: 检查是否以 #define MIDI_VER_SELECT 开头
        echo !line! | findstr /b /c:"#define MIDI_VER_SELECT" >nul
        if !errorlevel! equ 0 (
            echo !replace_line!
        ) else (
            echo(!line!
        )
    ) else echo(
)) > "%temp_file%"

:: 替换原文件
move /y "%temp_file%" "%target_file%" >nul

echo Done!
echo MIDI_VER_SELECT has been set to MIDI_VER_!param! in %target_file%




