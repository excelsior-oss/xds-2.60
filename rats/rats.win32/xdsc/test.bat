@echo off
if "%REP_DIR%" == "" call ctr_env.bat


if "%1" == ""   goto INVALID_PARAM
if not exist %1 goto INVALID_PARAM

if not "%2" == "%CURRENTDIR%" echo Testing %2


set CURRENTDIR=%2

call clean

echo   --- PROCESSING TEMPLATE %~n1 ---
if not exist %REP_DIR%\%CURRENTDIR%  mkdir %REP_DIR%\%CURRENTDIR%
..\ctr\ctr.exe %1 %REP_DIR%\%CURRENTDIR%\%~n1.rep 1>%REP_DIR%\%CURRENTDIR%\%~n1.log 2>&1
if exist tmp\mod\*.mod goto FAIL
if exist tmp\ob2\*.ob2 goto FAIL
if exist tmp\def\*.def goto FAIL
goto DEL_LOG


:FAIL
echo   *** FAIL ***
mkdir %REP_DIR%\%CURRENTDIR%\%~n1 1>nul 2>nul
for %%i in (tmp\mod\*.mod) do copy %%i %REP_DIR%\%CURRENTDIR%\%~n1 1>nul 2>>%REP_DIR%\%CURRENTDIR%\%~n1.err
for %%i in (tmp\def\*.def) do copy %%i %REP_DIR%\%CURRENTDIR%\%~n1 1>nul 2>>%REP_DIR%\%CURRENTDIR%\%~n1.err
for %%i in (tmp\ob2\*.ob2) do copy %%i %REP_DIR%\%CURRENTDIR%\%~n1 1>nul 2>>%REP_DIR%\%CURRENTDIR%\%~n1.err
goto :EOF

:DEL_LOG
echo   --- OK ---
del %REP_DIR%\%CURRENTDIR%\%~n1.log 1>nul 2>nul
del %REP_DIR%\%CURRENTDIR%\%~n1.err 1>nul 2>nul
goto :EOF

:INVALID_PARAM
echo *** INVALID PARAMETER ***
goto :EOF
