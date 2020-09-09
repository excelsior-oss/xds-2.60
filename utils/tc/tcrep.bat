@echo off 
set OUT_FILE=%~n1.tcr

set TC_FILE=%JET_HOME%\bin
for %%i in (%JC%) do set TC_FILE=%TC_FILE%\%%~ni.tc
if not exist %TC_FILE% set TC_FILE=%1
if not exist %TC_FILE% goto show_help

for /F "usebackq eol=E delims=@" %%i IN (`time /T`) DO set TIME_STR=%%i

echo Run: %1 - %COMPUTERNAME% - %TIME_STR% > %OUT_FILE%
tc2shell.exe -c -po -f %TC_FILE% >> %OUT_FILE%
if exist tc2shellCfg.ini  del tc2shellCfg.ini
goto EOF

:show_help
echo Create test coverage persantage report
echo Usage: tcrep out_file
echo.
echo Try to find %JC%.tc file and call tc2shell utility to
echo create report file with percentage of test coverage 
goto EOF
