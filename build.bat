@echo off
echo Building Hello project with CMake...

REM 配置项目
cmake --preset windows-debug

REM 构建项目
cmake --build build/windows-debug

echo.
echo Build complete! Executable is in: build/windows-debug/bin/Hello.exe
pause
