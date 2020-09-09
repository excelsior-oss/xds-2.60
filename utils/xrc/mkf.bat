@echo off
set COMPONET_NAME=XDS-86 Resource-Compiler

call %~dp0\..\..\.config\setenv.bat

set LOG_DIR=%~dp0\log

for %%i in (log) do if not exist %%i mkdir %%i 

set WAS_MAKE_ERROR=no

set DLL_OPTIONS=-dbgfmt=HLL -decor=rt -woff+

rem ============================================= Make component
:lbl_xrc
set TASK_NAME=xrc
echo Build %TASK_NAME%

set MAKE_OPTIONS=CFG="xrc - Win32 Debug"
set MSBuild=%MSBUILD_HOME%\Bin\MSBuild.exe
set MAKE_OPTIONS=/t:Build /property:configuration=Debug /property:platform=Win32

echo "%MSBuild%" xrc.sln  %MAKE_OPTIONS%  1>  "%LOG_DIR%\%TASK_NAME%.log" 2>&1
"%MSBuild%" xrc.sln  %MAKE_OPTIONS%       1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
if errorlevel 1  call :lbl_Error %TASK_NAME%

rem set MAKE_OPTIONS=CFG="xrc - Win32 Debug"
rem pushd src
rem echo nmake /f xrc.mak %MAKE_OPTIONS%  1>  "%LOG_DIR%\%TASK_NAME%.log" 2>&1
rem nmake /f xrc.mak %MAKE_OPTIONS%       1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
rem if errorlevel 1  call :lbl_Error %TASK_NAME%
rem popd
rem if exist src\Debug\xrc.exe  copy src\Debug\xrc.exe  .\


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
