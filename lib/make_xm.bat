@echo off
set COMPONET_NAME=XDS-C libraries

call %~dp0\..\.config\setenv.bat

set LOG_DIR=%~dp0\log

for %%i in (enduser,enduser\def\ob2) do if not exist %%i mkdir %%i 
for %%i in (enduser\lib\C)           do if not exist %%i mkdir %%i 
for %%i in (log,ch\C,sym\C,include\C)  do if not exist %%i mkdir %%i 
for %%i in (enduser\C,enduser\include) do if not exist %%i mkdir %%i 

set WAS_MAKE_ERROR=no

echo Update C-sources 
xcopy /R /Y src\os\posix\*.c      ch\C\       1>nul 2>nul || exit /B 1 
xcopy /R /Y src\os\posix\*.h      include\C\  1>nul 2>nul || exit /B 1
xcopy /R /Y src\xr\C\*.c          ch\C\       1>nul 2>nul || exit /B 1
xcopy /R /Y src\xr\C\*.h          include\C\  1>nul 2>nul || exit /B 1
xcopy /R /Y ..\API\src\Win32\*.h  include\C\  1>nul 2>nul || exit /B 1

xcopy /R /Y src\os\posix\*.c      enduser\C\C\        1>nul 2>nul || exit /B 1 
xcopy /R /Y src\xr\C\*.c          enduser\C\C\        1>nul 2>nul || exit /B 1
xcopy /R /Y src\os\posix\*.h      enduser\include\C\  1>nul 2>nul || exit /B 1
xcopy /R /Y src\xr\C\*.h          enduser\include\C\  1>nul 2>nul || exit /B 1
xcopy /R /Y ..\API\src\Win32\*.h  enduser\include\C\  1>nul 2>nul || exit /B 1

xcopy /R /Y src\TSlibs\POSIX\*.c  enduser\C\C\        1>nul 2>nul || exit /B 1 


rem ============================================= Make MSVC library
:lbl_msvc_lib
set TASK_NAME=msvc_lib
echo Build %TASK_NAME%

set XDS_OPT=-cc=MSVC -mode=user -placement=current -env_target=winnt -decor=rht
set TARGET_OPT=-target_fs=unc -target_family=win32 -target_os=winnt 

echo "xm.exe" =p src\m\lib.prj %XDS_OPT% %TARGET_OPT%    1>  "%LOG_DIR%\%TASK_NAME%.log" 2>&1
xm.exe =p src\m\lib.prj %XDS_OPT% %TARGET_OPT%           1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1  
if errorlevel 1  call :lbl_Error %TASK_NAME%

rem ============================================= Generate MSVC makefile
:lbl_msvc_mkf
set TASK_NAME=msvc.mkf
echo Build %TASK_NAME%

set XDS_OPT=-cc=MSVC -env_target=winnt -decor=rht 
set TARGET_OPT=-target_fs=unc -target_family=win32 -target_os=winnt  
set MAKE_OPTIONS=-mkf_name:=msvc %XDS_OPT% %TARGET_OPT%

pushd enduser\lib\C
echo xm.exe ..\..\..\src\m\lib.prj =g -mode:=user %MAKE_OPTIONS%  1>  "%LOG_DIR%\%TASK_NAME%.log" 2>&1
xm.exe ..\..\..\src\m\lib.prj =g -mode:=user %MAKE_OPTIONS%       1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
if errorlevel 1  call :lbl_Error %TASK_NAME%
popd

xcopy /R /Y ch\C\*.c        enduser\C\C\        1>nul 2>nul || exit /B 1 
xcopy /R /Y include\C\*.h   enduser\include\C\  1>nul 2>nul || exit /B 1 


rem ============================================= Generate LLVM makefile
:lbl_clangcl_mkf
set TASK_NAME=clangcl.mkf
echo Build %TASK_NAME%

set XDS_OPT=-cc=CLangCL -env_target=clangnt -decor=rht 
set TARGET_OPT=-target_fs=unc -target_family=win32 -target_os=winnt  
set MAKE_OPTIONS=-mkf_name:=clangcl %XDS_OPT% %TARGET_OPT%

pushd enduser\lib\C
echo xm.exe ..\..\..\src\m\lib.prj =g -mode:=user %MAKE_OPTIONS%  1>  "%LOG_DIR%\%TASK_NAME%.log" 2>&1
xm.exe ..\..\..\src\m\lib.prj =g -mode:=user %MAKE_OPTIONS%       1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
if errorlevel 1  call :lbl_Error %TASK_NAME%
popd


rem ============================================= Generate Unix makefile
:lbl_unix_mkf
set TASK_NAME=unix.mkf
echo Build %TASK_NAME%

set XDS_OPT=-cc=GCC -env_target= -decor=rht
set TARGET_OPT=-target_fs=unix -target_family=unix -target_os=unix -libext=a 
set MAKE_OPTIONS=-mkf_name:=unix %XDS_OPT% %TARGET_OPT%

pushd enduser\lib\C
echo xm.exe ..\..\..\src\m\lib.prj =g -mode:=user %MAKE_OPTIONS%  1>  "%LOG_DIR%\%TASK_NAME%.log" 2>&1
xm.exe ..\..\..\src\m\lib.prj =g -mode:=user %MAKE_OPTIONS%       1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
if errorlevel 1  call :lbl_Error %TASK_NAME%
popd


rem ============================================= Generate Watcom makefile
:lbl_watcom_mkf
set TASK_NAME=watcom.mkf
echo Build %TASK_NAME%

set XDS_OPT=-cc=Watcom -env_target=watcomnt -decor=rht
set TARGET_OPT=-target_family=win32 
set MAKE_OPTIONS=-mkf_name:=watcom %XDS_OPT% %TARGET_OPT%

pushd enduser\lib\C
echo xm.exe ..\..\..\src\m\lib.prj =g -mode:=user %MAKE_OPTIONS%  1>  "%LOG_DIR%\%TASK_NAME%.log" 2>&1
xm.exe ..\..\..\src\m\lib.prj =g -mode:=user %MAKE_OPTIONS%       1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
if errorlevel 1  call :lbl_Error %TASK_NAME%
popd



rem ============================================= Make MSVC TopSpeed library
:lbl_msvcts_lib
set TASK_NAME=msvcts_lib
echo Build %TASK_NAME%

set XDS_OPT=-cc=MSVC -env_target=winnt -env_host=winnt
set TARGET_OPT=-target_fs=unc -target_family=win32 -target_os=winnt 

xcopy /R /Y src\TSlibs\POSIX\*.c  ch\C\   1>nul 2>nul || exit /B 1 
copy src\TSlibs\IO.*  src\TSlibs\IO_.*    1>nul 2>nul

echo "xm.exe" =p src\TSlibs\tslib.prj %XDS_OPT% %TARGET_OPT%    1>  "%LOG_DIR%\%TASK_NAME%.log" 2>&1
xm.exe =p src\TSlibs\tslib.prj %XDS_OPT% %TARGET_OPT%           1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1  
if errorlevel 1  call :lbl_Error %TASK_NAME%


rem ============================================= Generate TopSpeed MSVC makefile
:lbl_msvcts_mkf
set TASK_NAME=msvcts.mkf
echo Build %TASK_NAME%

set XDS_OPT=-cc=MSVC -env_target=winnt -decor=rht 
set TARGET_OPT=-target_fs=unc -target_family=win32 -target_os=winnt  
set MAKE_OPTIONS=-mkf_name:=msvcts %XDS_OPT% %TARGET_OPT%

pushd enduser\lib\C
echo xm.exe ..\..\..\src\TSlibs\tslib.prj =g -mode:=user %MAKE_OPTIONS%  1>  "%LOG_DIR%\%TASK_NAME%.log" 2>&1
xm.exe ..\..\..\src\TSlibs\tslib.prj =g -mode:=user %MAKE_OPTIONS%       1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
if errorlevel 1  call :lbl_Error %TASK_NAME%
popd

xcopy /R /Y ch\C\*.c        enduser\C\C\        1>nul 2>nul || exit /B 1 
xcopy /R /Y include\C\*.h   enduser\include\C\  1>nul 2>nul || exit /B 1 


rem ============================================= Generate TopSpeed LLVM makefile
:lbl_clangclts_mkf
set TASK_NAME=clangclts.mkf
echo Build %TASK_NAME%

set XDS_OPT=-cc=CLangCL -env_target=clangnt -decor=rht
set TARGET_OPT=-target_fs=unc -target_family=win32 -target_os=winnt
set MAKE_OPTIONS=-mkf_name:=clangclts %XDS_OPT% %TARGET_OPT%

pushd enduser\lib\C
echo xm.exe ..\..\..\src\TSlibs\tslib.prj =g -mode:=user %MAKE_OPTIONS%  1>  "%LOG_DIR%\%TASK_NAME%.log" 2>&1
xm.exe ..\..\..\src\TSlibs\tslib.prj =g -mode:=user %MAKE_OPTIONS%       1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
if errorlevel 1  call :lbl_Error %TASK_NAME%
popd


rem ============================================= Generate Unix makefile
:lbl_unix_mkf
set TASK_NAME=unixts_mkf
echo Build %TASK_NAME%

set XDS_OPT=-cc=GCC -env_target= -decor=rht
set TARGET_OPT=-target_fs=unix -target_family=unix -target_os=unix -libext=a 
set MAKE_OPTIONS=-mkf_name:=unixts %XDS_OPT% %TARGET_OPT%

pushd enduser\lib\C
echo xm.exe ..\..\..\src\TSlibs\tslib.prj =g -mode:=user %MAKE_OPTIONS%  1>  "%LOG_DIR%\%TASK_NAME%.log" 2>&1
xm.exe ..\..\..\src\TSlibs\tslib.prj =g -mode:=user %MAKE_OPTIONS%       1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
if errorlevel 1  call :lbl_Error %TASK_NAME%
popd


rem ============================================= Generate Watcom makefile
:lbl_watcomts_mkf
set TASK_NAME=watcomts.mkf
echo Build %TASK_NAME%

set XDS_OPT=-cc=Watcom -env_target=watcomnt -decor=rht
set TARGET_OPT=-target_family=win32 
set MAKE_OPTIONS=-mkf_name:=watcomts %XDS_OPT% %TARGET_OPT%

pushd enduser\lib\C
echo xm.exe ..\..\..\src\TSlibs\tslib.prj =g -mode:=user %MAKE_OPTIONS%  1>  "%LOG_DIR%\%TASK_NAME%.log" 2>&1
xm.exe ..\..\..\src\TSlibs\tslib.prj =g -mode:=user %MAKE_OPTIONS%       1>> "%LOG_DIR%\%TASK_NAME%.log" 2>>&1
if errorlevel 1  call :lbl_Error %TASK_NAME%
popd


rem ============================================= End
if "%WAS_MAKE_ERROR%" == "no"  goto  :lbl_Success
echo *** Build %COMPONET_NAME% *** Failed!
goto :EOF

rem ============================================= Script Messages
:lbl_Success
for %%i in (enduser\lib\C\lib.mkf)    do  if exist %%i del /Q %%i  1>nul 2>nul 
for %%i in (enduser\lib\C\tslib.mkf)  do  if exist %%i del /Q %%i  1>nul 2>nul 
echo === Build %COMPONET_NAME% === OK!
goto :EOF


:lbl_Error
echo *** Build %~1 *** Failed!
set WAS_MAKE_ERROR=yes
goto :EOF

