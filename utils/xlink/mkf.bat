@echo off
set COMPONET_NAME=XDS-86 Linker

call %~dp0\..\..\.config\setenv.bat

set LOG_DIR=%~dp0\log

for %%i in (log,ch) do if not exist %%i mkdir %%i 

set WAS_MAKE_ERROR=no

set DLL_OPTIONS=-dbgfmt=HLL -decor=rt -woff+

rem ============================================= Make component
:lbl_xlink
set TASK_NAME=xlink
echo Build %TASK_NAME%

set MSBuild=%MSBUILD_HOME%\Bin\MSBuild.exe
set MAKE_OPTIONS=/t:Build /property:configuration=Debug /property:platform=Win32

echo "%MSBuild%" xlink.sln  %MAKE_OPTIONS%  1>  "%LOG_DIR%\%TASK_NAME%.log" 2>&1
"%MSBuild%" xlink.sln  %MAKE_OPTIONS%       1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
if errorlevel 1  call :lbl_Error %TASK_NAME%

rem echo call nmake -f src\xlink.mkf.msvc  1>  "%LOG_DIR%\%TASK_NAME%.log" 2>&1
rem call nmake -f src\xlink.mkf.msvc       1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
rem if errorlevel 1  call :lbl_Error %TASK_NAME%


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
