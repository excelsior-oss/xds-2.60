@echo off

for %%i in (log,enduser\tmp) do if exist %%i rmdir /Q /S %%i

for %%i in (exe,dll)  do  if exist enduser\xds\bin\*.%%i  del /Q enduser\xds\bin\*.%%i
