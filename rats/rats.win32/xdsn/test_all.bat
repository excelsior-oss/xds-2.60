@echo off
call ctr_env.bat
call clean_rep.bat

for /F "eol=; tokens=*" %%i IN (template.txt) do  call :lbl_run_tests %%i 

goto :EOF


:lbl_run_tests
if "%1" == "--"  goto :EOF
if "%1" == ""    goto :EOF
for %%i in (%TEMPL_DIR%\%1\*.tg)  do call test %%i %1

goto :EOF
