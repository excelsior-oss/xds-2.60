@echo off
set COMPONET_NAME=XDS Native libraries

call %~dp0\..\.config\setenv.bat

set LOG_DIR=%~dp0\log

for %%i in (enduser,enduser\def\ob2) do if not exist %%i mkdir %%i 
for %%i in (enduser\lib\x86,enduser\sym\x86) do if not exist %%i mkdir %%i 
for %%i in (log,ch\x86,sym\x86)      do if not exist %%i mkdir %%i 

set WAS_MAKE_ERROR=no

set LIB_OPTIONS=-dbgfmt=HLL -decor=rt -woff+
set DLL_OPTIONS=-dbgfmt=HLL -decor=rt -woff+

rem ============================================= Make Single Thread Library
:lbl_x86_libxds
set TASK_NAME=x86_libxds.lib
echo Build %TASK_NAME%

if exist ch\x86\*.* del /Q ch\x86\*.*
if exist sym\x86\*.* del /Q sym\x86\*.*

set MAKE_OPTIONS=-multithread- %LIB_OPTIONS%

echo xc.exe =p lib.prj %MAKE_OPTIONS%  1>  "%LOG_DIR%\%TASK_NAME%.log" 2>&1
xc.exe =p src\m\lib.prj %MAKE_OPTIONS% 1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
if errorlevel 1  call :lbl_Error %TASK_NAME%

rem ============================================= Make Single Thread TopSpeed Library
:lbl_x86_libts
set TASK_NAME=x86_libts.lib
echo Build %TASK_NAME%

set MAKE_OPTIONS=-multithread- %LIB_OPTIONS%

echo xc.exe =p src\TSlibs\tslib.prj %MAKE_OPTIONS%  1>  "%LOG_DIR%\%TASK_NAME%.log" 2>&1
xc.exe =p src\TSlibs\tslib.prj %MAKE_OPTIONS%       1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
if errorlevel 1  call :lbl_Error %TASK_NAME%


rem ============================================= Make Import32 Library
:lbl_import32	
set TASK_NAME=import32.lib
echo Build %TASK_NAME%

set SYS=%WINDIR%\system32
set SYSTEM32_DLLs="%SYS%\ADVAPI32.dll" "%SYS%\KERNEL32.dll" "%SYS%\LZ32.dll"     "%SYS%\NETAPI32.dll"  ^
                  "%SYS%\VDMDBG.dll"   "%SYS%\WSOCK32.dll"  "%SYS%\COMCTL32.dll" "%SYS%\COMDLG32.dll"  ^
                  "%SYS%\CTL3D32.dll"  "%SYS%\GDI32.dll"    "%SYS%\SHELL32.dll"  "%SYS%\USER32.dll"    ^
                  "%SYS%\WINMM.dll"    "%SYS%\IMM32.dll"    "%SYS%\OLEDLG.dll"   "%SYS%\TAPI32.dll"    ^
                  "%SYS%\AVICAP32.dll" "%SYS%\MSACM32.dll"  "%SYS%\MSVFW32.dll"  "%SYS%\WINSPOOL.DRV"

echo xlib /implib /nobak enduser\lib\x86\import32 %SYSTEM32_DLLs%  1>  "%LOG_DIR%\%TASK_NAME%.log" 2>&1
xlib /implib /nobak enduser\lib\x86\import32 %SYSTEM32_DLLs%       1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
if errorlevel 1  call :lbl_Error %TASK_NAME%


rem ============================================= Make Startup Libraries: xstart.lib, xstartd.lib, xstartc.lib, xstartx.lib
:lbl_xstart	
set TASK_NAME=xstart
echo Build %TASK_NAME%

set MAKE_OPTIONS= 

pushd src\os\w95\startup
echo nmake /f startup.mkf %MAKE_OPTIONS%   1>  "%LOG_DIR%\%TASK_NAME%.log" 2>&1
nmake /f startup.mkf %MAKE_OPTIONS%        1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
if errorlevel 1  call :lbl_Error %TASK_NAME%
popd


rem ============================================= Make Single Thread DLL
:lbl_stdll
set TASK_NAME=xds26.dll
echo Build %TASK_NAME%

for %%i in (obj,o) do if exist ch\x86\*.%%i del /Q ch\x86\*.%%i

set MAKE_OPTIONS=-gendll+ -multithread- %DLL_OPTIONS%

echo xc.exe =p src\m\lib.prj %MAKE_OPTIONS%  1>  "%LOG_DIR%\%TASK_NAME%.log" 2>&1
xc.exe =p =a src\m\lib.prj %MAKE_OPTIONS%    1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
if errorlevel 1  call :lbl_Error %TASK_NAME%


rem ============================================= Make Single Thread DLL import library
:lbl_stimplib
set TASK_NAME=xds26i.lib
echo Build %TASK_NAME%

set MAKE_OPTIONS=XC_OPTIONS="%DLL_OPTIONS%"

echo xlib /implib /nobak enduser\lib\x86\xds26i.lib enduser\lib\x86\xds26.dll  1>  "%LOG_DIR%\%TASK_NAME%.log" 2>&1
xlib /implib /nobak enduser\lib\x86\xds26i.lib enduser\lib\x86\xds26.dll   1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
if errorlevel 1  call :lbl_Error %TASK_NAME%

echo.                                               1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
pushd src\os\w95
echo nmake /a /f linkinto.mkf %MAKE_OPTIONS%        1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
nmake /a /f linkinto.mkf %MAKE_OPTIONS%             1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
if errorlevel 1  call :lbl_Error %TASK_NAME%
popd

echo.                                                           1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
echo xlib /nobak enduser\lib\x86\xds26i.lib +lib\x86\li.lib     1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
xlib /nobak enduser\lib\x86\xds26i.lib +enduser\lib\x86\li.lib  1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
if errorlevel 1  call :lbl_Error %TASK_NAME%                    


rem ============================================= Make Single Thread ToopSpeed DLL
:lbl_tsstdll
set TASK_NAME=xts26.dll
echo Build %TASK_NAME%

for %%i in (obj,o) do if exist ch\x86\*.%%i del /Q ch\x86\*.%%i

set MAKE_OPTIONS=-gendll+ -multithread- %DLL_OPTIONS%

echo xc.exe =p =a src\TSlibs\tslib.prj %MAKE_OPTIONS%  1>  "%LOG_DIR%\%TASK_NAME%.log" 2>&1
xc.exe =p =a src\TSlibs\tslib.prj %MAKE_OPTIONS%       1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
if errorlevel 1  call :lbl_Error %TASK_NAME%

rem ============================================= Make Single Thread ToopSpeed Import Library
:lbl_tsstimplib
set TASK_NAME=xts26i.dll
echo Build %TASK_NAME%

echo xlib /implib /nobak enduser\lib\x86\xts26i.lib enduser\lib\x86\xts26.dll  1>  "%LOG_DIR%\%TASK_NAME%.log" 2>&1
xlib /implib /nobak enduser\lib\x86\xts26i.lib enduser\lib\x86\xts26.dll       1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
if errorlevel 1  call :lbl_Error %TASK_NAME%



rem ============================================= Make Single Thread Library
:lbl_x86_libxdsmt
set TASK_NAME=x86_libxdsmt.lib
echo Build %TASK_NAME%

set MAKE_OPTIONS=-multithread+ %LIB_OPTIONS%

echo xc.exe =p src\m\lib.prj =a %MAKE_OPTIONS%  1>  "%LOG_DIR%\%TASK_NAME%.log" 2>&1
xc.exe =p src\m\lib.prj =a %MAKE_OPTIONS%       1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
if errorlevel 1  call :lbl_Error %TASK_NAME%

rem ============================================= Make Multi Thread TopSpeed Library
:lbl_x86_libtsmt
set TASK_NAME=x86_libtsmt.lib
echo Build %TASK_NAME%

set MAKE_OPTIONS=-multithread+ %LIB_OPTIONS%

echo xc.exe =p src\TSlibs\tslib.prj =a %MAKE_OPTIONS%  1>  "%LOG_DIR%\%TASK_NAME%.log" 2>&1
xc.exe =p src\TSlibs\tslib.prj =a %MAKE_OPTIONS%       1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
if errorlevel 1  call :lbl_Error %TASK_NAME%


rem ============================================= Make Multi Thread DLL
:lbl_mtdll
set TASK_NAME=xds26m.dll
echo Build %TASK_NAME%

set MAKE_OPTIONS=-gendll+ -multithread+ %DLL_OPTIONS%

echo xc.exe =p src\m\lib.prj =a %MAKE_OPTIONS%  1>  "%LOG_DIR%\%TASK_NAME%.log" 2>&1
xc.exe =p src\m\lib.prj =a %MAKE_OPTIONS%       1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
if errorlevel 1  call :lbl_Error %TASK_NAME%

rem ============================================= Make Mutlti Thread DLL import library
:lbl_stimplib
set TASK_NAME=xds26mi.lib
echo Build %TASK_NAME%

set MAKE_OPTIONS=XC_OPTIONS="%DLL_OPTIONS%"

echo xlib /implib /nobak enduser\lib\x86\xds26mi.lib enduser\lib\x86\xds26m.dll  1>  "%LOG_DIR%\%TASK_NAME%.log" 2>&1
xlib /implib /nobak enduser\lib\x86\xds26mi.lib enduser\lib\x86\xds26m.dll       1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
if errorlevel 1  call :lbl_Error %TASK_NAME%

echo.                                               1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
pushd src\os\w95
echo nmake /a /f linkinto.mkf %MAKE_OPTIONS%        1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
nmake /a /f linkinto.mkf %MAKE_OPTIONS%             1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
if errorlevel 1  call :lbl_Error %TASK_NAME%
popd

echo.                                               1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
echo xlib /nobak enduser\lib\x86\xds26mi.lib +lib\x86\li.lib     1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
xlib /nobak enduser\lib\x86\xds26mi.lib +enduser\lib\x86\li.lib  1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
if errorlevel 1  call :lbl_Error %TASK_NAME%


rem ============================================= Make Single Thread ToopSpeed DLL
:lbl_tsstdll
set TASK_NAME=xts26m.dll
echo Build %TASK_NAME%

for %%i in (obj,o) do if exist ch\x86\*.%%i del /Q ch\x86\*.%%i

set MAKE_OPTIONS=-gendll+ -multithread+ %DLL_OPTIONS%

echo xc.exe =p src\TSlibs\tslib.prj =a %MAKE_OPTIONS%  1>  "%LOG_DIR%\%TASK_NAME%.log" 2>&1
xc.exe =p src\TSlibs\tslib.prj =a %MAKE_OPTIONS%       1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
if errorlevel 1  call :lbl_Error %TASK_NAME%

rem ============================================= Make Single Thread ToopSpeed Import Library
:lbl_tsstimplib
set TASK_NAME=xts26mi.lib
echo Build %TASK_NAME%

set MAKE_OPTIONS=XC_OPTIONS="%DLL_OPTIONS%"

echo xlib /implib /nobak enduser\lib\x86\xts26mi.lib enduser\lib\x86\xts26m.dll  1>  "%LOG_DIR%\%TASK_NAME%.log" 2>&1
xlib /implib /nobak enduser\lib\x86\xts26mi.lib enduser\lib\x86\xts26m.dll       1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
if errorlevel 1  call :lbl_Error %TASK_NAME%


rem ============================================= Make VTerm Library
:lbl_vterm
set TASK_NAME=vterm.lib
echo Build %TASK_NAME%

set MAKE_OPTIONS=%LIB_OPTIONS%

echo xc.exe =p src\vterm\vterm %MAKE_OPTIONS%  1>  "%LOG_DIR%\%TASK_NAME%.log" 2>&1
xc.exe =p =a src\vterm\vterm %MAKE_OPTIONS%    1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
if errorlevel 1  call :lbl_Error %TASK_NAME%


rem ============================================= End
if "%WAS_MAKE_ERROR%" == "no"  goto  :lbl_Success
echo *** Build %COMPONET_NAME% *** Failed!
goto :EOF

rem ============================================= Script Messages
:lbl_Success
for %%i in (enduser\lib\C\lib.mkf)  do  if exist %%i del /Q %%i  1>nul 2>nul 
for %%i in (enduser\lib\x86\li.lib)  do  if exist %%i del /Q %%i  1>nul 2>nul 
echo === Build %COMPONET_NAME% === OK!
goto :EOF


:lbl_Error
echo *** Build %~1 *** Failed!
set WAS_MAKE_ERROR=yes
goto :EOF
