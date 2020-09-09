@echo off

for %%i in (log,ch,sym) do if exist %%i rmdir /Q /S %%i

for %%i in (mkf,$$$,lnk,out,exe)  do  if exist *.%%i  del /Q *.%%i 
