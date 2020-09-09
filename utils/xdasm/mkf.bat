@echo off
set COMPONET_NAME=XDS-x86 Disassembler

call %~dp0\..\..\.config\setenv.bat

set LOG_DIR=%~dp0\log

for %%i in (log) do if not exist %%i mkdir %%i 

set WAS_MAKE_ERROR=no

set DLL_OPTIONS=-dbgfmt=HLL -decor=rt -woff+

rem ============================================= Make component
:lbl_xdasm
set TASK_NAME=xdasm
echo Build %TASK_NAME%

set MAKE_OPTIONS=%DLL_OPTIONS%

echo xc =p src\xdasm.prj %MAKE_OPTIONS%  1>  "%LOG_DIR%\%TASK_NAME%.log" 2>&1
xc =p src\xdasm.prj  %MAKE_OPTIONS%      1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
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
