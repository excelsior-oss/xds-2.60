@echo off
set Pattern=%1
if "%Pattern%" == ""  set Pattern=Total


date /t 
echo Conditions %%covered  Module              run command     
echo ---------- -------- -------------------  ---------------  

for /R %%i in (*.tcr) do call :process_tc_file %%i
goto EOF

:process_tc_file
rem echo process %~nx1
pushd %~p1
set TC_FILE=%~nx1

for /F "usebackq delims=@" %%i IN (`grep -w Run:      %TC_FILE%`) DO set ARG_STR=%%i
for /F "usebackq delims=@" %%i IN (`grep -w %Pattern% %TC_FILE%`) DO set STAT_STR=%%i

echo %STAT_STR%   %ARG_STR% 
popd

:EOF