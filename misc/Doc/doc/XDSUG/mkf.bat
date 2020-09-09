@echo off

set PRJ_FILE=
if "%1" == ""    set PRJ_FILE=xc.mkf
if "%1" == "xc"  set PRJ_FILE=xc.mkf
if "%1" == "xm"  set PRJ_FILE=xm.mkf


set TARGET_FMT=
if "%2" == ""     set TARGET_FMT=hlp
if "%2" == "hlp"  set TARGET_FMT=hlp
if "%2" == "pdf"  set TARGET_FMT=pdf
if "%2" == "chm"  set TARGET_FMT=chm

if "%TARGET_FMT%" == "" goto :invalid_args

echo Building %PRJ_FILE% in %TARGET_FMT% format
call nmake /f %PRJ_FILE%  FORMAT=%TARGET_FMT%
if errorlevel 1 goto make_error
echo ============ make %PRJ_FILE% ============ OK!
goto :EOF

:make_error
echo *********** make %PRJ_FILE% failed *********** Erorr(s)!
exit 1

:invalid_args
echo Specify the target format
echo   mfk.bat xc^|xm hlp  - build Windows Help
echo   mfk.bat xc^|xm pdf  - build Abode PDF
echo   mfk.bat xc^|xm chm  - build Microsoft Compiled HTML Help
exit 1
