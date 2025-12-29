@echo off
echo Generating translation files...

REM 设置 Qt 路径
set QT_PATH=C:\Qt\6.8.3\msvc2022_64\bin

REM 更新 .ts 文件（从源代码提取翻译字符串）
"%QT_PATH%\lupdate.exe" Hello -ts translations\Hello_zh_CN.ts translations\Hello_en_US.ts

REM 编译 .ts 文件为 .qm 文件
"%QT_PATH%\lrelease.exe" translations\Hello_zh_CN.ts -qm translations\Hello_zh_CN.qm
"%QT_PATH%\lrelease.exe" translations\Hello_en_US.ts -qm translations\Hello_en_US.qm

echo.
echo Translation files generated successfully!
echo - translations/Hello_zh_CN.qm
echo - translations/Hello_en_US.qm
echo.
echo You can now edit .ts files with Qt Linguist and regenerate .qm files.
pause
