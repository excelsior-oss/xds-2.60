@echo off

set REP_COUNT_FILE=.\rep_count.txt
if not exist %REP_COUNT_FILE%  echo 0 > %REP_COUNT_FILE%
for /F %%i in (%REP_COUNT_FILE%) do  set /A NEW_COUNT=%%i+1
set REP_COUNT=%NEW_COUNT%
if %NEW_COUNT% LSS 10    set REP_COUNT=0%REP_COUNT%
if %NEW_COUNT% LSS 100   set REP_COUNT=0%REP_COUNT%
echo %NEW_COUNT% > %REP_COUNT_FILE%

for /F "eol=; tokens=1*" %%i IN (testing.txt) do  call :lbl_process %%i %%j 

goto :EOF


:lbl_process
if "%1" == "--"  goto :EOF
if "%1" == ""    goto :EOF
echo Run tests for %1
for /F "usebackq eol=E delims=@" %%i IN (`time /T`) DO set TIME_STR=%%i
echo   start %TIME_STR%

pushd .\%1
if exist reports rename reports reports_%REP_COUNT%
call test_all.bat 1> test_all.log 2>&1
popd

call list_failed_tests.bat %1\reports

for /F "usebackq eol=E delims=@" %%i IN (`time /T`) DO set TIME_STR=%%i
echo   end   %TIME_STR%

goto :EOF
