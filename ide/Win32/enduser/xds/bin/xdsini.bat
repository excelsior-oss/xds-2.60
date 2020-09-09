@echo off
if "%1"=="xm" goto  :configure_to_XDS_C 
if "%1"=="xc" goto  :configure_to_XDS_x86 

echo XDS Environment configuration script.
echo Usage:
echo   xdsini.bat xc  - configure for Native XDS-x86
echo   xdsini.bat xm  - configure for XDS-C
goto :EOF 


:configure_to_XDS_C
copy /Y xdsc.ini xds.ini  1> nul
if errorlevel 1 goto :error_handler 
echo XDS Environment has been configured for XDS-C
goto :launch_xds_ide 


:configure_to_XDS_x86
copy /Y  xdsx86.ini xds.ini 1> nul
if errorlevel 1 goto :error_handler 
echo XDS Environment has been configured for Native XDS-x86
goto :launch_xds_ide 


:launch_xds_ide
echo.
set /P XDSLAUNCH=Do you wish to launch XDS Environment (Y/N)? 
if "%XDSLAUNCH%"=="Y"  start xds.exe 
if "%XDSLAUNCH%"=="y"  start xds.exe 
goto :EOF 


:error_handler
echo Cannot retarget XDS Environment
exit 1 
