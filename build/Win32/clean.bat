@echo off
set CLEAN_MODE=%~1
if "%CLEAN_MODE%" == ""  set CLEAN_MODE=ENDUSER

call setenv.bat

nmake cleanup MODE=%CLEAN_MODE% || exit /B 1
