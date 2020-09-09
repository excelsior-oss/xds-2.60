@echo off
set COMPONET_NAME=H2D Converter

call %~dp0\..\..\.config\setenv.bat

set LOG_DIR=%~dp0\log

for %%i in (log) do if not exist %%i mkdir %%i 

set WAS_MAKE_ERROR=no

rem ============================================= Make component
:lbl_h2d
set TASK_NAME=ximp
echo Build %TASK_NAME%

echo xc =p src\ximp.prj   1>  "%LOG_DIR%\%TASK_NAME%.log" 2>&1
xc =p src\ximp.prj        1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
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
