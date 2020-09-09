@IF NOT EXIST ECHO.ON ECHO OFF

set log=xtest.log

echo ====== xTest log file
echo ====== xTest log file>%log%

IF .%1. == .. GOTO USAGE
REM IF NOT EXIST %1 GOTO MISSING

echo ====== Save header of executable file
echo ====== Save header of executable file>>%log%
hdrstrip %1 >>%log%
if errorlevel 1 goto error

echo ====== Save debug information
echo ====== Save debug information>>%log%
xstrip %1 >>%log%
if errorlevel 1 goto error

echo ====== Restore debug information
echo ====== Restore debug information>>%log%
xstrip -a=%1 >>%log%
if errorlevel 1 goto error

echo ====== Load program
echo ====== Load program>>%log%
xload %1 >>%log%
if errorlevel 1 goto error

GOTO DONE

:ERROR
echo !!!!!! ERROR DETECTED
echo !!!!!! ERROR DETECTED>>%log%
GOTO DONE

:MISSING
@ECHO.
@ECHO Cannot locate file '%1'.
@ECHO Cannot locate file '%1'. >> %log%
@ECHO.
GOTO DONE

:USAGE
ECHO.
ECHO Usage: %0 [Program name without extension]
ECHO Usage: %0 [Program name without extension]>>%log%
ECHO.

:DONE
echo ====== Done
echo ====== Done >>%log%


