@echo off
call ctr_env.bat
call ..\..\..\build\Win32\setenv.bat

if not exist %CHECKDIR%\xds\lib\C\libxds.lib  call :BUILD_LIBRARY 
if not exist %CHECKDIR%\xds\lib\C\libts.lib   call :BUILD_LIBRARY 

set XDSDIR=%CHECKDIR%\xds

%*

goto :EOF


:BUILD_LIBRARY
pushd %CHECKDIR%\xds\lib\C 
nmake /f msvc.mkf
nmake /f msvcts.mkf
popd

goto :EOF
