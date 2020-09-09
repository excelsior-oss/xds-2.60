@echo off
call %~dp0\..\..\..\.config\setenv.bat

call clean.bat
call mkf.bat
call update.bat