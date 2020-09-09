@echo off
call ..\env.bat

set PRJ_FILE=convertn.mak

call nmake /f %PRJ_FILE%
if errorlevel 1 goto make_error
echo ============ make %PRJ_FILE% ============ OK!
goto :EOF

:make_error
echo *********** make %PRJ_FILE% failed *********** Erorr(s)!
exit 1

