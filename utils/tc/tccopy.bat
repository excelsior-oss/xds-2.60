@echo off
if "%1" == ""  goto :show_help

set DEST_DIR=%1
if not exist %DEST_DIR%  mkdir %DEST_DIR%

pushd %DEST_DIR%
set FDEST_DIR=%CD%\
popd

if "%2" == ""  goto start_in_cur_dir 
for /R %2 %%i in (*.tcr) do call :process_tc_file %%i
goto EOF

:start_in_cur_dir
for /R %%i in (*.tcr) do call :process_tc_file %%i
goto EOF


:show_help
echo tcr-files search and copy script 
echo Usage: tccopy  dest_dir  [src_dir]
echo. 
echo Walks the directory tree rooted at 'src_dir' or current 
echo directory search tcr-file in each directory of the tree
echo and copy it into 'dest_dir'
goto EOF

:process_tc_file
if "%FDEST_DIR%" == "%~dp1"  goto EOF
if exist %DEST_DIR%\%~nx1  del /Q %DEST_DIR%\%~nx1
echo copy %~f1 to %DEST_DIR%\
copy %~f1  %DEST_DIR%\
goto EOF

:EOF