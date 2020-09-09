@echo off
rem ------------------------------------------------------------------------------
rem                          Excelsior Rats System Utility
rem                            (c) 2000, Excelsior Ltd.
rem
rem Module:       report_all
rem Modified By:  
rem Created:      19-Apr-2001
rem Purpose:      Analysis all avalabel rats rep-files and create summary report 
rem Main:         report_all
rem Required:     Componet - report_all, report, set_param, countt
rem               Pakages  - cygwin-b20, u2d.exe
rem ------------------------------------------------------------------------------

SET REP_DIR=%1
SET OUTPUT_PREFIX=%2
SET TMP_OUT=tmp.out
SET TMP1_OUT=tmp1.out
SET ts_num_total=0
SET ts_num_real=0
SET totalreps=0

rem --------------- Create output file name
dir /T > %TMP_OUT%
grep %TMP_OUT% %TMP_OUT% > %TMP1_OUT%
FOR /F "tokens=1,2" %%i in (%TMP1_OUT%) do set name=rep_%%i
del %TMP_OUT%
del %TMP1_OUT%
set namefile=%OUTPUT_PREFIX%_%name%.txt
rem set namefile=mike.txt

rem --------------- Store previous report file name
if exist %name%_old.txt del %name%_old.txt
if exist %namefile%   ren  %namefile%  %name%_old.txt
if exist del_files.txt del del_files.txt


echo Whole Report For M2 Tests > %namefile%
DATE /T>>%namefile%
TIME /T>>%namefile%

echo Whole Report For M2 and O2 Tests in  %namefile%

FOR %%i in (%REP_DIR%\*.rep) DO call report  %%~fi >>%namefile%
if exist fileparam.buk del fileparam.buk
rem All  empty *rep files was deleted..

rem --------------- Count number of failed test files
ls -s -S -1 -p %REP_DIR%/*.rep > %TMP_OUT%
u2d %TMP_OUT% 1>nul 2>nul
FOR /F "tokens=1,2"  %%i in (%TMP_OUT%) do  call countt %%i
del %TMP_OUT%


call report %REP_DIR%/*.rep 
SET /A ts_num_real=%ts_num_real%-1 
echo ---------------------------------------->>%namefile%
echo ----------------------------------------

rem echo Failed AND Zavis m2 test(s): %ts_num_total% >> %namefile%
echo Total  m2 test(s): %totalreps% >>%namefile%
echo Failed m2 test(s): %ts_num_real% >> %namefile%
echo Total m2 error(s): %kol1%   >> %namefile%
echo          compilation error(s):%kol2% >>%namefile%
echo          linking error(s)    :%kol3% >>%namefile%
echo          execution error(s)  :%kol4% >>%namefile%
echo see %namefile%

