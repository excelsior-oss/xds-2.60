@echo off

for %%i in (Debug)  do  if exist %%i rmdir /Q /S %%i 
for %%i in (exe) do  if exist *.%%i         del /F /Q *.%%i
for %%i in (exe) do  if exist ..\*.%%i      del /F /Q ..\*.%%i
rem for %%i in (exe) do  if exist ..\doc\*.%%i  del /F /Q ..\doc\*.%%i
