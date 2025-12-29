@echo off
echo Cleaning Qt project cache...

rmdir /s /q .vs 2>nul
rmdir /s /q x64 2>nul
rmdir /s /q Hello\x64 2>nul
del /q Hello\Hello.vcxproj.user 2>nul

echo Done! Please reopen Visual Studio and rebuild the project.
pause
