@echo off
if "%1" == "" goto show_help 
set FList=%1

for /F "eol=- skip=3 tokens=3*" %%i IN (%FList%) DO call tcmrep.bat %%i
goto EOF

:show_help
echo Usage: tcfmerge  file_with_module_list 
goto EOF

:EOF