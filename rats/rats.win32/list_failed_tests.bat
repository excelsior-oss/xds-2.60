@echo off
rem ------------------------------------------------------------------------------
rem                          Excelsior Rats System Utility
rem                            (c) 2007, Excelsior Ltd.
rem
rem Module:       report_all
rem Modified By:  
rem Created:      16-March-2007
rem Purpose:      List all filed test in report subdirectories. 
rem Main:         
rem Required:     Pakages  - cygwin-b20: grep, cat
rem ------------------------------------------------------------------------------

set REP_DIR=%1

rem --------------- Create output file name
set list_file=failed_tests_list_%DATE%.txt
set store_list_file=failed_tests_list_%DATE%_old.txt

pushd %REP_DIR% 

if exist %store_list_file%  del %store_list_file%
if exist %list_file%        ren %list_file%  %store_list_file%

for /D %%i in (*) do call :lbl_parse_test_group %%i
popd

goto :EOF


:lbl_parse_test_group
if not exist %1\*.rep  goto :EOF

echo Test group: %1 >> %list_file%
echo ======================== >> %list_file%
for /D %%i in (%1\*) do call :lbl_pares_dir %%i

goto :EOF


:lbl_pares_dir
echo %1 >> %list_file%
echo ------------------------  >> %list_file%
for %%i in (%1\*) do  echo    %%i >> %list_file%
goto :EOF
