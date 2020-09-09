@echo off
if .%1 == . goto help
if .%2 == . goto help
cd %1
if errorlevel 1 goto nodir

if .%3 == . goto same
nmake -nologo -f %3.mkf FORMAT=%2
if errorlevel 1 goto quit
cd ..
goto quit

:same
nmake -nologo -f %1.mkf FORMAT=%2
if errorlevel 1 goto quit
cd ..
goto quit

:help
echo Usage:
echo     MAKE directory format [file]
goto quit

:nodir
echo %1 not exists or is not a directory
goto quit

:quit
