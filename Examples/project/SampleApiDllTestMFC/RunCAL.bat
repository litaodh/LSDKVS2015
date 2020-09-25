@echo off

set toUpadte=Y
if %toUpadte%==Y (
for /f "delims=  " %%i in ('where EasyRanger.exe') do set exepath=%%i
)

start /d "%exepath:~0,-14%" Ranger3Setup.exe

exit