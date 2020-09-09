@echo off
for %%i in (ch,log,sym) do if exist %%i rmdir /Q /S %%i
for %%i in (exe,lnk,mkf)  do  if exist *.%%i  del /Q *.%%i
if exist enduser/xds/bin/*.exe  del /Q enduser\xds\bin\*.exe  
 
 