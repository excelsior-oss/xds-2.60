@echo off

for /F "eol=; tokens=1*" %%i IN (testing.txt) do  call :lbl_top_level %%i %%j 

goto :EOF

:lbl_top_level
set ROOT_DIR=%1
for /F "eol=; tokens=*" %%i IN (%ROOT_DIR%\..\template.txt) do  call :lbl_sub_level %%i 

goto :EOF


:lbl_sub_level
set REP_DIR=%1
if not exist %ROOT_DIR%\reports\%REP_DIR%\*.rep  goto :EOF
echo Parse: %ROOT_DIR%\report\%REP_DIR%
call report_all.bat %ROOT_DIR%/reports/%REP_DIR% %ROOT_DIR%\reports\%REP_DIR%

goto :EOF
