@echo off
rem Make both XDS-x86 compiler and XDS-C convereter

set COMPONET_NAME=XDS-x86 Compiler

set LOG_DIR=%~dp0\log

for %%i in (ch,log,sym) do if not exist %%i mkdir %%i 

set WAS_MAKE_ERROR=no

set DLL_OPTIONS=-dbgfmt=HLL -decor=rt -woff+

rem ============================================= Make component
:lbl_xd
set TASK_NAME=xc
echo Build %TASK_NAME%

call make_xc.bat  1>  "%LOG_DIR%\%TASK_NAME%.log" 2>&1
if errorlevel 1  call :lbl_Error %TASK_NAME%

rem ============================================= Make component
:lbl_xd
set TASK_NAME=xm
echo Build %TASK_NAME%

call make_xm.bat  1>  "%LOG_DIR%\%TASK_NAME%.log" 2>&1
if errorlevel 1  call :lbl_Error %TASK_NAME%


rem ============================================= End
if "%WAS_MAKE_ERROR%" == "no"  goto  :lbl_Success
echo *** Build %COMPONET_NAME% *** Failed!
goto :EOF

rem ============================================= Script Messages
:lbl_Success
echo === Build %COMPONET_NAME% === OK!
goto :EOF


:lbl_Error
echo *** Build %~1 *** Failed!
set WAS_MAKE_ERROR=yes
goto :EOF
