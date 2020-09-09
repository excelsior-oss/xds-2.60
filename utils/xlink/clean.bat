@echo off

for %%i in (log,ch,sym,enduser) do if exist %%i rmdir /Q /S %%i

for %%i in (mkf,$$$,lnk,out)  do  if exist *.%%i  del /Q *.%%i 
for %%i in (exp,ilk,lib,pdb,exe)  do  if exist *.%%i  del /Q *.%%i 
