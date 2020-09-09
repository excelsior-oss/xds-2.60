@echo off
SET MSVC=C:\Program Files\Microsoft Visual Studio 9.0\VC
SET MSDEV=C:\Program Files\Microsoft Visual Studio 9.0\Common7\IDE
SET WINSDK=c:\Program Files\Microsoft SDKs\Windows\v6.1

SET INCLUDE=%WINSDK%\include;%MSVC%\include;%MSVC%\atlmfc\include;
SET LIB=%WINSDK%\lib;%MSVC%\lib;%MSVC%\atlmfc\lib;
SET PATH=%MSVC%\bin;%MSDEV%;%WINSDK%\Bin;%PATH%

if "%1" == "" goto :EOF
call %*
goto :EOF
