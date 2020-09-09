@echo off
if "%XDS_BUILD_SYSTEM_ENV%" == "%~dp0\setenv.bat"  goto :EOF

set CONFIG_DIR=%~dp0\.config
if exist "%CONFIG_DIR%"  goto :lbl_ReadConfig 

set CONFIG_DIR=%~dp0\..\..\.config
if exist "%CONFIG_DIR%"  goto :lbl_ReadConfig 

echo *** Environment Error: cannot find ".config" directory
exit /B 1

rem ============================================= Read Config file
:lbl_ReadConfig
call "%CONFIG_DIR%\setenv.bat" || exit /B 1
set XDS_BUILD_SYSTEM_ENV=%~dp0\setenv.bat
exit /B 0
