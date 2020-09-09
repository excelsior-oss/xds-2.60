@echo off
call ctr_env.bat
call clean_rep.bat
call ..\..\..\build\Win32\setenv.bat

if not exist %CHECKDIR%\xds\lib\C\libxds.lib  call :lbl_build_libraries 
if not exist %CHECKDIR%\xds\lib\C\libts.lib   call :lbl_build_libraries 

set XDSDIR=%CHECKDIR%\xds


for /F "eol=; tokens=*" %%i IN (template.txt) do  call :lbl_run_tests %%i 

goto :EOF


:lbl_run_tests
if "%1" == "--"  goto :EOF
if "%1" == ""    goto :EOF
for %%i in (%TEMPL_DIR%\%1\*.tg)  do call test %%i %1

goto :EOF


:lbl_build_libraries
pushd %CHECKDIR%\xds\lib\c 
nmake /f msvc.mkf
nmake /f msvcts.mkf
popd

goto :EOF
