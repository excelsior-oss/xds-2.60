@echo off
rem ------------------------------------------------------------------------------
rem                          Excelsior Rats System Utility
rem                            (c) 2000, Excelsior Ltd.
rem
rem Module:       report
rem Modified By:  
rem Created:      19-Apr-2001
rem Purpose:      Analysis one rats rep-files and create error summary report 
rem Main:         report_all
rem Required:     Componet - report_all, report, set_param, countt
rem               Pakages  - cygwin-b20, u2d.exe
rem ------------------------------------------------------------------------------

if %1 NEQ "" SET name=%1
SET test_name=%~n1.tg

if exist fileparam.buk del fileparam.buk
   @grep : %name% | wc -l           >> fileparam.buk 
   @grep compilation %name% | wc -l >> fileparam.buk 
   @grep linking %name% | wc -l     >> fileparam.buk 
   @grep execution %name% | wc -l   >> fileparam.buk 


SET A=0
 u2d fileparam.buk 1>nul 2>nul
 FOR /F "tokens=1,2" %%i in (fileparam.buk) do call set_param %%i  
 IF %kol1% EQU 0   goto no_errors

    echo  %test_name%                            Failed!
    echo    Total error(s): %kol1%
    echo          compilation error(s): %kol2%
    echo          linking error(s)    : %kol3%
    echo          execution error(s)  : %kol4%
    SET /A ts_num_real=%ts_num_real%+1

:no_errors

if exist fileparam.buk del fileparam.buk
