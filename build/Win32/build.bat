@echo off

if not exist enduser  call get.bat

call setenv.bat

call nmake MODE=ENDUSER %*
