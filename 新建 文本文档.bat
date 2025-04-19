@echo off
chcp 936 > nul
title 自动连续烧录工具

set AIRISPTOOL=C:\Users\15951\AppData\Local\Arduino15\packages\AirM2M\tools\AirISP\0.2.0/AirISP-next.exe
set HEXFILE=D:\arduino_build_882167/new.ino.hex
set PORT=COM3
set COUNT=0

echo 自动连续烧录程序已启动
echo 使用 Ctrl+C 可随时终止程序
echo.

:loop
set /a COUNT+=1
echo [%DATE% %TIME%] 正在进行第 %COUNT% 次烧录...

%AIRISPTOOL% --chip auto --port %PORT% --baud 115200 --before default_reset --after hard_reset write_flash -e 0x08000000 %HEXFILE%

if %ERRORLEVEL% EQU 0 (
    echo [%DATE% %TIME%] 第 %COUNT% 次烧录成功！
    echo ^G
) else (
    echo [%DATE% %TIME%] 第 %COUNT% 次烧录失败，错误代码：%ERRORLEVEL%
    echo ^G^G
)

echo 等待1秒后开始下一次烧录...
timeout /t 1 /nobreak > nul
goto loop